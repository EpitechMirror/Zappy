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
    if (strcmp(client_message, "msz")){
        send_map_size(fd, conf, client_message);
    }
}
