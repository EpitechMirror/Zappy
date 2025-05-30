#!/usr/bin/env python3

import unittest
from unittest.mock import Mock, patch, MagicMock
import socket
import sys
import os

# Add the parent directory of the tests folder to the path to import the main module
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '../src')))

# Import the classes we want to test (assuming the main file is named ai.py)
from ai import ZappyAI, Direction, RessourceType, Position, GameState

class TestDirection(unittest.TestCase):
    """Test the Direction enum"""
    
    def test_direction_values(self):
        """Test that direction enum has correct values"""
        self.assertEqual(Direction.NORTH.value, 1)
        self.assertEqual(Direction.EAST.value, 2)
        self.assertEqual(Direction.SOUTH.value, 3)
        self.assertEqual(Direction.WEST.value, 4)

class TestRessourceType(unittest.TestCase):
    """Test the RessourceType enum"""
    
    def test_resource_values(self):
        """Test that resource types have correct string values"""
        self.assertEqual(RessourceType.FOOD.value, "food")
        self.assertEqual(RessourceType.LINEMATE.value, "linemate")
        self.assertEqual(RessourceType.DERAUMERE.value, "deraumere")
        self.assertEqual(RessourceType.SIBUR.value, "sibur")
        self.assertEqual(RessourceType.MENDIANE.value, "mendiane")
        self.assertEqual(RessourceType.PHIRAS.value, "phiras")
        self.assertEqual(RessourceType.THYSTAME.value, "thystame")

class TestPosition(unittest.TestCase):
    """Test the Position dataclass"""
    
    def test_position_creation(self):
        """Test position creation"""
        pos = Position(5, 10)
        self.assertEqual(pos.x, 5)
        self.assertEqual(pos.y, 10)

class TestGameState(unittest.TestCase):
    """Test the GameState dataclass"""
    
    def test_default_game_state(self):
        """Test default game state initialization"""
        state = GameState()
        self.assertEqual(state.level, 1)
        self.assertEqual(state.direction, Direction.NORTH)
        self.assertEqual(state.world_width, 0)
        self.assertEqual(state.world_height, 0)
        self.assertEqual(state.team_slots, 0)
        self.assertEqual(state.food_units, 10)
        
        # Test default inventory
        expected_inventory = {
            "food": 10,
            "linemate": 0,
            "deraumere": 0,
            "sibur": 0,
            "mendiane": 0,
            "phiras": 0,
            "thystame": 0
        }
        self.assertEqual(state.inventory, expected_inventory)
    
    def test_custom_game_state(self):
        """Test custom game state initialization"""
        custom_inventory = {"food": 20, "linemate": 5}
        state = GameState(
            level=3,
            world_width=10,
            world_height=15,
            inventory=custom_inventory
        )
        self.assertEqual(state.level, 3)
        self.assertEqual(state.world_width, 10)
        self.assertEqual(state.world_height, 15)
        self.assertEqual(state.inventory, custom_inventory)

class TestZappyAI(unittest.TestCase):
    """Test the main ZappyAI class"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.ai = ZappyAI(port=4242, team_name="test_team", hostname="localhost")
    
    def test_ai_initialization(self):
        """Test AI initialization"""
        self.assertEqual(self.ai.port, 4242)
        self.assertEqual(self.ai.team_name, "test_team")
        self.assertEqual(self.ai.hostname, "localhost")
        self.assertIsNone(self.ai.socket)
        self.assertIsInstance(self.ai.state, GameState)
        self.assertEqual(self.ai.command_queue, [])
        self.assertEqual(self.ai.current_goal, "explore")
    
    def test_elevation_requirements(self):
        """Test that elevation requirements are correctly defined"""
        # Test level 1 -> 2 requirements
        req_1 = ZappyAI.ELEVATION_REQUIREMENTS[1]
        self.assertEqual(req_1["players"], 1)
        self.assertEqual(req_1["linemate"], 1)
        self.assertEqual(req_1["deraumere"], 0)
        
        # Test level 7 -> 8 requirements (highest level)
        req_7 = ZappyAI.ELEVATION_REQUIREMENTS[7]
        self.assertEqual(req_7["players"], 6)
        self.assertEqual(req_7["linemate"], 2)
        self.assertEqual(req_7["deraumere"], 2)
        self.assertEqual(req_7["sibur"], 2)
        self.assertEqual(req_7["mendiane"], 2)
        self.assertEqual(req_7["phiras"], 2)
        self.assertEqual(req_7["thystame"], 1)
    
    @patch('socket.socket')
    def test_connect_success(self, mock_socket_class):
        """Test successful connection to server"""
        # Create mock socket instance
        mock_socket = Mock()
        mock_socket_class.return_value = mock_socket
        
        # Mock the receive messages
        mock_socket.recv.side_effect = [
            b"WELCOME\n",
            b"5\n",  # client slots
            b"10 15\n"  # world dimensions
        ]
        
        result = self.ai.connect()
        
        self.assertTrue(result)
        self.assertEqual(self.ai.state.team_slots, 5)
        self.assertEqual(self.ai.state.world_width, 10)
        self.assertEqual(self.ai.state.world_height, 15)
        
        # Verify socket operations
        mock_socket.connect.assert_called_once_with(("localhost", 4242))
        mock_socket.send.assert_called_once_with(b"test_team\n")
    
    @patch('socket.socket')
    def test_connect_failure(self, mock_socket_class):
        """Test connection failure"""
        mock_socket = Mock()
        mock_socket_class.return_value = mock_socket
        mock_socket.connect.side_effect = Exception("Connection failed")
        
        result = self.ai.connect()
        self.assertFalse(result)
    
    def test_send_command(self):
        """Test command sending and queue management"""
        self.ai.socket = Mock()
        
        self.ai.send_command("Forward")
        self.ai.send_command("Look")
        
        self.assertEqual(len(self.ai.command_queue), 2)
        self.assertIn("Forward", self.ai.command_queue)
        self.assertIn("Look", self.ai.command_queue)
        
        # Test queue limit (max 10 commands)
        for i in range(15):
            self.ai.send_command(f"Command{i}")
        
        self.assertEqual(len(self.ai.command_queue), 10)
        self.assertNotIn("Forward", self.ai.command_queue)
        self.assertNotIn("Look", self.ai.command_queue)
    
    def test_turn_right(self):
        """Test turning right updates direction correctly"""
        self.ai.socket = Mock()
        
        # Start facing North
        self.assertEqual(self.ai.state.direction, Direction.NORTH)
        
        self.ai.turn_right()
        self.assertEqual(self.ai.state.direction, Direction.EAST)
        
        self.ai.turn_right()
        self.assertEqual(self.ai.state.direction, Direction.SOUTH)
        
        self.ai.turn_right()
        self.assertEqual(self.ai.state.direction, Direction.WEST)
        
        self.ai.turn_right()
        self.assertEqual(self.ai.state.direction, Direction.NORTH)
    
    def test_turn_left(self):
        """Test turning left updates direction correctly"""
        self.ai.socket = Mock()
        
        # Start facing North
        self.assertEqual(self.ai.state.direction, Direction.NORTH)
        
        self.ai.turn_left()
        self.assertEqual(self.ai.state.direction, Direction.WEST)
        
        self.ai.turn_left()
        self.assertEqual(self.ai.state.direction, Direction.SOUTH)
        
        self.ai.turn_left()
        self.assertEqual(self.ai.state.direction, Direction.EAST)
        
        self.ai.turn_left()
        self.assertEqual(self.ai.state.direction, Direction.NORTH)
    
    def test_parse_look_result_simple(self):
        """Test parsing simple look result"""
        result = "[player,food,linemate,,]"
        tiles = self.ai.parse_look_result(result)
        
        expected = [["player"], ["food"], ["linemate"], [], []]
        self.assertEqual(tiles, expected)
    
    def test_parse_look_result_complex(self):
        """Test parsing complex look result with multiple objects"""
        result = "[player, player deraumere,,thystame food,]"
        tiles = self.ai.parse_look_result(result)
        
        expected = [["player"], ["player", "deraumere"], [], ["thystame", "food"], []]
        self.assertEqual(tiles, expected)
    
    def test_parse_look_result_empty(self):
        """Test parsing empty look result"""
        result = "[,,,]"
        tiles = self.ai.parse_look_result(result)
        
        expected = [[], [], [], []]
        self.assertEqual(tiles, expected)
    
    def test_parse_inventory(self):
        """Test parsing inventory response"""
        inv_str = "[food 345, linemate 2, sibur 3, deraumere 0]"
        self.ai.parse_inventory(inv_str)
        
        self.assertEqual(self.ai.state.inventory["food"], 345)
        self.assertEqual(self.ai.state.inventory["linemate"], 2)
        self.assertEqual(self.ai.state.inventory["sibur"], 3)
        self.assertEqual(self.ai.state.inventory["deraumere"], 0)
    
    def test_calculate_needed_resources_level_1(self):
        """Test calculating needed resources for level 1"""
        self.ai.state.level = 1
        self.ai.state.inventory = {
            "food": 10,
            "linemate": 0,
            "deraumere": 0,
            "sibur": 0,
            "mendiane": 0,
            "phiras": 0,
            "thystame": 0
        }
        
        needed = self.ai.calculate_needed_resources()
        expected = {"linemate": 1}
        self.assertEqual(needed, expected)
    
    def test_calculate_needed_resources_level_2(self):
        """Test calculating needed resources for level 2"""
        self.ai.state.level = 2
        self.ai.state.inventory = {
            "food": 10,
            "linemate": 1,
            "deraumere": 0,
            "sibur": 1,
            "mendiane": 0,
            "phiras": 0,
            "thystame": 0
        }
        
        needed = self.ai.calculate_needed_resources()
        expected = {"deraumere": 1}
        self.assertEqual(needed, expected)
    
    def test_calculate_needed_resources_max_level(self):
        """Test calculating needed resources at max level"""
        self.ai.state.level = 8  # Beyond max level
        needed = self.ai.calculate_needed_resources()
        self.assertEqual(needed, {})
    
    def test_find_resources_in_vision(self):
        """Test finding resources in vision tiles"""
        tiles = [
            ["player"],
            ["food", "linemate"],
            [],
            ["sibur"],
            ["player", "deraumere"]
        ]
        
        resources = self.ai.find_resources_in_vision(tiles)
        expected = [(1, "food"), (1, "linemate"), (3, "sibur"), (4, "deraumere")]
        self.assertEqual(resources, expected)
    
    def test_should_reproduce_true(self):
        """Test reproduction decision when conditions are met"""
        self.ai.state.inventory["food"] = 60
        self.ai.state.team_slots = 2
        self.ai.state.level = 3
        self.ai.reproduction_cooldown = 0
        
        self.assertTrue(self.ai.should_reproduce())
    
    def test_should_reproduce_false_low_food(self):
        """Test reproduction decision with low food"""
        self.ai.state.inventory["food"] = 30
        self.ai.state.team_slots = 2
        self.ai.state.level = 3
        self.ai.reproduction_cooldown = 0
        
        self.assertFalse(self.ai.should_reproduce())
    
    def test_should_reproduce_false_no_slots(self):
        """Test reproduction decision with no team slots"""
        self.ai.state.inventory["food"] = 60
        self.ai.state.team_slots = 0
        self.ai.state.level = 3
        self.ai.reproduction_cooldown = 0
        
        self.assertFalse(self.ai.should_reproduce())
    
    def test_should_reproduce_false_low_level(self):
        """Test reproduction decision at low level"""
        self.ai.state.inventory["food"] = 60
        self.ai.state.team_slots = 2
        self.ai.state.level = 1
        self.ai.reproduction_cooldown = 0
        
        self.assertFalse(self.ai.should_reproduce())
    
    def test_should_reproduce_false_cooldown(self):
        """Test reproduction decision during cooldown"""
        self.ai.state.inventory["food"] = 60
        self.ai.state.team_slots = 2
        self.ai.state.level = 3
        self.ai.reproduction_cooldown = 50
        
        self.assertFalse(self.ai.should_reproduce())
    
    def test_decide_next_action_find_food(self):
        """Test goal decision when food is low"""
        self.ai.state.inventory["food"] = 15
        self.ai.decide_next_action()
        self.assertEqual(self.ai.current_goal, "find_food")
    
    def test_decide_next_action_reproduce(self):
        """Test goal decision for reproduction"""
        self.ai.state.inventory["food"] = 60
        self.ai.state.team_slots = 2
        self.ai.state.level = 3
        self.ai.reproduction_cooldown = 0
        
        self.ai.decide_next_action()
        self.assertEqual(self.ai.current_goal, "reproduce")
    
    def test_decide_next_action_gather(self):
        """Test goal decision for gathering resources"""
        self.ai.state.inventory["food"] = 50
        self.ai.state.level = 1
        self.ai.state.inventory["linemate"] = 0
        
        self.ai.decide_next_action()
        self.assertEqual(self.ai.current_goal, "gather")
        self.assertEqual(self.ai.target_resources, {"linemate": 1})
    
    def test_decide_next_action_elevate(self):
        """Test goal decision for elevation"""
        self.ai.state.inventory["food"] = 50
        self.ai.state.level = 1
        self.ai.state.inventory["linemate"] = 1  # Has required resources
        
        self.ai.decide_next_action()
        self.assertEqual(self.ai.current_goal, "elevate")
    
    def test_process_response_look_result(self):
        """Test processing look command response"""
        response = "[player,food,linemate,,]"
        result = self.ai.process_response(response)
        
        self.assertTrue(result)
        expected_tiles = [["player"], ["food"], ["linemate"], [], []]
        self.assertEqual(self.ai.last_look_result, expected_tiles)
    
    def test_process_response_broadcast(self):
        """Test processing broadcast message"""
        response = "message 3, ELEVATE_L2"
        self.ai.state.level = 2
        self.ai.current_goal = "elevate"
        
        result = self.ai.process_response(response)
        self.assertTrue(result)
    
    def test_process_response_ok(self):
        """Test processing OK response"""
        self.ai.command_queue = ["Forward", "Look"]
        response = "ok"
        
        result = self.ai.process_response(response)
        self.assertTrue(result)
        self.assertEqual(len(self.ai.command_queue), 1)
        self.assertEqual(self.ai.command_queue[0], "Look")
    
    def test_process_response_ko(self):
        """Test processing KO response"""
        self.ai.command_queue = ["InvalidCommand"]
        response = "ko"
        
        with patch('builtins.print') as mock_print:
            result = self.ai.process_response(response)
            self.assertTrue(result)
            mock_print.assert_called_with("Command failed: InvalidCommand")
    
    def test_process_response_dead(self):
        """Test processing death response"""
        response = "dead"
        
        with patch('builtins.print') as mock_print:
            result = self.ai.process_response(response)
            self.assertFalse(result)
            mock_print.assert_called_with("Player died!")
    
    def test_process_response_elevation_underway(self):
        """Test processing elevation start response"""
        response = "Elevation underway"
        
        with patch('builtins.print') as mock_print:
            result = self.ai.process_response(response)
            self.assertTrue(result)
            mock_print.assert_called_with("Elevation started!")
    
    def test_process_response_current_level(self):
        """Test processing level update response"""
        response = "Current level: 3"
        
        with patch('builtins.print') as mock_print:
            result = self.ai.process_response(response)
            self.assertTrue(result)
            self.assertEqual(self.ai.state.level, 3)
            mock_print.assert_called_with("New level: 3")
    
    def test_handle_broadcast_elevation(self):
        """Test handling elevation broadcast"""
        self.ai.state.level = 2
        self.ai.current_goal = "elevate"
        
        with patch('builtins.print') as mock_print:
            self.ai.handle_broadcast(3, "ELEVATE_L2")
            mock_print.assert_called_with("Received elevation call for level 2")
    
    @patch('builtins.print')
    def test_handle_broadcast_wrong_level(self, mock_print):
        """Test handling elevation broadcast for wrong level"""
        self.ai.state.level = 1
        self.ai.current_goal = "elevate"
        
        self.ai.handle_broadcast(3, "ELEVATE_L2")
        mock_print.assert_not_called()

class TestIntegration(unittest.TestCase):
    """Integration tests for the AI"""
    
    def setUp(self):
        """Set up integration test fixtures"""
        self.ai = ZappyAI(port=4242, team_name="test_team")
    
    @patch('socket.socket')
    def test_full_connection_flow(self, mock_socket_class):
        """Test the complete connection and initial setup flow"""
        mock_socket = Mock()
        mock_socket_class.return_value = mock_socket
        
        # Mock the complete handshake
        mock_socket.recv.side_effect = [
            b"WELCOME\n",
            b"3\n",
            b"20 25\n"
        ]
        
        # Connect
        result = self.ai.connect()
        self.assertTrue(result)
        
        # Verify state is correctly set
        self.assertEqual(self.ai.state.team_slots, 3)
        self.assertEqual(self.ai.state.world_width, 20)
        self.assertEqual(self.ai.state.world_height, 25)
        
        # Verify correct messages were sent
        mock_socket.send.assert_called_once_with(b"test_team\n")
    
    def test_goal_transition_sequence(self):
        """Test goal transitions based on game state"""
        # Start with low food
        self.ai.state.inventory["food"] = 15
        self.ai.decide_next_action()
        self.assertEqual(self.ai.current_goal, "find_food")
        
        # Get enough food, but need resources
        self.ai.state.inventory["food"] = 50
        self.ai.state.level = 1
        self.ai.state.inventory["linemate"] = 0
        self.ai.decide_next_action()
        self.assertEqual(self.ai.current_goal, "gather")
        
        # Get required resources
        self.ai.state.inventory["linemate"] = 1
        self.ai.decide_next_action()
        self.assertEqual(self.ai.current_goal, "elevate")
    
    def test_command_queue_overflow_protection(self):
        """Test that command queue doesn't exceed 10 commands"""
        self.ai.socket = Mock()
        
        # Send 15 commands
        for i in range(15):
            self.ai.send_command(f"Command{i}")
        
        # Should only have the last 10
        self.assertEqual(len(self.ai.command_queue), 10)
        self.assertEqual(self.ai.command_queue[0], "Command5")
        self.assertEqual(self.ai.command_queue[-1], "Command14")

def run_tests():
    """Run all tests"""
    # Create test suite
    test_suite = unittest.TestSuite()
    
    # Add all test classes
    test_classes = [
        TestDirection,
        TestRessourceType,
        TestPosition,
        TestGameState,
        TestZappyAI,
        TestIntegration
    ]
    
    for test_class in test_classes:
        tests = unittest.TestLoader().loadTestsFromTestCase(test_class)
        test_suite.addTests(tests)
    
    # Run tests
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(test_suite)
    
    return result.wasSuccessful()

if __name__ == "__main__":
    success = run_tests()
    sys.exit(0 if success else 1)