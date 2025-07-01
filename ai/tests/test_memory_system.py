##
## test_memory_system.py for Zappy in /home/adrien/Epitech/B-YEP-410/Zappy/ai/tests
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

from MemorySystem import MemorySystem
from Classes import Experience


class TestMemorySystem(unittest.TestCase):
    """Test cases for MemorySystem class"""
    
    def setUp(self):
        """Set up test fixtures before each test method"""
        self.memory = MemorySystem(capacity=100)
        
        # Create sample experiences
        self.exp1 = Experience([1.0, 2.0], 0, 5.0, [1.1, 2.1], False)
        self.exp2 = Experience([2.0, 3.0], 1, -2.0, [2.1, 3.1], False)
        self.exp3 = Experience([3.0, 4.0], 2, 10.0, [3.1, 4.1], True)

    def test_memory_initialization(self):
        """Test memory system initialization"""
        self.assertEqual(len(self.memory.experiences), 0)
        self.assertEqual(len(self.memory.short_term_memory), 0)
        self.assertEqual(len(self.memory.successful_strategies), 0)
        self.assertEqual(len(self.memory.failed_strategies), 0)
        self.assertEqual(self.memory.capacity, 100)

    def test_memory_initialization_with_custom_capacity(self):
        """Test memory system initialization with custom capacity"""
        memory = MemorySystem(capacity=50)
        self.assertEqual(memory.capacity, 50)

    def test_add_experience(self):
        """Test adding a single experience"""
        self.memory.add_experience(self.exp1)
        
        self.assertEqual(len(self.memory.experiences), 1)
        self.assertEqual(len(self.memory.short_term_memory), 1)
        self.assertEqual(self.memory.experiences[0], self.exp1)
        self.assertEqual(self.memory.short_term_memory[0], self.exp1)

    def test_add_multiple_experiences(self):
        """Test adding multiple experiences"""
        experiences = [self.exp1, self.exp2, self.exp3]
        
        for exp in experiences:
            self.memory.add_experience(exp)
        
        self.assertEqual(len(self.memory.experiences), 3)
        self.assertEqual(len(self.memory.short_term_memory), 3)

    def test_capacity_limit_experiences(self):
        """Test that experiences respect capacity limit"""
        small_memory = MemorySystem(capacity=2)
        
        # Add 3 experiences to a capacity-2 memory
        small_memory.add_experience(self.exp1)
        small_memory.add_experience(self.exp2)
        small_memory.add_experience(self.exp3)
        
        # Should only keep the last 2 experiences
        self.assertEqual(len(small_memory.experiences), 2)
        self.assertNotIn(self.exp1, small_memory.experiences)
        self.assertIn(self.exp2, small_memory.experiences)
        self.assertIn(self.exp3, small_memory.experiences)

    def test_capacity_limit_short_term_memory(self):
        """Test that short-term memory respects its limit (100)"""
        # Create 101 experiences
        for i in range(101):
            exp = Experience([float(i)], i % 3, float(i), [float(i) + 0.1], i == 100)
            self.memory.add_experience(exp)
        
        # Short-term memory should only keep the last 100
        self.assertEqual(len(self.memory.short_term_memory), 100)

    def test_get_batch_sufficient_experiences(self):
        """Test getting a batch when there are enough experiences"""
        experiences = [self.exp1, self.exp2, self.exp3]
        for exp in experiences:
            self.memory.add_experience(exp)
        
        batch = self.memory.get_batch(2)
        
        self.assertEqual(len(batch), 2)
        # All returned experiences should be from our added experiences
        for exp in batch:
            self.assertIn(exp, experiences)

    def test_get_batch_insufficient_experiences(self):
        """Test getting a batch when there are not enough experiences"""
        self.memory.add_experience(self.exp1)
        
        batch = self.memory.get_batch(5)
        
        # Should return all available experiences
        self.assertEqual(len(batch), 1)
        self.assertEqual(batch[0], self.exp1)

    def test_get_batch_empty_memory(self):
        """Test getting a batch from empty memory"""
        batch = self.memory.get_batch(3)
        
        self.assertEqual(len(batch), 0)
        self.assertEqual(batch, [])

    def test_get_recent_experiences(self):
        """Test getting recent experiences"""
        experiences = [self.exp1, self.exp2, self.exp3]
        for exp in experiences:
            self.memory.add_experience(exp)
        
        recent = self.memory.get_recent_experiences(2)
        
        self.assertEqual(len(recent), 2)
        self.assertEqual(recent[-1], self.exp3)  # Most recent
        self.assertEqual(recent[-2], self.exp2)  # Second most recent

    def test_get_recent_experiences_more_than_available(self):
        """Test getting more recent experiences than available"""
        self.memory.add_experience(self.exp1)
        
        recent = self.memory.get_recent_experiences(5)
        
        self.assertEqual(len(recent), 1)
        self.assertEqual(recent[0], self.exp1)

    def test_add_successful_strategy(self):
        """Test adding a successful strategy"""
        situation = "low_food"
        actions = [0, 1, 2]
        reward = 15.0
        
        self.memory.add_strategy(situation, actions, success=True, reward=reward)
        
        self.assertIn(situation, self.memory.successful_strategies)
        strategy = self.memory.successful_strategies[situation][0]
        self.assertEqual(strategy['actions'], actions)
        self.assertEqual(strategy['reward'], reward)
        self.assertEqual(strategy['frequency'], 1)

    def test_add_failed_strategy(self):
        """Test adding a failed strategy"""
        situation = "surrounded_by_enemies"
        actions = [3, 4, 5]
        reward = -10.0
        
        self.memory.add_strategy(situation, actions, success=False, reward=reward)
        
        self.assertIn(situation, self.memory.failed_strategies)
        strategy = self.memory.failed_strategies[situation][0]
        self.assertEqual(strategy['actions'], actions)
        self.assertEqual(strategy['penalty'], 10.0)  # Negative reward becomes positive penalty
        self.assertEqual(strategy['frequency'], 1)

    def test_add_multiple_strategies_same_situation(self):
        """Test adding multiple strategies for the same situation"""
        situation = "need_resources"
        actions1 = [0, 1]
        actions2 = [2, 3]
        
        self.memory.add_strategy(situation, actions1, success=True, reward=5.0)
        self.memory.add_strategy(situation, actions2, success=True, reward=10.0)
        
        self.assertEqual(len(self.memory.successful_strategies[situation]), 2)

    def test_get_best_strategy_exists(self):
        """Test getting the best strategy when strategies exist"""
        situation = "gather_food"
        
        # Add multiple strategies with different rewards
        self.memory.add_strategy(situation, [0, 1], success=True, reward=5.0)
        self.memory.add_strategy(situation, [2, 3], success=True, reward=15.0)
        self.memory.add_strategy(situation, [4, 5], success=True, reward=10.0)
        
        best_strategy = self.memory.get_best_strategy(situation)
        
        self.assertEqual(best_strategy, [2, 3])  # Highest reward strategy

    def test_get_best_strategy_with_frequency(self):
        """Test that best strategy considers frequency"""
        situation = "explore"
        
        # Add strategies and manually set frequency
        self.memory.add_strategy(situation, [0, 1], success=True, reward=5.0)
        self.memory.add_strategy(situation, [2, 3], success=True, reward=4.0)
        
        # Manually increase frequency of lower reward strategy
        self.memory.successful_strategies[situation][1]['frequency'] = 5
        
        best_strategy = self.memory.get_best_strategy(situation)
        
        # Should return [2, 3] because 4.0 * 5 = 20 > 5.0 * 1 = 5
        self.assertEqual(best_strategy, [2, 3])

    def test_get_best_strategy_not_exists(self):
        """Test getting best strategy when no strategies exist"""
        best_strategy = self.memory.get_best_strategy("unknown_situation")
        
        self.assertIsNone(best_strategy)

    def test_get_best_strategy_only_failed_strategies(self):
        """Test getting best strategy when only failed strategies exist"""
        situation = "bad_situation"
        self.memory.add_strategy(situation, [0, 1], success=False, reward=-5.0)
        
        best_strategy = self.memory.get_best_strategy(situation)
        
        self.assertIsNone(best_strategy)

    @patch('random.sample')
    def test_get_batch_randomness(self, mock_sample):
        """Test that get_batch uses random sampling"""
        experiences = [self.exp1, self.exp2, self.exp3]
        for exp in experiences:
            self.memory.add_experience(exp)
        
        mock_sample.return_value = [self.exp1, self.exp2]
        
        batch = self.memory.get_batch(2)
        
        mock_sample.assert_called_once()
        self.assertEqual(batch, [self.exp1, self.exp2])


if __name__ == '__main__':
    unittest.main()
