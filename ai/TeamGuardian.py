##
## TeamGuardian.py for AI New in /home/adrien/Desktop/AI New
##
## Made by adrien.marette@epitech.eu
## Login   <adrien.marette@epitech.eu>
##
## Started on  Fri Jul 4 12:00:00 AM 2025 adrien.marette@epitech.eu
## Last update Sat Jul 4 12:10:20 AM 2025 adrien.marette@epitech.eu
##

import random
import time
from Team import Team, TeamRole, elevation_requirements

class TeamGuardian(Team):
    """Guardian AI that focuses on protecting teammates and securing areas."""
    
    def __init__(self, host: str, port: int, team_name: str, is_spawned: bool = False):
        super().__init__(host, port, team_name, TeamRole.GUARDIAN, is_spawned)
        self.protected_areas = []
        self.patrol_route = []
        self.current_patrol_index = 0
        self.threat_level = 0
        self.last_threat_check = 0
        
    def execute_role_strategy(self):
        """Execute guardian-specific strategy."""
        # Monitor team status
        if random.random() < 0.1:
            self.send_command("Inventory", "normal")
        
        # Check for threats and team needs
        self.assess_threat_level()
        
        # Find team members in need of protection
        vulnerable_members = self.find_vulnerable_members()
        
        if vulnerable_members:
            self.protect_vulnerable_members(vulnerable_members)
        else:
            # No immediate threats, patrol territory
            self.patrol_territory()
        
        # Secure resources for team elevations
        if self.should_secure_elevation_site():
            self.secure_elevation_site()

    def assess_threat_level(self):
        """Assess current threat level based on team status."""
        current_time = time.time()
        
        if current_time - self.last_threat_check < 5:
            return
            
        self.last_threat_check = current_time
        
        # Check team member status
        threatened_members = 0
        for member in self.team_members.values():
            if member.inventory.get("food", 0) < 20:
                threatened_members += 1
        
        # Update threat level
        if threatened_members > 2:
            self.threat_level = 3  # High threat
        elif threatened_members > 0:
            self.threat_level = 2  # Medium threat
        else:
            self.threat_level = 1  # Low threat
            
        if self.threat_level > 1:
            print(f"Threat level: {self.threat_level}, {threatened_members} members at risk")

    def find_vulnerable_members(self) -> list:
        """Find team members who need protection."""
        vulnerable = []
        
        for member in self.team_members.values():
            # Check if member is in danger
            if (member.inventory.get("food", 0) < 15 or 
                member.status == "elevation" or
                member.level < self.level - 1):
                vulnerable.append(member)
        
        return vulnerable

    def protect_vulnerable_members(self, vulnerable_members: list):
        """Protect vulnerable team members."""
        # Move towards the most vulnerable member
        if vulnerable_members:
            target_member = min(vulnerable_members, 
                              key=lambda m: m.inventory.get("food", 0))
            
            # Move towards the member
            self.set_target_position(target_member.position)
            
            # Share resources if possible
            if self.inventory["food"] > 50:
                self.drop_resource_for_team("food", 10)
                
            print(f"Protecting member {target_member.id} at {target_member.position}")

    def patrol_territory(self):
        """Patrol the team's territory."""
        if not self.patrol_route:
            self.establish_patrol_route()
        
        if self.patrol_route:
            current_target = self.patrol_route[self.current_patrol_index]
            
            # Move towards patrol point
            if self.calculate_distance(self.position, current_target) < 2:
                # Reached patrol point, move to next
                self.current_patrol_index = (self.current_patrol_index + 1) % len(self.patrol_route)
                print(f"Patrol point reached, moving to next point")
            else:
                self.move_towards_target_position(current_target)
        else:
            # No patrol route, move randomly
            self.send_command("Forward", "normal")

    def establish_patrol_route(self):
        """Establish a patrol route based on team member positions."""
        if not self.team_members:
            return
            
        # Create patrol route around team members
        member_positions = [member.position for member in self.team_members.values()]
        
        if member_positions:
            # Calculate center of team
            center_x = sum(pos[0] for pos in member_positions) // len(member_positions)
            center_y = sum(pos[1] for pos in member_positions) // len(member_positions)
            
            # Create patrol points around the center
            self.patrol_route = [
                (center_x - 3, center_y - 3),
                (center_x + 3, center_y - 3),
                (center_x + 3, center_y + 3),
                (center_x - 3, center_y + 3)
            ]
            
            print(f"Established patrol route around team center: {center_x}, {center_y}")

    def should_secure_elevation_site(self) -> bool:
        """Check if we should secure an elevation site."""
        # Check if any team member is ready for elevation
        for member in self.team_members.values():
            if member.level < 8:  # Max level is 8
                requirements = elevation_requirements.get(member.level, {})
                ready = True
                
                for resource, needed in requirements.items():
                    if resource != "players":
                        if member.inventory.get(resource, 0) < needed:
                            ready = False
                            break
                
                if ready:
                    return True
        
        return False

    def secure_elevation_site(self):
        """Secure an area for team elevation."""
        # Look for a good elevation site
        self.send_command("Look", "high")
        
        # Move to secure the area
        self.send_command("Forward", "normal")
        
        # Broadcast secure site
        self.broadcast_team_message({
            "type": "secure_site",
            "id": self.player_id,
            "position": self.position,
            "secured_for": "elevation",
            "timestamp": time.time()
        })

    def move_towards_target_position(self, target_pos):
        """Move towards a specific target position."""
        current_x, current_y = self.position
        target_x, target_y = target_pos
        
        # Calculate direction to target
        dx = target_x - current_x
        dy = target_y - current_y
        
        # Move towards target
        if abs(dx) > abs(dy):
            if dx > 0:
                self.turn_to_direction(2)  # East
            else:
                self.turn_to_direction(4)  # West
        else:
            if dy > 0:
                self.turn_to_direction(3)  # South
            else:
                self.turn_to_direction(1)  # North
                
        self.send_command("Forward", "normal")

    def drop_resource_for_team(self, resource: str, amount: int):
        """Drop resources for team members."""
        for _ in range(min(amount, self.inventory.get(resource, 0))):
            self.send_command(f"Set {resource}", "normal")
            
        # Announce the drop
        self.broadcast_team_message({
            "type": "resource_drop",
            "id": self.player_id,
            "resource": resource,
            "amount": amount,
            "position": self.position,
            "purpose": "team_support"
        })

    def process_team_message(self, message: str, direction: int):
        """Process team messages with guardian-specific handling."""
        super().process_team_message(message, direction)
        
        if message.startswith("TEAM:"):
            try:
                import json
                data = json.loads(message[5:])
                msg_type = data.get("type")
                
                if msg_type == "distress":
                    self.handle_distress_call(data, direction)
                elif msg_type == "elevation_request":
                    self.handle_elevation_protection(data, direction)
                elif msg_type == "threat_alert":
                    self.handle_threat_alert(data, direction)
                    
            except:
                pass

    def handle_distress_call(self, data: dict, direction: int):
        """Handle distress calls from team members."""
        distressed_member_pos = tuple(data.get("position", (0, 0)))
        distress_type = data.get("distress_type", "general")
        
        print(f"Distress call received: {distress_type} at {distressed_member_pos}")
        
        # Immediately move to help
        self.set_target_position(distressed_member_pos)
        
        # Send help based on distress type
        if distress_type == "food":
            if self.inventory["food"] > 30:
                self.planned_food_drop = True
        elif distress_type == "resources":
            # Prepare to share resources
            self.planned_resource_share = True

    def handle_elevation_protection(self, data: dict, direction: int):
        """Handle elevation protection requests."""
        elevation_pos = tuple(data.get("position", (0, 0)))
        required_level = data.get("level", 1)
        
        # Move to protect the elevation site
        if self.level >= required_level:
            self.set_target_position(elevation_pos)
            print(f"Moving to protect elevation at {elevation_pos}")
            
            # Confirm protection
            self.broadcast_team_message({
                "type": "protection_confirm",
                "id": self.player_id,
                "target_position": elevation_pos,
                "eta": self.calculate_travel_time(elevation_pos)
            })

    def handle_threat_alert(self, data: dict, direction: int):
        """Handle threat alerts from team members."""
        threat_pos = tuple(data.get("position", (0, 0)))
        threat_type = data.get("threat_type", "unknown")
        
        print(f"Threat alert: {threat_type} at {threat_pos}")
        
        # Increase threat level
        self.threat_level = min(self.threat_level + 1, 3)
        
        # Move to investigate
        self.set_target_position(threat_pos)

    def emergency_response(self):
        """Emergency response for critical situations."""
        # Drop all non-essential resources and focus on food
        for resource in ["sibur", "mendiane", "phiras", "thystame"]:
            if self.inventory.get(resource, 0) > 2:
                self.send_command(f"Set {resource}", "normal")
                
        # Share food with team
        if self.inventory["food"] > 20:
            self.drop_resource_for_team("food", 5)
