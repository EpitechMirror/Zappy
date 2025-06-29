/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** movement
*/

#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "client_info.h"
#include "flag.h"
#include "ressources.h"
#include "server.h"

static void move_north(client_t *client, server_config_t *conf)
{
    client->y = (client->y + conf->height - 1) % conf->height;
}

static void move_east(client_t *client, server_config_t *conf)
{
    client->x = (client->x + 1) % conf->width;
}

static void move_south(client_t *client, server_config_t *conf)
{
    client->y = (client->y + 1) % conf->height;
}

static void move_west(client_t *client, server_config_t *conf)
{
    client->x = (client->x + conf->width - 1) % conf->width;
}

void move_player(client_t *client, server_config_t *conf, direction_t direction)
{
    static void (*move_functions[])(client_t *, server_config_t *) = {
        move_north,
        move_east,
        move_south,
        move_west
    };

    if (direction >= 0 && direction < 4)
        move_functions[direction](client, conf);
}
