##
## TeamExplorer.py for AI New in /home/adrien/Desktop/AI New
##
## Made by adrien.marette@epitech.eu
## Login   <adrien.marette@epitech.eu>
##
## Started on  Fri Jul 4 12:00:00 AM 2025 adrien.marette@epitech.eu
## Last update Sat Jul 4 12:46:48 AM 2025 adrien.marette@epitech.eu
##

import random
import time
from Team import Team, TeamRole

class TeamExplorer(Team):
    """Explorer AI that focuses on mapping the world and finding resources."""
    
    def __init__(self, host: str, port: int, team_name: str, is_spawned: bool = False):
        super().__init__(host, port, team_name, TeamRole.EXPLORER, is_spawned)
        self.exploration_pattern = 0
        self.visited_positions = set()
        self.exploration_targets = []
        self.mapping_mode = True
        
    def execute_role_strategy(self):
        """Execute explorer-specific strategy."""
        # Prioritize exploration and resource discovery
        if random.random() < 0.1:
            self.send_command("Inventory", "normal")
        
        if self.mapping_mode:
            self.systematic_exploration()
        else:
            self.intelligent_exploration()
        
        # Occasionally share discovered resources with team
        if random.random() < 0.05:
            self.share_resource_discoveries()

    def systematic_exploration(self):
        """Perform systematic exploration of the world."""
        # Look around first to gather information
        self.send_command("Look", "normal")
        
        # Try to take any resources we find while exploring
        needed_resources = self.calculate_needed_resources()
        if needed_resources:
            resource = needed_resources[0]
            self.send_command(f"Take {resource}", "normal")
        
        # Systematic movement pattern - create a search grid
        if not hasattr(self, 'exploration_pattern'):
            self.exploration_pattern = 0
        
        # Create a more systematic exploration pattern
        pattern_step = self.exploration_pattern % 12
        
        if pattern_step < 4:
            self.send_command("Forward", "normal")
        elif pattern_step == 4:
            self.send_command("Right", "normal")
        elif pattern_step < 8:
            self.send_command("Forward", "normal")
        elif pattern_step == 8:
            self.send_command("Right", "normal")
        elif pattern_step < 11:
            self.send_command("Forward", "normal")
        else:
            self.send_command("Left", "normal")
        
        self.exploration_pattern += 1

    def intelligent_exploration(self):
        """Intelligent exploration based on discovered information."""
        # Mark current position as visited
        self.visited_positions.add(self.position)
        
        # Always try to collect resources while exploring
        self.send_command("Look", "normal")
        needed_resources = self.calculate_needed_resources()
        if needed_resources:
            resource = needed_resources[0]
            self.send_command(f"Take {resource}", "normal")
        
        # Look for unexplored areas
        if self.exploration_targets:
            target = self.exploration_targets[0]
            self.move_towards_target_position(target)
            
            # Remove target if reached
            if self.calculate_distance(self.position, target) < 2:
                self.exploration_targets.pop(0)
        else:
            # No specific targets, explore randomly
            self.advanced_exploration_pattern()

    def advanced_exploration_pattern(self):
        """Advanced exploration pattern that avoids revisiting areas."""
        # Choose direction that leads to unexplored areas
        directions = ["Forward", "Left", "Right"]
        
        # Weight directions based on exploration history
        best_direction = random.choice(directions)
        
        # Check if we've been in this general area before
        nearby_visited = sum(1 for pos in self.visited_positions 
                           if self.calculate_distance(self.position, pos) < 5)
        
        if nearby_visited > 10:  # Too many visits in this area
            # Try to move to a different area
            self.send_command("Right", "normal")
            self.send_command("Forward", "normal")
        else:
            self.send_command(best_direction, "normal")

    def share_resource_discoveries(self):
        """Share discovered resources with team members."""
        if self.resource_map:
            # Share the most valuable resource discoveries
            valuable_resources = ["linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame"]
            
            for tile_index, resources in self.resource_map.items():
                for resource in valuable_resources:
                    if resource in resources:
                        self.broadcast_team_message({
                            "type": "resource_discovery",
                            "id": self.player_id,
                            "resource": resource,
                            "tile_index": tile_index,
                            "position": self.position,
                            "timestamp": time.time()
                        })

    def move_towards_target_position(self, target_pos):
        """Move towards a specific target position."""
        current_x, current_y = self.position
        target_x, target_y = target_pos
        
        # Simple pathfinding
        if abs(target_x - current_x) > abs(target_y - current_y):
            if target_x > current_x:
                self.turn_to_direction(2)  # East
            else:
                self.turn_to_direction(4)  # West
        else:
            if target_y > current_y:
                self.turn_to_direction(3)  # South
            else:
                self.turn_to_direction(1)  # North
                
        self.send_command("Forward", "normal")

    def process_team_message(self, message: str, direction: int):
        """Process team messages with explorer-specific handling."""
        super().process_team_message(message, direction)
        
        if message.startswith("TEAM:"):
            try:
                import json
                data = json.loads(message[5:])
                msg_type = data.get("type")
                
                if msg_type == "resource_request":
                    # As an explorer, help locate requested resources
                    resource = data.get("resource")
                    if resource in ["linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame"]:
                        self.prioritize_resource_search(resource)
                        
            except:
                pass

    def prioritize_resource_search(self, resource: str):
        """Prioritize searching for a specific resource."""
        print(f"Prioritizing search for {resource}")
        # Switch to resource-focused exploration
        self.mapping_mode = False
        
        # Add resource search behavior
        for _ in range(3):
            self.send_command("Look", "high")
            if random.random() < 0.5:
                self.send_command("Forward", "normal")
            else:
                self.send_command("Right", "normal")
