/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** accept
*/

#include "server.h"

static void add_last_client_fd(struct pollfd *fds, client_t *clients,
    int *client_count)
{
    client_t *last = clients;
    int i = 0;

    while (last && last->next)
        last = last->next;
    if (!last)
        return;
    for (i = 0; i < *client_count; i++) {
        if (fds[i].fd == last->fd)
            return;
    }
    fds[*client_count].fd = last->fd;
    fds[*client_count].events = POLLIN;
    fds[*client_count].revents = 0;
    (*client_count)++;
    printf("coucou Ajout du fd %d au tableau fds[]\n", last->fd);
}

static void handle_server_connection(poll_context_t *ctx)
{
    handle_new_connection(ctx);
}

static void handle_client_disconnection(struct pollfd *fds,
    int *client_count, int i)
{
    if (fds[i].fd >= 0) {
        close(fds[i].fd);
        fds[i].fd = -1;
    }
    *client_count = remove_client_fd(fds, *client_count, i);
}

static void handle_existing_client(poll_context_t *ctx, int *i)
{
    bool disconnected = false;

    disconnected = handle_client_data(ctx->clients,
        ctx->fds[*i].fd, ctx->conf);
    if (disconnected) {
        handle_client_disconnection(ctx->fds, ctx->client_count, *i);
        (*i)--;
    }
}

void poll_clients(poll_context_t *ctx)
{
    int i = 0;

    for (i = 0; i < *ctx->client_count; i++) {
        if (ctx->fds[i].fd < 0)
            continue;
        if (!(ctx->fds[i].revents & POLLIN))
            continue;
        if (ctx->fds[i].fd == ctx->server_fd)
            handle_server_connection(ctx);
        else
            handle_existing_client(ctx, &i);
    }
}

static void set_non_blocking(int fd)
{
    int flags = 0;

    flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl get");
        return;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
        perror("fcntl set");
}

static void init_pollfds(struct pollfd *fds)
{
    int i = 0;

    for (i = 0; i < MAX_CLIENTS; i++) {
        fds[i].fd = -1;
        fds[i].events = 0;
        fds[i].revents = 0;
    }
}

static void setup_poll_context(poll_context_t *ctx, int server_fd,
    server_config_t *conf, int *client_count)
{
    ctx->server_fd = server_fd;
    ctx->conf = conf;
    ctx->clients = &conf->clients;
    ctx->client_count = client_count;
    init_pollfds(ctx->fds);
    set_non_blocking(server_fd);
    ctx->fds[0].fd = server_fd;
    ctx->fds[0].events = POLLIN;
}

void accept_clients_loop(int server_fd, server_config_t *conf)
{
    int client_count = 1;
    poll_context_t ctx;
    int poll_count = 0;

    setup_poll_context(&ctx, server_fd, conf, &client_count);
    while (1) {
        poll_count = poll(ctx.fds, client_count, -1);
        if (poll_count == -1) {
            perror("poll");
            break;
        }
        poll_clients(&ctx);
    }
    while (ctx.clients)
        remove_client(&ctx.clients, ctx.clients->fd);
}
