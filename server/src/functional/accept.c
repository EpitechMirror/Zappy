/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** accept
*/

#include "server.h"

void accept_clients_loop(int server_fd, server_config_t *conf)
{
    client_t *clients = NULL;
    fd_set readfds;
    int max_fd;

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_fd = server_fd;

        for (client_t *c = clients; c; c = c->next) {
            FD_SET(c->fd, &readfds);
            if (c->fd > max_fd)
                max_fd = c->fd;
        }

        if (select(max_fd + 1, &readfds, NULL, NULL, NULL) < 0) {
            perror("select");
            break;
        }

        if (FD_ISSET(server_fd, &readfds))
            handle_new_connection(server_fd, &clients, conf);

        client_t *c = clients, *next;
        char buffer[1024];
        while (c) {
            next = c->next;
            if (FD_ISSET(c->fd, &readfds)) {
                ssize_t r = read(c->fd, buffer, sizeof(buffer) - 1);
                if (r <= 0) {
                    remove_client(&clients, c->fd);
                } else {
                    buffer[r] = '\0';
                    printf("Client %d: %s\n", c->fd, buffer);
                    send(c->fd, "hello world!\n", 13, 0);
                }
            }
            c = next;
        }
    }
    while (clients)
        remove_client(&clients, clients->fd);
}