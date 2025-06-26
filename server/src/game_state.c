/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** game_state
*/

#include "client_info.h"
#include "flag.h"
#include "ressources.h"
#include "server.h"

void periodic_resource_respawn(server_config_t *conf)
{
    int total_tiles = conf->width * conf->height;
    for (int res = 0; res < RESOURCE_TYPES; res++) {
        int min_qty = total_tiles * resource_min_ratio[res];
        int total_qty = count_total_resource(conf, res);
        if (total_qty < min_qty) {
            int to_add = rand() % 3 + 1;
            for (int i = 0; i < to_add; i++) {
                int x = rand() % conf->width;
                int y = rand() % conf->height;
                int *tile_resources = (int *)&conf->map[y][x];
                tile_resources[res]++;
                send_tile_content_to_graphics(conf, x, y);
            }
        }
    }
}

void send_tile_content_to_graphics(server_config_t *conf, int x, int y)
{
    tile_t *tile = &conf->map[y][x];
    char msg[128];
    snprintf(msg, sizeof(msg),
        "bct %d %d %d %d %d %d %d %d %d\n",
        x, y, tile->food, tile->linemate, tile->deraumere,
        tile->sibur, tile->mendiane, tile->phiras, tile->thystame);
    for (int i = 0; i < conf->nb_graphics; i++)
        send(conf->graphic_fds[i], msg, strlen(msg), 0);
}

void *game_tick_thread(void *arg)
{
    server_config_t *conf = (server_config_t *)arg;
    while (conf->running) {
        usleep(1000000 / conf->freq);
        pthread_mutex_lock(&conf->mutex);
        update_players_hunger(conf);
        periodic_resource_respawn(conf);
        pthread_mutex_unlock(&conf->mutex);
    }
    return NULL;
}

int count_total_resource(server_config_t *conf, int resource_offset)
{
    int total = 0;
    for (int y = 0; y < conf->height; y++) {
        for (int x = 0; x < conf->width; x++) {
            int *tile_resources = (int *)&conf->map[y][x];
            total += tile_resources[resource_offset];
        }
    }
    return total;
}

void update_players_hunger(server_config_t *conf)
{
    client_t *client = conf->clients;
    while (client) {
        if (!client->is_graphic && client->state == AUTHENTICATED && client->is_alive) {
            client->hunger_tick++;
            if (client->hunger_tick >= (126 * 100 / conf->freq)) {
                client->hunger_tick = 0;
                if (client->inventory.food > 0) {
                    client->inventory.food--;
                    pin_graphics(client, conf);
                } else {
                    client->is_alive = false;
                    send(client->fd, "dead\n", 5, 0);
                    char msg[64];
                    snprintf(msg, sizeof(msg), "pdi #%d\n", client->fd);
                    for (int i = 0; i < conf->nb_graphics; i++)
                        send(conf->graphic_fds[i], msg, strlen(msg), 0);
                    close(client->fd);
                    remove_client(&conf->clients, client->fd);
                }
            }
        }
        client = client->next;
    }
}
