/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** handle_client_data
*/

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "../../include/client_info.h"
#include "../../include/flag.h"
#include "../../include/ressources.h"

static int calculate_direction_tile(client_t *sender, client_t *receiver)
{
    int dx = sender->x - receiver->x;
    int dy = sender->y - receiver->y;
    int x_idx = 1;
    int y_idx = 1;
    static const int dir_map[3][3] = {{3, 2, 1}, {8, 0, 4}, {7, 6, 5}};

    if (dx == 0 && dy == 0)
        return 0;
    if (dx > 0)
        x_idx = 2;
    else if (dx < 0)
        x_idx = 0;
    if (dy > 0)
        y_idx = 2;
    else if (dy < 0)
        y_idx = 0;
    return dir_map[y_idx][x_idx];
}

void handle_broadcast(int fd, server_config_t *conf, char *client_message,
    client_t *client)
{
    char *message = client_message + 10;
    char *newline = strchr(message, '\n');
    int client_count = 0;
    int direction_tile;
    char broadcast_msg[512];

    if (newline)
        *newline = '\0';
    printf("[DEBUG] Broadcasting message: '%s' from fd %d\n", message, fd);
    for (client_t *c = conf->clients; c != NULL; c = c->next) {
        client_count++;
        printf("[DEBUG] Client %d: fd=%d, is_alive=%d, is_graphic=%d\n",
                client_count, c->fd, c->is_alive, c->is_graphic);
        if (c->is_alive && c->fd != fd && !c->is_graphic) {
            direction_tile = calculate_direction_tile(client, c);
            snprintf(broadcast_msg, sizeof(broadcast_msg), "message %d, %s\n",
                    direction_tile, message);
            printf("[DEBUG] Sending to fd %d: %s", c->fd, broadcast_msg);
            send(c->fd, broadcast_msg, strlen(broadcast_msg), 0);
        }
    }
    printf("[DEBUG] Total clients found: %d\n", client_count);
    send(fd, "ok\n", 3, 0);
}
