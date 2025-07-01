/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** handle_client_data
*/

#include "../../include/client_info.h"
#include "../../include/flag.h"
#include "../../include/ressources.h"
#include "../../include/server.h"

void send_pdr(int client_id, int resource_index, server_config_t *conf)
{
    char buffer[64];
    int len = snprintf(buffer, sizeof(buffer), "pdr %d %d\n", client_id, resource_index);
    
    for (int i = 0; i < conf->clients_nb; i++) {
        if (conf->clients[i].is_graphic && conf->clients[i].fd != -1) {
            send(conf->clients[i].fd, buffer, len, 0);
        }
    }
}


void handle_drop_command(client_t *client, server_config_t *conf,
    const char *cmd)
{
    char *resource = (char *)cmd + 5;
    tile_t *tile = &conf->map[client->y][client->x];
    resource_mapping_t mappings[] = {
        {"linemate", &tile->linemate, &client->inventory.linemate},
        {"deraumere", &tile->deraumere, &client->inventory.deraumere},
        {"sibur", &tile->sibur, &client->inventory.sibur},
        {"mendiane", &tile->mendiane, &client->inventory.mendiane},
        {"phiras", &tile->phiras, &client->inventory.phiras},
        {"thystame", &tile->thystame, &client->inventory.thystame},
        {"food", &tile->food, &client->inventory.food}
    };

    for (int i = 0; i < 7; i++) {
        if (strcmp(resource, mappings[i].name) == 0 && *mappings[i].inventory_ptr > 0) {
            (*mappings[i].tile_ptr)++;
            (*mappings[i].inventory_ptr)--;
            send(client->fd, "ok\n", 3, 0);
            send_pdr(client->fd, i, conf);
            notify_graphics_player_update(client, conf, 1);
            return;
        }
    }
    send(client->fd, "ko\n", 3, 0);
}

static const command_handler_t command_handlers[] = {
    {"Forward", 7, handle_movement_commands},
    {"Right", 5, handle_movement_commands},
    {"Left", 4, handle_movement_commands},
    {"Inventory", 9, handle_inventory_wrapper},
    {"Take", 4, handle_take_command},
    {"Look", 4, handle_look_command},
    {"Fork", 4, handle_fork_command},
    {"Broadcast", 9, handle_broadcast_command},
    {"Connect_nbr", 11, handle_connect_nbr_command},
    {"Set", 3, handle_set_command},
    {"Eject", 5, handle_eject_wrapper},
    {"Incantation", 11, handle_incantation_command},
    {"Drop", 4, handle_drop_command},
    {NULL, 0, NULL}
};

static void handle_movement_commands(client_t *client, server_config_t *conf,
    const char *cmd)
{
    if (strncmp(cmd, "Forward", 7) == 0) {
        move_player(client, conf, client->direction);
        send(client->fd, "ok\n", 3, 0);
        ppo_graphics(client, conf);
        pin_graphics(client, conf);
        return;
    }
    if (strncmp(cmd, "Right", 5) == 0) {
        client->direction = (client->direction + 1) % 4;
        send(client->fd, "ok\n", 3, 0);
        ppo_graphics(client, conf);
        pin_graphics(client, conf);
        return;
    } else if (strncmp(cmd, "Left", 4) == 0) {
        client->direction = (client->direction + 3) % 4;
        send(client->fd, "ok\n", 3, 0);
        ppo_graphics(client, conf);
        pin_graphics(client, conf);
        return;
    }
}

static void handle_inventory_command(client_t *client)
{
    char inventory[256];

    snprintf(inventory, sizeof(inventory), "[linemate %d, deraumere %d, "
    "sibur %d, mendiane %d, phiras %d, thystame %d, food %d]\n",
        client->inventory.linemate,
        client->inventory.deraumere,
        client->inventory.sibur,
        client->inventory.mendiane,
        client->inventory.phiras,
        client->inventory.thystame,
        client->inventory.food
    );
    send(client->fd, inventory, strlen(inventory), 0);
}

static void handle_broadcast_command(client_t *client, server_config_t *conf,
    const char *cmd)
{
    char *message = (char *)cmd + 10;
    int direction_tile;
    char broadcast_msg[512];

    for (client_t *c = conf->clients; c != NULL; c = c->next) {
        if (c != client) {
            direction_tile = calculate_direction_tile(client, c);
            snprintf(broadcast_msg, sizeof(broadcast_msg),
                "message %d, %s\n", direction_tile, message);
            send(c->fd, broadcast_msg, strlen(broadcast_msg), 0);
        }
    }
    send(client->fd, "ok\n", 3, 0);
}

static void handle_eject_command(client_t *client, server_config_t *conf)
{
    int eject_direction;
    int result = -1;

    for (client_t *c = conf->clients; c != NULL; c = c->next) {
        if (c != client && c->x == client->x && c->y == client->y) {
            eject_direction = (client->direction + 2) % 4;
            move_player(c, conf, eject_direction);
            notify_graphics_player_update(c, conf, result);
        }
    }
    send(client->fd, "ok\n", 3, 0);
}

static void handle_connect_nbr_command(client_t *client, server_config_t *conf,
    const char *cmd)
{
    char response[32];

    (void)cmd;
    snprintf(response, sizeof(response), "%d\n", conf->team_count *
        conf->clients_nb - find_nb_teams(client) + 1);
    send(client->fd, response, strlen(response), 0);
}

static void handle_set_command(client_t *client, server_config_t *conf,
    const char *cmd)
{
    set_object(client, conf, (char *)cmd, client->fd);
}

static void handle_incantation_command(client_t *client, server_config_t *conf,
    const char *cmd)
{
    (void)cmd;
    handle_incantation(client, conf, client->fd);
}

static void handle_inventory_wrapper(client_t *client, server_config_t *conf,
    const char *cmd)
{
    (void)conf;
    (void)cmd;
    handle_inventory_command(client);
}

static void handle_eject_wrapper(client_t *client, server_config_t *conf,
    const char *cmd)
{
    (void)cmd;
    handle_eject_command(client, conf);
}

void execute_command(client_t *client, server_config_t *conf, const char *cmd)
{
    for (int i = 0; command_handlers[i].command != NULL; i++) {
        if (strncmp(cmd, command_handlers[i].command,
            command_handlers[i].length) == 0) {
            command_handlers[i].handler(client, conf, cmd);
            return;
        }
    }
    send(client->fd, "ko\n", 3, 0);
}
