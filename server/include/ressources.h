/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** ressources
*/

#ifndef RESSOURCES_H_
    #define RESSOURCES_H_

    #include "flag.h"

typedef struct tile_s {
    int food;
    int linemate;
    int deraumere;
    int sibur;
    int mendiane;
    int phiras;
    int thystame;
} tile_t;

typedef struct server_config_s server_config_t;

void generate_map_resources(server_config_t *conf);
void send_whole_map(int fd, server_config_t *conf);
void generate_tile_resources(tile_t *tile);
void send_tile_content(int fd, int x, int y, tile_t *tile);

#endif /* !RESSOURCES_H_ */
