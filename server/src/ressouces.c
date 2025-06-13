/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** ressouces
*/

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "ressources.h"
#include "server.h"

static void clear_map_resources(server_config_t *conf)
{
    int y = 0;
    int x = 0;
    tile_t *tile;

    for (y = 0; y < conf->height; y++) {
        for (x = 0; x < conf->width; x++) {
            tile = &conf->map[y][x];
            tile->food = 0;
            tile->linemate = 0;
            tile->deraumere = 0;
            tile->sibur = 0;
            tile->mendiane = 0;
            tile->phiras = 0;
            tile->thystame = 0;
        }
    }
}

static void place_resource_randomly(server_config_t *conf,
    int resource_offset, int quantity)
{
    int w = conf->width;
    int h = conf->height;
    int i = 0;
    int x;
    int y;
    tile_t *tile;
    int *resource_ptr;

    for (i = 0; i < quantity; i++) {
        x = rand() % w;
        y = rand() % h;
        tile = &conf->map[y][x];
        resource_ptr = ((int *)tile) + resource_offset;
        (*resource_ptr)++;
    }
}

void generate_map_resources(server_config_t *conf)
{
    int total_tiles = conf->width * conf->height;
    int food_qty = total_tiles * 0.5;
    int linemate_qty = total_tiles * 0.3;
    int deraumere_qty = total_tiles * 0.15;
    int sibur_qty = total_tiles * 0.1;
    int mendiane_qty = total_tiles * 0.1;
    int phiras_qty = total_tiles * 0.08;
    int thystame_qty = total_tiles * 0.05;

    srand(time(NULL));
    clear_map_resources(conf);
    place_resource_randomly(conf, 0, food_qty);
    place_resource_randomly(conf, 1, linemate_qty);
    place_resource_randomly(conf, 2, deraumere_qty);
    place_resource_randomly(conf, 3, sibur_qty);
    place_resource_randomly(conf, 4, mendiane_qty);
    place_resource_randomly(conf, 5, phiras_qty);
    place_resource_randomly(conf, 6, thystame_qty);
}

void send_tile_content(int fd, int x, int y, tile_t *tile)
{
    char msg[128];

    if (!tile)
        return;
    snprintf(msg, sizeof(msg),
        "bct %d %d %d %d %d %d %d %d %d\n",
        x, y, tile->food, tile->linemate, tile->deraumere,
        tile->sibur, tile->mendiane, tile->phiras, tile->thystame);
    send(fd, msg, strlen(msg), 0);
}

void send_whole_map(int fd, server_config_t *conf)
{
    int y = 0;
    int x = 0;

    for (y = 0; y < conf->height; y++) {
        for (x = 0; x < conf->width; x++) {
            send_tile_content(fd, x, y, &conf->map[y][x]);
        }
    }
}
