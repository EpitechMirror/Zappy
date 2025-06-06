/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** connection
*/

#include "server.h"


void handle_new_connection(int server_fd, client_t **clients, server_config_t *conf)
{
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);
    int new_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);
    if (new_fd < 0) {
        perror("accept");
        return;
    }
    int count = 0;
    for (client_t *c = *clients; c; c = c->next)
        count++;
    if (count >= conf->clients_nb * conf->team_count) {
        const char *msg = "Too many clients\n";
        send(new_fd, msg, strlen(msg), 0);
        close(new_fd);
        return;
    }
    client_t *new_client = malloc(sizeof(client_t));
    if (!new_client) {
        perror("malloc");
        close(new_fd);
        return;
    }
    new_client->fd = new_fd;
    new_client->connected = true;
    new_client->is_ready = false;
    new_client->is_alive = true;
    new_client->next = *clients;
    *clients = new_client;
    printf("New client connected: fd=%d\n", new_fd);
}
