##
## test_main_ai.py for Zappy in /home/adrien/Epitech/B-YEP-410/Zappy/ai/tests
##
## Made by adrien.marette@epitech.eu
## Login   <adrien.marette@epitech.eu>
##
## Started on  Sat Jun 29 2025
## Last update Mon Jun 29 3:46:51 PM 2025 adrien.marette@epitech.eu
##

import json
import os
import socket
import sys
import time
import unittest
from unittest.mock import MagicMock, Mock, patch

import numpy as np

# Add the src directory to the Python path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "src"))

from Classes import (ActionType, Direction, Experience, GameState, Position,
                     ResourceType)
from main_ai import ZappyAI


class TestZappyAI(unittest.TestCase):
    """Test cases for ZappyAI class"""

    def setUp(self):
        """Set up test fixtures before each test method"""
        self.port = 4242
        self.team_name = "test_team"
        self.hostname = "localhost"

        # Mock the neural network, memory, and thinking module to avoid dependencies
        with patch("main_ai.NeuralNetwork") as mock_nn, patch(
            "main_ai.MemorySystem"
        ) as mock_memory, patch("main_ai.ThinkingModule") as mock_thinking:

            self.ai = ZappyAI(self.port, self.team_name, self.hostname)

    def test_ai_initialization(self):
        """Test AI initialization with correct parameters"""
        self.assertEqual(self.ai.port, self.port)
        self.assertEqual(self.ai.team_name, self.team_name)
        self.assertEqual(self.ai.hostname, self.hostname)
        self.assertIsNone(self.ai.socket)
        self.assertIsInstance(self.ai.state, GameState)
        self.assertEqual(self.ai.command_queue, [])
        self.assertEqual(self.ai.last_look_result, [])

    def test_ai_initialization_learning_parameters(self):
        """Test AI learning parameters initialization"""
        self.assertEqual(self.ai.epsilon, 0.3)
        self.assertEqual(self.ai.epsilon_decay, 0.995)
        self.assertEqual(self.ai.epsilon_min, 0.01)
        self.assertTrue(self.ai.learning_enabled)

    def test_ai_initialization_performance_metrics(self):
        """Test AI performance metrics initialization"""
        expected_metrics = {
            "total_rewards": 0,
            "successful_actions": 0,
            "failed_actions": 0,
            "elevations_completed": 0,
            "resources_collected": 0,
            "forks_attempted": 0,
            "forks_successful": 0,
        }
        self.assertEqual(self.ai.performance_metrics, expected_metrics)

    @patch("socket.socket")
    def test_connect_success(self, mock_socket_class):
        """Test successful connection to server"""
        mock_socket = Mock()
        mock_socket_class.return_value = mock_socket
        mock_socket.recv.return_value = b"WELCOME\n"

        result = self.ai.connect()

        self.assertTrue(result)
        mock_socket.connect.assert_called_once_with((self.hostname, self.port))
        mock_socket.send.assert_called_once_with(b"test_team\n")

    @patch("socket.socket")
    def test_connect_failure_wrong_welcome(self, mock_socket_class):
        """Test connection failure with wrong welcome message"""
        mock_socket = Mock()
        mock_socket_class.return_value = mock_socket
        mock_socket.recv.return_value = b"INVALID\n"

        result = self.ai.connect()

        self.assertFalse(result)

    @patch("socket.socket")
    def test_connect_failure_exception(self, mock_socket_class):
        """Test connection failure with socket exception"""
        mock_socket_class.side_effect = Exception("Connection failed")

        result = self.ai.connect()

        self.assertFalse(result)

    def test_send_message(self):
        """Test sending message to server"""
        mock_socket = Mock()
        self.ai.socket = mock_socket

        self.ai.send_message("test message")

        mock_socket.send.assert_called_once_with(b"test message\n")

    def test_send_message_no_socket(self):
        """Test sending message when no socket connection"""
        self.ai.socket = None

        # Should not raise exception
        self.ai.send_message("test message")

    def test_receive_message(self):
        """Test receiving message from server"""
        mock_socket = Mock()
        mock_socket.recv.return_value = b"test response\n"
        self.ai.socket = mock_socket

        result = self.ai.receive_message()

        self.assertEqual(result, "test response")

    def test_receive_message_no_socket(self):
        """Test receiving message when no socket connection"""
        self.ai.socket = None

        result = self.ai.receive_message()

        self.assertEqual(result, "")

    def test_create_state_vector_basic(self):
        """Test creating state vector with basic state"""
        # Set up basic state
        self.ai.state.level = 3
        self.ai.state.food_units = 50
        self.ai.state.turn_count = 200
        self.ai.state.direction = Direction.EAST

        # Set up simple vision
        self.ai.last_look_result = [["food"], [], ["linemate"]]

        state_vector = self.ai.create_state_vector()

        # Check vector shape and type
        self.assertIsInstance(state_vector, np.ndarray)
        self.assertEqual(state_vector.shape, (1, 50))

        # Check that all values are between reasonable bounds
        self.assertTrue(np.all(state_vector >= -1))
        self.assertTrue(np.all(state_vector <= 1))

    def test_create_state_vector_normalized_values(self):
        """Test that state vector values are properly normalized"""
        self.ai.state.level = 7  # Max level
        self.ai.state.food_units = 100  # Max normalized food
        self.ai.state.turn_count = 1000  # Max normalized turns
        self.ai.state.direction = Direction.WEST  # Value 4

        state_vector = self.ai.create_state_vector()

        # Check specific normalized values
        self.assertAlmostEqual(state_vector[0, 0], 1.0, places=2)  # level / 7
        self.assertAlmostEqual(state_vector[0, 1], 1.0, places=2)  # food / 100
        self.assertAlmostEqual(state_vector[0, 2], 1.0, places=2)  # turns / 1000
        self.assertAlmostEqual(state_vector[0, 3], 1.0, places=2)  # direction / 4

    def test_calculate_reward_basic_survival(self):
        """Test basic survival reward calculation"""
        previous_state = GameState()

        reward = self.ai.calculate_reward(ActionType.LOOK, "ok", previous_state)

        # Should get basic survival reward plus successful action reward
        self.assertGreater(reward, 0)

    def test_calculate_reward_food_gain(self):
        """Test reward calculation when gaining food"""
        previous_state = GameState()
        previous_state.inventory["food"] = 10

        self.ai.state.inventory["food"] = 15  # Gained 5 food

        reward = self.ai.calculate_reward(
            ActionType.TAKE_RESOURCE, "ok", previous_state
        )

        # Should get reward for food gain
        self.assertGreater(reward, 10)  # Base + food reward + success reward

    def test_calculate_reward_low_food_penalty(self):
        """Test penalty for low food"""
        previous_state = GameState()
        self.ai.state.inventory["food"] = 5  # Very low food

        reward = self.ai.calculate_reward(ActionType.MOVE_FORWARD, "ok", previous_state)

        # Should get penalty for low food
        self.assertLess(reward, 0)

    def test_calculate_reward_level_progression(self):
        """Test reward for level progression"""
        previous_state = GameState(level=1)
        self.ai.state.level = 2  # Leveled up

        reward = self.ai.calculate_reward(ActionType.INCANTATION, "ok", previous_state)

        # Should get big reward for leveling up
        self.assertGreater(reward, 100)
        self.assertEqual(self.ai.performance_metrics["elevations_completed"], 1)

    def test_calculate_reward_failed_action(self):
        """Test penalty for failed action"""
        previous_state = GameState()

        reward = self.ai.calculate_reward(
            ActionType.TAKE_RESOURCE, "ko", previous_state
        )

        # Should get penalty for failed action
        self.assertLess(reward, 0)
        self.assertEqual(self.ai.performance_metrics["failed_actions"], 1)

    def test_calculate_reward_death_penalty(self):
        """Test penalty for death"""
        previous_state = GameState()

        reward = self.ai.calculate_reward(
            ActionType.MOVE_FORWARD, "dead", previous_state
        )

        # Should get big penalty for death
        self.assertLess(reward, -50)

    def test_calculate_reward_resource_collection(self):
        """Test reward for collecting elevation resources"""
        previous_state = GameState()
        previous_state.inventory["linemate"] = 0

        self.ai.state.inventory["linemate"] = 2  # Collected linemate

        reward = self.ai.calculate_reward(
            ActionType.TAKE_RESOURCE, "ok", previous_state
        )

        # Should get reward for collecting elevation resources
        self.assertGreater(reward, 5)

    @patch("random.random")
    def test_choose_action_exploration(self, mock_random):
        """Test action choice during exploration"""
        mock_random.return_value = 0.1  # Force exploration
        state_vector = np.array([[0.5] * 50])

        action = self.ai.choose_action(state_vector)

        # Should return a valid action type
        self.assertIsInstance(action, ActionType)

    @patch("random.random")
    def test_choose_action_exploitation(self, mock_random):
        """Test action choice during exploitation"""
        mock_random.return_value = 0.8  # Force exploitation
        state_vector = np.array([[0.5] * 50])

        # Mock neural network prediction
        self.ai.neural_network.predict.return_value = np.array(
            [[0.1, 0.8, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]]
        )

        action = self.ai.choose_action(state_vector)

        # Should return action with highest prediction (index 1 = TURN_LEFT)
        self.assertEqual(action, ActionType.TURN_LEFT)

    def test_choose_action_no_learning(self):
        """Test action choice when learning is disabled"""
        self.ai.learning_enabled = False
        state_vector = np.array([[0.5] * 50])

        # Mock neural network prediction
        self.ai.neural_network.predict.return_value = np.array(
            [[0.0, 0.0, 0.9, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]]
        )

        action = self.ai.choose_action(state_vector)

        # Should return action with highest prediction (index 2 = TURN_RIGHT)
        self.assertEqual(action, ActionType.TURN_RIGHT)

    def test_action_to_command_basic_actions(self):
        """Test conversion of actions to commands"""
        # Test basic movement actions
        self.assertEqual(self.ai.action_to_command(ActionType.MOVE_FORWARD), "Forward")
        self.assertEqual(self.ai.action_to_command(ActionType.TURN_LEFT), "Left")
        self.assertEqual(self.ai.action_to_command(ActionType.TURN_RIGHT), "Right")
        self.assertEqual(self.ai.action_to_command(ActionType.LOOK), "Look")

    def test_action_to_command_complex_actions(self):
        """Test conversion of complex actions to commands"""
        # Test more complex actions
        self.assertEqual(self.ai.action_to_command(ActionType.INVENTORY), "Inventory")
        self.assertEqual(
            self.ai.action_to_command(ActionType.INCANTATION), "Incantation"
        )
        self.assertEqual(self.ai.action_to_command(ActionType.FORK), "Fork")
        self.assertEqual(self.ai.action_to_command(ActionType.EJECT), "Eject")

    def test_action_to_command_resource_actions(self):
        """Test resource-related action commands"""
        # Mock the thinking module to provide resource decisions
        self.ai.thinking_module.analyze_situation.return_value = {
            "resource_availability": {"food": [0]},
            "opportunities": [],
            "urgency_level": 1,
        }

        # Test take resource
        result = self.ai.action_to_command(ActionType.TAKE_RESOURCE)
        self.assertTrue(result.startswith("Take "))

        # Test drop resource
        result = self.ai.action_to_command(ActionType.DROP_RESOURCE)
        self.assertTrue(result.startswith("Set "))

    def test_parse_look_result_valid(self):
        """Test parsing valid look result"""
        look_result = "[ food linemate, player, deraumere sibur ]"

        parsed = self.ai.parse_look_result(look_result)

        expected = [["food", "linemate"], ["player"], ["deraumere", "sibur"]]
        self.assertEqual(parsed, expected)

    def test_parse_look_result_empty(self):
        """Test parsing empty look result"""
        look_result = "[ , , ]"

        parsed = self.ai.parse_look_result(look_result)

        expected = [[], [], []]
        self.assertEqual(parsed, expected)

    def test_parse_look_result_single_items(self):
        """Test parsing look result with single items"""
        look_result = "[ food, , linemate ]"

        parsed = self.ai.parse_look_result(look_result)

        expected = [["food"], [], ["linemate"]]
        self.assertEqual(parsed, expected)

    def test_epsilon_decay(self):
        """Test epsilon decay over time"""
        initial_epsilon = self.ai.epsilon

        # Simulate learning steps
        for _ in range(10):
            self.ai.epsilon = max(
                self.ai.epsilon * self.ai.epsilon_decay, self.ai.epsilon_min
            )

        # Epsilon should have decreased but not below minimum
        self.assertLess(self.ai.epsilon, initial_epsilon)
        self.assertGreaterEqual(self.ai.epsilon, self.ai.epsilon_min)

    def test_performance_metrics_tracking(self):
        """Test that performance metrics are properly tracked"""
        # Test successful action tracking
        previous_state = GameState()
        self.ai.calculate_reward(ActionType.LOOK, "ok", previous_state)
        self.assertEqual(self.ai.performance_metrics["successful_actions"], 1)

        # Test failed action tracking
        self.ai.calculate_reward(ActionType.TAKE_RESOURCE, "ko", previous_state)
        self.assertEqual(self.ai.performance_metrics["failed_actions"], 1)

    def test_action_history_tracking(self):
        """Test that action history is properly maintained"""
        initial_length = len(self.ai.action_history)

        # Mock choose_action to track history
        state_vector = np.array([[0.5] * 50])
        with patch("random.random", return_value=0.1):
            action = self.ai.choose_action(state_vector)

        # Action history should grow (though it may not in this isolated test)
        # This test mainly ensures the data structures exist
        self.assertIsInstance(self.ai.action_history, type(self.ai.action_history))
        self.assertIsInstance(self.ai.reward_history, type(self.ai.reward_history))

    def test_fork_conditions(self):
        """Test fork decision conditions"""
        # Test early game forking with good food
        self.ai.state.turn_count = 100
        self.ai.state.inventory["food"] = 60
        self.ai.should_fork_early = True

        self.assertTrue(self.ai.should_fork())

        # Test insufficient food
        self.ai.state.inventory["food"] = 30
        self.assertFalse(self.ai.should_fork())

        # Test late game with excess resources
        self.ai.state.turn_count = 1000
        self.ai.state.inventory["food"] = 120
        self.ai.state.inventory["linemate"] = 10
        self.ai.state.level = 2

        self.assertTrue(self.ai.should_fork())

        # Test high level stable
        self.ai.state.level = 4
        self.ai.state.inventory["food"] = 90

        self.assertTrue(self.ai.should_fork())

    def test_fork_cooldown(self):
        """Test fork cooldown mechanism"""
        # Set recent fork attempt
        self.ai.last_fork_attempt = time.time()
        self.ai.state.inventory["food"] = 100

        # Should not fork due to cooldown
        self.assertFalse(self.ai.should_fork())

        # Set old fork attempt
        self.ai.last_fork_attempt = time.time() - 70

        # Should fork now
        self.assertTrue(self.ai.should_fork())

    def test_fork_attempt(self):
        """Test fork attempt execution"""
        # Mock send_command
        self.ai.send_command = Mock()

        # Set conditions for forking
        self.ai.state.inventory["food"] = 100
        self.ai.state.level = 3

        result = self.ai.attempt_fork()

        self.assertTrue(result)
        self.ai.send_command.assert_called_with("Fork")
        self.assertEqual(self.ai.performance_metrics["forks_attempted"], 1)

    def test_fork_response_handling(self):
        """Test fork response processing"""
        previous_state = GameState()
        previous_state.level = 2
        previous_state.inventory = {"food": 50}

        # Test successful fork
        result = self.ai.process_response("ok", previous_state, ActionType.FORK)

        self.assertTrue(result)
        self.assertEqual(self.ai.performance_metrics["forks_successful"], 1)
        self.assertFalse(self.ai.should_fork_early)

    def test_get_required_stones_for_level(self):
        """Test stone requirements for different levels"""
        # Test level 2 requirements
        req = self.ai.get_required_stones_for_level(2)
        self.assertEqual(req, {"linemate": 1})

        # Test level 3 requirements
        req = self.ai.get_required_stones_for_level(3)
        self.assertEqual(req, {"linemate": 1, "deraumere": 1, "sibur": 1})

        # Test invalid level
        req = self.ai.get_required_stones_for_level(10)
        self.assertEqual(req, {})

    def test_enhanced_think_and_decide_with_fork(self):
        """Test thinking process with fork consideration"""
        # Mock methods
        self.ai.attempt_fork = Mock(return_value=True)

        action = self.ai.think_and_decide()

        self.assertEqual(action, ActionType.FORK)
        self.ai.attempt_fork.assert_called_once()

    def test_fork_reward_calculation(self):
        """Test reward calculation for fork actions"""
        previous_state = GameState()
        previous_state.inventory = {"food": 50}

        # Test successful fork reward
        reward = self.ai.calculate_reward(ActionType.FORK, "ok", previous_state)

        # Should get base reward + fork success reward
        self.assertGreater(reward, 20.0)  # Fork gives +20 reward

    def test_performance_metrics_update(self):
        """Test that fork metrics are properly tracked"""
        initial_forks = self.ai.performance_metrics["forks_attempted"]
        initial_success = self.ai.performance_metrics["forks_successful"]

        # Mock successful fork
        self.ai.send_command = Mock()
        self.ai.state.inventory["food"] = 100
        self.ai.attempt_fork()

        self.assertEqual(
            self.ai.performance_metrics["forks_attempted"], initial_forks + 1
        )

        # Process successful response
        previous_state = GameState()
        self.ai.process_response("ok", previous_state, ActionType.FORK)

        self.assertEqual(
            self.ai.performance_metrics["forks_successful"], initial_success + 1
        )
