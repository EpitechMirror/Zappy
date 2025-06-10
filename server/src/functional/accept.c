/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** accept
*/

#include "server.h"

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

static void handle_server_connection(poll_context_t *ctx)
{
    handle_new_connection(ctx);
}

static void handle_client_disconnection(struct pollfd *fds, int *client_count, int i)
{
    close(fds[i].fd);
    for (int j = i; j < *client_count - 1; j++)
        fds[j] = fds[j + 1];
    (*client_count)--;
}

static void handle_existing_client(poll_context_t *ctx, int *i)
{
    bool disconnected = handle_client_data(ctx->clients, ctx->fds[*i].fd, ctx->conf);
    if (disconnected) {
        handle_client_disconnection(ctx->fds, ctx->client_count, *i);
        (*i)--;
    }
}

void poll_clients(poll_context_t *ctx)
{
    for (int i = 0; i < ctx->client_count; i++) {
        if (!(ctx->fds[i].revents & POLLIN))
            continue;
        if (ctx->fds[i].fd == ctx->server_fd) {
            handle_server_connection(ctx);
        } else {
            handle_existing_client(ctx, &i);
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
    poll_context_t ctx = {
        .server_fd = server_fd,
        .conf = conf,
        .clients = NULL,
        .client_count = 1,
    };
    set_non_blocking(server_fd);
    ctx.fds[0].fd = server_fd;
    ctx.fds[0].events = POLLIN;
    while (1) {
        int poll_count = poll(ctx.fds, ctx.client_count, -1);
        if (poll_count == -1) {
            perror("poll");
            break;
        }
        poll_clients(&ctx);
    }
    while (ctx.clients)
        remove_client(&ctx.clients, ctx.clients->fd);
}