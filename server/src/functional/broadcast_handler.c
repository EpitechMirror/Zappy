/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** handle_client_data
*/

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "client_info.h"
#include "flag.h"
#include "ressources.h"
#include "server.h"

static int calculate_direction_tile(client_t *sender, client_t *receiver)
{
    int dx = sender->x - receiver->x;
    int dy = sender->y - receiver->y;
    
    if (dx == 0 && dy == 0)
        return 0;
    
    if (dy < 0) {
        if (dx < 0) return 1;
        if (dx == 0) return 2;
        return 3;
    } else if (dy == 0) {
        if (dx < 0) return 8;
        return 4;
    } else {
        if (dx < 0) return 7;
        if (dx == 0) return 6;
        return 5;
    }
}

void handle_broadcast(int fd, server_config_t *conf, char *client_message, client_t *client)
{
    char *message = client_message + 10;
    char *newline = strchr(message, '\n');
    
    if (newline)
        *newline = '\0';
    
    printf("[DEBUG] Broadcasting message: '%s' from fd %d\n", message, fd);
    
    int client_count = 0;
    for (client_t *c = conf->clients; c != NULL; c = c->next) {
        client_count++;
        printf("[DEBUG] Client %d: fd=%d, is_alive=%d, is_graphic=%d\n", 
               client_count, c->fd, c->is_alive, c->is_graphic);
        
        if (c->is_alive && c->fd != fd && !c->is_graphic) {
            int direction_tile = calculate_direction_tile(client, c);
            char broadcast_msg[512];
            snprintf(broadcast_msg, sizeof(broadcast_msg), "message %d, %s\n", 
                    direction_tile, message);
            printf("[DEBUG] Sending to fd %d: %s", c->fd, broadcast_msg);
            send(c->fd, broadcast_msg, strlen(broadcast_msg), 0);
        }
    }
    
    printf("[DEBUG] Total clients found: %d\n", client_count);
    send(fd, "ok\n", 3, 0);
}
