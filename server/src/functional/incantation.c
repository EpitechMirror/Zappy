/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** incantation
*/

#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "client_info.h"
#include "flag.h"
#include "ressources.h"
#include "server.h"

void elevate_players_on_tile(server_config_t *conf, int x, int y, int level)
{
    for (client_t *c = conf->clients; c != NULL; c = c->next) {
        if (c->x == x && c->y == y && c->level == level) {
            c->level++;
            notify_graphics_player_update(c, conf);
            send(c->fd, "Current level: ", 15, 0);
            char level_msg[32];
            snprintf(level_msg, sizeof(level_msg), "%d\n", c->level);
            send(c->fd, level_msg, strlen(level_msg), 0);
        }
    }
}

// Elevation requirements table (level 1->2, 2->3, etc.)
static const elevation_req_t elevation_requirements[] = {
    {1, 1, 0, 0, 0, 0, 0},  // 1->2
    {2, 1, 1, 1, 0, 0, 0},  // 2->3
    {2, 2, 0, 1, 0, 2, 0},  // 3->4
    {4, 1, 1, 2, 0, 1, 0},  // 4->5
    {4, 1, 2, 1, 3, 0, 0},  // 5->6
    {6, 1, 2, 3, 0, 1, 0},  // 6->7
    {6, 2, 2, 2, 2, 2, 1}   // 7->8
};

void handle_incantation(client_t *client, server_config_t *conf, int fd)
{
    if (client->level >= 8) {
        send(fd, "ko\n", 3, 0);
        return;
    }

    const elevation_req_t *req = &elevation_requirements[client->level - 1];
    int players_on_tile = count_players_on_tile(conf, client->x, client->y, client->level);
    if (players_on_tile < req->nb_players) {
        send(fd, "ko\n", 3, 0);
        return;
    }
    if (!check_tile_resources(conf, client->x, client->y, req)) {
        send(fd, "ko\n", 3, 0);
        return;
    }
    consume_tile_resources(conf, client->x, client->y, req);
    elevate_players_on_tile(conf, client->x, client->y, client->level);
}
