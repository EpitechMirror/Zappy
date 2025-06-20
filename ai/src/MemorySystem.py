##
## MemorySystem.py for Zappy in /home/adrien/Epitech/B-YEP-410/Zappy/ai/src
##
## Made by adrien.marette@epitech.eu
## Login   <adrien.marette@epitech.eu>
##
## Started on  Tue Jun 10 1:14:34 PM 2025 adrien.marette@epitech.eu
## Last update Wed Jun 10 1:37:18 PM 2025 adrien.marette@epitech.eu
##

import random
from collections import deque
from Classes import Experience
from typing import List, Optional


class MemorySystem:
    """Advanced memory system for storing and retrieving experiences"""
    
    def __init__(self, capacity: int = 10000):
        self.capacity = capacity
        self.experiences = deque(maxlen=capacity)
        self.short_term_memory = deque(maxlen=100)
        self.successful_strategies = {}
        self.failed_strategies = {}
        
    def add_experience(self, experience: Experience):
        self.experiences.append(experience)
        self.short_term_memory.append(experience)
    
    def get_batch(self, batch_size: int) -> List[Experience]:
        if len(self.experiences) < batch_size:
            return list(self.experiences)
        return random.sample(list(self.experiences), batch_size)
    
    def get_recent_experiences(self, count: int) -> List[Experience]:
        return list(self.short_term_memory)[-count:]
    
    def add_strategy(self, situation: str, action_sequence: List[int], success: bool, reward: float):
        if success:
            if situation not in self.successful_strategies:
                self.successful_strategies[situation] = []
            self.successful_strategies[situation].append({
                'actions': action_sequence,
                'reward': reward,
                'frequency': 1
            })
        else:
            if situation not in self.failed_strategies:
                self.failed_strategies[situation] = []
            self.failed_strategies[situation].append({
                'actions': action_sequence,
                'penalty': -reward,
                'frequency': 1
            })
    
    def get_best_strategy(self, situation: str) -> Optional[List[int]]:
        if situation in self.successful_strategies:
            strategies = self.successful_strategies[situation]
            best_strategy = max(strategies, key=lambda x: x['reward'] * x['frequency'])
            return best_strategy['actions']
        return None
