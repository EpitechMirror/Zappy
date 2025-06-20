##
## Classes.py for Zappy in /home/adrien/Epitech/B-YEP-410/Zappy/ai/src
##
## Made by adrien.marette@epitech.eu
## Login   <adrien.marette@epitech.eu>
##
## Started on  Tue Jun 10 1:35:41 PM 2025 adrien.marette@epitech.eu
## Last update Wed Jun 10 1:36:48 PM 2025 adrien.marette@epitech.eu
##

import math
from enum import Enum
from typing import Dict, List
from dataclasses import dataclass


class Direction(Enum):
    NORTH = 1
    EAST = 2
    SOUTH = 3
    WEST = 4


class ResourceType(Enum):
    FOOD = "food"
    LINEMATE = "linemate"
    DERAUMERE = "deraumere"
    SIBUR = "sibur"
    MENDIANE = "mendiane"
    PHIRAS = "phiras"
    THYSTAME = "thystame"


class ActionType(Enum):
    MOVE_FORWARD = 0
    TURN_LEFT = 1
    TURN_RIGHT = 2
    TAKE_RESOURCE = 3
    DROP_RESOURCE = 4
    LOOK = 5
    BROADCAST = 6
    INCANTATION = 7
    FORK = 8
    EJECT = 9
    INVENTORY = 10


@dataclass
class Position:
    x: int
    y: int

    def distance_to(self, other: "Position") -> float:
        return math.sqrt((self.x - other.x) ** 2 + (self.y - other.y) ** 2)


@dataclass
class GameState:
    level: int = 1
    position: Position = None
    direction: Direction = Direction.NORTH
    inventory: Dict[str, int] = None
    world_width: int = 0
    world_height: int = 0
    team_slots: int = 0
    food_units: int = 0
    turn_count: int = 0
    last_action_reward: float = 0.0

    def __post_init__(self):
        if self.inventory is None:
            self.inventory = {
                "food": 10,
                "linemate": 0,
                "deraumere": 0,
                "sibur": 0,
                "mendiane": 0,
                "phiras": 0,
                "thystame": 0,
            }
        if self.position is None:
            self.position = Position(0, 0)


@dataclass
class Experience:
    state: List[float]
    action: int
    reward: float
    next_state: List[float]
    done: bool
