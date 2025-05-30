## GUI Client Architecture

The GUI client needs to visualize the game world and all activities. I'll outline a solution using Python with Pygame, which is suitable for 2D game visualization.

### Core Components

1. **Network Module**
   - Socket connection to server
   - Protocol handling for GUI-specific messages
   - Data reception and parsing

2. **Rendering Engine**
   - Map visualization
   - Player and resource rendering
   - Animation handling

3. **User Interface**
   - Game controls
   - Information display
   - Debugging tools

### Implementation Approach

```python
import pygame
import socket
import threading
import json
import time

class ZappyGUI:
    def __init__(self, host='localhost', port=4242):
        self.host = host
        self.port = port

        # Game state data
        self.map_width = 0
        self.map_height = 0
        self.tiles = []
        self.players = {}
        self.teams = {}

        # Network
        self.socket = None
        self.connected = False
        self.receive_thread = None

        # Pygame setup
        pygame.init()
        self.screen = None
        self.clock = pygame.time.Clock()
        self.running = False

        # Assets
        self.sprites = {}
        self.load_assets()

    def connect(self):
        """Connect to the server"""
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.connect((self.host, self.port))

            # Send GUI connection message
            self.send_message("GUI")

            # Get map dimensions
            dimensions = self.receive_message().split()
            self.map_width = int(dimensions[0])
            self.map_height = int(dimensions[1])

            # Initialize tiles
            self.tiles = [[{} for _ in range(self.map_width)] for _ in range(self.map_height)]

            self.connected = True

            # Start receive thread
            self.receive_thread = threading.Thread(target=self.receive_updates)
            self.receive_thread.daemon = True
            self.receive_thread.start()

            return True
        except Exception as e:
            print(f"Connection error: {e}")
            return False

    def load_assets(self):
        """Load sprites and other assets"""
        # Player sprites
        self.sprites['player'] = pygame.image.load('assets/player.png')

        # Resource sprites
        self.sprites['food'] = pygame.image.load('assets/food.png')
        self.sprites['linemate'] = pygame.image.load('assets/linemate.png')
        self.sprites['deraumere'] = pygame.image.load('assets/deraumere.png')
        self.sprites['sibur'] = pygame.image.load('assets/sibur.png')
        self.sprites['mendiane'] = pygame.image.load('assets/mendiane.png')
        self.sprites['phiras'] = pygame.image.load('assets/phiras.png')
        self.sprites['thystame'] = pygame.image.load('assets/thystame.png')

        # Tile sprite
        self.sprites['tile'] = pygame.image.load('assets/tile.png')

    def receive_updates(self):
        """Thread function to receive updates from server"""
        while self.connected:
            try:
                message = self.receive_message()
                self.parse_server_message(message)
            except Exception as e:
                print(f"Error receiving update: {e}")
                self.connected = False
                break

    def parse_server_message(self, message):
        """Parse messages from server and update game state"""
        parts = message.split()

        # Example protocol parsing
        if parts[0] == "pnw":  # New player
            # pnw #n X Y O L N
            player_id = int(parts[1][1:])  # Remove '#'
            x = int(parts[2])
            y = int(parts[3])
            orientation = int(parts[4])
            level = int(parts[5])
            team_name = parts[6]

            self.players[player_id] = {
                'x': x,
                'y': y,
                'orientation': orientation,
                'level': level,
                'team': team_name
            }

        elif parts[0] == "ppo":  # Player position
            # ppo #n X Y O
            player_id = int(parts[1][1:])
            x = int(parts[2])
            y = int(parts[3])
            orientation = int(parts[4])

            if player_id in self.players:
                self.players[player_id]['x'] = x
                self.players[player_id]['y'] = y
                self.players[player_id]['orientation'] = orientation

        elif parts[0] == "plv":  # Player level
            # plv #n L
            player_id = int(parts[1][1:])
            level = int(parts[2])

            if player_id in self.players:
                self.players[player_id]['level'] = level

        elif parts[0] == "bct":  # Tile content
            # bct X Y q q q q q q q
            x = int(parts[1])
            y = int(parts[2])
            food = int(parts[3])
            linemate = int(parts[4])
            deraumere = int(parts[5])
            sibur = int(parts[6])
            mendiane = int(parts[7])
            phiras = int(parts[8])
            thystame = int(parts[9])

            self.tiles[y][x] = {
                'food': food,
                'linemate': linemate,
                'deraumere': deraumere,
                'sibur': sibur,
                'mendiane': mendiane,
                'phiras': phiras,
                'thystame': thystame
            }

        elif parts[0] == "pic":  # Start incantation
            # pic X Y L #n #n ...
            x = int(parts[1])
            y = int(parts[2])
            level = int(parts[3])

            # Mark tile as having incantation
            if 0 <= y < self.map_height and 0 <= x < self.map_width:
                self.tiles[y][x]['incantation'] = True

        elif parts[0] == "pie":  # End incantation
            # pie X Y R
            x = int(parts[1])
            y = int(parts[2])
            result = int(parts[3])  # 0=fail, 1=success

            # Remove incantation marker
            if 0 <= y < self.map_height and 0 <= x < self.map_width:
                if 'incantation' in self.tiles[y][x]:
                    del self.tiles[y][x]['incantation']

        # Add more message handlers as needed

    def run(self):
        """Main GUI loop"""
        if not self.connected:
            if not self.connect():
                return

        # Setup display based on map dimensions
        tile_size = 32
        screen_width = self.map_width * tile_size
        screen_height = self.map_height * tile_size
        self.screen = pygame.display.set_mode((screen_width, screen_height))
        pygame.display.set_caption("Zappy GUI")

        self.running = True
        while self.running:
            # Handle events
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    self.running = False

            # Render game
            self.render()

            # Cap at 60 FPS
            self.clock.tick(60)

        pygame.quit()

    def render(self):
        """Render the game state"""
        # Clear screen
        self.screen.fill((0, 0, 0))

        tile_size = 32

        # Draw tiles and resources
        for y in range(self.map_height):
            for x in range(self.map_width):
                # Draw tile
                tile_rect = pygame.Rect(x * tile_size, y * tile_size, tile_size, tile_size)
                self.screen.blit(self.sprites['tile'], tile_rect)

                # Draw resources on tile
                if self.tiles[y][x]:
                    tile = self.tiles[y][x]
                    offset = 0

                    # Draw each resource type
                    for resource in ['food', 'linemate', 'deraumere', 'sibur', 'mendiane', 'phiras', 'thystame']:
                        count = tile.get(resource, 0)
                        for i in range(count):
                            resource_rect = pygame.Rect(
                                x * tile_size + (offset % 4) * 8,
                                y * tile_size + (offset // 4) * 8,
                                8, 8
                            )
                            self.screen.blit(self.sprites[resource], resource_rect)
                            offset += 1

                    # Draw incantation indicator
                    if tile.get('incantation', False):
                        pygame.draw.circle(
                            self.screen,
                            (255, 255, 0),
                            (x * tile_size + tile_size // 2, y * tile_size + tile_size // 2),
                            tile_size // 3,
                            2
                        )

        # Draw players
        for player_id, player in self.players.items():
            x = player['x']
            y = player['y']

            # Use different colors for different teams
            team = player['team']
            if team not in self.team_colors:
                # Generate a color for this team
                r = hash(team) % 200 + 55
                g = (hash(team) // 256) % 200 + 55
                b = (hash(team) // 65536) % 200 + 55
                self.team_colors[team] = (r, g, b)

            # Draw player with team color
            player_rect = pygame.Rect(x * tile_size, y * tile_size, tile_size, tile_size)
            rotated_sprite = pygame.transform.rotate(
                self.sprites['player'],
                90 * player['orientation']  # 0=N, 1=E, 2=S, 3=W
            )

            # Tint sprite with team color
            colored_sprite = rotated_sprite.copy()
            colored_sprite.fill(self.team_colors[team], special_flags=pygame.BLEND_RGB_MULT)

            self.screen.blit(colored_sprite, player_rect)

            # Draw level indicator
            font = pygame.font.SysFont(None, 20)
            level_text = font.render(str(player['level']), True, (255, 255, 255))
            self.screen.blit(level_text, (x * tile_size, y * tile_size))

        # Update display
        pygame.display.flip()

    def send_message(self, message):
        """Send message to server"""
        if self.connected:
            self.socket.sendall((message + "\n").encode())

    def receive_message(self):
        """Receive message from server"""
        if self.connected:
            data = b""
            while b"\n" not in data:
                chunk = self.socket.recv(4096)
                if not chunk:
                    raise ConnectionError("Server disconnected")
                data += chunk

            message = data.split(b"\n")[0].decode()
            return message
        return ""

if __name__ == "__main__":
    import sys

    host = "localhost"
    port = 4242

    # Parse command-line arguments
    if len(sys.argv) > 1:
        i = 1
        while i < len(sys.argv):
            if sys.argv[i] == "-p" and i + 1 < len(sys.argv):
                port = int(sys.argv[i + 1])
                i += 2
            elif sys.argv[i] == "-h" and i + 1 < len(sys.argv):
                host = sys.argv[i + 1]
                i += 2
            else:
                i += 1

    gui = ZappyGUI(host, port)
    gui.run()
```

### GUI Protocol Interaction

The server needs to provide the GUI with details about the game state. Here's a potential protocol for communication:

1. **Map Information**:
   - `msz X Y\n` - Map size (width X, height Y)
   - `bct X Y q q q q q q q\n` - Tile content (food, linemate, deraumere, sibur, mendiane, phiras, thystame)
   - `mct\n` - Request for all tile content

2. **Team Information**:
   - `tna N\n` - Team name (N)
   - `sgt T\n` - Time unit (T)

3. **Player Information**:
   - `pnw #n X Y O L N\n` - New player (id, x, y, orientation, level, team name)
   - `ppo #n X Y O\n` - Player position (id, x, y, orientation)
   - `plv #n L\n` - Player level (id, level)
   - `pin #n X Y q q q q q q q\n` - Player inventory (id, x, y, food, resources...)
   - `pex #n\n` - Player expulsion (id)
   - `pbc #n M\n` - Player broadcast (id, message)
   - `pfk #n\n` - Player fork (id)
   - `pdi #n\n` - Player death (id)

4. **Game Events**:
   - `pic X Y L #n #n ...\n` - Start incantation (x, y, level, players...)
   - `pie X Y R\n` - End incantation (x, y, result)
   - `enw #e #n X Y\n` - Egg laid (egg id, player id, x, y)
   - `ebo #e\n` - Player connection for egg (egg id)
   - `edi #e\n` - Egg death (egg id)
   - `sst T\n` - Time unit modification (T)
   - `seg N\n` - End of game (winning team)

This protocol allows the GUI to maintain a complete representation of the game state and update it as events occur.
