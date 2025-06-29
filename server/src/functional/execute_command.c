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

    for (client_t *c = conf->clients; c != NULL; c = c->next) {
        if (c != client && c->x == client->x && c->y == client->y) {
            eject_direction = (client->direction + 2) % 4;
            move_player(c, conf, eject_direction);
            notify_graphics_player_update(c, conf);
        }
    }
    send(client->fd, "ok\n", 3, 0);
}

void execute_command(client_t *client, server_config_t *conf, const char *cmd)
{
    char response[32];

    if (strncmp(cmd, "Forward", 7) == 0 || strncmp(cmd, "Right", 5) == 0 || strncmp(cmd, "Left", 4) == 0) {
        handle_movement_commands(client, conf, cmd);
        return;
    }
    if (strncmp(cmd, "Inventory", 9) == 0) {
        handle_inventory_command(client);
        return;
    }
    if (strncmp(cmd, "Take", 4) == 0) {
        take_object(client, conf, (char *)cmd);
        pin_graphics(client, conf);
        return;
    }
    if (strncmp(cmd, "Look", 4) == 0) {
        look_around(client, conf);
        return;
    }
    if (strncmp(cmd, "Fork", 4) == 0) {
        create_egg_fork(conf, client);
        send(client->fd, "ok\n", 3, 0);
        return;
    }
    if (strncmp(cmd, "Broadcast", 9) == 0) {
        handle_broadcast_command(client, conf, cmd);
        return;
    }
    if (strncmp(cmd, "Connect_nbr", 11) == 0) {
        snprintf(response, sizeof(response), "%d\n", conf->team_count *
            conf->clients_nb - find_nb_teams(client) + 1);
        send(client->fd, response, strlen(response), 0);
        return;
    }
    if (strncmp(cmd, "Set", 3) == 0) {
        set_object(client, conf, (char *)cmd, client->fd);
        return;
    }
    if (strncmp(cmd, "Eject", 5) == 0) {
        handle_eject_command(client, conf);
        return;
    }
    if (strncmp(cmd, "Incantation", 11) == 0) {
        handle_incantation(client, conf, client->fd);
        return;
    }
    send(client->fd, "ko\n", 3, 0);
}
