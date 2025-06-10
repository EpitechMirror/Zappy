/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** accept
*/

#include "../../include/server.h"

static void add_last_client_fd(struct pollfd *fds, client_t *clients, int *client_count)
{
    client_t *last = clients;

    while (last && last->next)
        last = last->next;
    if (last) {
        for (int i = 0; i < *client_count; i++) {
            if (fds[i].fd == last->fd)
                return;
        }
        fds[*client_count].fd = last->fd;
        fds[*client_count].events = POLLIN;
        (*client_count)++;
        printf("Ajout du fd %d au tableau fds[]\n", last->fd);
    }
}

void poll_clients(int server_fd, struct pollfd *fds, int *client_count, client_t **clients, server_config_t *conf)
{
    for (int i = 0; i < *client_count; i++) {
        if (!(fds[i].revents & POLLIN))
            continue;

        if (fds[i].fd == server_fd) {
            handle_new_connection(server_fd, fds, client_count, clients, conf);
        } else {
            bool disconnected = handle_client_data(clients, fds[i].fd, conf);
            if (disconnected) {
                close(fds[i].fd);
                for (int j = i; j < *client_count - 1; j++)
                    fds[j] = fds[j + 1];
                (*client_count)--;
                i--;
            }
        }
    }
}

void set_non_blocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl get");
        return;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl set");
    }
}


void accept_clients_loop(int server_fd, server_config_t *conf)
{
    set_non_blocking(server_fd);

    struct pollfd fds[MAX_CLIENTS];
    client_t *clients = NULL;
    int client_count = 1;

    fds[0].fd = server_fd;
    fds[0].events = POLLIN;

    while (1) {
        int poll_count = poll(fds, client_count, -1);
        if (poll_count == -1) {
            perror("poll");
            break;
        }
        poll_clients(server_fd, fds, &client_count, &clients, conf);
    }

    while (clients)
        remove_client(&clients, clients->fd);
}