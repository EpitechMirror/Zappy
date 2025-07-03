#!/usr/bin/env python3
##
## test_team_roles.py for AI New in /home/adrien/Desktop/AI New
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

from TeamExplorer import TeamExplorer
from TeamGuardian import TeamGuardian
from TeamCoordinator import TeamCoordinator
from Team import TeamRole, TeamMember

class TestTeamExplorer(unittest.TestCase):
    """Unit tests for the TeamExplorer class."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.explorer = TeamExplorer("localhost", 4242, "test_team")
        
    def test_initialization(self):
        """Test TeamExplorer initialization."""
        self.assertEqual(self.explorer.role, TeamRole.EXPLORER)
        self.assertEqual(self.explorer.exploration_pattern, 0)
        self.assertEqual(self.explorer.visited_positions, set())
        self.assertEqual(self.explorer.exploration_targets, [])
        self.assertTrue(self.explorer.mapping_mode)
        
    @patch.object(TeamExplorer, 'send_command')
    @patch.object(TeamExplorer, 'systematic_exploration')
    def test_execute_role_strategy_mapping_mode(self, mock_systematic, mock_send):
        """Test role strategy in mapping mode."""
        self.explorer.mapping_mode = True
        
        with patch('random.random', return_value=0.2):  # Skip inventory check
            self.explorer.execute_role_strategy()
            
        mock_systematic.assert_called_once()
        
    @patch.object(TeamExplorer, 'send_command')
    @patch.object(TeamExplorer, 'intelligent_exploration')
    def test_execute_role_strategy_intelligent_mode(self, mock_intelligent, mock_send):
        """Test role strategy in intelligent mode."""
        self.explorer.mapping_mode = False
        
        with patch('random.random', return_value=0.2):  # Skip inventory and sharing
            self.explorer.execute_role_strategy()
            
        mock_intelligent.assert_called_once()
        
    @patch.object(TeamExplorer, 'send_command')
    @patch.object(TeamExplorer, 'calculate_needed_resources')
    def test_systematic_exploration(self, mock_resources, mock_send):
        """Test systematic exploration pattern."""
        mock_resources.return_value = ["linemate"]
        
        # Test different pattern steps
        for pattern in range(12):
            self.explorer.exploration_pattern = pattern
            self.explorer.systematic_exploration()
            
        # Should have made various movement commands
        self.assertTrue(mock_send.called)
        mock_send.assert_any_call("Look", "normal")
        
    @patch.object(TeamExplorer, 'send_command')
    def test_intelligent_exploration_with_targets(self, mock_send):
        """Test intelligent exploration with targets."""
        self.explorer.exploration_targets = [(10, 10)]
        self.explorer.position = (5, 5)
        
        with patch.object(self.explorer, 'move_towards_target_position') as mock_move:
            with patch.object(self.explorer, 'calculate_distance', return_value=1.0):
                self.explorer.intelligent_exploration()
                
        # Should remove target when reached
        self.assertEqual(len(self.explorer.exploration_targets), 0)
        
    @patch.object(TeamExplorer, 'send_command')
    def test_intelligent_exploration_no_targets(self, mock_send):
        """Test intelligent exploration without targets."""
        self.explorer.exploration_targets = []
        
        with patch.object(self.explorer, 'advanced_exploration_pattern') as mock_pattern:
            self.explorer.intelligent_exploration()
            
        mock_pattern.assert_called_once()
        
    @patch.object(TeamExplorer, 'send_command')
    def test_advanced_exploration_pattern_crowded_area(self, mock_send):
        """Test exploration pattern in crowded area."""
        # Add many visited positions nearby
        for x in range(10):
            for y in range(10):
                self.explorer.visited_positions.add((x, y))
                
        self.explorer.position = (5, 5)
        
        with patch.object(self.explorer, 'calculate_distance', return_value=2.0):
            self.explorer.advanced_exploration_pattern()
            
        # Should try to move away from crowded area
        mock_send.assert_called()
        
    @patch.object(TeamExplorer, 'broadcast_team_message')
    def test_share_resource_discoveries(self, mock_broadcast):
        """Test sharing resource discoveries."""
        self.explorer.resource_map = {
            1: ["linemate", "food"],
            2: ["deraumere"]
        }
        self.explorer.position = (3, 3)
        
        with patch('time.time', return_value=1000):
            self.explorer.share_resource_discoveries()
            
        # Should broadcast discoveries
        self.assertGreaterEqual(mock_broadcast.call_count, 1)
        
    @patch.object(TeamExplorer, 'turn_to_direction')
    @patch.object(TeamExplorer, 'send_command')
    def test_move_towards_target_position(self, mock_send, mock_turn):
        """Test moving towards target position."""
        self.explorer.position = (0, 0)
        target = (5, 3)
        
        self.explorer.move_towards_target_position(target)
        
        # Should turn towards target and move forward
        mock_turn.assert_called_once()
        mock_send.assert_called_with("Forward", "normal")
        
    def test_process_team_message_resource_request(self):
        """Test processing resource request."""
        message_data = {
            "type": "resource_request",
            "resource": "linemate"
        }
        message = f"TEAM:{json.dumps(message_data)}"
        
        with patch.object(self.explorer, 'prioritize_resource_search') as mock_prioritize:
            self.explorer.process_team_message(message, 1)
            mock_prioritize.assert_called_once_with("linemate")
            
    @patch.object(TeamExplorer, 'send_command')
    def test_prioritize_resource_search(self, mock_send):
        """Test prioritizing resource search."""
        self.explorer.prioritize_resource_search("deraumere")
        
        # Should switch to non-mapping mode
        self.assertFalse(self.explorer.mapping_mode)
        
        # Should perform search actions
        mock_send.assert_called()


class TestTeamGuardian(unittest.TestCase):
    """Unit tests for the TeamGuardian class."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.guardian = TeamGuardian("localhost", 4242, "test_team")
        
    def test_initialization(self):
        """Test TeamGuardian initialization."""
        self.assertEqual(self.guardian.role, TeamRole.GUARDIAN)
        self.assertEqual(self.guardian.protected_areas, [])
        self.assertEqual(self.guardian.patrol_route, [])
        self.assertEqual(self.guardian.current_patrol_index, 0)
        self.assertEqual(self.guardian.threat_level, 0)
        
    @patch.object(TeamGuardian, 'assess_threat_level')
    @patch.object(TeamGuardian, 'find_vulnerable_members')
    @patch.object(TeamGuardian, 'protect_vulnerable_members')
    def test_execute_role_strategy_with_threats(self, mock_protect, mock_find, mock_assess):
        """Test role strategy with vulnerable members."""
        vulnerable_member = Mock()
        vulnerable_member.inventory = {"food": 10}
        mock_find.return_value = [vulnerable_member]
        
        with patch('random.random', return_value=0.2):  # Skip inventory
            self.guardian.execute_role_strategy()
            
        mock_assess.assert_called_once()
        mock_find.assert_called_once()
        mock_protect.assert_called_once_with([vulnerable_member])
        
    @patch.object(TeamGuardian, 'patrol_territory')
    def test_execute_role_strategy_no_threats(self, mock_patrol):
        """Test role strategy with no threats."""
        with patch.object(self.guardian, 'find_vulnerable_members', return_value=[]):
            with patch('random.random', return_value=0.2):  # Skip inventory
                self.guardian.execute_role_strategy()
                
        mock_patrol.assert_called_once()
        
    def test_assess_threat_level(self):
        """Test threat level assessment."""
        # Add team members with low food
        member1 = Mock()
        member1.inventory = {"food": 15}
        member2 = Mock()
        member2.inventory = {"food": 5}
        member3 = Mock()
        member3.inventory = {"food": 30}
        
        self.guardian.team_members = {1: member1, 2: member2, 3: member3}
        
        with patch('time.time', return_value=1000):
            self.guardian.last_threat_check = 990  # Force assessment
            self.guardian.assess_threat_level()
            
        # Should detect medium threat (2 members < 20 food)
        self.assertEqual(self.guardian.threat_level, 2)
        
    def test_find_vulnerable_members(self):
        """Test finding vulnerable members."""
        member1 = Mock()
        member1.inventory = {"food": 10}  # Vulnerable
        member1.status = "active"
        member1.level = 2
        
        member2 = Mock()
        member2.inventory = {"food": 50}  # Safe
        member2.status = "active"
        member2.level = 3
        
        self.guardian.team_members = {1: member1, 2: member2}
        self.guardian.level = 3
        
        vulnerable = self.guardian.find_vulnerable_members()
        
        self.assertIn(member1, vulnerable)
        self.assertNotIn(member2, vulnerable)
        
    @patch.object(TeamGuardian, 'set_target_position')
    @patch.object(TeamGuardian, 'drop_resource_for_team')
    def test_protect_vulnerable_members(self, mock_drop, mock_target):
        """Test protecting vulnerable members."""
        member = Mock()
        member.inventory = {"food": 5}
        member.position = (10, 10)
        member.id = 1
        
        self.guardian.inventory["food"] = 60
        
        self.guardian.protect_vulnerable_members([member])
        
        mock_target.assert_called_once_with((10, 10))
        mock_drop.assert_called_once_with("food", 10)
        
    def test_establish_patrol_route(self):
        """Test establishing patrol route."""
        member1 = Mock()
        member1.position = (5, 5)
        member2 = Mock()
        member2.position = (15, 15)
        
        self.guardian.team_members = {1: member1, 2: member2}
        
        self.guardian.establish_patrol_route()
        
        # Should create patrol route around team center
        self.assertEqual(len(self.guardian.patrol_route), 4)
        
    @patch.object(TeamGuardian, 'send_command')
    @patch.object(TeamGuardian, 'broadcast_team_message')
    def test_secure_elevation_site(self, mock_broadcast, mock_send):
        """Test securing elevation site."""
        self.guardian.position = (5, 5)
        
        with patch('time.time', return_value=1000):
            self.guardian.secure_elevation_site()
            
        mock_send.assert_any_call("Look", "high")
        mock_send.assert_any_call("Forward", "normal")
        mock_broadcast.assert_called_once()


class TestTeamCoordinator(unittest.TestCase):
    """Unit tests for the TeamCoordinator class."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.coordinator = TeamCoordinator("localhost", 4242, "test_team")
        
    def test_initialization(self):
        """Test TeamCoordinator initialization."""
        self.assertEqual(self.coordinator.role, TeamRole.COORDINATOR)
        self.assertEqual(self.coordinator.elevation_plans, {})
        self.assertEqual(self.coordinator.resource_assignments, {})
        self.assertEqual(self.coordinator.team_strategy, "exploration")
        
    @patch.object(TeamCoordinator, 'update_team_strategy')
    @patch.object(TeamCoordinator, 'manage_team_elevations')
    def test_execute_role_strategy(self, mock_elevations, mock_strategy):
        """Test coordinator role strategy."""
        with patch('time.time', return_value=1000):
            self.coordinator.last_strategy_update = 950  # Force update
            self.coordinator.execute_role_strategy()
            
        mock_strategy.assert_called_once()
        mock_elevations.assert_called_once()
        
    @patch.object(TeamCoordinator, 'broadcast_team_message')
    def test_update_team_strategy(self, mock_broadcast):
        """Test team strategy update."""
        # Add team members with different levels
        member1 = Mock()
        member1.level = 3
        member2 = Mock()
        member2.level = 4
        
        self.coordinator.team_members = {1: member1, 2: member2}
        
        self.coordinator.update_team_strategy()
        
        # Should switch to collection strategy (avg level 3.5)
        self.assertEqual(self.coordinator.team_strategy, "collection")
        mock_broadcast.assert_called_once()
        
    def test_find_elevation_candidates(self):
        """Test finding elevation candidates."""
        member1 = Mock()
        member1.level = 2
        member1.inventory = {"food": 60, "linemate": 2, "deraumere": 2, "sibur": 2}
        
        member2 = Mock()
        member2.level = 2
        member2.inventory = {"food": 30, "linemate": 1}  # Not ready
        
        self.coordinator.team_members = {1: member1, 2: member2}
        
        with patch.object(self.coordinator, 'is_ready_for_elevation') as mock_ready:
            mock_ready.side_effect = lambda m: m.inventory["food"] > 50
            candidates = self.coordinator.find_elevation_candidates()
            
        self.assertIn(member1, candidates)
        self.assertNotIn(member2, candidates)
        
    @patch.object(TeamCoordinator, 'broadcast_team_message')
    def test_organize_elevation(self, mock_broadcast):
        """Test organizing elevation."""
        member1 = Mock()
        member1.position = (5, 5)
        member2 = Mock()
        member2.position = (7, 7)
        
        members = [member1, member2]
        
        with patch('time.time', return_value=1000):
            self.coordinator.organize_elevation(members, 2)
            
        # Should create elevation plan
        self.assertEqual(len(self.coordinator.elevation_plans), 1)
        mock_broadcast.assert_called_once()
        
    def test_calculate_center_position(self):
        """Test calculating center position."""
        member1 = Mock()
        member1.position = (0, 0)
        member2 = Mock()
        member2.position = (10, 10)
        
        center = self.coordinator.calculate_center_position([member1, member2])
        self.assertEqual(center, (5, 5))
        
    def test_analyze_team_resource_needs(self):
        """Test analyzing team resource needs."""
        member = Mock()
        member.level = 2
        member.inventory = {"linemate": 0, "deraumere": 0}
        
        self.coordinator.team_members = {1: member}
        
        needs = self.coordinator.analyze_team_resource_needs()
        
        # Should need resources for level 2 elevation
        self.assertIn("linemate", needs)
        self.assertIn("deraumere", needs)


if __name__ == '__main__':
    unittest.main()
