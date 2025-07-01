#!/usr/bin/env python3
##
## test_ai_manager.py for Zappy in /home/adrien/Epitech/B-YEP-410/Zappy/ai/tests
##
## Made by adrien.marette@epitech.eu
## Login   <adrien.marette@epitech.eu>
##
## Started on  Wed Jun 25 11:39:38 AM 2025 adrien.marette@epitech.eu
## Last update Mon Jun 29 3:46:51 PM 2025 adrien.marette@epitech.eu
##

import unittest
import sys
import os
import time
import multiprocessing
from unittest.mock import Mock, patch, MagicMock

# Add src directory to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'src'))

from ai_manager import ZappyAIManager

class TestZappyAIManager(unittest.TestCase):
    """Test the AI Manager for multi-process spawning"""
    
    def setUp(self):
        """Set up test environment"""
        self.manager = ZappyAIManager(
            port=4242,
            team_name="test_team",
            hostname="localhost",
            max_processes=3
        )
    
    def test_init(self):
        """Test AI Manager initialization"""
        self.assertEqual(self.manager.port, 4242)
        self.assertEqual(self.manager.team_name, "test_team")
        self.assertEqual(self.manager.hostname, "localhost")
        self.assertEqual(self.manager.max_processes, 3)
        self.assertEqual(len(self.manager.active_processes), 0)
        self.assertEqual(self.manager.process_counter, 0)
    
    @patch("socket.socket")
    def test_get_server_info_success(self, mock_socket_class):
        """Test successful server info retrieval"""
        # Create mock socket
        mock_socket = Mock()
        mock_socket_class.return_value = mock_socket
        
        # Mock server responses
        mock_socket.recv.side_effect = [
            b"WELCOME\n",
            b"5\n",  # Available slots
            b"10 15\n"  # World dimensions
        ]
        
        result = self.manager.get_server_info()
        
        self.assertTrue(result)
        self.assertEqual(self.manager.available_slots, 5)
        self.assertEqual(self.manager.world_width, 10)
        self.assertEqual(self.manager.world_height, 15)
        
        # Verify socket operations
        mock_socket.connect.assert_called_once_with(("localhost", 4242))
        mock_socket.send.assert_called_once_with(b"test_team\n")
        mock_socket.close.assert_called_once()
    
    @patch("socket.socket")
    def test_get_server_info_failure(self, mock_socket_class):
        """Test failed server info retrieval"""
        # Mock connection failure
        mock_socket_class.side_effect = ConnectionRefusedError("Connection refused")
        
        result = self.manager.get_server_info()
        self.assertFalse(result)
    
    def test_should_spawn_new_ai_conditions(self):
        """Test conditions for spawning new AI"""
        # Initially should spawn (no active processes)
        self.assertTrue(self.manager.should_spawn_new_ai())
        
        # Set available slots
        self.manager.available_slots = 2
        
        # Should still spawn (within limits)
        self.assertTrue(self.manager.should_spawn_new_ai())
        
        # Add fake processes to reach limit
        for i in range(2):
            self.manager.active_processes[i] = Mock()
        
        # Should not spawn (at slot limit)
        self.assertFalse(self.manager.should_spawn_new_ai())
        
        # Clear slots limit but set process limit
        self.manager.available_slots = None
        for i in range(3):
            self.manager.active_processes[i] = Mock()
        
        # Should not spawn (at process limit)
        self.assertFalse(self.manager.should_spawn_new_ai())
    
    def test_spawn_interval_check(self):
        """Test spawn interval timing"""
        # Set last spawn attempt to now
        self.manager.last_spawn_attempt = time.time()
        
        # Should not spawn immediately (within interval)
        self.assertFalse(self.manager.should_spawn_new_ai())
        
        # Set last spawn attempt to past
        self.manager.last_spawn_attempt = time.time() - 10
        
        # Should spawn (past interval)
        self.assertTrue(self.manager.should_spawn_new_ai())
    
    @patch("multiprocessing.Process")
    def test_spawn_ai_process_success(self, mock_process_class):
        """Test successful AI process spawning"""
        # Create mock process
        mock_process = Mock()
        mock_process_class.return_value = mock_process
        
        result = self.manager.spawn_ai_process()
        
        self.assertTrue(result)
        self.assertEqual(len(self.manager.active_processes), 1)
        self.assertEqual(self.manager.process_counter, 1)
        self.assertEqual(self.manager.stats['total_spawned'], 1)
        self.assertEqual(self.manager.stats['successful_spawns'], 1)
        
        # Verify process creation and start
        mock_process.start.assert_called_once()
    
    def test_spawn_ai_process_limit_reached(self):
        """Test spawning when limit is reached"""
        # Fill up to max processes
        for i in range(3):
            self.manager.active_processes[i] = Mock()
        
        result = self.manager.spawn_ai_process()
        
        self.assertFalse(result)
        self.assertEqual(len(self.manager.active_processes), 3)  # No change
    
    def test_cleanup_completed_processes(self):
        """Test cleanup of completed processes"""
        # Add mock processes
        mock_process1 = Mock()
        mock_process1.is_alive.return_value = False
        mock_process1.exitcode = 0
        
        mock_process2 = Mock()
        mock_process2.is_alive.return_value = True
        
        mock_process3 = Mock()
        mock_process3.is_alive.return_value = False
        mock_process3.exitcode = 1
        
        self.manager.active_processes = {
            1: mock_process1,
            2: mock_process2,
            3: mock_process3
        }
        
        self.manager.cleanup_completed_processes()
        
        # Should keep only the alive process
        self.assertEqual(len(self.manager.active_processes), 1)
        self.assertIn(2, self.manager.active_processes)
        self.assertEqual(self.manager.stats['completed_ais'], 1)
        
        # Verify join was called on completed processes
        mock_process1.join.assert_called_once()
        mock_process3.join.assert_called_once()
    
    def test_cleanup_all_processes(self):
        """Test cleanup of all processes"""
        # Add mock processes
        mock_process1 = Mock()
        mock_process1.is_alive.return_value = True
        
        mock_process2 = Mock()
        mock_process2.is_alive.return_value = True
        
        self.manager.active_processes = {
            1: mock_process1,
            2: mock_process2
        }
        
        self.manager.cleanup_all_processes()
        
        # All processes should be removed
        self.assertEqual(len(self.manager.active_processes), 0)
        
        # Verify terminate and join were called
        mock_process1.terminate.assert_called_once()
        mock_process1.join.assert_called()
        mock_process2.terminate.assert_called_once()
        mock_process2.join.assert_called()
    
    def test_stats_tracking(self):
        """Test statistics tracking"""
        initial_stats = self.manager.stats.copy()
        
        # All stats should start at 0
        for value in initial_stats.values():
            self.assertEqual(value, 0)
        
        # Test stats updates during operation
        self.manager.stats['total_spawned'] = 5
        self.manager.stats['successful_spawns'] = 4
        self.manager.stats['failed_spawns'] = 1
        
        self.assertEqual(self.manager.stats['total_spawned'], 5)
        self.assertEqual(self.manager.stats['successful_spawns'], 4)
        self.assertEqual(self.manager.stats['failed_spawns'], 1)
    
    def test_signal_handler(self):
        """Test signal handling"""
        # Mock cleanup method
        self.manager.cleanup_all_processes = Mock()
        
        # Test signal handler (should not raise)
        try:
            self.manager.signal_handler(2, None)  # SIGINT
        except SystemExit:
            pass  # Expected
        
        # Verify cleanup was called
        self.manager.cleanup_all_processes.assert_called_once()


class TestAIManagerIntegration(unittest.TestCase):
    """Integration tests for AI Manager"""
    
    def setUp(self):
        """Set up test environment"""
        # Use minimal settings for integration tests
        multiprocessing.set_start_method('spawn', force=True)
    
    def test_manager_initialization(self):
        """Test that manager can be initialized properly"""
        manager = ZappyAIManager(
            port=4242,
            team_name="integration_test",
            hostname="localhost",
            max_processes=1
        )
        
        self.assertIsInstance(manager, ZappyAIManager)
        self.assertEqual(manager.team_name, "integration_test")
        self.assertEqual(manager.max_processes, 1)
    
    @patch("ai_manager.ZappyAI")
    @patch("multiprocessing.Process")
    def test_ai_process_creation(self, mock_process_class, mock_ai_class):
        """Test that AI processes can be created"""
        # Mock the process and AI
        mock_process = Mock()
        mock_process_class.return_value = mock_process
        
        manager = ZappyAIManager(
            port=4242,
            team_name="test",
            hostname="localhost",
            max_processes=1
        )
        
        # Attempt to spawn
        result = manager.spawn_ai_process()
        
        self.assertTrue(result)
        mock_process.start.assert_called_once()


if __name__ == '__main__':
    # Set up multiprocessing for tests
    if __name__ == '__main__':
        multiprocessing.set_start_method('spawn', force=True)
    
    unittest.main()
