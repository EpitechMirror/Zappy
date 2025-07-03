##
## Team.py for AI New in /home/adrien/Desktop/AI New
##
## Made by adrien.marette@epitech.eu
## Login   <adrien.marette@epitech.eu>
##
## Started on  Thu Jul 3 11:38:37 PM 2025 adrien.marette@epitech.eu
## Last update Sat Jul 4 12:57:01 AM 2025 adrien.marette@epitech.eu
##

import socket
import time
import json
import random
import subprocess
import sys
import os
import argparse
from enum import Enum
from dataclasses import dataclass
from typing import Dict, List, Set, Tuple, Optional

class TeamRole(Enum):
    EXPLORER = "explorer"
    COLLECTOR = "collector"
    GUARDIAN = "guardian"
    COORDINATOR = "coordinator"

@dataclass
class TeamMember:
    id: int
    level: int
    position: Tuple[int, int]
    role: TeamRole
    inventory: Dict[str, int]
    status: str
    last_update: float

elevation_requirements = {
    1: {"players": 1, "linemate": 1, "deraumere": 0, "sibur": 0, "mendiane": 0, "phiras": 0, "thystame": 0},
    2: {"players": 2, "linemate": 1, "deraumere": 1, "sibur": 1, "mendiane": 0, "phiras": 0, "thystame": 0},
    3: {"players": 2, "linemate": 2, "deraumere": 0, "sibur": 1, "mendiane": 0, "phiras": 2, "thystame": 0},
    4: {"players": 4, "linemate": 1, "deraumere": 1, "sibur": 2, "mendiane": 0, "phiras": 1, "thystame": 0},
    5: {"players": 4, "linemate": 1, "deraumere": 2, "sibur": 1, "mendiane": 3, "phiras": 0, "thystame": 0},
    6: {"players": 6, "linemate": 1, "deraumere": 2, "sibur": 3, "mendiane": 0, "phiras": 1, "thystame": 0},
    7: {"players": 6, "linemate": 2, "deraumere": 2, "sibur": 2, "mendiane": 2, "phiras": 2, "thystame": 1}
}

class Team:
    def __init__(self, host: str, port: int, team_name: str, role: TeamRole, is_spawned: bool = False):
        self.host = host
        self.port = port
        self.team_name = team_name
        self.role = role
        self.socket = None
        self.is_spawned = is_spawned  # Flag to indicate if this AI was spawned from another AI
        
        # Individual state
        self.player_id = None
        self.level = 1
        self.direction = 1  # North
        self.position = (0, 0)
        self.world_size = (0, 0)
        self.available_slots = 0
        self.inventory = {
            "food": 10, "linemate": 0, "deraumere": 0, "sibur": 0,
            "mendiane": 0, "phiras": 0, "thystame": 0
        }
        
        # Team coordination
        self.team_members = {}
        self.team_objectives = {}
        self.resource_map = {}
        self.elevation_zones = {}
        
        # Strategy parameters
        self.cooperation_radius = 5
        self.resource_sharing_enabled = True
        self.formation_tactics = True
        
        # Command management - simplified without queue
        self.last_command_time = time.time()
        self.command_delay = 0.3  # Minimum delay between commands
        
        # Process spawning management
        self.last_spawn_time = 0
        self.spawned_processes = []
        self.spawn_cooldown = 3.0
        self.spawned_roles = []  # Track spawned roles for balance

    def connect_to_server(self) -> bool:
        """Connect to the Zappy server."""
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.settimeout(10)
            self.socket.connect((self.host, self.port))
            
            # Authentication
            welcome = self.receive_message()
            print(f"Server: {welcome}")
            
            self.send_message(self.team_name)
            print(f"Team: {self.team_name}")
            
            # Receive combined response: "slots\nX Y"
            response = self.receive_message()
            try:
                lines = response.split('\n')
                if len(lines) >= 2:
                    # Parse available slots from first line
                    self.available_slots = int(lines[0])
                    print(f"Available slots: {self.available_slots}")
                    
                    if self.available_slots <= 0:
                        raise Exception("No available team slots")
                    
                    # Parse world dimensions from second line (but we won't rely on them)
                    dimensions = lines[1].split()
                    if len(dimensions) >= 2:
                        self.world_size = (int(dimensions[0]), int(dimensions[1]))
                        print(f"World dimensions: {self.world_size[0]}x{self.world_size[1]} (note: AI doesn't rely on this)")
                    else:
                        print(f"Warning: Unexpected dimensions format: {lines[1]}")
                else:
                    raise Exception(f"Invalid response format - expected 'slots\\nX Y', got: {response}")
            except (ValueError, IndexError) as e:
                raise Exception(f"Failed to parse server response: {response}, error: {e}")
                
            # Set socket to non-blocking for game loop
            self.socket.settimeout(None)
            self.socket.setblocking(False)  # Set to non-blocking mode

            # Generate unique player ID based on connection time
            self.player_id = int(time.time() * 1000) % 10000
            
            print(f"Connected! ID: {self.player_id}, Role: {self.role.value}, Available slots: {self.available_slots}")
            print(f"This AI is spawned: {self.is_spawned}")
            
            # If there are available slots (existing eggs), spawn AIs to fill them
            # Both original and spawned AIs should help fill existing eggs
            if self.available_slots > 1:  # Keep 1 slot for ourselves
                eggs_to_fill = self.available_slots - 1
                print(f"Found {eggs_to_fill} existing eggs to fill, spawning AIs immediately...")
                
                for i in range(eggs_to_fill):  # Spawn all needed AIs
                    if self.spawn_new_ai_process(bypass_cooldown=True):
                        print(f"Spawned AI {i+1}/{eggs_to_fill} to fill existing egg")
                        self.available_slots -= 1
                        time.sleep(0.3)  # Small delay between spawns
                    else:
                        print(f"Failed to spawn AI {i+1}, stopping spawn sequence")
                        break
                
                print(f"Initial egg filling complete. Remaining slots: {self.available_slots}")
            else:
                print(f"No existing eggs to fill (available slots: {self.available_slots})")
            
            # Announce to team
            self.broadcast_team_message({
                "type": "join",
                "id": self.player_id,
                "role": self.role.value,
                "level": self.level,
                "position": self.position
            })
            
            return True
            
        except Exception as e:
            print(f"Connection failed: {e}")
            return False

    def send_message(self, message: str):
        """Send message to server."""
        if self.socket:
            try:
                self.socket.send(f"{message}\n".encode())
                print(f"Sent: {message}")
            except Exception as e:
                print(f"Send error: {e}")

    def receive_message(self) -> str:
        """Receive message from server."""
        if self.socket:
            try:
                data = self.socket.recv(4096)
                if not data:
                    raise Exception("Server closed connection")
                message = data.decode().strip()
                if message:  # Only print non-empty messages
                    print(f"Received: {message}")
                return message
            except socket.error as e:
                # Handle non-blocking socket errors
                if e.errno == 11 or e.errno == 35:  # EAGAIN or EWOULDBLOCK
                    return ""  # No data available
                else:
                    print(f"Receive error: {e}")
                    return ""
            except Exception as e:
                print(f"Receive error: {e}")
                sys.exit(0)
        return ""

    def send_command(self, command: str, priority: str = "normal"):
        """Send command and wait for server response."""
        self.send_message(command)
        print(f"Command sent: {command} [{priority}]")
        self.last_command_time = time.time()
        
        # Wait for response before sending next command
        self.wait_for_response()

    def wait_for_response(self, timeout: float = 2.0):
        """Wait for server response with timeout."""
        start_time = time.time()
        
        while time.time() - start_time < timeout:
            response = self.receive_message()
            if response:
                # Process the response immediately
                self.process_server_response(response)
                return
            # Small delay to prevent busy waiting
            time.sleep(0.05)
        
        # Timeout reached - continue anyway
        print("Warning: No response received within timeout")

    def broadcast_team_message(self, message_data: Dict):
        """Broadcast message to team members."""
        message = f"TEAM:{json.dumps(message_data)}"
        for member in self.team_members.values():
            if member.id != self.player_id:
                member.send_message(message)
        # The broadcast command seems to have issues with long messages
        pass

    def process_team_message(self, message: str, direction: int):
        """Process team coordination messages."""
        if not message.startswith("TEAM:"):
            return
        
        try:
            data = json.loads(message[5:])  # Remove "TEAM:" prefix
            sender_id = data.get("id")
            
            if sender_id == self.player_id:
                return  # Ignore our own messages
            
            msg_type = data.get("type")
            
            if msg_type == "join":
                # New team member joined
                self.team_members[sender_id] = TeamMember(
                    id=sender_id,
                    level=data.get("level", 1),
                    position=tuple(data.get("position", (0, 0))),
                    role=TeamRole(data.get("role", "explorer")),
                    inventory={},
                    status="active",
                    last_update=time.time()
                )
                print(f"Team member {sender_id} joined as {data.get('role')}")
                
            elif msg_type == "status_update":
                # Update team member info
                if sender_id in self.team_members:
                    member = self.team_members[sender_id]
                    member.level = data.get("level", member.level)
                    member.position = tuple(data.get("position", member.position))
                    member.inventory = data.get("inventory", member.inventory)
                    member.status = data.get("status", member.status)
                    member.last_update = time.time()
                    
            elif msg_type == "elevation_request":
                # Someone wants to elevate
                self.handle_elevation_request(data, direction)
                
            elif msg_type == "resource_request":
                # Someone needs resources
                self.handle_resource_request(data, direction)
                
            elif msg_type == "coordinate_attack":
                # Coordinate group actions
                self.handle_coordinate_request(data, direction)
                
        except json.JSONDecodeError:
            pass  # Invalid team message

    def handle_elevation_request(self, data: Dict, direction: int):
        """Handle elevation coordination requests."""
        requester_level = data.get("level", 1)
        requester_pos = tuple(data.get("position", (0, 0)))
        players_needed = data.get("players_needed", 1)
        
        # Check if we can help with elevation
        if (self.level == requester_level and 
            self.can_help_with_elevation() and
            self.calculate_distance(self.position, requester_pos) < self.cooperation_radius):
            
            # Move towards requester
            self.set_target_position(requester_pos)
            
            # Confirm participation
            self.broadcast_team_message({
                "type": "elevation_confirm",
                "id": self.player_id,
                "target_level": requester_level,
                "eta": self.calculate_travel_time(requester_pos)
            })

    def handle_resource_request(self, data: Dict, direction: int):
        """Handle resource sharing requests."""
        if not self.resource_sharing_enabled:
            return
            
        requested_resource = data.get("resource")
        amount_needed = data.get("amount", 1)
        requester_pos = tuple(data.get("position", (0, 0)))
        
        # Check if we have excess resources to share
        current_amount = self.inventory.get(requested_resource, 0)
        needed_for_elevation = self.get_resource_needed_for_next_elevation(requested_resource)
        
        if current_amount > needed_for_elevation + amount_needed:
            # Move towards requester and share resources
            self.set_target_position(requester_pos)

    def handle_coordinate_request(self, data: Dict, direction: int):
        """Handle group coordination requests."""
        if self.role == TeamRole.GUARDIAN or self.role == TeamRole.COORDINATOR:
            # Coordinate group actions
            pass

    def can_help_with_elevation(self) -> bool:
        """Check if we can help with elevation."""
        if self.inventory["food"] < 50:
            return False
        
        # Check if we have minimum resources for our own elevation
        requirements = elevation_requirements.get(self.level, {})
        for resource, needed in requirements.items():
            if resource != "players" and self.inventory.get(resource, 0) < needed:
                return False
        
        return True

    def get_resource_needed_for_next_elevation(self, resource: str) -> int:
        """Get amount of resource needed for next elevation."""
        requirements = elevation_requirements.get(self.level, {})
        return requirements.get(resource, 0)

    def calculate_distance(self, pos1: Tuple[int, int], pos2: Tuple[int, int]) -> float:
        """Calculate distance between two positions with world wrapping."""
        x1, y1 = pos1
        x2, y2 = pos2
        
        # Handle wrapping
        if self.world_size[0] > 0 and self.world_size[1] > 0:
            dx = min(abs(x2 - x1), self.world_size[0] - abs(x2 - x1))
            dy = min(abs(y2 - y1), self.world_size[1] - abs(y2 - y1))
        else:
            dx = abs(x2 - x1)
            dy = abs(y2 - y1)
        
        return (dx ** 2 + dy ** 2) ** 0.5

    def calculate_travel_time(self, target_pos: Tuple[int, int]) -> int:
        """Estimate travel time to target position."""
        distance = self.calculate_distance(self.position, target_pos)
        return int(distance * 7)  # Assuming 7 time units per move

    def set_target_position(self, target_pos: Tuple[int, int]):
        """Set target position for movement."""
        self.target_position = target_pos
        print(f"Setting target: {target_pos}")

    def spawn_new_ai_process(self, bypass_cooldown: bool = False) -> bool:
        """Spawn a new AI process with balanced role selection."""
        try:
            current_time = time.time()
            if not bypass_cooldown and current_time - self.last_spawn_time < self.spawn_cooldown:
                print(f"Spawn cooldown active, time remaining: {self.spawn_cooldown - (current_time - self.last_spawn_time):.1f}s")
                return False
                
            # Determine role for new AI with better balance
            role = self.select_balanced_role()
            
            print(f"Attempting to spawn AI with role: {role}")
            
            # Create command to spawn new AI
            cmd = [
                sys.executable, "zappy_ai",
                "-p", str(self.port),
                "-n", self.team_name,
                "-H", self.host,
                "-r", role,
                "--spawned"
            ]
            
            print(f"Spawn command: {' '.join(cmd)}")
            
            # Start the process
            process = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            self.spawned_processes.append(process)
            self.spawned_roles.append(role)
            self.last_spawn_time = current_time
            
            print(f"Spawned new AI with role {role} (Total spawned: {len(self.spawned_roles)})")
            return True
            
        except Exception as e:
            print(f"Failed to spawn new AI: {e}")
            return False

    def select_balanced_role(self) -> str:
        """Select a role for new AI to maintain team balance."""
        # Count existing roles including self
        role_counts = {
            "explorer": 0,
            "collector": 0,
            "guardian": 0,
            "coordinator": 0
        }
        
        # Count our own role
        role_counts[self.role.value] += 1
        
        # Count spawned roles
        for role in self.spawned_roles:
            if role in role_counts:
                role_counts[role] += 1
        
        # Count team members roles (if we have that info)
        for member in self.team_members.values():
            if member.role.value in role_counts:
                role_counts[member.role.value] += 1
        
        # Determine what role is needed most
        total_members = sum(role_counts.values())
        
        # Early game: need more explorers and collectors
        if total_members <= 3:
            if role_counts["explorer"] < 2:
                return "explorer"
            elif role_counts["collector"] < 1:
                return "collector"
        
        # Mid game: need balanced team
        elif total_members <= 6:
            if role_counts["collector"] < 2:
                return "collector"
            elif role_counts["guardian"] < 1:
                return "guardian"
            elif role_counts["coordinator"] < 1:
                return "coordinator"
        
        # Late game: focus on specialists
        else:
            if role_counts["guardian"] < 2:
                return "guardian"
            elif role_counts["coordinator"] < 1:
                return "coordinator"
        
        # Default: find the least represented role
        min_count = min(role_counts.values())
        least_represented = [role for role, count in role_counts.items() if count == min_count]
        
        return random.choice(least_represented)

    def cleanup_spawned_processes(self):
        """Clean up finished spawned processes."""
        for i, process in enumerate(self.spawned_processes[:]):
            if process.poll() is not None:
                # Process has finished - check if it was successful
                stdout, stderr = process.communicate()
                print(f"Process {i} finished with return code: {process.returncode}")
                if stdout:
                    print(f"Process {i} stdout: {stdout.decode()}")
                if stderr:
                    print(f"Process {i} stderr: {stderr.decode()}")
                
                self.spawned_processes.remove(process)
                # Also remove the corresponding role from tracking
                if i < len(self.spawned_roles):
                    removed_role = self.spawned_roles.pop(i)
                    print(f"Cleaned up finished process with role: {removed_role}")

    def look_for_food(self):
        """Look for food in the environment."""
        self.send_command("look", "high")

    def look_for_stones(self):
        """Look for stones in the environment."""
        self.send_command("Look", "normal")

    def look_for_resources(self):
        """Look for resources in the environment."""
        self.send_command("Look", "normal")

    def move_randomly(self):
        """Move in a random direction."""
        actions = ["Forward", "Left", "Right"]
        action = random.choice(actions)
        self.send_command(action, "normal")

    def move_towards_target(self):
        """Move towards the target position."""
        if not hasattr(self, 'target_position'):
            return
            
        target_x, target_y = self.target_position
        current_x, current_y = self.position
        
        # Simple movement towards target
        if target_x > current_x:
            self.turn_to_direction(2)  # East
        elif target_x < current_x:
            self.turn_to_direction(4)  # West
        elif target_y > current_y:
            self.turn_to_direction(3)  # South
        elif target_y < current_y:
            self.turn_to_direction(1)  # North
        
        self.send_command("Forward", "normal")

    def turn_to_direction(self, target_direction: int):
        """Turn to face a specific direction."""
        current_dir = self.direction
        
        if current_dir == target_direction:
            return
            
        # Calculate turns needed
        turns_needed = (target_direction - current_dir) % 4
        
        if turns_needed == 1:
            self.send_command("Right", "normal")
            self.direction = (self.direction + 1) % 4  # Update direction
        elif turns_needed == 2:
            self.send_command("Right", "normal")
            self.direction = (self.direction + 1) % 4
            self.send_command("Right", "normal")
            self.direction = (self.direction + 1) % 4
        elif turns_needed == 3:
            self.send_command("Left", "normal")
            self.direction = (self.direction - 1) % 4  # Update direction

    def update_status(self):
        """Update and broadcast status to team."""
        self.broadcast_team_message({
            "type": "status_update",
            "id": self.player_id,
            "level": self.level,
            "position": self.position,
            "inventory": self.inventory,
            "status": "active"
        })

    def emergency_food_search(self):
        """Emergency food search when starving."""
        self.send_command("Look", "critical")
        # Always try to take food when starving
        self.send_command("Take food", "critical")
        if self.inventory["food"] < 10:
            self.send_command("Forward", "high")

    def systematic_resource_search(self):
        """Systematic search for resources."""
        self.send_command("Look", "normal")
        
        # Try to take needed resources
        needed_resources = self.calculate_needed_resources()
        if needed_resources:
            resource = needed_resources[0]
            self.send_command(f"Take {resource}", "normal")
        
        if random.random() < 0.3:
            self.move_randomly()

    def attempt_elevation(self):
        """Attempt to elevate if requirements are met."""
        if self.is_ready_for_elevation():
            self.send_command("Incantation", "critical")

    def is_ready_for_elevation(self) -> bool:
        """Check if ready for elevation."""
        requirements = elevation_requirements.get(self.level, {})
        
        for resource, needed in requirements.items():
            if resource == "players":
                continue
            if self.inventory.get(resource, 0) < needed:
                return False
        
        return True

    def should_fork_new_player(self) -> bool:
        """Check if should fork a new player."""
        return (self.available_slots > 0 and 
                self.inventory["food"] > 50 and
                time.time() - self.last_spawn_time > self.spawn_cooldown)

    def fork_and_spawn_new_player(self):
        """Fork and spawn a new player."""
        self.send_command("Fork", "normal")
        if self.spawn_new_ai_process():
            self.available_slots -= 1

    def process_server_response(self, response: str) -> bool:
        """Process server response."""
        if not response:
            return True

        response = response.strip()

        if response == "ok":
            return True
        elif response == "ko":
            print("Command failed")
            return True
        elif response.startswith("Current level:"):
            # Level information
            try:
                self.level = int(response.split(":")[1].strip())
                print(f"Level updated to: {self.level}")
            except:
                pass
        elif response.startswith("["):
            # Could be inventory or vision response - need to distinguish
            # Inventory format: [ object number, object number ]
            # Vision format: [ object object object..., object object ]
            if self.is_inventory_response(response):
                self.parse_inventory(response)
            else:
                self.parse_vision(response)
        elif "," in response:
            # Position response
            try:
                parts = response.split(",")
                if len(parts) >= 2:
                    self.position = (int(parts[0]), int(parts[1]))
                    print(f"Position updated: {self.position}")
            except:
                pass
        elif response.startswith("message"):
            # Team message
            parts = response.split(",", 1)
            if len(parts) == 2:
                direction = int(parts[0].split()[1])
                message = parts[1].strip()
                self.process_team_message(message, direction)
        elif response.startswith("dead"):
            print("Player died!")
            return False
        elif response.startswith("Elevation underway"):
            print("Elevation started!")
        elif response.startswith("egg"):
            print("Egg laid!")
            self.available_slots += 1
            
        return True

    def parse_inventory(self, response: str):
        """Parse inventory response: [ object number, object number ]"""
        try:
            # Remove brackets and split by comma
            items = response.strip("[]").split(", ")
            inventory = {}
            
            for item in items:
                item = item.strip()
                # Split by space - last part should be the number
                parts = item.split()
                if len(parts) >= 2:
                    # Last part is the count, everything else is the object name
                    count = int(parts[-1])
                    name = " ".join(parts[:-1])
                    inventory[name] = count
            
            self.inventory.update(inventory)
            print(f"Inventory updated: {self.inventory}")
            
        except Exception as e:
            print(f"Error parsing inventory: {e}")

    def parse_vision(self, response: str):
        """Parse vision response: [ object object object..., object object ]"""
        try:
            # Remove outer brackets
            content = response.strip("[]")
            
            # Split by comma to get each tile
            tiles = []
            current_tile = ""
            
            # Simple split by comma and clean up
            raw_tiles = content.split(",")
            
            for raw_tile in raw_tiles:
                tile_content = raw_tile.strip()
                tiles.append(tile_content)
            
            print(f"Vision parsed: {len(tiles)} tiles")
            self.update_resource_map(tiles)
            self.plan_smart_movement(tiles)
            
        except Exception as e:
            print(f"Error parsing vision: {e}")

    def update_resource_map(self, tiles):
        """Update resource map based on vision."""
        for i, tile in enumerate(tiles):
            if tile:  # If tile contains resources
                self.resource_map[i] = tile.split()

    def plan_smart_movement(self, tiles):
        """Plan movement based on vision."""
        # Find tiles with needed resources
        needed_resources = self.calculate_needed_resources()
        
        for i, tile in enumerate(tiles):
            if any(resource in tile for resource in needed_resources):
                self.calculate_movement_to_tile(i)
                break

    def calculate_movement_to_tile(self, tile_index):
        """Calculate movement to reach a specific tile."""
        # If at tile 0 (current position), try to take resources
        if tile_index == 0:
            # Check what's at current position and take needed resources
            current_tile = self.resource_map.get(0, [])
            needed_resources = self.calculate_needed_resources()
            
            for resource in needed_resources:
                if resource in current_tile:
                    self.send_command(f"Take {resource}", "high")
                    return
            return
        elif tile_index <= 3:
            self.send_command("Forward", "normal")
        elif tile_index <= 6:
            self.send_command("Left", "normal")
        elif tile_index <= 9:
            self.send_command("Right", "normal")

    def calculate_needed_resources(self) -> List[str]:
        """Calculate what resources are needed."""
        needed = []
        requirements = elevation_requirements.get(self.level, {})
        
        for resource, needed_amount in requirements.items():
            if resource != "players" and self.inventory.get(resource, 0) < needed_amount:
                needed.append(resource)
        
        if self.inventory["food"] < 50:
            needed.append("food")
            
        return needed

    def make_decision(self):
        """Make strategic decisions based on current state."""
        # Check if enough time has passed since last command
        if time.time() - self.last_command_time < self.command_delay:
            return  # Wait before sending next command

        # Emergency food search if starving
        if self.inventory["food"] < 10:
            self.emergency_food_search()
            return

        # Try elevation if ready
        if self.is_ready_for_elevation():
            self.attempt_elevation()
            return

        # Fork new player if conditions are met
        if self.should_fork_new_player():
            self.fork_and_spawn_new_player()
            return

        # Execute role-specific strategy
        self.execute_role_strategy()

    def execute_role_strategy(self):
        """Execute strategy based on role - to be overridden by subclasses."""
        # Default strategy: explore and collect with better resource taking
        
        # Check inventory periodically
        if random.random() < 0.1:
            self.send_command("Inventory", "normal")
        
        # Always try to take food and resources when looking around
        if random.random() < 0.5:
            self.look_for_resources()
            # After looking, try to take needed resources
            needed_resources = self.calculate_needed_resources()
            if needed_resources:
                # Try to take the most needed resource
                resource = needed_resources[0]
                self.send_command(f"Take {resource}", "high")
        else:
            self.move_randomly()

    def run(self):
        """Main game loop."""
        if not self.connect_to_server():
            return
            
        print(f"Starting AI with role: {self.role.value}")
        
        # Main game loop
        while True:
            try:
                # Check for any additional server responses that weren't caught by wait_for_response
                response = self.receive_message()
                if response:
                    if not self.process_server_response(response):
                        break
                
                # Make strategic decisions (this will send commands and wait for responses)
                self.make_decision()
                
                # Clean up processes
                self.cleanup_spawned_processes()
                
                # Small delay before next iteration
                time.sleep(0.2)
                
            except KeyboardInterrupt:
                print("Shutting down...")
                break
            except Exception as e:
                print(f"Error in main loop: {e}")
                time.sleep(1)
        
        # Cleanup
        if self.socket:
            self.socket.close()
        
        # Clean up spawned processes
        for process in self.spawned_processes:
            try:
                process.terminate()
            except:
                pass

    def is_inventory_response(self, response: str) -> bool:
        """Determine if response is inventory format vs vision format."""
        try:
            # Remove brackets and check format
            content = response.strip("[]")
            if not content:
                return False
                
            # Split by comma
            parts = content.split(",")
            
            # Inventory should have items with numbers at the end
            # Vision should have items without specific numbers
            for part in parts:
                part = part.strip()
                if part:
                    tokens = part.split()
                    if len(tokens) >= 2:
                        # Check if last token is a number (inventory format)
                        try:
                            int(tokens[-1])
                            return True  # Found a number, likely inventory
                        except ValueError:
                            continue
            
            return False  # No numbers found, likely vision
            
        except Exception:
            return False  # Default to vision if can't determine