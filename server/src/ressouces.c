/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** ressouces
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "ressources.h"
#include "server.h"

void generate_map_resources(server_config_t *conf)
{
    srand(time(NULL));
    for (int y = 0; y < conf->height; y++)
        for (int x = 0; x < conf->width; x++)
            generate_tile_resources(&conf->map[y][x]);
}

void send_whole_map(int fd, server_config_t *conf)
{
    printf("sending whole map to fd %d\n", fd);
    for (int y = 0; y < conf->height; y++) {
        for (int x = 0; x < conf->width; x++) {
            send_tile_content(fd, x, y, &conf->map[y][x]);
        }
    }
}

void generate_tile_resources(tile_t *tile)
{
    if (!tile)
        return;
    tile->food = rand() % 5;
    tile->linemate = rand() % 3;
    tile->deraumere = rand() % 2;
    tile->sibur = rand() % 2;
    tile->mendiane = rand() % 2;
    tile->phiras = rand() % 2;
    tile->thystame = rand() % 1;
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
    printf("[DEBUG] send_tile_content: %s", msg);
}
