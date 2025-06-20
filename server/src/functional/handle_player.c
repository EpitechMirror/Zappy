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

int respond_to_server_fd(int fd, server_config_t *conf, char *client_message, client_t *client)
{
    if (strncmp(client_message, "Forward", 7) == 0) {
        client->x += 1;
        send(fd, "ok\n", 3, 0);
    }
    if (strncmp(client_message, "Right", 5) == 0) {
        client->direction = (client->direction + 1) % 4;
        send(fd, "ok\n", 3, 0);
    }
    if (strncmp(client_message, "Left", 4) == 0) {
        client->direction = (client->direction + 3) % 4;
        send(fd, "ok\n", 3, 0);
    }
    if (strncmp(client_message, "Inventory", 9) == 0) {
        char inventory[256];
        snprintf(inventory, sizeof(inventory), "Inventory: %d items\n", client->id);
        send(fd, inventory, strlen(inventory), 0);
    }
    
    return 0;
}
