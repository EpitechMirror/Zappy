/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** handle_client_data
*/

#include <string.h>
#include <sys/socket.h>
#include "Client_Info.h"
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

int respond_to_server_fd(int fd, server_config_t *conf, char *client_message, client_t *client)
{
    int x, y;

    if (strcmp(client_message, "msz")){
        send_map_size(fd, conf, client_message);
    }
    if (strcmp(client_message, "mct") == 0){
        send_whole_map(fd, conf);
    }
    if (strncmp(client_message, "bct", 3) == 0) {
        if (sscanf(client_message, "bct %d %d", &x, &y) == 2) {
            send_tile_content(fd , x, y, *conf->map);
        } else {
            send(fd, "sbp\n", 4, 0); // bad parameter
        }
    }

    return 0;
}
