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

void set_object(client_t *client, server_config_t *conf, char *client_message,
    int fd)
{
    char *resourc = client_message + 4;
    tile_t *tile = &conf->map[client->y][client->x];

    if (strcmp(resourc, "linemate") == 0 && client->inventory.linemate > 0) {
        tile->linemate++;
        client->inventory.linemate--;
        send(fd, "ok\n", 3, 0);
    }
    if (strcmp(resourc, "deraumere") == 0 && client->inventory.deraumere > 0) {
        tile->deraumere++;
        client->inventory.deraumere--;
        send(fd, "ok\n", 3, 0);
    }
    if (strcmp(resourc, "sibur") == 0 && client->inventory.sibur > 0) {
        tile->sibur++;
        client->inventory.sibur--;
        send(fd, "ok\n", 3, 0);
    }
    if (strcmp(resourc, "mendiane") == 0 && client->inventory.mendiane > 0) {
        tile->mendiane++;
        client->inventory.mendiane--;
        send(fd, "ok\n", 3, 0);
    }
    if (strcmp(resourc, "phiras") == 0 && client->inventory.phiras > 0) {
        tile->phiras++;
        client->inventory.phiras--;
        send(fd, "ok\n", 3, 0);
    }
    if (strcmp(resourc, "thystame") == 0 && client->inventory.thystame > 0) {
        tile->thystame++;
        client->inventory.thystame--;
        send(fd, "ok\n", 3, 0);
    }
    if (strcmp(resourc, "food") == 0 && client->inventory.food > 0) {
        tile->food++;
        client->inventory.food--;
        send(fd, "ok\n", 3, 0);
    } else
        send(fd, "ko\n", 3, 0);
    notify_graphics_player_update(client, conf);
}

int count_players_on_tile(server_config_t *conf, int x, int y, int level)
{
    int count = 0;

    for (client_t *c = conf->clients; c != NULL; c = c->next)
        if (c->x == x && c->y == y && c->level == level)
            count++;
    return count;
}

int check_tile_resources(server_config_t *conf, int x, int y,
    const elevation_req_t *req)
{
    tile_t *tile = &conf->map[y][x];

    return (tile->linemate >= req->linemate &&
            tile->deraumere >= req->deraumere &&
            tile->sibur >= req->sibur &&
            tile->mendiane >= req->mendiane &&
            tile->phiras >= req->phiras &&
            tile->thystame >= req->thystame);
}

void consume_tile_resources(server_config_t *conf, int x, int y,
    const elevation_req_t *req)
{
    tile_t *tile = &conf->map[y][x];

    tile->linemate -= req->linemate;
    tile->deraumere -= req->deraumere;
    tile->sibur -= req->sibur;
    tile->mendiane -= req->mendiane;
    tile->phiras -= req->phiras;
    tile->thystame -= req->thystame;
}

void elevate_players_on_tile(server_config_t *conf, int x, int y, int level)
{
    char level_msg[32];

    for (client_t *c = conf->clients; c != NULL; c = c->next) {
        if (c->x == x && c->y == y && c->level == level) {
            c->level++;
            notify_graphics_player_update(c, conf);
            send(c->fd, "Current level: ", 15, 0);
            snprintf(level_msg, sizeof(level_msg), "%d\n", c->level);
            send(c->fd, level_msg, strlen(level_msg), 0);
        }
    }
}
