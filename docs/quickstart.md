# Quick Start Guide

Get Zappy up and running in 5 minutes! This guide will walk you through the essential steps to build and run your first Zappy game.

## 📋 Prerequisites

Before we begin, make sure you have the required dependencies installed on your system.

### Ubuntu/Debian

```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config libsfml-dev git
```

### CentOS/RHEL/Fedora

```bash
# CentOS/RHEL
sudo yum groupinstall "Development Tools"
sudo yum install cmake sfml-devel git

# Fedora
sudo dnf groupinstall "Development Tools"
sudo dnf install cmake SFML-devel git
```

### macOS

```bash
# Install Homebrew if you haven't already
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake sfml pkg-config
```

## 🚀 Build Zappy

### Step 1: Clone the Repository

```bash
git clone https://github.com/EpitechMirror/Zappy.git
cd Zappy
```

### Step 2: Build All Components

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build all components (server, GUI, AI)
make -j$(nproc)
```

> [!TIP]
> The `-j$(nproc)` flag uses all available CPU cores for faster compilation. You can also use `make -j4` for 4 cores, or just `make` for single-threaded compilation.

### Expected Output

After successful compilation, you should see three executables:
- `zappy_server` - The game server
- `zappy_gui` - The graphical client  
- `zappy_ai` - The AI client

## 🎮 Run Your First Game

Now let's start a game with 2 teams and see it in action!

### Step 3: Start the Server

Open a terminal and run:

```bash
./zappy_server -p 4242 -x 10 -y 10 -n TeamRed TeamBlue -c 3 -f 100
```

**Parameters explained:**
- `-p 4242` - Server port
- `-x 10 -y 10` - Map size (10x10 grid)
- `-n TeamRed TeamBlue` - Two teams named "TeamRed" and "TeamBlue"
- `-c 3` - 3 clients allowed per team
- `-f 100` - Frequency (100 time units per second)

You should see output like:
```
Server started on port 4242
Map size: 10x10
Teams: TeamRed, TeamBlue
Waiting for clients...
```

### Step 4: Start the GUI Client

Open a **second terminal** and run:

```bash
./zappy_gui -p 4242 -h localhost
```

This will launch the 3D visualization window where you can watch the game unfold in real-time!

### Step 5: Add AI Players

Open **third and fourth terminals** to add AI players:

```bash
# Terminal 3 - TeamRed player
./zappy_ai -p 4242 -n TeamRed -h localhost

# Terminal 4 - TeamBlue player  
./zappy_ai -p 4242 -n TeamBlue -h localhost
```

## 🎊 Congratulations!

You now have a complete Zappy game running with:
- ✅ Server managing the game world
- ✅ GUI client showing real-time 3D visualization
- ✅ AI players from both teams competing autonomously

## 🎮 What You'll See

### In the GUI Window
- **3D Grid World** - The toroidal game map
- **Colored Players** - Each team has different colors
- **Resources** - Food and stones scattered across the map
- **Real-time Actions** - Watch players move, collect resources, and level up
- **Team Statistics** - Player counts, levels, and scores

### In the Terminal
- **Server Log** - Connection events, player actions, game state changes
- **AI Client Logs** - Decision making, strategies, and actions

## 🎯 Next Steps

Now that you have Zappy running, explore these areas:

### 📚 Learn More
- **[Game Mechanics](README.md#game-mechanics)** - Understand resources, leveling, and victory conditions
- **[Protocol Documentation](protocol.md)** - Learn the communication protocol
- **[Architecture Guide](architecture.md)** - Dive into the system design

### 🔧 Customize
- **[Server Configuration](server/configuration.md)** - Adjust game parameters
- **[GUI Controls](gui/controls.md)** - Learn camera controls and UI features
- **[AI Strategies](ai/strategies.md)** - Understand and modify AI behavior

### 🛠️ Develop
- **[Contributing Guide](contributing.md)** - Add features or fix bugs
- **[Testing](testing.md)** - Run the test suites
- **[Development Setup](development.md)** - Set up a development environment

## 🔧 Common Issues

### Build Errors

**CMake not found:**
```bash
# Ubuntu/Debian
sudo apt install cmake

# macOS
brew install cmake
```

**SFML not found:**
```bash
# Ubuntu/Debian
sudo apt install libsfml-dev

# macOS
brew install sfml
```

### Runtime Issues

**"Address already in use" error:**
```bash
# Kill any existing server processes
pkill zappy_server
# Or use a different port
./zappy_server -p 4243 ...
```

**GUI window doesn't open:**
- Check that you have a display/graphics environment
- On Linux, make sure X11 forwarding is enabled if using SSH
- Try running with `DISPLAY=:0 ./zappy_gui ...`

**AI clients can't connect:**
- Verify the server is running and port is correct
- Check firewall settings
- Ensure team names match exactly (case-sensitive)

## 🆘 Need Help?

- 📖 **Full Documentation**: Browse the complete [documentation](README.md)
- 🐛 **Report Issues**: [GitHub Issues](https://github.com/EpitechMirror/Zappy/issues)
- 💬 **Ask Questions**: [GitHub Discussions](https://github.com/EpitechMirror/Zappy/discussions)
- 📧 **Contact Team**: Reach out to the development team

---

> [!SUCCESS]
> You've successfully set up Zappy! Welcome to the world of Trantor. May the best team win! 🏆
