##
## test_thinking_module.py for Zappy in /home/adrien/Epitech/B-YEP-410/Zappy/ai/tests
##
## Made by adrien.marette@epitech.eu
## Login   <adrien.marette@epitech.eu>
##
## Started on  Sat Jun 29 2025
## Last update Mon Jun 29 2:39:35 PM 2025 adrien.marette@epitech.eu
##

import unittest
from unittest.mock import Mock, patch
import sys
import os

# Add the src directory to the Python path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'src'))

from ThinkingModule import ThinkingModule
from Classes import GameState, ResourceType, ActionType, Position


class TestThinkingModule(unittest.TestCase):
    """Test cases for ThinkingModule class"""
    
    def setUp(self):
        """Set up test fixtures before each test method"""
        self.thinking_module = ThinkingModule()
        
        # Create sample game state
        self.sample_state = GameState(
            level=2,
            position=Position(5, 10),
            inventory={
                "food": 25,
                "linemate": 1,
                "deraumere": 0,
                "sibur": 1,
                "mendiane": 0,
                "phiras": 0,
                "thystame": 0,
            }
        )
        
        # Sample vision data
        self.sample_vision = [
            ["food", "linemate"],  # Tile 0 (current position)
            ["player"],           # Tile 1
            [],                   # Tile 2 (empty)
            ["deraumere", "sibur"], # Tile 3
            ["food"],             # Tile 4
            [],                   # Tile 5
            ["phiras"],           # Tile 6
            [],                   # Tile 7
            ["thystame"]          # Tile 8
        ]

    def test_thinking_module_initialization(self):
        """Test thinking module initialization"""
        self.assertEqual(len(self.thinking_module.decision_tree), 0)
        self.assertEqual(len(self.thinking_module.problem_solving_cache), 0)
        self.assertEqual(len(self.thinking_module.goal_stack), 0)
        self.assertEqual(len(self.thinking_module.current_plan), 0)
        
        # Check meta-knowledge structure
        self.assertIn('resource_locations', self.thinking_module.meta_knowledge)
        self.assertIn('player_behaviors', self.thinking_module.meta_knowledge)
        self.assertIn('map_knowledge', self.thinking_module.meta_knowledge)
        self.assertIn('time_patterns', self.thinking_module.meta_knowledge)

    def test_analyze_situation_normal(self):
        """Test situation analysis with normal conditions"""
        analysis = self.thinking_module.analyze_situation(self.sample_state, self.sample_vision)
        
        # Check analysis structure
        self.assertIn('immediate_threats', analysis)
        self.assertIn('opportunities', analysis)
        self.assertIn('resource_availability', analysis)
        self.assertIn('strategic_position', analysis)
        self.assertIn('urgency_level', analysis)
        
        # With food = 25, should not have low_food threat
        self.assertNotIn('low_food', analysis['immediate_threats'])
        
        # Should detect resources in vision
        self.assertIn('food', analysis['resource_availability'])
        self.assertIn('linemate', analysis['resource_availability'])
        self.assertIn('deraumere', analysis['resource_availability'])

    def test_analyze_situation_low_food(self):
        """Test situation analysis with low food"""
        low_food_state = GameState(
            level=1,
            inventory={"food": 15}  # Low food
        )
        
        analysis = self.thinking_module.analyze_situation(low_food_state, self.sample_vision)
        
        # Should detect low food threat
        self.assertIn('low_food', analysis['immediate_threats'])
        self.assertGreaterEqual(analysis['urgency_level'], 3)

    def test_analyze_situation_very_low_food(self):
        """Test situation analysis with very low food"""
        very_low_food_state = GameState(
            level=1,
            inventory={"food": 5}  # Very low food
        )
        
        analysis = self.thinking_module.analyze_situation(very_low_food_state, self.sample_vision)
        
        # Should detect low food threat
        self.assertIn('low_food', analysis['immediate_threats'])
        self.assertGreaterEqual(analysis['urgency_level'], 3)

    def test_analyze_situation_resource_detection(self):
        """Test that resources in vision are properly detected"""
        analysis = self.thinking_module.analyze_situation(self.sample_state, self.sample_vision)
        
        # Check specific resource locations
        self.assertEqual(analysis['resource_availability']['food'], [0, 4])
        self.assertEqual(analysis['resource_availability']['linemate'], [0])
        self.assertEqual(analysis['resource_availability']['deraumere'], [3])
        self.assertEqual(analysis['resource_availability']['sibur'], [3])
        self.assertEqual(analysis['resource_availability']['phiras'], [6])
        self.assertEqual(analysis['resource_availability']['thystame'], [8])

    def test_analyze_situation_elevation_opportunity(self):
        """Test detection of elevation opportunity"""
        # Create state that can elevate (has all required resources for level 2)
        ready_state = GameState(
            level=2,
            inventory={
                "food": 30,
                "linemate": 1,
                "deraumere": 1,
                "sibur": 1,
                "mendiane": 0,
                "phiras": 0,
                "thystame": 0,
            }
        )
        
        analysis = self.thinking_module.analyze_situation(ready_state, self.sample_vision)
        
        # Should detect elevation opportunity
        self.assertIn('can_elevate', analysis['opportunities'])
        self.assertGreaterEqual(analysis['urgency_level'], 2)

    def test_calculate_elevation_needs_level_1(self):
        """Test elevation needs calculation for level 1"""
        level_1_state = GameState(level=1, inventory={"food": 10, "linemate": 0})
        
        needs = self.thinking_module.calculate_elevation_needs(level_1_state)
        
        self.assertEqual(needs, {"linemate": 1})

    def test_calculate_elevation_needs_level_2(self):
        """Test elevation needs calculation for level 2"""
        level_2_state = GameState(
            level=2, 
            inventory={
                "food": 10,
                "linemate": 0,
                "deraumere": 1,
                "sibur": 0
            }
        )
        
        needs = self.thinking_module.calculate_elevation_needs(level_2_state)
        
        expected_needs = {"linemate": 1, "sibur": 1}
        self.assertEqual(needs, expected_needs)

    def test_calculate_elevation_needs_level_7(self):
        """Test elevation needs calculation for max level"""
        max_level_state = GameState(level=7)
        
        needs = self.thinking_module.calculate_elevation_needs(max_level_state)
        
        # No needs for max level
        self.assertEqual(needs, {})

    def test_calculate_elevation_needs_level_8(self):
        """Test elevation needs calculation for beyond max level"""
        beyond_max_state = GameState(level=8)
        
        needs = self.thinking_module.calculate_elevation_needs(beyond_max_state)
        
        # No needs for beyond max level
        self.assertEqual(needs, {})

    def test_calculate_elevation_needs_has_resources(self):
        """Test elevation needs when player already has required resources"""
        well_equipped_state = GameState(
            level=1,
            inventory={
                "food": 10,
                "linemate": 5,  # More than needed
                "deraumere": 2,
                "sibur": 3
            }
        )
        
        needs = self.thinking_module.calculate_elevation_needs(well_equipped_state)
        
        # Should need nothing for level 1
        self.assertEqual(needs, {})

    def test_create_plan_survive(self):
        """Test creating a survival plan"""
        high_urgency_analysis = {
            'urgency_level': 5,
            'immediate_threats': ['low_food'],
            'opportunities': [],
            'resource_availability': {}
        }
        
        plan = self.thinking_module.create_plan("survive", self.sample_state, high_urgency_analysis)
        
        # Should have exploration actions
        self.assertIn(ActionType.LOOK, plan)
        self.assertIn(ActionType.MOVE_FORWARD, plan)

    def test_create_plan_gather_food_current_tile(self):
        """Test creating a plan to gather food when food is on current tile"""
        food_analysis = {
            'urgency_level': 1,
            'resource_availability': {'food': [0, 4]}  # Food on current tile (0)
        }
        
        plan = self.thinking_module.create_plan("gather_food", self.sample_state, food_analysis)
        
        # Should take resource immediately
        self.assertIn(ActionType.TAKE_RESOURCE, plan)

    def test_create_plan_gather_food_nearby_tile(self):
        """Test creating a plan to gather food when food is on nearby tile"""
        food_analysis = {
            'urgency_level': 1,
            'resource_availability': {'food': [4]}  # Food on tile 4
        }
        
        plan = self.thinking_module.create_plan("gather_food", self.sample_state, food_analysis)
        
        # Should move and then take resource
        self.assertIn(ActionType.MOVE_FORWARD, plan)
        self.assertIn(ActionType.TAKE_RESOURCE, plan)

    def test_create_plan_gather_resources(self):
        """Test creating a plan to gather elevation resources"""
        resource_analysis = {
            'urgency_level': 2,
            'resource_availability': {
                'deraumere': [3],
                'sibur': [3],
                'phiras': [6]
            }
        }
        
        # Mock the calculate_elevation_needs to return specific needs
        with patch.object(self.thinking_module, 'calculate_elevation_needs') as mock_needs:
            mock_needs.return_value = {'deraumere': 1, 'sibur': 1}
            
            plan = self.thinking_module.create_plan("gather_resources", self.sample_state, resource_analysis)
            
            # Should plan to take resources
            self.assertIn(ActionType.TAKE_RESOURCE, plan)

    def test_create_plan_elevate(self):
        """Test creating an elevation plan"""
        elevation_analysis = {
            'urgency_level': 3,
            'opportunities': ['can_elevate']
        }
        
        plan = self.thinking_module.create_plan("elevate", self.sample_state, elevation_analysis)
        
        # Should broadcast and then incantate
        self.assertEqual(plan, [ActionType.BROADCAST, ActionType.INCANTATION])

    def test_create_plan_explore(self):
        """Test creating an exploration plan"""
        exploration_analysis = {
            'urgency_level': 1,
            'opportunities': []
        }
        
        with patch('random.random', return_value=0.1):  # Force turn
            plan = self.thinking_module.create_plan("explore", self.sample_state, exploration_analysis)
            
            # Should include turn and movement
            self.assertIn(ActionType.LOOK, plan)
            self.assertIn(ActionType.MOVE_FORWARD, plan)
            self.assertTrue(
                ActionType.TURN_LEFT in plan or ActionType.TURN_RIGHT in plan
            )

    def test_create_plan_explore_no_turn(self):
        """Test creating an exploration plan without turning"""
        exploration_analysis = {
            'urgency_level': 1,
            'opportunities': []
        }
        
        with patch('random.random', return_value=0.8):  # No turn
            plan = self.thinking_module.create_plan("explore", self.sample_state, exploration_analysis)
            
            # Should include look and move but no turn
            self.assertIn(ActionType.LOOK, plan)
            self.assertIn(ActionType.MOVE_FORWARD, plan)
            self.assertNotIn(ActionType.TURN_LEFT, plan)
            self.assertNotIn(ActionType.TURN_RIGHT, plan)

    def test_update_meta_knowledge_action_patterns(self):
        """Test updating meta-knowledge with action patterns"""
        initial_turn = 10
        state_with_turn = GameState(turn_count=initial_turn, position=Position(1, 2))
        
        self.thinking_module.update_meta_knowledge(
            state_with_turn, 
            ActionType.MOVE_FORWARD, 
            "ok"
        )
        
        # Check that action pattern is recorded
        self.assertIn('MOVE_FORWARD', self.thinking_module.meta_knowledge['time_patterns'])
        self.assertIn(initial_turn, self.thinking_module.meta_knowledge['time_patterns']['MOVE_FORWARD'])

    def test_update_meta_knowledge_map_knowledge(self):
        """Test updating meta-knowledge with map information"""
        position = Position(3, 7)
        state_with_pos = GameState(position=position, turn_count=15)
        
        self.thinking_module.update_meta_knowledge(
            state_with_pos,
            ActionType.LOOK,
            "ok"
        )
        
        # Check that map knowledge is updated
        pos_key = "3,7"
        self.assertIn(pos_key, self.thinking_module.meta_knowledge['map_knowledge'])
        self.assertEqual(
            self.thinking_module.meta_knowledge['map_knowledge'][pos_key]['visited_count'], 
            1
        )

    def test_update_meta_knowledge_multiple_visits(self):
        """Test updating meta-knowledge with multiple visits to same location"""
        position = Position(5, 5)
        state = GameState(position=position, turn_count=20)
        
        # Visit the same location multiple times
        for _ in range(3):
            self.thinking_module.update_meta_knowledge(
                state,
                ActionType.MOVE_FORWARD,
                "ok"
            )
        
        # Check visit count
        pos_key = "5,5"
        self.assertEqual(
            self.thinking_module.meta_knowledge['map_knowledge'][pos_key]['visited_count'], 
            3
        )

    def test_elevation_requirements_completeness(self):
        """Test that elevation requirements are defined for all levels 1-7"""
        # This tests the internal elevation_requirements dictionary
        for level in range(1, 8):
            state = GameState(level=level)
            needs = self.thinking_module.calculate_elevation_needs(state)
            # Should not raise an exception and should return a dict
            self.assertIsInstance(needs, dict)


if __name__ == '__main__':
    unittest.main()
