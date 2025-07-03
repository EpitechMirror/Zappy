##
## TeamCoordinator.py for AI New in /home/adrien/Desktop/AI New
##
## Made by adrien.marette@epitech.eu
## Login   <adrien.marette@epitech.eu>
##
## Started on  Fri Jul 4 12:00:00 AM 2025 adrien.marette@epitech.eu
## Last update Sat Jul 4 12:46:49 AM 2025 adrien.marette@epitech.eu
##

import random
import time
from Team import Team, TeamRole, elevation_requirements

class TeamCoordinator(Team):
    """Coordinator AI that manages team strategy and elevations."""
    
    def __init__(self, host: str, port: int, team_name: str, is_spawned: bool = False):
        super().__init__(host, port, team_name, TeamRole.COORDINATOR, is_spawned)
        self.elevation_plans = {}
        self.resource_assignments = {}
        self.team_strategy = "exploration"  # exploration, collection, elevation
        self.last_strategy_update = 0
        self.coordination_cooldown = 0
        
    def execute_role_strategy(self):
        """Execute coordinator-specific strategy."""
        # Update team strategy
        if time.time() - self.last_strategy_update > 30:  # Every 30 seconds
            self.update_team_strategy()
            self.last_strategy_update = time.time()
        
        # Coordinate elevations
        self.manage_team_elevations()
        
        # Optimize resource distribution
        if time.time() > self.coordination_cooldown:
            self.coordinate_resource_distribution()
            self.coordination_cooldown = time.time() + 10  # 10 second cooldown
        
        # Monitor team performance
        self.monitor_team_performance()
        
        # Execute personal strategy based on team needs
        self.execute_personal_strategy()

    def update_team_strategy(self):
        """Update overall team strategy based on current situation."""
        team_size = len(self.team_members)
        avg_level = sum(member.level for member in self.team_members.values()) / max(team_size, 1)
        
        # Determine strategy based on team state
        if avg_level < 2:
            self.team_strategy = "exploration"
        elif avg_level < 4:
            self.team_strategy = "collection"
        else:
            self.team_strategy = "elevation"
            
        # Broadcast strategy update
        self.broadcast_team_message({
            "type": "strategy_update",
            "id": self.player_id,
            "strategy": self.team_strategy,
            "avg_level": avg_level,
            "team_size": team_size
        })
        
        print(f"Team strategy updated to: {self.team_strategy}")

    def manage_team_elevations(self):
        """Manage and coordinate team elevations."""
        # Find members ready for elevation
        elevation_candidates = self.find_elevation_candidates()
        
        if elevation_candidates:
            # Group by level for coordinated elevations
            level_groups = {}
            for member in elevation_candidates:
                level = member.level
                if level not in level_groups:
                    level_groups[level] = []
                level_groups[level].append(member)
            
            # Coordinate elevations for each level
            for level, members in level_groups.items():
                if len(members) >= elevation_requirements[level]["players"]:
                    self.organize_elevation(members, level)

    def find_elevation_candidates(self) -> list:
        """Find team members ready for elevation."""
        candidates = []
        
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
                    candidates.append(member)
        
        return candidates

    def organize_elevation(self, members: list, target_level: int):
        """Organize a coordinated elevation."""
        requirements = elevation_requirements.get(target_level, {})
        players_needed = requirements.get("players", 1)
        
        if len(members) >= players_needed:
            # Select best candidates
            selected_members = members[:players_needed]
            
            # Calculate center position for gathering
            center_pos = self.calculate_center_position(selected_members)
            
            # Create elevation plan
            plan_id = f"elevation_{target_level}_{int(time.time())}"
            self.elevation_plans[plan_id] = {
                "level": target_level,
                "members": selected_members,
                "center": center_pos,
                "status": "gathering",
                "created": time.time()
            }
            
            # Broadcast elevation plan
            self.broadcast_team_message({
                "type": "elevation_plan",
                "id": self.player_id,
                "plan_id": plan_id,
                "target_level": target_level,
                "gathering_point": center_pos,
                "members_needed": players_needed,
                "requirements": requirements
            })
            
            print(f"Organized elevation for level {target_level} with {len(selected_members)} members")

    def calculate_center_position(self, members: list) -> tuple:
        """Calculate optimal center position for a group of members."""
        if not members:
            return self.position
            
        total_x = sum(member.position[0] for member in members)
        total_y = sum(member.position[1] for member in members)
        
        center_x = total_x // len(members)
        center_y = total_y // len(members)
        
        return (center_x, center_y)

    def coordinate_resource_distribution(self):
        """Coordinate resource distribution among team members."""
        # Analyze team resource needs
        team_needs = self.analyze_team_resource_needs()
        
        if team_needs:
            # Find members with excess resources
            for resource, total_needed in team_needs.items():
                excess_holders = self.find_excess_holders(resource)
                
                if excess_holders:
                    # Coordinate resource transfers
                    self.coordinate_resource_transfers(resource, excess_holders, total_needed)

    def analyze_team_resource_needs(self) -> dict:
        """Analyze what resources the team needs."""
        team_needs = {}
        
        for member in self.team_members.values():
            if member.level < 8:
                requirements = elevation_requirements.get(member.level, {})
                
                for resource, needed in requirements.items():
                    if resource != "players":
                        current = member.inventory.get(resource, 0)
                        if current < needed:
                            shortage = needed - current
                            team_needs[resource] = team_needs.get(resource, 0) + shortage
        
        return team_needs

    def find_excess_holders(self, resource: str) -> list:
        """Find team members with excess of a specific resource."""
        excess_holders = []
        
        for member in self.team_members.values():
            requirements = elevation_requirements.get(member.level, {})
            needed = requirements.get(resource, 0)
            current = member.inventory.get(resource, 0)
            
            if current > needed + 2:  # Has excess (keeping 2 as buffer)
                excess_holders.append({
                    "member": member,
                    "excess": current - needed - 2
                })
        
        return sorted(excess_holders, key=lambda x: x["excess"], reverse=True)

    def coordinate_resource_transfers(self, resource: str, excess_holders: list, total_needed: int):
        """Coordinate resource transfers between team members."""
        # Broadcast resource coordination request
        self.broadcast_team_message({
            "type": "resource_coordination",
            "id": self.player_id,
            "resource": resource,
            "total_needed": total_needed,
            "excess_holders": [holder["member"].id for holder in excess_holders]
        })

    def monitor_team_performance(self):
        """Monitor team performance and adjust strategy."""
        # Check team member health
        starving_members = [member for member in self.team_members.values() 
                           if member.inventory.get("food", 0) < 20]
        
        if len(starving_members) > 2:
            # Emergency food distribution
            self.emergency_food_distribution(starving_members)
        
        # Check team progress
        team_levels = [member.level for member in self.team_members.values()]
        if team_levels:
            avg_level = sum(team_levels) / len(team_levels)
            if avg_level < 2 and time.time() > 300:  # After 5 minutes, if avg level < 2
                self.broadcast_team_message({
                    "type": "performance_alert",
                    "id": self.player_id,
                    "alert": "slow_progress",
                    "suggestion": "focus_on_food_and_basic_resources"
                })

    def emergency_food_distribution(self, starving_members: list):
        """Emergency food distribution for starving members."""
        # Find members with excess food
        food_holders = [member for member in self.team_members.values()
                       if member.inventory.get("food", 0) > 50]
        
        if food_holders:
            # Coordinate emergency food sharing
            self.broadcast_team_message({
                "type": "emergency_food_distribution",
                "id": self.player_id,
                "starving_members": [member.id for member in starving_members],
                "food_holders": [member.id for member in food_holders]
            })

    def execute_personal_strategy(self):
        """Execute personal strategy based on team needs."""
        # Check what the team needs most
        if self.team_strategy == "exploration":
            # Help with exploration
            self.send_command("Look", "normal")
            if random.random() < 0.5:
                self.send_command("Forward", "normal")
            else:
                self.send_command("Right", "normal")
                
        elif self.team_strategy == "collection":
            # Help with resource collection
            needed_resources = list(self.analyze_team_resource_needs().keys())
            if needed_resources:
                resource = needed_resources[0]
                if random.random() < 0.3:
                    self.send_command(f"take {resource}", "normal")
            
            self.send_command("Look", "normal")
            if random.random() < 0.4:
                self.send_command("Forward", "normal")
                
        elif self.team_strategy == "elevation":
            # Focus on elevation support
            if self.is_ready_for_elevation():
                # Look for elevation opportunities
                self.send_command("Look", "high")
            else:
                # Collect resources needed for elevation
                needed = self.calculate_needed_resources()
                if needed:
                    resource = needed[0]
                    if random.random() < 0.3:
                        self.send_command(f"take {resource}", "normal")

    def process_team_message(self, message: str, direction: int):
        """Process team messages with coordinator-specific handling."""
        super().process_team_message(message, direction)
        
        if message.startswith("TEAM:"):
            try:
                import json
                data = json.loads(message[5:])
                msg_type = data.get("type")
                
                if msg_type == "elevation_request":
                    self.handle_elevation_request_coordination(data, direction)
                elif msg_type == "resource_crisis":
                    self.handle_resource_crisis(data, direction)
                elif msg_type == "strategy_feedback":
                    self.handle_strategy_feedback(data, direction)
                    
            except:
                pass

    def handle_elevation_request_coordination(self, data: dict, direction: int):
        """Handle elevation requests with coordination."""
        requester_level = data.get("level", 1)
        requester_pos = tuple(data.get("position", (0, 0)))
        
        # Check if we can coordinate this elevation
        same_level_members = [member for member in self.team_members.values()
                             if member.level == requester_level]
        
        requirements = elevation_requirements.get(requester_level, {})
        players_needed = requirements.get("players", 1)
        
        if len(same_level_members) >= players_needed:
            # We can coordinate this elevation
            self.organize_elevation(same_level_members, requester_level)

    def handle_resource_crisis(self, data: dict, direction: int):
        """Handle resource crisis situations."""
        crisis_type = data.get("crisis_type", "unknown")
        affected_members = data.get("affected_members", [])
        
        if crisis_type == "food_shortage":
            self.emergency_food_distribution(affected_members)
        elif crisis_type == "resource_shortage":
            needed_resource = data.get("resource", "linemate")
            self.coordinate_resource_distribution()

    def handle_strategy_feedback(self, data: dict, direction: int):
        """Handle strategy feedback from team members."""
        feedback = data.get("feedback", "")
        suggestion = data.get("suggestion", "")
        
        print(f"Strategy feedback received: {feedback}")
        
        # Adjust strategy based on feedback
        if "need_more_exploration" in feedback:
            self.team_strategy = "exploration"
        elif "need_more_resources" in feedback:
            self.team_strategy = "collection"
