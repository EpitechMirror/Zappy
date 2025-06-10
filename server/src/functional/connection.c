/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** connection
*/

#include "../../include/server.h"

void handle_new_connection(int server_fd, struct pollfd *fds, int *client_count, client_t **clients, server_config_t *conf)
{
    int new_fd;
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);

    while ((new_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen)) != -1) {
        printf("New client connected: fd=%d\n", new_fd);


        client_t *new_client = malloc(sizeof(client_t));
        if (!new_client) {
            perror("malloc");
            close(new_fd);
            continue;
        }
        new_client->fd = new_fd;
        new_client->connected = true;
        new_client->is_ready = false;
        new_client->is_alive = true;
        new_client->state = WAITING_NAME;
        new_client->team_name = NULL;
        new_client->next = *clients;
        *clients = new_client;
        send(new_fd, "WELCOME\n", 8, 0);
        printf("envoie welcome au client %d\n", new_fd);

        if (*client_count < MAX_CLIENTS) {
            fds[*client_count].fd = new_fd;
            fds[*client_count].events = POLLIN;
        (*client_count)++;
        printf("Ajout du fd %d au tableau fds[]\n", new_fd);
        } 
        //else {
        //     printf("Max clients atteints, fermeture fd %d\n", new_fd);
        //     close(new_fd);
        // }
        addrlen = sizeof(client_addr);
    }
    if (errno != EAGAIN && errno != EWOULDBLOCK) {
        perror("accept");
    }
}
