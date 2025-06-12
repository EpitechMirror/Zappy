##
## ThinkingModule.py for Zappy in /home/adrien/Epitech/B-YEP-410/Zappy/ai/src
##
## Made by adrien.marette@epitech.eu
## Login   <adrien.marette@epitech.eu>
##
## Started on  Tue Jun 10 1:17:07 PM 2025 adrien.marette@epitech.eu
## Last update Wed Jun 10 1:37:06 PM 2025 adrien.marette@epitech.eu
##

import random
from typing import List, Dict, Any
from Classes import GameState, ResourceType, ActionType


class ThinkingModule:
    """Advanced thinking and reasoning module"""

    def __init__(self):
        self.decision_tree = {}
        self.problem_solving_cache = {}
        self.goal_stack = []
        self.current_plan = []
        self.meta_knowledge = {
            "resource_locations": {},
            "player_behaviors": {},
            "map_knowledge": {},
            "time_patterns": {},
        }

    def analyze_situation(
        self, state: GameState, vision: List[List[str]]
    ) -> Dict[str, Any]:
        """Analyze current situation and provide insights"""
        analysis = {
            "immediate_threats": [],
            "opportunities": [],
            "resource_availability": {},
            "strategic_position": "neutral",
            "urgency_level": 0,
        }

        # Analyze food situation
        food_level = state.inventory.get("food", 0)
        if food_level < 20:
            analysis["immediate_threats"].append("low_food")
            analysis["urgency_level"] += 3

        # Analyze resources in vision
        for tile_idx, tile_content in enumerate(vision):
            for item in tile_content:
                if item in ResourceType._value2member_map_:
                    if item not in analysis["resource_availability"]:
                        analysis["resource_availability"][item] = []
                    analysis["resource_availability"][item].append(tile_idx)
                    analysis["opportunities"].append(f"resource_{item}_at_{tile_idx}")

        # Analyze elevation potential
        needed_resources = self.calculate_elevation_needs(state)
        if not needed_resources:
            analysis["opportunities"].append("can_elevate")
            analysis["urgency_level"] += 2

        return analysis

    def calculate_elevation_needs(self, state: GameState) -> Dict[str, int]:
        """Calculate what resources are needed for elevation"""
        elevation_requirements = {
            1: {
                "players": 1,
                "linemate": 1,
                "deraumere": 0,
                "sibur": 0,
                "mendiane": 0,
                "phiras": 0,
                "thystame": 0,
            },
            2: {
                "players": 2,
                "linemate": 1,
                "deraumere": 1,
                "sibur": 1,
                "mendiane": 0,
                "phiras": 0,
                "thystame": 0,
            },
            3: {
                "players": 2,
                "linemate": 2,
                "deraumere": 0,
                "sibur": 1,
                "mendiane": 0,
                "phiras": 2,
                "thystame": 0,
            },
            4: {
                "players": 4,
                "linemate": 1,
                "deraumere": 1,
                "sibur": 2,
                "mendiane": 0,
                "phiras": 1,
                "thystame": 0,
            },
            5: {
                "players": 4,
                "linemate": 1,
                "deraumere": 2,
                "sibur": 1,
                "mendiane": 3,
                "phiras": 0,
                "thystame": 0,
            },
            6: {
                "players": 6,
                "linemate": 1,
                "deraumere": 2,
                "sibur": 3,
                "mendiane": 0,
                "phiras": 1,
                "thystame": 0,
            },
            7: {
                "players": 6,
                "linemate": 2,
                "deraumere": 2,
                "sibur": 2,
                "mendiane": 2,
                "phiras": 2,
                "thystame": 1,
            },
        }

        if state.level >= 7:
            return {}

        requirements = elevation_requirements[state.level]
        needed = {}

        for resource in [
            "linemate",
            "deraumere",
            "sibur",
            "mendiane",
            "phiras",
            "thystame",
        ]:
            required = requirements[resource]
            current = state.inventory.get(resource, 0)
            if required > current:
                needed[resource] = required - current

        return needed

    def create_plan(
        self, goal: str, state: GameState, analysis: Dict[str, Any]
    ) -> List[ActionType]:
        """Create a sequence of actions to achieve a goal"""
        plan = []

        if goal == "survive":
            if analysis["urgency_level"] >= 3:
                plan.extend([ActionType.LOOK, ActionType.MOVE_FORWARD, ActionType.LOOK])
        elif goal == "gather_food":
            if "food" in analysis["resource_availability"]:
                food_locations = analysis["resource_availability"]["food"]
                closest_food = min(food_locations)
                if closest_food == 0:
                    plan.append(ActionType.TAKE_RESOURCE)
                else:
                    plan.extend([ActionType.MOVE_FORWARD] * (closest_food // 2 + 1))
                    plan.append(ActionType.TAKE_RESOURCE)
        elif goal == "gather_resources":
            needed = self.calculate_elevation_needs(state)
            for resource, amount in needed.items():
                if resource in analysis["resource_availability"]:
                    plan.append(ActionType.TAKE_RESOURCE)
        elif goal == "elevate":
            plan.extend([ActionType.BROADCAST, ActionType.INCANTATION])
        elif goal == "explore":
            # Intelligent exploration pattern
            if random.random() < 0.3:
                plan.append(
                    ActionType.TURN_LEFT
                    if random.random() < 0.5
                    else ActionType.TURN_RIGHT
                )
            plan.extend([ActionType.LOOK, ActionType.MOVE_FORWARD])

        return plan

    def update_meta_knowledge(self, state: GameState, action: ActionType, result: str):
        """Update meta-knowledge based on experiences"""
        turn = state.turn_count

        # Update time patterns
        if action.name not in self.meta_knowledge["time_patterns"]:
            self.meta_knowledge["time_patterns"][action.name] = []
        self.meta_knowledge["time_patterns"][action.name].append(turn)

        # Update map knowledge
        pos_key = f"{state.position.x},{state.position.y}"
        if pos_key not in self.meta_knowledge["map_knowledge"]:
            self.meta_knowledge["map_knowledge"][pos_key] = {
                "visited_count": 0,
                "resources_found": [],
            }
        self.meta_knowledge["map_knowledge"][pos_key]["visited_count"] += 1
