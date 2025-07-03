#!/usr/bin/env python3
##
## test_team_base.py for AI New in /home/adrien/Desktop/AI New
##
## Made by adrien.marette@epitech.eu
## Login   <adrien.marette@epitech.eu>
##
## Started on  Sat Jul 4 01:00:00 AM 2025 adrien.marette@epitech.eu
## Last update Sat Jul 4 01:00:00 AM 2025 adrien.marette@epitech.eu
##

import unittest
import time
import json
from unittest.mock import Mock, patch, MagicMock
import socket

from Team import Team, TeamRole, TeamMember, elevation_requirements

class TestTeamBase(unittest.TestCase):
    """Unit tests for the base Team class."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.team = Team("localhost", 4242, "test_team", TeamRole.EXPLORER)
        
    def test_initialization(self):
        """Test Team initialization."""
        self.assertEqual(self.team.host, "localhost")
        self.assertEqual(self.team.port, 4242)
        self.assertEqual(self.team.team_name, "test_team")
        self.assertEqual(self.team.role, TeamRole.EXPLORER)
        self.assertFalse(self.team.is_spawned)
        self.assertEqual(self.team.level, 1)
        self.assertEqual(self.team.position, (0, 0))
        self.assertEqual(self.team.inventory["food"], 10)
        
    def test_spawned_initialization(self):
        """Test Team initialization with spawned flag."""
        spawned_team = Team("localhost", 4242, "test_team", TeamRole.COLLECTOR, True)
        self.assertTrue(spawned_team.is_spawned)
        
    def test_inventory_initialization(self):
        """Test inventory is properly initialized."""
        expected_inventory = {
            "food": 10, "linemate": 0, "deraumere": 0, "sibur": 0,
            "mendiane": 0, "phiras": 0, "thystame": 0
        }
        self.assertEqual(self.team.inventory, expected_inventory)
        
    def test_team_members_dict(self):
        """Test team members dictionary initialization."""
        self.assertIsInstance(self.team.team_members, dict)
        self.assertEqual(len(self.team.team_members), 0)
        
    @patch('socket.socket')
    def test_connect_to_server_success(self, mock_socket):
        """Test successful server connection."""
        mock_sock = Mock()
        mock_socket.return_value = mock_sock
        mock_sock.recv.side_effect = [b"WELCOME\n", b"3\n10 10\n"]
        
        result = self.team.connect_to_server()
        self.assertTrue(result)
        self.assertEqual(self.team.socket, mock_sock)
        
    @patch('socket.socket')
    def test_connect_to_server_failure(self, mock_socket):
        """Test failed server connection."""
        mock_socket.side_effect = ConnectionRefusedError()
        
        result = self.team.connect_to_server()
        self.assertFalse(result)
        self.assertIsNone(self.team.socket)
        
    def test_send_message_no_socket(self):
        """Test sending message without socket connection."""
        # Should not raise exception
        self.team.send_message("test")
        
    def test_receive_message_no_socket(self):
        """Test receiving message without socket connection."""
        result = self.team.receive_message()
        self.assertEqual(result, "")
        
    @patch('time.time')
    def test_command_delay(self, mock_time):
        """Test command delay mechanism."""
        mock_time.return_value = 1000.0
        self.team.last_command_time = 999.0
        self.team.command_delay = 0.5
        
        with patch.object(self.team, 'send_message') as mock_send:
            with patch.object(self.team, 'wait_for_response') as mock_wait:
                self.team.send_command("test")
                mock_send.assert_called_once_with("test")
                mock_wait.assert_called_once()
                
    def test_calculate_distance_simple(self):
        """Test distance calculation without world wrapping."""
        distance = self.team.calculate_distance((0, 0), (3, 4))
        self.assertEqual(distance, 5.0)  # 3-4-5 triangle
        
    def test_calculate_distance_with_wrapping(self):
        """Test distance calculation with world wrapping."""
        self.team.world_size = (10, 10)
        # Test wrapping on x-axis
        distance = self.team.calculate_distance((1, 0), (9, 0))
        # Should wrap: min(8, 2) = 2
        self.assertEqual(distance, 2.0)
        
    def test_calculate_travel_time(self):
        """Test travel time calculation."""
        travel_time = self.team.calculate_travel_time((3, 4))
        expected = int(5.0 * 7)  # distance * 7
        self.assertEqual(travel_time, expected)
        
    def test_set_target_position(self):
        """Test setting target position."""
        target = (10, 20)
        self.team.set_target_position(target)
        self.assertEqual(self.team.target_position, target)
        
    def test_broadcast_team_message(self):
        """Test broadcasting team messages."""
        message_data = {"type": "test", "id": "player1"}
        
        # Should not raise exception even without socket
        self.team.broadcast_team_message(message_data)
        
    def test_process_team_message_invalid(self):
        """Test processing invalid team messages."""
        # Should not raise exception
        self.team.process_team_message("invalid message", 1)
        self.team.process_team_message("TEAM:invalid_json", 1)
        
    def test_process_team_message_valid(self):
        """Test processing valid team messages."""
        message_data = {"type": "test", "id": "player1"}
        message = f"TEAM:{json.dumps(message_data)}"
        
        # Should not raise exception
        self.team.process_team_message(message, 1)
        
    def test_get_resource_needed_for_next_elevation(self):
        """Test getting required resources for elevation."""
        self.team.level = 1
        needed = self.team.get_resource_needed_for_next_elevation("linemate")
        self.assertEqual(needed, 1)  # Level 1 requires 1 linemate
        
        needed = self.team.get_resource_needed_for_next_elevation("deraumere")
        self.assertEqual(needed, 0)  # Level 1 requires 0 deraumere
        
    def test_can_help_with_elevation_low_food(self):
        """Test elevation help check with low food."""
        self.team.inventory["food"] = 30
        result = self.team.can_help_with_elevation()
        self.assertFalse(result)
        
    def test_can_help_with_elevation_sufficient_resources(self):
        """Test elevation help check with sufficient resources."""
        self.team.inventory = {
            "food": 60, "linemate": 2, "deraumere": 1, "sibur": 1,
            "mendiane": 1, "phiras": 1, "thystame": 1
        }
        result = self.team.can_help_with_elevation()
        self.assertTrue(result)
        
    def test_select_balanced_role_early_game(self):
        """Test role selection in early game."""
        # Empty spawned roles list
        self.team.spawned_roles = []
        role = self.team.select_balanced_role()
        self.assertIn(role, ["explorer", "collector"])
        
    def test_select_balanced_role_mid_game(self):
        """Test role selection in mid game."""
        self.team.spawned_roles = ["explorer", "collector", "explorer"]
        role = self.team.select_balanced_role()
        # Should prefer guardian or coordinator, but collector is also valid in mid-game
        self.assertIn(role, ["guardian", "coordinator", "collector"])
        
    def test_spawn_cooldown(self):
        """Test spawn cooldown mechanism."""
        self.team.last_spawn_time = time.time()
        result = self.team.spawn_new_ai_process()
        self.assertFalse(result)  # Should fail due to cooldown
        
    def test_spawn_bypass_cooldown(self):
        """Test spawning with cooldown bypass."""
        with patch('subprocess.Popen') as mock_popen:
            mock_process = Mock()
            mock_popen.return_value = mock_process
            
            result = self.team.spawn_new_ai_process(bypass_cooldown=True)
            self.assertTrue(result)
            mock_popen.assert_called_once()
            
    def test_cleanup_spawned_processes(self):
        """Test cleanup of spawned processes."""
        mock_process = Mock()
        mock_process.poll.return_value = None  # Still running
        self.team.spawned_processes = [mock_process]
        
        self.team.cleanup_spawned_processes()
        # Process should still be in list if running
        self.assertIn(mock_process, self.team.spawned_processes)
        
        # Test completed process cleanup
        mock_process.poll.return_value = 0  # Completed
        mock_process.communicate.return_value = (b"", b"")  # Mock stdout, stderr
        self.team.cleanup_spawned_processes()
        self.assertNotIn(mock_process, self.team.spawned_processes)


class TestTeamMember(unittest.TestCase):
    """Unit tests for TeamMember dataclass."""
    
    def test_team_member_creation(self):
        """Test TeamMember creation."""
        inventory = {"food": 50, "linemate": 2}
        member = TeamMember(
            id=1, level=2, position=(5, 10), role=TeamRole.COLLECTOR,
            inventory=inventory, status="active", last_update=time.time()
        )
        
        self.assertEqual(member.id, 1)
        self.assertEqual(member.level, 2)
        self.assertEqual(member.position, (5, 10))
        self.assertEqual(member.role, TeamRole.COLLECTOR)
        self.assertEqual(member.inventory, inventory)
        self.assertEqual(member.status, "active")


class TestElevationRequirements(unittest.TestCase):
    """Unit tests for elevation requirements."""
    
    def test_all_levels_present(self):
        """Test all elevation levels are defined."""
        for level in range(1, 8):
            self.assertIn(level, elevation_requirements)
            
    def test_level_1_requirements(self):
        """Test level 1 elevation requirements."""
        req = elevation_requirements[1]
        self.assertEqual(req["players"], 1)
        self.assertEqual(req["linemate"], 1)
        self.assertEqual(req["deraumere"], 0)
        
    def test_level_7_requirements(self):
        """Test level 7 (max) elevation requirements."""
        req = elevation_requirements[7]
        self.assertEqual(req["players"], 6)
        self.assertEqual(req["linemate"], 2)
        self.assertEqual(req["thystame"], 1)
        
    def test_all_resources_present(self):
        """Test all resources are present in requirements."""
        required_resources = [
            "players", "linemate", "deraumere", "sibur",
            "mendiane", "phiras", "thystame"
        ]
        
        for level, req in elevation_requirements.items():
            for resource in required_resources:
                self.assertIn(resource, req)


if __name__ == '__main__':
    unittest.main()
