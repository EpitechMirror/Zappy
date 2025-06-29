/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** graphics
*/

#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "client_info.h"
#include "flag.h"
#include "ressources.h"
#include "server.h"

void ppo_graphics(client_t *client, server_config_t *conf)
{
    char msg[128];
    snprintf(msg, sizeof(msg), "ppo #%d %d %d %d\n", client->fd, client->x, client->y, client->direction + 1);
    for (int i = 0; i < conf->nb_graphics; i++)
        send(conf->graphic_fds[i], msg, strlen(msg), 0);
}

void plv_graphics(client_t *client, server_config_t *conf)
{
    char msg[64];
    snprintf(msg, sizeof(msg), "plv #%d %d\n", client->fd, client->level);
    for (int i = 0; i < conf->nb_graphics; i++)
        send(conf->graphic_fds[i], msg, strlen(msg), 0);
}

void pin_graphics(client_t *client, server_config_t *conf)
{
    char msg[256];
    snprintf(msg, sizeof(msg), "pin #%d %d %d %d %d %d %d %d %d %d\n",
        client->fd, client->x, client->y,
        client->inventory.food,
        client->inventory.linemate,
        client->inventory.deraumere,
        client->inventory.sibur,
        client->inventory.mendiane,
        client->inventory.phiras,
        client->inventory.thystame
    );
    for (int i = 0; i < conf->nb_graphics; i++)
        send(conf->graphic_fds[i], msg, strlen(msg), 0);
}

void notify_graphics_player_update(client_t *client, server_config_t *conf)
{
    ppo_graphics(client, conf);
    plv_graphics(client, conf);
    pin_graphics(client, conf);
}

void send_tile_update_to_gui(server_config_t *conf, int x, int y)
{
    char msg[256];
    tile_t *tile = &conf->map[y][x];

    snprintf(msg, sizeof(msg),
        "bct %d %d %d %d %d %d %d %d %d\n",
        x, y, tile->food, tile->linemate, tile->deraumere,
        tile->sibur, tile->mendiane, tile->phiras, tile->thystame);

    for (int i = 0; i < conf->nb_graphics; i++) {
        send(conf->graphic_fds[i], msg, strlen(msg), 0);
    }
}