/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** tiles
*/

#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "client_info.h"
#include "flag.h"
#include "ressources.h"
#include "server.h"

static void add_resource_to_buffer(char *buffer, char *name, int count, int *first)
{
    for (int i = 0; i < count; i++) {
        if (!*first) strcat(buffer, " ");
        strcat(buffer, name);
        *first = 0;
    }
}

void get_tile_content_string(server_config_t *conf, int x, int y, char *buffer, int buffer_size)
{
    if (x < 0 || x >= conf->width || y < 0 || y >= conf->height) {
        strcpy(buffer, "");
        return;
    }

    tile_t *tile = &conf->map[y][x];
    char temp[512] = "";
    int first = 1;
    add_resource_to_buffer(temp, "food", tile->food, &first);
    add_resource_to_buffer(temp, "linemate", tile->linemate, &first);
    add_resource_to_buffer(temp, "deraumere", tile->deraumere, &first);
    add_resource_to_buffer(temp, "sibur", tile->sibur, &first);
    add_resource_to_buffer(temp, "mendiane", tile->mendiane, &first);
    add_resource_to_buffer(temp, "phiras", tile->phiras, &first);
    add_resource_to_buffer(temp, "thystame", tile->thystame, &first);
    strncpy(buffer, temp, buffer_size - 1);
    buffer[buffer_size - 1] = '\0';
}

void add_players_to_tile(server_config_t *conf, int x, int y, char *buffer, int buffer_size, client_t *current_client)
{
    char temp[512];
    strcpy(temp, buffer);
    if (x == current_client->x && y == current_client->y) {
        if (strlen(temp) > 0) {
            strcat(temp, " ");
        }
        strcat(temp, "player");
    }
    strncpy(buffer, temp, buffer_size - 1);
    buffer[buffer_size - 1] = '\0';
}

int count_players_on_tile(server_config_t *conf, int x, int y, int level)
{
    int count = 0;
    for (client_t *c = conf->clients; c != NULL; c = c->next) {
        if (c->x == x && c->y == y && c->level == level) {
            count++;
        }
    }
    return count;
}

int check_tile_resources(server_config_t *conf, int x, int y, const elevation_req_t *req)
{
    tile_t *tile = &conf->map[y][x];

    return (tile->linemate >= req->linemate &&
            tile->deraumere >= req->deraumere &&
            tile->sibur >= req->sibur &&
            tile->mendiane >= req->mendiane &&
            tile->phiras >= req->phiras &&
            tile->thystame >= req->thystame);
}

void consume_tile_resources(server_config_t *conf, int x, int y, const elevation_req_t *req)
{
    tile_t *tile = &conf->map[y][x];

    tile->linemate -= req->linemate;
    tile->deraumere -= req->deraumere;
    tile->sibur -= req->sibur;
    tile->mendiane -= req->mendiane;
    tile->phiras -= req->phiras;
    tile->thystame -= req->thystame;
}
