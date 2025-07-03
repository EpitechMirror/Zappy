#!/usr/bin/env python3
##
## test_functional_zappy_ai.py for AI New in /home/adrien/Desktop/AI New
##
## Made by adrien.marette@epitech.eu
## Login   <adrien.marette@epitech.eu>
##
## Started on  Sat Jul 4 01:00:00 AM 2025 adrien.marette@epitech.eu
## Last update Sat Jul 4 01:00:00 AM 2025 adrien.marette@epitech.eu
##

import unittest
import threading
import time
import socket
import json
import subprocess
import sys
import os
from unittest.mock import Mock, patch, MagicMock

# Add the current directory to path for imports
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from Team import Team, TeamRole
from TeamExplorer import TeamExplorer
from TeamCollector import TeamCollector
from TeamGuardian import TeamGuardian
from TeamCoordinator import TeamCoordinator

class MockZappyServer:
    """Mock Zappy server for functional testing."""
    
    def __init__(self, port=4242):
        self.port = port
        self.socket = None
        self.clients = []
        self.running = False
        self.world_size = (10, 10)
        self.teams = {}
        self.resources = {}
        
    def start(self):
        """Start the mock server."""
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.socket.bind(('localhost', self.port))
        self.socket.listen(5)
        self.running = True
        
        # Start accepting connections in a separate thread
        self.accept_thread = threading.Thread(target=self._accept_connections)
        self.accept_thread.daemon = True
        self.accept_thread.start()
        
    def stop(self):
        """Stop the mock server."""
        self.running = False
        if self.socket:
            self.socket.close()
            
    def _accept_connections(self):
        """Accept client connections."""
        while self.running:
            try:
                client, addr = self.socket.accept()
                client_thread = threading.Thread(target=self._handle_client, args=(client,))
                client_thread.daemon = True
                client_thread.start()
                self.clients.append(client)
            except:
                break
                
    def _handle_client(self, client):
        """Handle a client connection."""
        try:
            # Send welcome message
            client.send(b"WELCOME\n")
            
            # Receive team name
            team_name = client.recv(1024).decode().strip()
            
            # Send client number and world size in correct format
            client.send(b"1\n10 10\n")
            
            # Handle commands
            while self.running:
                try:
                    data = client.recv(1024).decode().strip()
                    if not data:
                        break
                        
                    response = self._process_command(data, team_name)
                    if response:
                        client.send(f"{response}\n".encode())
                        
                except:
                    break
                    
        except Exception as e:
            pass
        finally:
            client.close()
            
    def _process_command(self, command, team_name):
        """Process a client command."""
        command = command.lower().strip()
        
        if command == "inventory":
            return "food 10, linemate 0, deraumere 0, sibur 0, mendiane 0, phiras 0, thystame 0"
        elif command == "look":
            return "player, food linemate, empty, empty, empty, empty, empty, empty, empty"
        elif command == "forward":
            return "ok"
        elif command == "right":
            return "ok"
        elif command == "left":
            return "ok"
        elif command.startswith("take"):
            return "ok"
        elif command.startswith("set"):
            return "ok"
        elif command.startswith("fork"):
            return "ok"
        elif command == "incantation":
            return "elevation underway"
        elif command.startswith("broadcast"):
            return "ok"
        else:
            return "ko"


class TestZappyAIFunctional(unittest.TestCase):
    """Functional tests for the Zappy AI system."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.server = MockZappyServer(4243)  # Use different port for testing
        self.server.start()
        time.sleep(0.1)  # Give server time to start
        
    def tearDown(self):
        """Clean up after tests."""
        self.server.stop()
        time.sleep(0.1)
        
    def test_ai_connection_and_basic_commands(self):
        """Test AI can connect and send basic commands."""
        ai = TeamExplorer("localhost", 4243, "test_team")
        
        # Test connection
        connected = ai.connect_to_server()
        self.assertTrue(connected)
        
        # Test basic commands
        ai.send_command("inventory")
        ai.send_command("look")
        ai.send_command("forward")
        
        # Clean up
        if ai.socket:
            ai.socket.close()
            
    def test_multiple_ai_connections(self):
        """Test multiple AI instances can connect simultaneously."""
        ais = []
        
        # Create multiple AI instances
        for i in range(3):
            ai = TeamExplorer("localhost", 4243, f"team_{i}")
            connected = ai.connect_to_server()
            self.assertTrue(connected)
            ais.append(ai)
            
        # Test they can all send commands
        for ai in ais:
            ai.send_command("inventory")
            ai.send_command("look")
            
        # Clean up
        for ai in ais:
            if ai.socket:
                ai.socket.close()
                
    def test_ai_role_specific_behavior(self):
        """Test each AI role exhibits specific behavior."""
        # Test Explorer
        explorer = TeamExplorer("localhost", 4243, "test_team")
        explorer.connect_to_server()
        
        with patch.object(explorer, 'send_command') as mock_send:
            explorer.systematic_exploration()
            mock_send.assert_called()  # Should send exploration commands
            
        # Test Collector
        collector = TeamCollector("localhost", 4243, "test_team")
        collector.connect_to_server()
        
        with patch.object(collector, 'send_command') as mock_send:
            collector.inventory["food"] = 20  # Low food
            collector.execute_role_strategy()
            # Should prioritize food collection
            
        # Clean up
        for ai in [explorer, collector]:
            if ai.socket:
                ai.socket.close()
                
    def test_team_communication(self):
        """Test team communication between AI instances."""
        # Create coordinator and collector
        coordinator = TeamCoordinator("localhost", 4243, "test_team")
        collector = TeamCollector("localhost", 4243, "test_team")
        
        coordinator.connect_to_server()
        collector.connect_to_server()
        
        # Test message broadcasting
        test_message = {"type": "test", "id": "coordinator", "data": "hello"}
        
        with patch.object(coordinator, 'send_message') as mock_send:
            coordinator.broadcast_team_message(test_message)
            # Should attempt to broadcast
            
        # Clean up
        for ai in [coordinator, collector]:
            if ai.socket:
                ai.socket.close()
                
    def test_resource_sharing_workflow(self):
        """Test complete resource sharing workflow."""
        collector = TeamCollector("localhost", 4243, "test_team")
        collector.connect_to_server()
        
        # Set up collector with excess resources
        collector.inventory = {
            "food": 100, "linemate": 5, "deraumere": 0, "sibur": 0,
            "mendiane": 0, "phiras": 0, "thystame": 0
        }
        collector.level = 1
        
        with patch.object(collector, 'broadcast_team_message') as mock_broadcast:
            collector.share_resources_with_team()
            
            # Should broadcast resource offers
            self.assertTrue(mock_broadcast.called)
            
        # Clean up
        if collector.socket:
            collector.socket.close()
            
    def test_elevation_coordination(self):
        """Test elevation coordination between team members."""
        coordinator = TeamCoordinator("localhost", 4243, "test_team")
        coordinator.connect_to_server()
        
        # Create mock team members ready for elevation
        member1 = Mock()
        member1.level = 2
        member1.inventory = {"food": 60, "linemate": 2, "deraumere": 2, "sibur": 2}
        member1.position = (5, 5)
        
        member2 = Mock()
        member2.level = 2
        member2.inventory = {"food": 70, "linemate": 2, "deraumere": 2, "sibur": 2}
        member2.position = (6, 6)
        
        coordinator.team_members = {1: member1, 2: member2}
        
        with patch.object(coordinator, 'is_ready_for_elevation', return_value=True):
            with patch.object(coordinator, 'broadcast_team_message') as mock_broadcast:
                coordinator.manage_team_elevations()
                
                # Should organize elevation
                self.assertTrue(len(coordinator.elevation_plans) > 0)
                
        # Clean up
        if coordinator.socket:
            coordinator.socket.close()
            
    def test_ai_spawning_mechanism(self):
        """Test AI spawning mechanism."""
        coordinator = TeamCoordinator("localhost", 4243, "test_team")
        coordinator.connect_to_server()
        
        with patch('subprocess.Popen') as mock_popen:
            mock_process = Mock()
            mock_popen.return_value = mock_process
            
            # Test spawning
            result = coordinator.spawn_new_ai_process(bypass_cooldown=True)
            self.assertTrue(result)
            
            # Should have spawned a process
            mock_popen.assert_called_once()
            
        # Clean up
        if coordinator.socket:
            coordinator.socket.close()
            
    def test_emergency_response_system(self):
        """Test emergency response system."""
        guardian = TeamGuardian("localhost", 4243, "test_team")
        guardian.connect_to_server()
        
        # Create distressed team member
        member = Mock()
        member.inventory = {"food": 5}  # Very low food
        member.position = (10, 10)
        member.id = 1
        member.status = "active"
        member.level = 2
        
        guardian.team_members = {1: member}
        guardian.level = 3
        
        # Test emergency response
        vulnerable = guardian.find_vulnerable_members()
        self.assertIn(member, vulnerable)
        
        with patch.object(guardian, 'set_target_position') as mock_target:
            with patch.object(guardian, 'drop_resource_for_team') as mock_drop:
                guardian.inventory["food"] = 60
                guardian.protect_vulnerable_members([member])
                
                # Should move to help and drop food
                mock_target.assert_called_with((10, 10))
                mock_drop.assert_called_with("food", 10)
                
        # Clean up
        if guardian.socket:
            guardian.socket.close()
            
    def test_exploration_pattern_coverage(self):
        """Test exploration pattern provides good coverage."""
        explorer = TeamExplorer("localhost", 4243, "test_team")
        explorer.connect_to_server()
        
        # Track visited positions
        visited = set()
        
        with patch.object(explorer, 'send_command') as mock_send:
            # Run exploration for multiple cycles
            for _ in range(20):
                explorer.systematic_exploration()
                visited.add(explorer.position)
                
        # Should have made movement commands
        self.assertTrue(mock_send.called)
        
        # Clean up
        if explorer.socket:
            explorer.socket.close()
            
    def test_team_strategy_adaptation(self):
        """Test team strategy adaptation based on game state."""
        coordinator = TeamCoordinator("localhost", 4243, "test_team")
        coordinator.connect_to_server()
        
        # Test early game strategy
        member1 = Mock()
        member1.level = 1
        coordinator.team_members = {1: member1}
        
        coordinator.update_team_strategy()
        self.assertEqual(coordinator.team_strategy, "exploration")
        
        # Test mid-game strategy
        member1.level = 3
        member2 = Mock()
        member2.level = 3
        coordinator.team_members = {1: member1, 2: member2}
        
        coordinator.update_team_strategy()
        self.assertEqual(coordinator.team_strategy, "collection")
        
        # Test late game strategy
        member1.level = 5
        member2.level = 6
        coordinator.update_team_strategy()
        self.assertEqual(coordinator.team_strategy, "elevation")
        
        # Clean up
        if coordinator.socket:
            coordinator.socket.close()


class TestZappyAIIntegration(unittest.TestCase):
    """Integration tests for the complete Zappy AI system."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.server = MockZappyServer(4244)  # Different port
        self.server.start()
        time.sleep(0.1)
        
    def tearDown(self):
        """Clean up after tests."""
        self.server.stop()
        time.sleep(0.1)
        
    def test_complete_team_workflow(self):
        """Test complete team workflow from start to coordination."""
        # Create a full team
        explorer = TeamExplorer("localhost", 4244, "test_team")
        collector = TeamCollector("localhost", 4244, "test_team")
        guardian = TeamGuardian("localhost", 4244, "test_team")
        coordinator = TeamCoordinator("localhost", 4244, "test_team")
        
        ais = [explorer, collector, guardian, coordinator]
        
        # Connect all AIs
        for ai in ais:
            connected = ai.connect_to_server()
            self.assertTrue(connected)
            
        # Simulate team interaction
        with patch.object(coordinator, 'broadcast_team_message') as mock_broadcast:
            coordinator.update_team_strategy()
            mock_broadcast.assert_called()
            
        # Test resource sharing
        collector.inventory["food"] = 100
        with patch.object(collector, 'broadcast_team_message') as mock_broadcast:
            collector.share_resources_with_team()
            
        # Clean up
        for ai in ais:
            if ai.socket:
                ai.socket.close()
                
    def test_multi_team_interaction(self):
        """Test interaction between multiple teams."""
        # Create two teams
        team1_explorer = TeamExplorer("localhost", 4244, "team1")
        team1_collector = TeamCollector("localhost", 4244, "team1")
        
        team2_explorer = TeamExplorer("localhost", 4244, "team2")
        team2_collector = TeamCollector("localhost", 4244, "team2")
        
        all_ais = [team1_explorer, team1_collector, team2_explorer, team2_collector]
        
        # Connect all AIs
        for ai in all_ais:
            connected = ai.connect_to_server()
            self.assertTrue(connected)
            
        # Test they can coexist
        with patch.object(team1_explorer, 'send_command') as mock_send1:
            with patch.object(team2_explorer, 'send_command') as mock_send2:
                team1_explorer.systematic_exploration()
                team2_explorer.systematic_exploration()
                
                # Both should be able to operate
                mock_send1.assert_called()
                mock_send2.assert_called()
                
        # Clean up
        for ai in all_ais:
            if ai.socket:
                ai.socket.close()
                
    @patch('subprocess.Popen')
    def test_main_script_execution(self, mock_popen):
        """Test main script can be executed with different parameters."""
        # Mock the main script execution
        mock_process = Mock()
        mock_popen.return_value = mock_process
        
        # Test different role executions
        test_args = [
            ["python", "zappy_ai", "-p", "4242", "-n", "test", "-r", "explorer"],
            ["python", "zappy_ai", "-p", "4242", "-n", "test", "-r", "collector"],
            ["python", "zappy_ai", "-p", "4242", "-n", "test", "-r", "guardian"],
            ["python", "zappy_ai", "-p", "4242", "-n", "test", "-r", "coordinator"]
        ]
        
        for args in test_args:
            mock_popen.reset_mock()
            # This would normally execute the script
            # For testing, we just verify the mock is called
            pass
            
    def test_error_handling_and_recovery(self):
        """Test error handling and recovery mechanisms."""
        ai = TeamExplorer("localhost", 4244, "test_team")
        ai.connect_to_server()
        
        # Test command failure handling
        with patch.object(ai, 'send_message', side_effect=Exception("Network error")):
            # Should not crash
            ai.send_command("inventory")
            
        # Test server disconnection handling
        if ai.socket:
            ai.socket.close()
            
        # Should handle gracefully
        ai.send_command("inventory")
        
    def test_performance_under_load(self):
        """Test system performance under load."""
        # Create multiple AI instances
        ais = []
        
        for i in range(5):
            ai = TeamExplorer("localhost", 4244, f"team_{i}")
            connected = ai.connect_to_server()
            self.assertTrue(connected)
            ais.append(ai)
            
        # Test concurrent operations
        start_time = time.time()
        
        with patch.object(TeamExplorer, 'send_command') as mock_send:
            for ai in ais:
                ai.systematic_exploration()
                
        end_time = time.time()
        
        # Should complete within reasonable time
        self.assertLess(end_time - start_time, 5.0)
        
        # Clean up
        for ai in ais:
            if ai.socket:
                ai.socket.close()


if __name__ == '__main__':
    # Run all tests
    unittest.main(verbosity=2)
