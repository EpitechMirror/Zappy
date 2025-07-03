##
## test_classes.py for Zappy in /home/adrien/Epitech/B-YEP-410/Zappy/ai/tests
##
## Made by adrien.marette@epitech.eu
## Login   <adrien.marette@epitech.eu>
##
## Started on  Sat Jun 29 2025
## Last update Mon Jun 29 2:39:35 PM 2025 adrien.marette@epitech.eu
##

import math
import os
import sys
import unittest
from unittest.mock import Mock, patch

# Add the src directory to the Python path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "src"))

from Classes import (ActionType, Direction, Experience, GameState, Position,
                     ResourceType)


class TestDirection(unittest.TestCase):
    """Test cases for Direction enum"""

    def test_direction_values(self):
        """Test that direction enum has correct values"""
        self.assertEqual(Direction.NORTH.value, 1)
        self.assertEqual(Direction.EAST.value, 2)
        self.assertEqual(Direction.SOUTH.value, 3)
        self.assertEqual(Direction.WEST.value, 4)

    def test_direction_count(self):
        """Test that there are exactly 4 directions"""
        self.assertEqual(len(Direction), 4)


class TestResourceType(unittest.TestCase):
    """Test cases for ResourceType enum"""

    def test_resource_values(self):
        """Test that resource types have correct string values"""
        self.assertEqual(ResourceType.FOOD.value, "food")
        self.assertEqual(ResourceType.LINEMATE.value, "linemate")
        self.assertEqual(ResourceType.DERAUMERE.value, "deraumere")
        self.assertEqual(ResourceType.SIBUR.value, "sibur")
        self.assertEqual(ResourceType.MENDIANE.value, "mendiane")
        self.assertEqual(ResourceType.PHIRAS.value, "phiras")
        self.assertEqual(ResourceType.THYSTAME.value, "thystame")

    def test_resource_count(self):
        """Test that there are exactly 7 resource types"""
        self.assertEqual(len(ResourceType), 7)


class TestActionType(unittest.TestCase):
    """Test cases for ActionType enum"""

    def test_action_values(self):
        """Test that action types have correct integer values"""
        self.assertEqual(ActionType.MOVE_FORWARD.value, 0)
        self.assertEqual(ActionType.TURN_LEFT.value, 1)
        self.assertEqual(ActionType.TURN_RIGHT.value, 2)
        self.assertEqual(ActionType.TAKE_RESOURCE.value, 3)
        self.assertEqual(ActionType.DROP_RESOURCE.value, 4)
        self.assertEqual(ActionType.LOOK.value, 5)
        self.assertEqual(ActionType.BROADCAST.value, 6)
        self.assertEqual(ActionType.INCANTATION.value, 7)
        self.assertEqual(ActionType.FORK.value, 8)
        self.assertEqual(ActionType.EJECT.value, 9)
        self.assertEqual(ActionType.INVENTORY.value, 10)

    def test_action_count(self):
        """Test that there are exactly 11 action types"""
        self.assertEqual(len(ActionType), 11)


class TestPosition(unittest.TestCase):
    """Test cases for Position dataclass"""

    def test_position_creation(self):
        """Test position creation with x and y coordinates"""
        pos = Position(5, 10)
        self.assertEqual(pos.x, 5)
        self.assertEqual(pos.y, 10)

    def test_distance_to_same_position(self):
        """Test distance calculation to the same position"""
        pos1 = Position(0, 0)
        pos2 = Position(0, 0)
        self.assertEqual(pos1.distance_to(pos2), 0.0)

    def test_distance_to_different_position(self):
        """Test distance calculation to different positions"""
        pos1 = Position(0, 0)
        pos2 = Position(3, 4)
        expected_distance = math.sqrt(3**2 + 4**2)  # 5.0
        self.assertEqual(pos1.distance_to(pos2), expected_distance)

    def test_distance_to_negative_coordinates(self):
        """Test distance calculation with negative coordinates"""
        pos1 = Position(-1, -1)
        pos2 = Position(2, 3)
        expected_distance = math.sqrt(
            (2 - (-1)) ** 2 + (3 - (-1)) ** 2
        )  # sqrt(9 + 16) = 5.0
        self.assertEqual(pos1.distance_to(pos2), expected_distance)

    def test_distance_symmetry(self):
        """Test that distance calculation is symmetric"""
        pos1 = Position(1, 2)
        pos2 = Position(4, 6)
        self.assertEqual(pos1.distance_to(pos2), pos2.distance_to(pos1))


class TestGameState(unittest.TestCase):
    """Test cases for GameState dataclass"""

    def test_default_game_state(self):
        """Test game state creation with default values"""
        state = GameState()
        self.assertEqual(state.level, 1)
        self.assertEqual(state.direction, Direction.NORTH)
        self.assertEqual(state.food_units, 0)
        self.assertEqual(state.turn_count, 0)
        self.assertEqual(state.last_action_reward, 0.0)
        self.assertIsNotNone(state.position)
        self.assertIsNotNone(state.inventory)

    def test_game_state_with_custom_values(self):
        """Test game state creation with custom values"""
        pos = Position(10, 20)
        inventory = {"food": 5, "linemate": 1}
        state = GameState(
            level=3,
            position=pos,
            direction=Direction.SOUTH,
            inventory=inventory,
            food_units=50,
            turn_count=100,
            last_action_reward=10.5,
        )

        self.assertEqual(state.level, 3)
        self.assertEqual(state.position, pos)
        self.assertEqual(state.direction, Direction.SOUTH)
        self.assertEqual(state.inventory, inventory)
        self.assertEqual(state.food_units, 50)
        self.assertEqual(state.turn_count, 100)
        self.assertEqual(state.last_action_reward, 10.5)

    def test_default_position_initialization(self):
        """Test that default position is created when none provided"""
        state = GameState()
        self.assertEqual(state.position.x, 0)
        self.assertEqual(state.position.y, 0)

    def test_default_inventory_initialization(self):
        """Test that default inventory is created when none provided"""
        state = GameState()
        expected_inventory = {
            "food": 10,
            "linemate": 0,
            "deraumere": 0,
            "sibur": 0,
            "mendiane": 0,
            "phiras": 0,
            "thystame": 0,
        }
        self.assertEqual(state.inventory, expected_inventory)

    def test_custom_inventory_preserved(self):
        """Test that custom inventory is preserved and not overwritten"""
        custom_inventory = {"food": 5, "linemate": 2}
        state = GameState(inventory=custom_inventory)
        self.assertEqual(state.inventory, custom_inventory)

    def test_custom_position_preserved(self):
        """Test that custom position is preserved and not overwritten"""
        custom_position = Position(15, 25)
        state = GameState(position=custom_position)
        self.assertEqual(state.position, custom_position)


class TestExperience(unittest.TestCase):
    """Test cases for Experience dataclass"""

    def test_experience_creation(self):
        """Test experience creation with all required fields"""
        state = [1.0, 2.0, 3.0]
        action = 5
        reward = 10.5
        next_state = [1.1, 2.1, 3.1]
        done = True

        exp = Experience(
            state=state, action=action, reward=reward, next_state=next_state, done=done
        )

        self.assertEqual(exp.state, state)
        self.assertEqual(exp.action, action)
        self.assertEqual(exp.reward, reward)
        self.assertEqual(exp.next_state, next_state)
        self.assertEqual(exp.done, done)

    def test_experience_with_false_done(self):
        """Test experience creation with done=False"""
        exp = Experience(
            state=[0.0], action=1, reward=-5.0, next_state=[0.1], done=False
        )

        self.assertFalse(exp.done)
        self.assertEqual(exp.reward, -5.0)

    def test_experience_with_empty_states(self):
        """Test experience creation with empty state vectors"""
        exp = Experience(state=[], action=0, reward=0.0, next_state=[], done=True)

        self.assertEqual(exp.state, [])
        self.assertEqual(exp.next_state, [])


if __name__ == "__main__":
    unittest.main()
