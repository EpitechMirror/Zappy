/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** game_state
*/

#include <string.h>
#include "../include/client_info.h"
#include "../include/flag.h"
#include "../include/ressources.h"
#include "../include/server.h"

static int count_total_resource(server_config_t *conf, int resource_offset)
{
    int total = 0;
    int *tile_resources;

    for (int y = 0; y < conf->height; y++) {
        for (int x = 0; x < conf->width; x++) {
            tile_resources = (int *)&conf->map[y][x];
            total += tile_resources[resource_offset];
        }
    }
    return total;
}

static void send_tile_content_to_graphics(server_config_t *conf, int x, int y)
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

static void handle_player_death(client_t *client, server_config_t *conf)
{
    char msg[64];

    client->is_alive = false;
    send(client->fd, "dead\n", 5, 0);
    snprintf(msg, sizeof(msg), "pdi #%d\n", client->fd);
    for (int i = 0; i < conf->nb_graphics; i++)
        send(conf->graphic_fds[i], msg, strlen(msg), 0);
    close(client->fd);
    remove_client(&conf->clients, client->fd);
}

static bool should_process_client(client_t *client)
{
    if (!client->is_graphic && client->state == AUTHENTICATED &&
        client->is_alive) {
            client->hunger_tick++;
            return true;
        }
    return false;
}

static void process_hunger_tick(client_t *client, server_config_t *conf)
{
    if (client->inventory.food > 0) {
        client->inventory.food--;
        pin_graphics(client, conf);
    } else {
        handle_player_death(client, conf);
    }
}

static void update_players_hunger(server_config_t *conf)
{
    client_t *client = conf->clients;

    while (client) {
        if (should_process_client(client) & (client->hunger_tick >=
            (126 * 100 / conf->freq))) {
            client->hunger_tick = 0;
            process_hunger_tick(client, conf);
        }
        client = client->next;
    }
}

static void add_resource_to_random_tile(server_config_t *conf, int res)
{
    int x = rand() % conf->width;
    int y = rand() % conf->height;
    int *tile_resources = (int *)&conf->map[y][x];

    tile_resources[res]++;
    send_tile_content_to_graphics(conf, x, y);
}

static void try_respawn_resource(server_config_t *conf, int res,
    int total_tiles)
{
    int min_qty = total_tiles * resource_min_ratio[res];
    int total_qty = count_total_resource(conf, res);
    int to_add;

    if (total_qty >= min_qty)
        return;
    to_add = rand() % 3 + 1;
    for (int i = 0; i < to_add; i++) {
        add_resource_to_random_tile(conf, res);
    }
}

void periodic_resource_respawn(server_config_t *conf)
{
    int total_tiles = conf->width * conf->height;

    for (int res = 0; res < RESOURCE_TYPES; res++) {
        try_respawn_resource(conf, res, total_tiles);
    }
}

void *game_tick_thread(void *arg)
{
    server_config_t *conf = (server_config_t *)arg;

    while (conf->running) {
        usleep(1000000 / conf->freq);
        pthread_mutex_lock(&conf->mutex);
        execute_pending_commands(conf);
        check_incantation_timers(conf);
        update_players_hunger(conf);
        periodic_resource_respawn(conf);
        pthread_mutex_unlock(&conf->mutex);
    }
    return NULL;
}
