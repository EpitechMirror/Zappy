# Zappy GUI Client

A realtime 3D visualization client for the Zappy multiplayer game server.
Written in modern C++17 and powered by [raylib](https://www.raylib.com/) for cross-platform graphics, this client renders the map, players, resources, and game events in an interactive GUI.

---

## Table of Contents

1. [Features](#features)
2. [Architecture](#architecture)
3. [Usage](#usage)
4. [Protocol & Communication](#protocol--communication)

---

## Features

- **Map Rendering**
  • Dynamic 2D/3D grid visualization of the game world
  • Textured tiles showing resource counts

- **Player & Resource Visualization**
  • Colored 3D models for each player, oriented by direction
  • Icons/meshes for resources (food, linemate, deraumere, sibur, mendiane, phiras, thystame)

- **Real-time Animations & Events**
  • Smooth movement and orientation updates
  • Incantation effects, broadcasts, egg laying, deaths

- **Interactive UI**
  • Camera controls
  • Overlay panels for player inventory, team scores, time unit
  • Debug console for raw protocol logs

---

## Architecture

```bash
+-----------------------+
|     Zappy Server      |
| (Game Logic & State)  |
+----------+------------+
           | TCP Connection
           | (Protocol Messages)
+----------v------------+
|    Network Module     |
|  - TCP Socket         |<-----------------+
|  - Message Queue      |                  |
|  - Protocol Parser    |                  |
+----------+------------+                  |
           |                               |
           | Parsed Events                 |
+----------v------------+                  |
|    Event Handler      |                  |
|  - Game State Update  |                  |
|  - Animation Triggers |                  |
+----------+------------+                  |
           |                               |
           | Updated State                 |
+----------v------------+     UI Events    |
|     Game State        |<-----------------+
|  - World Map          |                  |
|  - Players            |                  |
|  - Resources          |                  |
|  - Eggs               |                  |
+----------+------------+                  |
           |                               |
+----------v------------+     Commands     |
|  Rendering Engine     +<-----------------+
|  - Scene Graph        |                  |
|  - 3D Models          |                  |
|  - Camera System      |                  |
+----------+------------+                  |
           |                               |
           | Visual Output                 |
+----------v------------+                  |
|      3D Display       |                  |
+-----------------------+                  |
                                          |
+----------+------------+                  |
|   User Interface      +<-----------------+
|  - ImGui Overlay      |                  |
|  - Debug Console      |                  |
|  - Control System     +------------------+
+-----------------------+
```

- **Network Module**
  - Non-blocking TCP socket implementation
  - Protocol-compliant message parsing and dispatch
  - Thread-safe message queue

- **Rendering Engine**
  - Loads textures, models, and shaders on startup
  - Scene graph management for tiles, players, eggs
  - Frame-based update system

- **User Interface**
  - Dear ImGui overlay for debugging and controls
  - Camera controller (keyboard + mouse)
  - Context-sensitive information panels

---

## Usage
After connecting to server:

1. Listen map size: `msz`
2. Listen all tile contents: `mct`
3. Listen team names: `tna`
4. Listen for server events:
   - `bct` (tile updates)
   - `pnw`, `ppo`, `plv`, `pin` (player events)
   - `pic`, `pie` (incantations)
   - `enw`, `ebo`, `edi` (egg events)
   - `sgt`, `seg` (game state)

**Controls**:
- ZQSD: Camera movement
- Left click: Select objects
- F1: Toggle debug console

---

## Protocol & Communication

### Client Requests → Server
| Command | Parameters | Server Response |
|---------|------------|-----------------|
| `msz`   | None       | `msz X Y`      |
| `bct X Y`| Tile coordinates | `bct X Y q0 q1 q2 q3 q4 q5 q6` |
| `mct`   | None       | Sequence of `bct` for all tiles |
| `tna`   | None       | Sequence of `tna N` for all teams |
| `ppo #n`| Player ID  | `ppo #n X Y O` |
| `plv #n`| Player ID  | `plv #n L`     |
| `pin #n`| Player ID  | `pin #n X Y q0 q1 q2 q3 q4 q5 q6` |
| `sgt`   | None       | `sgt T`        |
| `sst T` | New time unit | `sst T`        |

### Server Events → Client
| Event | Parameters | Description |
|-------|------------|-------------|
| `pnw #n X Y O L N` | ID, position, orientation, level, team | New player |
| `ppo #n X Y O` | ID, position, orientation | Player position |
| `plv #n L` | ID, level | Player level |
| `pin #n X Y q0 q1 q2 q3 q4 q5 q6` | ID, position, inventory | Player inventory |
| `pex #n` | ID | Player expelled |
| `pbc #n M` | ID, message | Broadcast message |
| `pic X Y L #n #n...` | Position, level, player IDs | Incantation start |
| `pie X Y R` | Position, result (0/1) | Incantation end |
| `pfk #n` | ID | Egg laying |
| `pdr #n i` | ID, resource index | Resource drop |
| `pgt #n i` | ID, resource index | Resource collect |
| `pdi #n` | ID | Player death |
| `enw #e #n X Y` | Egg ID, player ID, position | Egg laid |
| `ebo #e` | Egg ID | Egg hatched |
| `edi #e` | Egg ID | Egg died |
| `sgt T` | Time unit | Time unit request |
| `sst T` | Time unit | Time unit modification |
| `seg N` | Team name | Game end |
| `smg M` | Message | Server message |
| `suc` | None | Unknown command |
| `sbp` | None | Bad parameter |

### Resource Indexes
```
0: food
1: linemate
2: deraumere
3: sibur
4: mendiane
5: phiras
6: thystame
```

### Orientation Codes
```
1: North
2: East
3: South
4: West
```

This protocol enables the GUI to maintain a synchronized state of the game world through bidirectional communication. The client implementation strictly follows this specification to ensure compatibility with Zappy servers.
