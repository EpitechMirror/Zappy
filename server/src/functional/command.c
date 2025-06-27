/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** command
*/

#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "client_info.h"
#include "flag.h"
#include "ressources.h"
#include "server.h"

int respond_to_server_fd(int fd, server_config_t *conf, char *client_message, client_t *client)
{
    double now = get_current_time();
    double delay = get_command_delay(client_message, conf);
    double exec_time = client->last_command_end > now ? client->last_command_end : now;
    exec_time += delay;

    command_t *new_cmd = malloc(sizeof(command_t));
    new_cmd->cmd = strdup(client_message);
    new_cmd->execute_at = exec_time;
    new_cmd->next = NULL;

    if (!client->command_queue)
        client->command_queue = new_cmd;
    else {
        command_t *tmp = client->command_queue;
        while (tmp->next) tmp = tmp->next;
        tmp->next = new_cmd;
    }
    client->last_command_end = exec_time;
    return 0;
}

void execute_pending_commands(server_config_t *conf)
{
    double now = get_current_time();
    for (client_t *c = conf->clients; c; c = c->next) {
        if (!c->is_alive || !c->command_queue)
            continue;
        command_t *cmd = c->command_queue;
        if (cmd->execute_at <= now) {
            execute_command(c, conf, cmd->cmd);
            c->command_queue = cmd->next;
            free(cmd->cmd);
            free(cmd);
        }
    }
}

void execute_command(client_t *client, server_config_t *conf, const char *cmd)
{
    char inventory[256];

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
    }
    if (strncmp(cmd, "Left", 4) == 0) {
        client->direction = (client->direction + 3) % 4;
        send(client->fd, "ok\n", 3, 0);
        ppo_graphics(client, conf);
        pin_graphics(client, conf);
        return;
    }
    if (strncmp(cmd, "Inventory", 9) == 0) {
        get_tile_contents(client->x, client->y, client, inventory);
        snprintf(inventory, sizeof(inventory), "[linemate %d, deraumere %d, sibur %d, mendiane %d, phiras %d, thystame %d, food %d]\n",
            client->inventory.linemate,
            client->inventory.deraumere,
            client->inventory.sibur,
            client->inventory.mendiane,
            client->inventory.phiras,
            client->inventory.thystame,
            client->inventory.food
        );
        send(client->fd, inventory, strlen(inventory), 0);
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
        create_egg_fork(client, conf);
        send(client->fd, "ok\n", 3, 0);
        return;
    }
    if (strncmp(cmd, "Broadcast", 9) == 0) {
        char *message = (char *)cmd + 10;
        for (client_t *c = conf->clients; c != NULL; c = c->next) {
            if (c != client) {
                int direction_tile = calculate_direction_tile(client, c);
                char broadcast_msg[512];
                snprintf(broadcast_msg, sizeof(broadcast_msg), "message %d, %s\n", direction_tile, message);
                send(c->fd, broadcast_msg, strlen(broadcast_msg), 0);
            }
        }
        send(client->fd, "ok\n", 3, 0);
        return;
    }
    if (strncmp(cmd, "Connect_nbr", 11) == 0) {
        char response[32];
        snprintf(response, sizeof(response), "%d\n", conf->team_count * conf->clients_nb - find_nb_teams(client) + 1);
        send(client->fd, response, strlen(response), 0);
        return;
    }
    if (strncmp(cmd, "Set", 3) == 0) {
        set_object(client, conf, (char *)cmd, client->fd);
        return;
    }
    if (strncmp(cmd, "Eject", 5) == 0) {
        for (client_t *c = conf->clients; c != NULL; c = c->next) {
            if (c != client && c->x == client->x && c->y == client->y) {
                int eject_direction = (client->direction + 2) % 4;
                move_player(c, conf, eject_direction);
                notify_graphics_player_update(c, conf);
            }
        }
        send(client->fd, "ok\n", 3, 0);
        return;
    }
    if (strncmp(cmd, "Incantation", 11) == 0) {
        handle_incantation(client, conf, client->fd);
        return;
    }
    send(client->fd, "ko\n", 3, 0);
}
