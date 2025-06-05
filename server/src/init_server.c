/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** init_server
*/

#include "server.h"

static int count_clients(client_t *clients)
{
    int count = 0;
    client_t *c = clients;
    while (c) {
        count++;
        c = c->next;
    }
    return count;
}

static void add_client(client_t **clients, int new_fd)
{
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

int create_server_socket(int port)
{
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        return -1;
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(server_fd);
        return -1;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        return -1;
    }
    if (listen(server_fd, 10) < 0) {
        perror("listen");
        close(server_fd);
        return -1;
    }
    printf("Server started on port %d\n", port);
    return server_fd;
}

void handle_new_connection(int server_fd, client_t **clients, server_config_t *conf)
{
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);
    int new_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);
    int count;

    if (new_fd < 0) {
        perror("accept");
        return;
    }
    count = count_clients(*clients);
    if (count >= conf->clients_nb * conf->team_count) {
        const char *msg = "Too many clients\n";
        send(new_fd, msg, strlen(msg), 0);
        close(new_fd);
        return;
    }
    add_client(clients, new_fd);
}

void remove_client(client_t **clients, int fd)
{
    client_t *prev = NULL;
    client_t *cur = *clients;

    while (cur) {
        if (cur->fd == fd) {
            if (prev)
                prev->next = cur->next;
            else
                *clients = cur->next;
            close(cur->fd);
            free(cur);
            printf("Client disconnected: fd=%d\n", fd);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

static void handle_client_data(client_t *c, client_t **clients)
{
    char buffer[1024];
    ssize_t r = read(c->fd, buffer, sizeof(buffer) - 1);

    if (r <= 0) {
        remove_client(clients, c->fd);
    } else {
        buffer[r] = '\0';
        printf("Client %d: %s\n", c->fd, buffer);
        send(c->fd, "hello world!\n", 13, 0);
    }
}

static void set_fds(int server_fd, client_t *clients, fd_set *readfds, int *max_fd)
{
    client_t *c = clients;
    FD_ZERO(readfds);
    FD_SET(server_fd, readfds);
    *max_fd = server_fd;
    while (c) {
        FD_SET(c->fd, readfds);
        if (c->fd > *max_fd)
            *max_fd = c->fd;
        c = c->next;
    }
}

static void handle_ready_clients(client_t *clients, fd_set *readfds, client_t **clients_head)
{
    client_t *c = clients;
    client_t *next;
    while (c) {
        next = c->next;
        if (FD_ISSET(c->fd, readfds))
            handle_client_data(c, clients_head);
        c = next;
    }
}

void accept_clients_loop(int server_fd, server_config_t *conf)
{
    client_t *clients = NULL;
    fd_set readfds;
    int max_fd;
    int ret;

    while (1) {
        set_fds(server_fd, clients, &readfds, &max_fd);
        ret = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        if (ret < 0) {
            perror("select");
            break;
        }
        if (FD_ISSET(server_fd, &readfds))
            handle_new_connection(server_fd, &clients, conf);
        handle_ready_clients(clients, &readfds, &clients);
    }
    while (clients)
        remove_client(&clients, clients->fd);
}