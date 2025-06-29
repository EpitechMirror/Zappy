# Enhanced Zappy AI - Multi-Process Forking System

## Overview

The Zappy AI has been enhanced with an intelligent multi-process forking system that automatically spawns multiple AI clients to maximize the use of available team eggs. This system ensures your team takes advantage of every possible slot on the server.

## New Features

### 1. AI Manager (`ai_manager.py`)

The AI Manager is a process orchestrator that:
- **Automatically detects available team slots** from the server
- **Spawns AI clients** for each available egg/slot
- **Monitors and manages** multiple AI processes
- **Handles process cleanup** when AIs complete or fail
- **Provides real-time statistics** about active processes

#### Key Features:
- **Smart spawning logic**: Spawns AIs based on server capacity and availability
- **Process management**: Tracks active processes and cleans up completed ones
- **Resource awareness**: Respects server limits and prevents overloading
- **Signal handling**: Graceful shutdown on interruption
- **Statistics tracking**: Monitors success rates and performance

### 2. Enhanced Main AI (`main_ai.py`)

The main AI has been enhanced with:
- **Intelligent forking logic**: Decides when to create new eggs based on game state
- **Fork condition analysis**: Considers food levels, resources, and game phase
- **Fork cooldown management**: Prevents excessive forking attempts
- **Enhanced reward system**: Rewards successful forking behavior
- **Performance tracking**: Monitors fork success rates

#### Fork Decision Logic:
- **Early game**: Fork when food > 50 and stable
- **Mid/late game**: Fork when resources are abundant (excess stones)
- **High level**: Fork when level ≥ 3 and food > 80
- **Cooldown**: 60-second minimum between fork attempts

### 3. Smart AI Wrapper (`smart_zappy_ai.py`)

A user-friendly wrapper that:
- **Auto-detects server capacity**: Connects and reads available slots
- **Chooses optimal strategy**: Single AI or multi-process manager
- **Provides easy CLI**: Simple command-line interface
- **Backward compatibility**: Can run as single AI if needed

## Usage

### Basic Multi-Process Mode (Recommended)

```bash
# Automatically spawn AIs for all available eggs
python src/smart_zappy_ai.py -p 4242 -n MyTeam

# Limit maximum processes
python src/smart_zappy_ai.py -p 4242 -n MyTeam -m 5

# Connect to remote server
python src/smart_zappy_ai.py -p 4242 -n MyTeam -H server.example.com
```

### Direct AI Manager

```bash
# Run AI manager directly with custom settings
python src/ai_manager.py -p 4242 -n MyTeam --max-processes 8 --spawn-interval 3.0
```

### Single AI Mode (Legacy)

```bash
# Run single AI instance
python src/smart_zappy_ai.py -p 4242 -n MyTeam --single
```

## Architecture

```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   Zappy Server  │◄──►│   AI Manager     │◄──►│  Smart Wrapper  │
│                 │    │  (Orchestrator)  │    │  (Entry Point)  │
└─────────────────┘    └──────────────────┘    └─────────────────┘
                              │
                              ▼
                       ┌──────────────────┐
                       │   AI Process 1   │
                       │  (main_ai.py)    │
                       └──────────────────┘
                              │
                              ▼
                       ┌──────────────────┐
                       │   AI Process 2   │
                       │  (main_ai.py)    │
                       └──────────────────┘
                              │
                             ...
                              │
                              ▼
                       ┌──────────────────┐
                       │   AI Process N   │
                       │  (main_ai.py)    │
                       └──────────────────┘
```

## Configuration

### AI Manager Settings

- `--max-processes`: Maximum number of concurrent AI processes (default: 10)
- `--spawn-interval`: Minimum time between spawn attempts in seconds (default: 2.0)
- `--hostname`: Server hostname (default: localhost)

### Fork Parameters (in main_ai.py)

- `fork_cooldown`: Time between fork attempts (60 seconds)
- `should_fork_early`: Aggressive early-game forking flag
- Fork conditions based on food levels, resources, and game phase

## Monitoring

The AI Manager provides real-time statistics:

```
📊 AI Manager Statistics:
   Total spawned: 8
   Currently active: 5
   Completed: 3
   Successful spawns: 8
   Failed spawns: 0
   Available slots: 6
   Max processes: 10
```

Each AI process logs with its own identifier:
```
[AI-1] 🧠 Turn 150: Thinking...
[AI-1]    Current Level: 2
[AI-1]    Food: 45
[AI-2] 🍴 Attempting to fork! (Food: 75, Level: 3)
[AI-3] ✅ AI Process #3 completed successfully
```

## Benefits

1. **Maximum Team Presence**: Utilizes every available team slot
2. **Automatic Scaling**: Adapts to server capacity and availability
3. **Fault Tolerance**: Individual AI failures don't affect others
4. **Resource Efficiency**: Intelligent spawning based on server state
5. **Performance Tracking**: Detailed metrics for optimization
6. **Easy Management**: Single command starts entire team

## Testing

The forking system includes comprehensive tests:

```bash
# Test AI Manager
python -m pytest tests/test_ai_manager.py -v

# Test fork functionality
python -m pytest tests/test_main_ai.py -k "fork" -v

# Run all tests
python -m pytest
```

## Troubleshooting

### Common Issues

1. **Too many processes**: Reduce `--max-processes` if system resources are limited
2. **Connection failures**: Check server availability and network settings
3. **Process cleanup**: Manager automatically handles process lifecycle
4. **Memory usage**: Each AI process uses ~50-100MB of RAM

### Debug Mode

Enable verbose logging by modifying the AI Manager or individual AIs to increase verbosity.

## Advanced Usage

### Custom Fork Strategy

Modify `should_fork()` in `main_ai.py` to implement custom forking logic:

```python
def should_fork(self) -> bool:
    # Custom logic here
    return your_custom_condition
```

### Integration with Existing Systems

The enhanced AI is fully backward compatible. Existing single-AI deployments continue to work without modification.

## Performance Impact

- **CPU**: Moderate increase due to multiple processes
- **Memory**: ~50-100MB per AI process
- **Network**: Proportional to number of active AIs
- **Server Load**: Distributed across multiple connections

## Future Enhancements

- Dynamic process scaling based on server load
- Cross-process communication for team coordination
- Advanced fork strategies based on game statistics
- Integration with server monitoring for optimal timing

---

This enhanced forking system transforms your Zappy AI from a single player into a coordinated team that automatically maximizes its presence on the server!
