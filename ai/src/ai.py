#!/usr/bin/env python3

import socket
import sys
import time
import random
import argparse
from enum import Enum
from dataclasses import dataclass
from typing import List, Dict, Tuple


class Direction(Enum):
    NORTH = 1
    EAST = 2
    SOUTH = 3
    WEST = 4


class RessourceType(Enum):
    FOOD = "food"
    LINEMATE = "linemate"
    DERAUMERE = "deraumere"
    SIBUR = "sibur"
    MENDIANE = "mendiane"
    PHIRAS = "phiras"
    THYSTAME = "thystame"


@dataclass
class Position:
    x: int
    y: int


@dataclass
class GameState:
    level: int = 1
    position: Position = None
    direction: Direction = Direction.NORTH
    inventory: Dict[str, int] = None
    world_width: int = 0
    world_height: int = 0
    team_slots: int = 0
    food_units: int = 10

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


class ZappyAI:
    ELEVATION_REQUIREMENTS = {
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

    def __init__(self, port: int, team_name: str, hostname: str = "localhost"):
        self.port = port
        self.team_name = team_name
        self.hostname = hostname
        self.socket = None
        self.state = GameState()
        self.command_queue = []
        self.last_look_result = []
        self.current_goal = "explore"
        self.target_resources = {}
        self.reproduction_cooldown = 0

    def connect(self):
        """Connect to the Zappy server"""
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.connect((self.hostname, self.port))

            welcome = self.receive_message()
            if not welcome.startswith("WELCOME"):
                raise Exception(f"Expected WELCOME, got: {welcome}")

            self.send_message(self.team_name)
            client_num = self.receive_message()
            world_dims = self.receive_message()

            self.state.team_slots = int(client_num)
            dims = world_dims.split()
            self.state.world_width = int(dims[0])
            self.state.world_height = int(dims[1])

            print(
                f"Connected to server. Team slots: {self.state.team_slots}, \
                World: {self.state.world_width}x{self.state.world_height}"
            )
            return True
        except Exception as e:
            print(f"Connection failed: {e}")
            return False

    def send_message(self, message: str):
        """Send a message to the server"""
        if self.socket:
            full_message = message + "\n"
            self.socket.send(full_message.encode())

    def receive_message(self) -> str:
        """Receive a message from the server"""
        if self.socket:
            data = self.socket.recv(1024).decode().strip()
            return data
        return ""

    def send_command(self, command: str):
        """Send a command and add it to queue for response tracking"""
        self.send_message(command)
        self.command_queue.append(command)

        if len(self.command_queue) > 10:
            self.command_queue.pop(0)

    def look_around(self):
        """Send look command to examine surroundings"""
        self.send_command("Look")

    def move_forward(self):
        """Move forward one tile"""
        self.send_command("Forward")

    def turn_right(self):
        """Turn 90 degrees right"""
        self.send_command("Right")

        self.state.direction = Direction((self.state.direction.value % 4) + 1)

    def turn_left(self):
        """Turn 90 degrees left"""
        self.send_command("Left")

        self.state.direction = Direction(((self.state.direction.value - 2) % 4) + 1)

    def take_object(self, obj: str):
        """Take an object from current tile"""
        self.send_command(f"Take {obj}")

    def set_object(self, obj: str):
        """Drop an object on current tile"""
        self.send_command(f"Set {obj}")

    def check_inventory(self):
        """Check current inventory"""
        self.send_command("Inventory")

    def broadcast_message(self, message: str):
        """Broadcast a message to all players"""
        self.send_command(f"Broadcast {message}")

    def start_incantation(self):
        """Start elevation ritual"""
        self.send_command("Incantation")

    def fork_player(self):
        """Create an egg for player reproduction"""
        self.send_command("Fork")

    def eject_players(self):
        """Eject other players from current tile"""
        self.send_command("Eject")

    def parse_look_result(self, result: str) -> List[List[str]]:
        """Parse the look command result"""
        content = result.strip("[]")
        tiles = []
        current_tile = []

        i = 0
        while i < len(content):
            if content[i] == ",":
                tiles.append(current_tile)
                current_tile = []
                i += 1
                if i < len(content) and content[i] == " ":
                    i += 1
            else:
                end = i
                while end < len(content) and content[end] != ",":
                    end += 1

                tile_content = content[i:end].strip()
                if tile_content:
                    current_tile = tile_content.split()
                else:
                    current_tile = []
                i = end

        if current_tile or i == len(content):
            tiles.append(current_tile)

        return tiles

    def parse_inventory(self, inv_str: str):
        """Parse inventory response and update state"""
        content = inv_str.strip("[]")
        items = content.split(",")

        for item in items:
            parts = item.strip().split()
            if len(parts) == 2:
                resource, quantity = parts[0], int(parts[1])
                self.state.inventory[resource] = quantity

    def calculate_needed_resources(self) -> Dict[str, int]:
        """Calculate what resources are needed for next elevation"""
        if self.state.level >= 7:
            return {}

        requirements = self.ELEVATION_REQUIREMENTS[self.state.level]
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
            current = self.state.inventory.get(resource, 0)
            if required > current:
                needed[resource] = required - current

        return needed

    def find_resources_in_vision(self, tiles: List[List[str]]) -> List[Tuple[int, str]]:
        """Find resources in the vision tiles"""
        resources_found = []
        for tile_idx, tile_content in enumerate(tiles):
            for item in tile_content:
                if item in [
                    "food",
                    "linemate",
                    "deraumere",
                    "sibur",
                    "mendiane",
                    "phiras",
                    "thystame",
                ]:
                    resources_found.append((tile_idx, item))
        return resources_found

    def navigate_to_tile(self, target_tile: int):
        """Navigate to a specific tile based on vision numbering"""
        if target_tile == 0:
            return

        if target_tile == 1:
            self.move_forward()
        elif target_tile in [2, 3, 4]:
            self.move_forward()
        else:
            self.move_forward()

    def should_reproduce(self) -> bool:
        """Decide if the player should reproduce"""
        return (
            self.state.inventory.get("food", 0) > 50
            and self.state.team_slots > 0
            and self.reproduction_cooldown <= 0
            and self.state.level >= 2
        )

    def decide_next_action(self):
        """Main AI decision-making logic"""
        if self.state.inventory.get("food", 0) < 20:
            self.current_goal = "find_food"
        elif self.should_reproduce():
            self.current_goal = "reproduce"
        else:
            needed_resources = self.calculate_needed_resources()
            if needed_resources:
                self.current_goal = "gather"
                self.target_resources = needed_resources
            else:
                self.current_goal = "elevate"

    def execute_goal(self):
        """Execute the current goal"""
        if self.current_goal == "find_food":
            self.look_around()
        elif self.current_goal == "gather":
            self.look_around()
        elif self.current_goal == "elevate":
            self.broadcast_message(f"ELEVATE_L{self.state.level}")
            self.start_incantation()
        elif self.current_goal == "reproduce":
            self.fork_player()
            self.reproduction_cooldown = 100
        else:
            if random.random() < 0.1:
                if random.random() < 0.5:
                    self.turn_left()
                else:
                    self.turn_right()
            else:
                self.move_forward()

    def process_response(self, response: str):
        """Process server response"""
        response = response.strip()

        if response.startswith("[") and response.endswith("]"):
            tiles = self.parse_look_result(response)
            self.last_look_result = tiles

            resources_found = self.find_resources_in_vision(tiles)

            if self.current_goal in ["find_food", "gather"]:
                for tile_idx, resource in resources_found:
                    if (self.current_goal == "find_food" and resource == "food") or (
                        self.current_goal == "gather"
                        and resource in self.target_resources
                    ):
                        if tile_idx == 0:
                            self.take_object(resource)
                        else:
                            self.navigate_to_tile(tile_idx)
                        return

                if random.random() < 0.3:
                    if random.random() < 0.5:
                        self.turn_left()
                    else:
                        self.turn_right()
                else:
                    self.move_forward()

        elif response.startswith("[") and "," in response:
            self.parse_inventory(response)

        elif response.startswith("message"):
            parts = response.split(",", 1)
            if len(parts) == 2:
                direction = parts[0].split()[1]
                message = parts[1].strip()
                self.handle_broadcast(int(direction), message)

        elif response in ["ok", "ko"]:
            if self.command_queue:
                command = self.command_queue.pop(0)
                if response == "ko":
                    print(f"Command failed: {command}")

        elif response == "dead":
            print("Player died!")
            return False

        elif response.startswith("Elevation underway"):
            print("Elevation started!")

        elif response.startswith("Current level:"):
            level_str = response.split(":")[1].strip()
            self.state.level = int(level_str)
            print(f"New level: {self.state.level}")

        return True

    def handle_broadcast(self, direction: int, message: str):
        """Handle received broadcast messages"""
        if message.startswith("ELEVATE_L"):
            level = int(message.split("_L")[1])
            if level == self.state.level and self.current_goal == "elevate":
                print(f"Received elevation call for level {level}")

    def run(self):
        """Main game loop"""
        if not self.connect():
            return

        print("Starting AI game loop...")

        self.check_inventory()
        self.look_around()

        try:
            while True:
                self.decide_next_action()

                self.execute_goal()

                try:
                    response = self.receive_message()
                    if response:
                        if not self.process_response(response):
                            break
                except socket.timeout:
                    pass

                if self.reproduction_cooldown > 0:
                    self.reproduction_cooldown -= 1

                time.sleep(0.1)

        except KeyboardInterrupt:
            print("AI stopped by user")
        except Exception as e:
            print(f"AI error: {e}")
        finally:
            if self.socket:
                self.socket.close()


def helper():
    print("Usage: zappy_ai.py -p <port> -n <team_name> [-h <hostname>]")
    print("Options:")
    print("  -p, --port       Port number to connect to the Zappy server")
    print("  -n, --name       Team name for the Zappy AI")
    print("  -H, --hostname   Hostname of the Zappy server (default: localhost)")
    print("  --help           Show this help message and exit")

def main():
    parser = argparse.ArgumentParser(description="Zappy AI Client")
    parser.add_argument("-p", "--port", type=int, required=True, help="Port number")
    parser.add_argument("-n", "--name", type=str, required=True, help="Team name")
    parser.add_argument("-H", "--hostname", type=str, default="localhost", help="Server hostname")

    if any(arg in ["--help"] for arg in sys.argv):
        helper()
        return

    args = parser.parse_args()

    ai = ZappyAI(args.port, args.name, args.hostname)
    ai.run()


if __name__ == "__main__":
    main()
