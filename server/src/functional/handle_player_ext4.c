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

static int handle_resource(const char *resource,
    const resource_mapping_t *mapping, int fd)
{
    if (strcmp(resource, mapping->name) == 0 && *mapping->inventory_ptr > 0) {
        (*mapping->tile_ptr)++;
        (*mapping->inventory_ptr)--;
        send(fd, "ok\n", 3, 0);
        return 1;
    }
    return 0;
}

void set_object(client_t *client, server_config_t *conf, char *client_message,
    int fd)
{
    char *resourc = client_message + 4;
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

    for (int i = 0; i < 7; i++)
        if (handle_resource(resourc, &mappings[i], fd)) {
            notify_graphics_player_update(client, conf);
            return;
        }
    send(fd, "ko\n", 3, 0);
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
