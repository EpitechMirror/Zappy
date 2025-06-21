/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** connection
*/

#include "server.h"

client_t *create_new_client(int fd)
{
    client_t *new_client = malloc(sizeof(client_t));

    if (!new_client) {
        perror("malloc");
        return NULL;
    }
    new_client->fd = fd;
    new_client->connected = true;
    new_client->is_ready = false;
    new_client->is_alive = true;
    new_client->state = WAITING_NAME;
    new_client->team_name = NULL;
    new_client->next = NULL;
    return new_client;
}

void add_client_to_list(client_t **clients, client_t *new_client)
{
    if (!clients) {
        fprintf(stderr, "Erreur: clients est NULL dans add_client_to_list\n");
        exit(1);
    }
    new_client->next = *clients;
    *clients = new_client;
}

void add_client_to_fds(struct pollfd *fds, int *client_count, int fd)
{
    if (*client_count < MAX_CLIENTS) {
        fds[*client_count].fd = fd;
        fds[*client_count].events = POLLIN;
        fds[*client_count].revents = 0;
        (*client_count)++;
        printf("Ajout du fd %d au tableau fds[]\n", fd);
    }
}

void send_welcome_message(int fd)
{
    send(fd, "WELCOME\n", 8, 0);
    printf("envoie welcome au client %d\n", fd);
}

static void accept_and_register_client(poll_context_t *ctx, int new_fd)
{
    client_t *new_client = NULL;

    printf("New client connected: fd=%d\n", new_fd);
    new_client = create_new_client(new_fd);
    if (!new_client) {
        close(new_fd);
        return;
    }
    add_client_to_list(&ctx->clients, new_client);
    send_welcome_message(new_fd);
    add_client_to_fds(ctx->fds, ctx->client_count, new_fd);
}

void handle_new_connection(poll_context_t *ctx)
{
    int new_fd = 0;
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);

    new_fd = accept(ctx->server_fd,
        (struct sockaddr *)&client_addr, &addrlen);
    while (new_fd != -1) {
        accept_and_register_client(ctx, new_fd);
        addrlen = sizeof(client_addr);
        new_fd = accept(ctx->server_fd,
            (struct sockaddr *)&client_addr, &addrlen);
    }
    if ((errno != EAGAIN) && (errno != EWOULDBLOCK)) {
        perror("accept");
    }
}
