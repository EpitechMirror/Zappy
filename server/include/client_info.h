/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** client_info
*/

#ifndef CLIENT_INFO_H_
    #define CLIENT_INFO_H_
    #include <stdbool.h>

typedef enum {
    WAITING_NAME,
    AUTHENTICATED
} client_state_t;


typedef enum {
    NORTH,
    EAST,
    SOUTH,
    WEST
} direction_t;

typedef struct tilecp_s {
    int linemate;
    int deraumere;
    int sibur;
    int mendiane;
    int phiras;
    int thystame;
    int food;
} tilecp_t;

typedef struct command_s {
    char *cmd;
    double execute_at;
    struct command_s *next;
} command_t;


typedef struct client_s {
    int fd;
    bool connected;
    int team_idx;
    bool is_ready;
    bool is_alive;
    int x, y, id;
    char *team_name;
    bool is_graphic;
    int level;
    tilecp_t inventory;
    direction_t direction;
    int hunger_tick;
    client_state_t state;
    struct client_s *next;
    command_t *command_queue;
    double last_command_end;
    
    // Champs pour l'incantation
    bool is_incanting;
    double incantation_end_time;
    int incantation_result;
} client_t;

typedef struct {
    int nb_players;
    int linemate;
    int deraumere;
    int sibur;
    int mendiane;
    int phiras;
    int thystame;
} elevation_req_t;


#endif /* !CLIENT_INFO_H_ */
