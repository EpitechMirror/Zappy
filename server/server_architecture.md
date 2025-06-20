## Server Architecture

### Core Components

1. **Network Module**
   - Socket management with `select()` for multiplexing
   - Client connection handling
   - Protocol implementation

2. **Game State Management**
   - World representation (2D grid)
   - Resource management and spawning
   - Player tracking
   - Time management

3. **Command Processor**
   - Parse and validate client commands
   - Execute commands with appropriate timing
   - Return results to clients

4. **Event System**
   - Manage timed events (resource spawning, egg hatching, etc.)
   - Handle player actions within time constraints

### Data Structures

```c
// World representation
typedef struct s_tile {
    int food;
    int linemate;
    int deraumere;
    int sibur;
    int mendiane;
    int phiras;
    int thystame;
    t_list *players;  // List of players on this tile
} t_tile;

typedef struct s_world {
    t_tile **tiles;   // 2D array of tiles
    int width;
    int height;
    int freq;         // Time unit reciprocal
} t_world;

// Player representation
typedef struct s_player {
    int id;
    int x;
    int y;
    int direction;    // 0:North, 1:East, 2:South, 3:West
    int level;
    int food;         // Food units (life)
    int linemate;
    int deraumere;
    int sibur;
    int mendiane;
    int phiras;
    int thystame;
    int socket_fd;
    char team_name[32];
    t_queue *command_queue;  // Queue of pending commands
} t_player;

// Team representation
typedef struct s_team {
    char name[32];
    int max_clients;
    int connected_clients;
    int available_slots;
    t_list *eggs;     // List of eggs waiting to hatch
} t_team;
```

### Main Server Loop

```c
int main(int argc, char **argv)
{
    // Parse command line arguments
    parse_args(argc, argv);

    // Initialize server
    init_server();

    // Main loop
    while (1) {
        // Prepare file descriptor sets for select()
        prepare_fd_sets();

        // Calculate next timeout for timed events
        struct timeval timeout = calculate_next_timeout();

        // Wait for socket activity or timeout
        select(max_fd + 1, &read_fds, &write_fds, NULL, &timeout);

        // Handle new connections
        if (FD_ISSET(server_socket, &read_fds))
            handle_new_connection();

        // Handle client communication
        for (each client socket) {
            if (FD_ISSET(client_socket, &read_fds))
                read_client_command(client_socket);

            if (FD_ISSET(client_socket, &write_fds))
                send_pending_responses(client_socket);
        }

        // Execute pending commands that are due
        execute_pending_commands();

        // Handle timed events (resource spawn, egg hatching, etc.)
        handle_timed_events();

        // Clean up disconnected clients
        clean_disconnected_clients();
    }

    return 0;
}
```

### Command Execution

Commands should be executed based on their specified time requirements. For example, the "Forward" command takes 7/f time units to complete. The server should:

1. Receive the command
2. Calculate the execution time
3. Store the command with its execution timestamp
4. Execute the command when the time is right
5. Send the response back to the client

```c
void schedule_command(t_player *player, char *command, int time_units)
{
    t_command *cmd = malloc(sizeof(t_command));

    cmd->player = player;
    cmd->command_str = strdup(command);
    cmd->execution_time = current_time + (time_units / world.freq);

    // Add to priority queue ordered by execution_time
    queue_add(command_queue, cmd);
}

void execute_pending_commands()
{
    while (!queue_empty(command_queue)) {
        t_command *cmd = queue_peek(command_queue);

        if (cmd->execution_time <= current_time) {
            queue_pop(command_queue);
            execute_command(cmd);
            free_command(cmd);
        } else {
            break; // No more commands to execute now
        }
    }
}
```

### Resource Management

Resources need to be spawned at start and periodically:

```c
void spawn_resources()
{
    int total_tiles = world.width * world.height;

    // Calculate quantities based on density and map size
    int food_qty = total_tiles * 0.5;
    int linemate_qty = total_tiles * 0.3;
    int deraumere_qty = total_tiles * 0.15;
    int sibur_qty = total_tiles * 0.1;
    int mendiane_qty = total_tiles * 0.1;
    int phiras_qty = total_tiles * 0.08;
    int thystame_qty = total_tiles * 0.05;

    // Place resources randomly on the map
    place_resource(FOOD, food_qty);
    place_resource(LINEMATE, linemate_qty);
    place_resource(DERAUMERE, deraumere_qty);
    place_resource(SIBUR, sibur_qty);
    place_resource(MENDIANE, mendiane_qty);
    place_resource(PHIRAS, phiras_qty);
    place_resource(THYSTAME, thystame_qty);
}
```
