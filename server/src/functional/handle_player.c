/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** handle_client_data
*/

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "Client_Info.h"
#include "flag.h"
#include "server.h"

int send_map_size(int fd, server_config_t *conf, char *client_message)
{
    char response[256];

    snprintf(response, sizeof(response), "msz %d %d\n", conf->width, conf->height);
    if (strcmp(client_message, "msz")){
        send(fd, response, strlen(response), 0);
    }
    return 0;
}

void send_team_names(int fd, server_config_t *conf)
{
    for (int i = 0; conf->team_names[i] != NULL; i++) {
        printf("Debug sending the message %s\n", conf->team_names[i]);
        send(fd, "tna ", 4, 0);
        send(fd, conf->team_names[i], strlen(conf->team_names[i]), 0);
        send(fd, "\n", 1, 0);
    }
}

void find_player_position(int fd, char *player_name, client_t *client)
{
    client_t *current = client;

    while (current) {
        if (strcmp(current->team_name, player_name) == 0) {
            char response[64];
            snprintf(response, sizeof(response), "ppo %d %d\n", current->x, current->y);
            send(fd, response, strlen(response), 0);
            return;
        }
        current = current->next;
    }
    send(fd, "sbp\n", 4, 0);
}

int respond_to_server_fd(int fd, server_config_t *conf, char *client_message, client_t *client)
{
    int x, y;
    char PlayerName[128];

    if (strncmp(client_message, "msz", 3) == 0)
        send_map_size(fd, conf, client_message);
    if (strncmp(client_message, "mct", 3) == 0)
        send_whole_map(fd, conf);
    if (strncmp(client_message, "bct", 3) == 0) {
        if (sscanf(client_message, "bct %d %d", &x, &y) == 2) {
            send_tile_content(fd , x, y, *conf->map);
        } else {
            send(fd, "sbp\n", 4, 0);
        }
    }
    if (strncmp(client_message, "tna", 3) == 0)
        send_team_names(fd, conf);
    if (strncmp(client_message, "ppo", 3) == 0) {
        if (sscanf(client_message, "ppo %s", PlayerName) == 1) {
            find_player_position(fd, PlayerName, client);
        } else {
            send(fd, "sbp\n", 4, 0);
        }
    }
    return 0;
}
