# Zappy AI Client - Complete Documentation

## Table of Contents
1. [Overview](#overview)
2. [Architecture](#architecture)
3. [Installation & Usage](#installation--usage)
4. [Core Components](#core-components)
5. [AI Strategy](#ai-strategy)
6. [Protocol Implementation](#protocol-implementation)
7. [API Reference](#api-reference)
8. [Configuration](#configuration)
9. [Troubleshooting](#troubleshooting)
10. [Development Guide](#development-guide)

## Overview

The Zappy AI Client is an autonomous player for the Zappy network game. It connects to a Zappy server, controls a virtual inhabitant (drone) on the planet Trantor, and competes with other teams to achieve the first team with 6 players reaching maximum elevation (level 8).

### Key Features
- **Autonomous Operation**: Runs independently without user intervention
- **Strategic Decision Making**: Implements intelligent goal-based behavior
- **Resource Management**: Efficiently collects and manages game resources
- **Team Coordination**: Uses broadcasts to coordinate with teammates
- **Protocol Compliance**: Fully implements the Zappy client/server protocol

### Game Objectives
- Survive by maintaining food levels
- Collect stones required for elevation rituals
- Coordinate with teammates for group elevations
- Reproduce to increase team size
- Reach level 8 before opposing teams

## Architecture

### High-Level Design
```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   Zappy Server  │◄──►│   Zappy AI       │◄──►│  Game State     │
│                 │    │   (Main Logic)   │    │  Management     │
└─────────────────┘    └──────────────────┘    └─────────────────┘
                              │
                              ▼
                       ┌──────────────────┐
                       │  Strategy Engine │
                       │  - Goal System   │
                       │  - Decision Tree │
                       │  - Pathfinding   │
                       └──────────────────┘
```

### Core Classes

#### `Direction` (Enum)
Represents the four cardinal directions with server protocol values:
- `NORTH = 1`
- `EAST = 2`
- `SOUTH = 3`
- `WEST = 4`

#### `ResourceType` (Enum)
Defines all collectible resources in the game:
- `FOOD` - Required for survival
- `LINEMATE` through `THYSTAME` - Stones needed for elevation

#### `Position` (Dataclass)
Simple coordinate container:
```python
@dataclass
class Position:
    x: int
    y: int
```

#### `GameState` (Dataclass)
Central state management for the AI:
```python
@dataclass
class GameState:
    level: int = 1                    # Current player level (1-8)
    position: Position = None         # Current world position
    direction: Direction = NORTH      # Current facing direction
    inventory: Dict[str, int] = None  # Resource inventory
    world_width: int = 0              # World dimensions
    world_height: int = 0
    team_slots: int = 0               # Available team connections
    food_units: int = 10              # Current food units
```

#### `ZappyAI` (Main Class)
The core AI controller containing all game logic and server communication.

## Installation & Usage

### Prerequisites
- Python 3.7 or higher
- Network access to Zappy server
- No external dependencies required (uses standard library only)

### Installation
1. Save the code as `zappy_ai.py`
2. Make executable: `chmod +x zappy_ai.py`

### Usage
```bash
# Basic usage
python3 zappy_ai.py -p <port> -n <team_name>

# With custom hostname
python3 zappy_ai.py -p 4242 -n TeamAlpha -h server.example.com

# Help
python3 zappy_ai.py --help
```

### Command Line Arguments
- `-p, --port` (required): Server port number
- `-n, --name` (required): Team name to join
- `-h, --hostname` (optional): Server hostname (default: localhost)

### Example
```bash
# Connect to local server on port 4242 as team "Robots"
python3 zappy_ai.py -p 4242 -n Robots

# Connect to remote server
python3 zappy_ai.py -p 8080 -n TeamBeta -h game.server.com
```

## Core Components

### 1. Connection Management

#### Initial Handshake
The AI follows the exact protocol sequence:
1. **Server**: `WELCOME\n`
2. **Client**: `TEAM_NAME\n`
3. **Server**: `CLIENT_NUM\n`
4. **Server**: `X Y\n` (world dimensions)

```python
def connect(self):
    # Socket creation and connection
    self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    self.socket.connect((self.hostname, self.port))

    # Protocol handshake
    welcome = self.receive_message()
    self.send_message(self.team_name)
    client_num = self.receive_message()
    world_dims = self.receive_message()
```

#### Command Queue Management
Implements the 10-command limit as specified in the protocol:
```python
def send_command(self, command: str):
    self.send_message(command)
    self.command_queue.append(command)
    if len(self.command_queue) > 10:
        self.command_queue.pop(0)  # Remove oldest command
```

### 2. Game State Management

#### Inventory Tracking
Parses server inventory responses and maintains current resource counts:
```python
def parse_inventory(self, inv_str: str):
    # Format: [food 345, sibur 3, phiras 5, ..., deraumere 0]
    content = inv_str.strip("[]")
    items = content.split(",")
    for item in items:
        parts = item.strip().split()
        if len(parts) == 2:
            resource, quantity = parts[0], int(parts[1])
            self.state.inventory[resource] = quantity
```

#### World State Tracking
Maintains awareness of:
- Current position and direction
- World boundaries for wraparound navigation
- Team slot availability
- Food consumption tracking

### 3. Vision System

#### Look Command Processing
Parses the complex look command response format:
```python
def parse_look_result(self, result: str) -> List[List[str]]:
    # Input: "[player,,,thystame,,food,,,,,thystame,,,,,,]"
    # Output: [["player"], [], [], ["thystame"], [], ["food"], ...]
```

#### Tile Numbering
Implements the server's tile numbering system for navigation:
- Tile 0: Current position
- Tile 1: Directly ahead
- Tiles 2-8: Expanding vision cone based on level

#### Resource Detection
Identifies available resources in the vision field:
```python
def find_resources_in_vision(self, tiles: List[List[str]]) -> List[Tuple[int, str]]:
    resources_found = []
    for tile_idx, tile_content in enumerate(tiles):
        for item in tile_content:
            if item in RESOURCE_TYPES:
                resources_found.append((tile_idx, item))
    return resources_found
```

## AI Strategy

### Goal-Based Decision System

The AI operates using a priority-based goal system:

#### 1. Survival Priority (Highest)
```python
if self.state.inventory.get("food", 0) < 20:
    self.current_goal = "find_food"
```
- **Trigger**: Food < 20 units
- **Action**: Search for and collect food
- **Priority**: Critical (overrides all other goals)

#### 2. Reproduction Strategy
```python
def should_reproduce(self) -> bool:
    return (self.state.inventory.get("food", 0) > 50 and
            self.state.team_slots > 0 and
            self.reproduction_cooldown <= 0 and
            self.state.level >= 2)
```
- **Conditions**:
  - Food > 50 units
  - Team has available slots
  - Not in cooldown period
  - Level ≥ 2
- **Action**: Execute `Fork` command

#### 3. Resource Gathering
```python
def calculate_needed_resources(self) -> Dict[str, int]:
    requirements = self.ELEVATION_REQUIREMENTS[self.state.level]
    needed = {}
    for resource in STONE_TYPES:
        required = requirements[resource]
        current = self.state.inventory.get(resource, 0)
        if required > current:
            needed[resource] = required - current
    return needed
```
- **Trigger**: Missing resources for next elevation
- **Action**: Systematically collect required stones
- **Strategy**: Prioritize resources with largest deficit

#### 4. Elevation Coordination
```python
def execute_goal(self):
    if self.current_goal == "elevate":
        self.broadcast_message(f"ELEVATE_L{self.state.level}")
        self.start_incantation()
```
- **Conditions**: All resources collected
- **Action**: Broadcast to coordinate with teammates
- **Coordination**: Wait for sufficient same-level players

#### 5. Exploration (Default)
- **Trigger**: No specific objectives
- **Behavior**: Random walk with bias toward forward movement
- **Strategy**: 90% forward movement, 10% turning

### Elevation Requirements Table

The AI maintains the exact elevation requirements from the game specification:

| Level | Players | Linemate | Deraumere | Sibur | Mendiane | Phiras | Thystame |
|-------|---------|----------|-----------|-------|----------|--------|----------|
| 1→2   | 1       | 1        | 0         | 0     | 0        | 0      | 0        |
| 2→3   | 2       | 1        | 1         | 1     | 0        | 0      | 0        |
| 3→4   | 2       | 2        | 0         | 1     | 0        | 2      | 0        |
| 4→5   | 4       | 1        | 1         | 2     | 0        | 1      | 0        |
| 5→6   | 4       | 1        | 2         | 1     | 3        | 0      | 0        |
| 6→7   | 6       | 1        | 2         | 3     | 0        | 1      | 0        |
| 7→8   | 6       | 2        | 2         | 2     | 2        | 2      | 1        |

### Navigation Strategy

#### Basic Movement
```python
def navigate_to_tile(self, target_tile: int):
    if target_tile == 0:
        return  # Already on target
    elif target_tile == 1:
        self.move_forward()
    else:
        # Move toward target and reassess
        self.move_forward()
```

#### Direction Management
```python
def turn_right(self):
    self.send_command("Right")
    self.state.direction = Direction((self.state.direction.value % 4) + 1)

def turn_left(self):
    self.send_command("Left")
    self.state.direction = Direction(((self.state.direction.value - 2) % 4) + 1)
```

## Protocol Implementation

### Command Set Implementation

| Game Action | Command | AI Method | Time Limit |
|-------------|---------|-----------|------------|
| Move forward | `Forward` | `move_forward()` | 7/f |
| Turn right | `Right` | `turn_right()` | 7/f |
| Turn left | `Left` | `turn_left()` | 7/f |
| Look around | `Look` | `look_around()` | 7/f |
| Check inventory | `Inventory` | `check_inventory()` | 1/f |
| Broadcast | `Broadcast <text>` | `broadcast_message()` | 7/f |
| Take object | `Take <object>` | `take_object()` | 7/f |
| Drop object | `Set <object>` | `set_object()` | 7/f |
| Start elevation | `Incantation` | `start_incantation()` | 300/f |
| Fork player | `Fork` | `fork_player()` | 42/f |
| Eject players | `Eject` | `eject_players()` | 7/f |

### Response Handling

#### Success Responses
- `"ok"`: Command executed successfully
- `"ko"`: Command failed or invalid

#### Data Responses
- Look results: `"[tile1,tile2,...]"`
- Inventory: `"[food 345, sibur 3, ...]"`
- Broadcast: `"message K, text"`

#### Status Updates
- Death: `"dead"`
- Elevation: `"Elevation underway"` → `"Current level: K"`

### Message Processing Pipeline

```python
def process_response(self, response: str):
    response = response.strip()

    if response.startswith("[") and response.endswith("]"):
        # Handle look or inventory results
    elif response.startswith("message"):
        # Handle broadcast messages
    elif response in ["ok", "ko"]:
        # Handle command confirmations
    elif response == "dead":
        # Handle player death
    elif response.startswith("Elevation"):
        # Handle elevation status
```

## API Reference

### ZappyAI Class

#### Constructor
```python
def __init__(self, port: int, team_name: str, hostname: str = "localhost"):
```
- **port**: Server port number
- **team_name**: Team identifier
- **hostname**: Server address (default: localhost)

#### Core Methods

##### Connection Management
```python
def connect(self) -> bool:
    """Establish connection to Zappy server"""

def send_message(self, message: str):
    """Send raw message to server"""

def receive_message(self) -> str:
    """Receive message from server"""
```

##### Game Commands
```python
def look_around(self):
    """Execute Look command"""

def move_forward(self):
    """Execute Forward command"""

def turn_right(self):
    """Execute Right command"""

def turn_left(self):
    """Execute Left command"""

def take_object(self, obj: str):
    """Execute Take command"""

def set_object(self, obj: str):
    """Execute Set command"""

def check_inventory(self):
    """Execute Inventory command"""

def broadcast_message(self, message: str):
    """Execute Broadcast command"""

def start_incantation(self):
    """Execute Incantation command"""

def fork_player(self):
    """Execute Fork command"""

def eject_players(self):
    """Execute Eject command"""
```

##### Strategy Methods
```python
def calculate_needed_resources(self) -> Dict[str, int]:
    """Calculate resources needed for next elevation"""

def decide_next_action(self):
    """Main decision-making logic"""

def execute_goal(self):
    """Execute current goal strategy"""

def should_reproduce(self) -> bool:
    """Determine if reproduction is advisable"""
```

##### Utility Methods
```python
def parse_look_result(self, result: str) -> List[List[str]]:
    """Parse look command response"""

def parse_inventory(self, inv_str: str):
    """Parse inventory response"""

def find_resources_in_vision(self, tiles: List[List[str]]) -> List[Tuple[int, str]]:
    """Locate resources in vision field"""

def navigate_to_tile(self, target_tile: int):
    """Navigate to specific tile"""
```

#### Main Execution
```python
def run(self):
    """Main game loop - call this to start the AI"""
```

### GameState Properties

```python
# Player status
state.level: int                    # Current elevation level (1-8)
state.position: Position           # World coordinates
state.direction: Direction         # Current facing direction
state.food_units: int             # Current food count

# World information
state.world_width: int            # World width
state.world_height: int           # World height
state.team_slots: int             # Available team connections

# Resources
state.inventory: Dict[str, int]   # Current inventory
# Keys: "food", "linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame"
```

## Configuration

### Behavioral Parameters

These constants can be modified to adjust AI behavior:

```python
# Survival thresholds
FOOD_CRITICAL_LEVEL = 20        # Trigger food search
FOOD_COMFORTABLE_LEVEL = 50     # Allow reproduction

# Reproduction settings
REPRODUCTION_COOLDOWN = 100     # Ticks between reproduction attempts
MIN_REPRODUCTION_LEVEL = 2      # Minimum level for reproduction

# Movement probabilities
EXPLORATION_TURN_CHANCE = 0.1   # Probability of turning while exploring
RANDOM_TURN_CHANCE = 0.3        # Probability of random turn when no resources found

# Strategy weights
LOOK_FREQUENCY = 1              # How often to look around
INVENTORY_CHECK_FREQUENCY = 10  # How often to check inventory
```

### Elevation Requirements Customization

The elevation requirements table can be modified:

```python
ELEVATION_REQUIREMENTS = {
    1: {"players": 1, "linemate": 1, ...},
    2: {"players": 2, "linemate": 1, ...},
    # ... customize as needed
}
```

### Communication Protocols

Broadcast message formats used by the AI:

```python
# Elevation coordination
f"ELEVATE_L{level}"              # Request elevation at specific level

# Resource sharing (potential extension)
f"NEED_{resource}_{quantity}"    # Request specific resources
f"HAVE_{resource}_{quantity}"    # Announce available resources

# Strategic coordination (potential extension)
f"DEFEND_{x}_{y}"               # Coordinate defense
f"ATTACK_{team}"                # Coordinate offense
```

## Troubleshooting

### Common Issues

#### Connection Problems
```
ConnectionRefusedError: [Errno 61] Connection refused
```
**Solutions:**
- Verify server is running
- Check port number matches server configuration
- Ensure hostname/IP is correct
- Check firewall settings

#### Protocol Errors
```
Expected WELCOME, got: <other message>
```
**Solutions:**
- Verify server protocol version compatibility
- Check team name validity
- Ensure server is accepting connections

#### Performance Issues
```
AI appears slow or unresponsive
```
**Solutions:**
- Reduce `time.sleep()` delay in main loop
- Optimize decision-making logic
- Check network latency
- Verify server performance

#### Resource Management
```
AI stuck in resource gathering
```
**Solutions:**
- Check resource density on map
- Verify vision parsing logic
- Adjust exploration strategy
- Check for resource respawn timing

### Debug Mode

Add debugging output by modifying the `process_response` method:

```python
def process_response(self, response: str):
    print(f"DEBUG: Received response: {response}")  # Add debug output
    response = response.strip()
    # ... rest of method
```

### Logging

Implement comprehensive logging:

```python
import logging

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('zappy_ai.log'),
        logging.StreamHandler()
    ]
)

# Use throughout the code
logging.info(f"Connected to server. Team slots: {self.state.team_slots}")
logging.warning(f"Command failed: {command}")
logging.error(f"AI error: {e}")
```

## Development Guide

### Code Architecture Principles

#### 1. Separation of Concerns
- **Communication**: Socket handling and protocol implementation
- **Strategy**: Decision-making and goal management
- **State**: Game state tracking and management
- **Utility**: Helper functions and data processing

#### 2. Extensibility
The code is designed for easy extension:

```python
# Add new goals
def decide_next_action(self):
    if self.custom_condition():
        self.current_goal = "new_goal"
    # ... existing logic

def execute_goal(self):
    if self.current_goal == "new_goal":
        self.execute_new_strategy()
    # ... existing logic
```

#### 3. Error Handling
Implement robust error handling:

```python
try:
    response = self.receive_message()
    if response:
        if not self.process_response(response):
            break
except socket.timeout:
    # Handle timeout gracefully
    pass
except ConnectionResetError:
    # Handle connection loss
    self.reconnect()
except Exception as e:
    # Log unexpected errors
    logging.error(f"Unexpected error: {e}")
```

### Testing Strategy

#### Unit Testing
```python
import unittest

class TestZappyAI(unittest.TestCase):
    def setUp(self):
        self.ai = ZappyAI(4242, "TestTeam")

    def test_parse_look_result(self):
        result = "[player,food,,linemate]"
        parsed = self.ai.parse_look_result(result)
        expected = [["player"], ["food"], [], ["linemate"]]
        self.assertEqual(parsed, expected)

    def test_calculate_needed_resources(self):
        self.ai.state.level = 2
        self.ai.state.inventory = {"linemate": 0, "deraumere": 0, "sibur": 1}
        needed = self.ai.calculate_needed_resources()
        expected = {"linemate": 1, "deraumere": 1}
        self.assertEqual(needed, expected)
```

#### Integration Testing
Test with actual server:
```bash
# Start test server
./zappy_server -p 4242 -x 10 -y 10 -n TestTeam -c 1 -f 100

# Run AI client
python3 zappy_ai.py -p 4242 -n TestTeam

# Monitor behavior
tail -f zappy_ai.log
```

### Performance Optimization

#### 1. Reduce Network Calls
- Batch commands when possible
- Cache frequently accessed data
- Minimize redundant requests

#### 2. Optimize Decision Making
```python
# Use lookup tables instead of calculations
RESOURCE_PRIORITIES = {
    1: ["linemate"],
    2: ["linemate", "deraumere", "sibur"],
    # ... precomputed priorities
}

def get_priority_resources(self):
    return RESOURCE_PRIORITIES.get(self.state.level, [])
```

#### 3. Memory Management
- Clean up old command queue entries
- Limit vision result storage
- Use generators for large data processing

### Advanced Features

#### Multi-Threading (Optional)
```python
import threading

class ZappyAI:
    def __init__(self, ...):
        self.response_thread = threading.Thread(target=self.response_handler)
        self.command_thread = threading.Thread(target=self.command_handler)

    def response_handler(self):
        while self.running:
            response = self.receive_message()
            self.process_response(response)

    def command_handler(self):
        while self.running:
            self.decide_next_action()
            self.execute_goal()
            time.sleep(0.1)
```

#### Machine Learning Integration
```python
# Potential extension for adaptive behavior
import pickle

class LearningAI(ZappyAI):
    def __init__(self, ...):
        super().__init__(...)
        self.experience = self.load_experience()

    def update_strategy(self, situation, action, result):
        # Update strategy based on outcomes
        self.experience[situation] = (action, result)

    def save_experience(self):
        with open('ai_experience.pkl', 'wb') as f:
            pickle.dump(self.experience, f)
```

#### Cooperative Strategies
```python
def coordinate_elevation(self):
    """Advanced elevation coordination"""
    required_players = self.ELEVATION_REQUIREMENTS[self.state.level]["players"]

    # Broadcast availability
    self.broadcast_message(f"READY_L{self.state.level}_{self.position.x}_{self.position.y}")

    # Wait for responses and coordinate meeting point
    # ... implementation details
```

### Contributing

When extending the AI, follow these guidelines:

1. **Maintain Protocol Compliance**: Ensure all changes respect the Zappy protocol
2. **Preserve Goal System**: New features should integrate with the existing goal-based architecture
3. **Add Logging**: Include appropriate logging for new features
4. **Update Documentation**: Modify this documentation to reflect changes
5. **Test Thoroughly**: Test new features with actual server instances

### Future Enhancements

Potential areas for improvement:

1. **Advanced Pathfinding**: Implement A* or similar algorithms for navigation
2. **Predictive Resource Management**: Anticipate resource needs based on team strategy
3. **Dynamic Team Coordination**: More sophisticated broadcast protocols
4. **Adaptive Learning**: Machine learning for strategy optimization
5. **Multi-Server Support**: Handle server disconnections and reconnections
6. **Statistical Analysis**: Track performance metrics and optimize accordingly

---

This documentation provides a complete reference for understanding, using, and extending the Zappy AI client. The implementation balances simplicity with effectiveness, providing a solid foundation for competitive gameplay while remaining accessible for further development.
