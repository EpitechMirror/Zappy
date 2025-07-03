##
## TeamCollector.py for AI New in /home/adrien/Desktop/AI New
##
## Made by adrien.marette@epitech.eu
## Login   <adrien.marette@epitech.eu>
##
## Started on  Fri Jul 4 12:00:00 AM 2025 adrien.marette@epitech.eu
## Last update Sat Jul 4 12:57:02 AM 2025 adrien.marette@epitech.eu
##

import random
import time
from Team import Team, TeamRole, elevation_requirements

class TeamCollector(Team):
    """Collector AI that focuses on resource collection and distribution."""
    
    def __init__(self, host: str, port: int, team_name: str, is_spawned: bool = False):
        super().__init__(host, port, team_name, TeamRole.COLLECTOR, is_spawned)
        self.collection_priorities = ["food", "linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame"]
        self.resource_targets = []
        self.sharing_cooldown = 0
        
    def execute_role_strategy(self):
        """Execute collector-specific strategy."""
        # Focus on resource collection and sharing
        if random.random() < 0.1:
            self.send_command("Inventory", "high")
        
        # Prioritize food collection if running low
        if self.inventory["food"] < 30:
            self.prioritize_food_collection()
        else:
            # Collect resources based on team needs
            needed_resources = self.get_needed_resources_for_team()
            if needed_resources:
                self.collect_specific_resources(needed_resources)
            else:
                self.collect_general_resources()
        
        # Share resources with team members
        if time.time() > self.sharing_cooldown:
            self.share_resources_with_team()
            self.sharing_cooldown = time.time() + 5  # 5 second cooldown

    def prioritize_food_collection(self):
        """Prioritize food collection when running low."""
        self.send_command("Look", "high")
        
        # Always try to take food from current position first
        self.send_command("Take food", "high")
        
        # Move around to find food - more balanced movement
        move_choice = random.random()
        if move_choice < 0.4:
            self.send_command("Forward", "high")
        elif move_choice < 0.7:
            self.send_command("Right", "normal")
        else:
            self.send_command("Left", "normal")

    def get_needed_resources_for_team(self) -> list:
        """Determine what resources the team needs most."""
        team_needs = {}
        
        # Analyze our own needs first
        requirements = elevation_requirements.get(self.level, {})
        for resource, needed in requirements.items():
            if resource != "players":
                current = self.inventory.get(resource, 0)
                if current < needed:
                    team_needs[resource] = needed - current
        
        # Add general resource needs
        for resource in ["linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame"]:
            if resource not in team_needs:
                team_needs[resource] = 1  # Always good to have some
        
        # Sort by priority
        return sorted(team_needs.keys(), key=lambda x: team_needs[x], reverse=True)

    def collect_specific_resources(self, resources: list):
        """Collect specific resources for team."""
        self.send_command("Look", "normal")
        
        # Look for the most needed resources
        for resource in resources[:3]:  # Focus on top 3 needed resources
            if random.random() < 0.2:
                self.send_command(f"take {resource}", "high")
        
        # Move to find more resources
        if random.random() < 0.4:
            self.send_command("Forward", "normal")
        else:
            self.turn_and_search()

    def collect_general_resources(self):
        """Collect resources in general."""
        self.send_command("Look", "normal")
        
        # Try to collect all types of resources
        for resource in self.collection_priorities:
            if random.random() < 0.15:
                self.send_command(f"take {resource}", "normal")
        
        # Move to explore more areas
        if random.random() < 0.5:
            self.send_command("Forward", "normal")
        else:
            self.send_command("Right", "normal")

    def turn_and_search(self):
        """Turn and search for resources."""
        direction = random.choice(["Left", "Right"])
        self.send_command(direction, "normal")
        self.send_command("Look", "normal")

    def share_resources_with_team(self):
        """Share excess resources with team members."""
        if not self.resource_sharing_enabled:
            return
            
        # Check what we have in excess
        excess_resources = self.calculate_excess_resources()
        
        if excess_resources:
            # Broadcast availability
            for resource, amount in excess_resources.items():
                if amount > 0:
                    self.broadcast_team_message({
                        "type": "resource_offer",
                        "id": self.player_id,
                        "resource": resource,
                        "amount": amount,
                        "position": self.position,
                        "timestamp": time.time()
                    })

    def calculate_excess_resources(self) -> dict:
        """Calculate what resources we have in excess."""
        excess = {}
        requirements = elevation_requirements.get(self.level, {})
        
        for resource, current_amount in self.inventory.items():
            if resource == "food":
                if current_amount > 80:  # Keep 80 food as safety margin
                    excess[resource] = current_amount - 80
            elif resource in requirements:
                needed = requirements[resource]
                if current_amount > needed + 2:  # Keep 2 extra as margin
                    excess[resource] = current_amount - needed - 2
            else:
                if current_amount > 3:  # Keep 3 of each as general stock
                    excess[resource] = current_amount - 3
                    
        return excess

    def process_team_message(self, message: str, direction: int):
        """Process team messages with collector-specific handling."""
        super().process_team_message(message, direction)
        
        if message.startswith("TEAM:"):
            try:
                import json
                data = json.loads(message[5:])
                msg_type = data.get("type")
                
                if msg_type == "resource_request":
                    self.handle_resource_request(data, direction)
                elif msg_type == "resource_discovery":
                    self.handle_resource_discovery(data, direction)
                    
            except:
                pass

    def handle_resource_request(self, data: dict, direction: int):
        """Handle resource requests from team members."""
        requested_resource = data.get("resource")
        amount_needed = data.get("amount", 1)
        requester_pos = tuple(data.get("position", (0, 0)))
        
        # Check if we can fulfill the request
        current_amount = self.inventory.get(requested_resource, 0)
        excess = self.calculate_excess_resources().get(requested_resource, 0)
        
        if excess >= amount_needed:
            # Move towards requester to share
            self.set_target_position(requester_pos)
            print(f"Moving to share {amount_needed} {requested_resource}")
            
            # Confirm we can help
            self.broadcast_team_message({
                "type": "resource_confirm",
                "id": self.player_id,
                "resource": requested_resource,
                "amount": min(excess, amount_needed),
                "eta": self.calculate_travel_time(requester_pos)
            })

    def handle_resource_discovery(self, data: dict, direction: int):
        """Handle resource discovery notifications."""
        resource = data.get("resource")
        discoverer_pos = tuple(data.get("position", (0, 0)))
        
        # If we need this resource, move towards the discovery
        needed_resources = self.get_needed_resources_for_team()
        if resource in needed_resources[:5]:  # Top 5 needed resources
            self.set_target_position(discoverer_pos)
            print(f"Moving to collect discovered {resource}")

    def drop_resource_for_team(self, resource: str, amount: int):
        """Drop resources for team members to pick up."""
        for _ in range(amount):
            self.send_command(f"Set {resource}", "normal")
            
        # Announce the drop
        self.broadcast_team_message({
            "type": "resource_drop",
            "id": self.player_id,
            "resource": resource,
            "amount": amount,
            "position": self.position,
            "timestamp": time.time()
        })

    def optimize_inventory(self):
        """Optimize inventory by dropping less important items."""
        # Calculate inventory utilization
        total_items = sum(self.inventory.values()) - self.inventory["food"]
        
        if total_items > 15:  # If inventory is getting full
            # Drop excess of less important resources
            for resource in ["sibur", "mendiane", "phiras", "thystame"]:
                if self.inventory.get(resource, 0) > 5:
                    self.send_command(f"Set {resource}", "normal")
                    print(f"Dropping excess {resource}")
                    break
