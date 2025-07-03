#!/usr/bin/env python3
##
## test_team_collector.py for AI New in /home/adrien/Desktop/AI New
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

from TeamCollector import TeamCollector
from Team import TeamRole, elevation_requirements

class TestTeamCollector(unittest.TestCase):
    """Unit tests for the TeamCollector class."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.collector = TeamCollector("localhost", 4242, "test_team")
        
    def test_initialization(self):
        """Test TeamCollector initialization."""
        self.assertEqual(self.collector.role, TeamRole.COLLECTOR)
        self.assertEqual(self.collector.collection_priorities, 
                        ["food", "linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame"])
        self.assertEqual(self.collector.resource_targets, [])
        self.assertEqual(self.collector.sharing_cooldown, 0)
        
    def test_spawned_initialization(self):
        """Test TeamCollector initialization with spawned flag."""
        spawned_collector = TeamCollector("localhost", 4242, "test_team", True)
        self.assertTrue(spawned_collector.is_spawned)
        
    @patch.object(TeamCollector, 'send_command')
    @patch.object(TeamCollector, 'prioritize_food_collection')
    def test_execute_role_strategy_low_food(self, mock_food, mock_send):
        """Test role strategy execution with low food."""
        self.collector.inventory["food"] = 20
        
        with patch('random.random', return_value=0.05):  # Trigger inventory check
            self.collector.execute_role_strategy()
            
        mock_send.assert_called_with("Inventory", "high")
        mock_food.assert_called_once()
        
    @patch.object(TeamCollector, 'send_command')
    @patch.object(TeamCollector, 'collect_general_resources')
    @patch.object(TeamCollector, 'get_needed_resources_for_team')
    def test_execute_role_strategy_normal_food(self, mock_needed, mock_collect, mock_send):
        """Test role strategy execution with normal food levels."""
        self.collector.inventory["food"] = 50
        mock_needed.return_value = []
        
        with patch('random.random', return_value=0.2):  # Skip inventory check
            with patch('time.time', return_value=1000):
                self.collector.execute_role_strategy()
                
        mock_collect.assert_called_once()
        
    @patch.object(TeamCollector, 'send_command')
    def test_prioritize_food_collection(self, mock_send):
        """Test food prioritization behavior."""
        with patch('random.random', return_value=0.3):  # Forward movement
            self.collector.prioritize_food_collection()
            
        # Should look and take food
        expected_calls = [
            unittest.mock.call("Look", "high"),
            unittest.mock.call("Take food", "high"),
            unittest.mock.call("Forward", "high")
        ]
        mock_send.assert_has_calls(expected_calls)
        
    def test_get_needed_resources_for_team(self):
        """Test getting needed resources for team."""
        self.collector.level = 2
        self.collector.inventory = {
            "food": 50, "linemate": 0, "deraumere": 0, "sibur": 0,
            "mendiane": 0, "phiras": 0, "thystame": 0
        }
        
        needed = self.collector.get_needed_resources_for_team()
        
        # Should need linemate, deraumere, sibur for level 2
        self.assertIn("linemate", needed)
        self.assertIn("deraumere", needed)
        self.assertIn("sibur", needed)
        
    @patch.object(TeamCollector, 'send_command')
    def test_collect_specific_resources(self, mock_send):
        """Test collecting specific resources."""
        resources = ["linemate", "deraumere"]
        
        with patch('random.random', return_value=0.1):  # Trigger take commands
            self.collector.collect_specific_resources(resources)
            
        # Should look and attempt to take resources
        mock_send.assert_any_call("Look", "normal")
        
    @patch.object(TeamCollector, 'send_command')
    def test_collect_general_resources(self, mock_send):
        """Test general resource collection."""
        with patch('random.random', return_value=0.1):  # Trigger take commands
            self.collector.collect_general_resources()
            
        mock_send.assert_any_call("Look", "normal")
        
    def test_calculate_excess_resources(self):
        """Test excess resource calculation."""
        self.collector.level = 1
        self.collector.inventory = {
            "food": 100, "linemate": 5, "deraumere": 3, "sibur": 1,
            "mendiane": 0, "phiras": 0, "thystame": 0
        }
        
        excess = self.collector.calculate_excess_resources()
        
        # Should have excess food (100 - 80 = 20)
        self.assertEqual(excess["food"], 20)
        # Should have excess linemate (5 - 1 - 2 = 2)
        self.assertEqual(excess["linemate"], 2)
        # Should have no excess deraumere for level 1 (3 > 0 + 2, so 1 excess)
        self.assertEqual(excess["deraumere"], 1)
        
    @patch.object(TeamCollector, 'broadcast_team_message')
    def test_share_resources_with_team(self, mock_broadcast):
        """Test sharing resources with team."""
        self.collector.resource_sharing_enabled = True
        self.collector.inventory["food"] = 100
        self.collector.level = 1
        
        with patch('time.time', return_value=1000):
            self.collector.share_resources_with_team()
            
        # Should broadcast resource offer
        mock_broadcast.assert_called()
        call_args = mock_broadcast.call_args[0][0]
        self.assertEqual(call_args["type"], "resource_offer")
        
    def test_share_resources_disabled(self):
        """Test resource sharing when disabled."""
        self.collector.resource_sharing_enabled = False
        
        with patch.object(self.collector, 'broadcast_team_message') as mock_broadcast:
            self.collector.share_resources_with_team()
            mock_broadcast.assert_not_called()
            
    def test_process_team_message_resource_request(self):
        """Test processing resource request messages."""
        message_data = {
            "type": "resource_request",
            "resource": "linemate",
            "amount": 2,
            "position": [5, 5]  # JSON lists become tuples in handler
        }
        message = f"TEAM:{json.dumps(message_data)}"
        
        with patch.object(self.collector, 'handle_resource_request') as mock_handle:
            self.collector.process_team_message(message, 1)
            # The position will be converted to tuple by handle_resource_request
            expected_data = message_data.copy()
            mock_handle.assert_called_once_with(expected_data, 1)
            
    def test_process_team_message_resource_discovery(self):
        """Test processing resource discovery messages."""
        message_data = {
            "type": "resource_discovery",
            "resource": "deraumere",
            "position": [10, 10]  # JSON lists become tuples in handler
        }
        message = f"TEAM:{json.dumps(message_data)}"
        
        with patch.object(self.collector, 'handle_resource_discovery') as mock_handle:
            self.collector.process_team_message(message, 1)
            expected_data = message_data.copy()
            mock_handle.assert_called_once_with(expected_data, 1)
            
    @patch.object(TeamCollector, 'broadcast_team_message')
    @patch.object(TeamCollector, 'set_target_position')
    def test_handle_resource_request(self, mock_target, mock_broadcast):
        """Test handling resource requests."""
        self.collector.inventory["linemate"] = 5
        self.collector.level = 1  # Needs 1 linemate, so has 2 excess (5-1-2)
        
        data = {
            "resource": "linemate",
            "amount": 1,
            "position": (5, 5)
        }
        
        self.collector.handle_resource_request(data, 1)
        
        # Should set target and broadcast confirmation
        mock_target.assert_called_once_with((5, 5))
        mock_broadcast.assert_called_once()
        
    @patch.object(TeamCollector, 'set_target_position')
    def test_handle_resource_discovery(self, mock_target):
        """Test handling resource discovery."""
        # Set up collector to need linemate
        self.collector.level = 1
        self.collector.inventory["linemate"] = 0
        
        data = {
            "resource": "linemate",
            "position": (8, 8)
        }
        
        with patch.object(self.collector, 'get_needed_resources_for_team', 
                         return_value=["linemate", "deraumere"]):
            self.collector.handle_resource_discovery(data, 1)
            
        mock_target.assert_called_once_with((8, 8))
        
    @patch.object(TeamCollector, 'send_command')
    @patch.object(TeamCollector, 'broadcast_team_message')
    def test_drop_resource_for_team(self, mock_broadcast, mock_send):
        """Test dropping resources for team."""
        self.collector.position = (3, 3)
        
        with patch('time.time', return_value=1000):
            self.collector.drop_resource_for_team("food", 3)
            
        # Should send Set commands
        expected_calls = [
            unittest.mock.call("Set food", "normal"),
            unittest.mock.call("Set food", "normal"),
            unittest.mock.call("Set food", "normal")
        ]
        mock_send.assert_has_calls(expected_calls)
        
        # Should broadcast the drop
        mock_broadcast.assert_called_once()
        call_args = mock_broadcast.call_args[0][0]
        self.assertEqual(call_args["type"], "resource_drop")
        self.assertEqual(call_args["amount"], 3)
        
    @patch.object(TeamCollector, 'send_command')
    def test_optimize_inventory_full(self, mock_send):
        """Test inventory optimization when full."""
        self.collector.inventory = {
            "food": 50, "linemate": 5, "deraumere": 3, "sibur": 8,
            "mendiane": 2, "phiras": 1, "thystame": 0
        }
        
        self.collector.optimize_inventory()
        
        # Should drop excess sibur (8 > 5)
        mock_send.assert_called_with("Set sibur", "normal")
        
    def test_optimize_inventory_not_full(self):
        """Test inventory optimization when not full."""
        self.collector.inventory = {
            "food": 50, "linemate": 2, "deraumere": 1, "sibur": 1,
            "mendiane": 1, "phiras": 1, "thystame": 1
        }
        
        with patch.object(self.collector, 'send_command') as mock_send:
            self.collector.optimize_inventory()
            mock_send.assert_not_called()  # Should not drop anything


class TestTeamCollectorIntegration(unittest.TestCase):
    """Integration tests for TeamCollector with mocked server."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.collector = TeamCollector("localhost", 4242, "test_team")
        
    @patch.object(TeamCollector, 'send_command')
    def test_full_strategy_execution(self, mock_send):
        """Test full strategy execution cycle."""
        # Set up scenario: low food, need resources
        self.collector.inventory["food"] = 15
        self.collector.level = 2
        
        with patch('random.random', return_value=0.05):  # Trigger inventory
            with patch('time.time', return_value=1000):
                self.collector.execute_role_strategy()
                
        # Should prioritize food collection
        mock_send.assert_any_call("Look", "high")
        mock_send.assert_any_call("Take food", "high")
        
    def test_resource_sharing_cycle(self):
        """Test complete resource sharing cycle."""
        # Set up excess resources
        self.collector.inventory = {
            "food": 100, "linemate": 5, "deraumere": 0, "sibur": 0,
            "mendiane": 0, "phiras": 0, "thystame": 0
        }
        self.collector.level = 1
        self.collector.resource_sharing_enabled = True
        
        with patch.object(self.collector, 'broadcast_team_message') as mock_broadcast:
            with patch('time.time', return_value=1000):
                self.collector.share_resources_with_team()
                
        # Should broadcast offers for excess resources
        self.assertGreaterEqual(mock_broadcast.call_count, 1)


if __name__ == '__main__':
    unittest.main()
