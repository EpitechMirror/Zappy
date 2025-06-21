/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** handle_client_data
*/

#include "server.h"
#include "flag.h"

void strip_newline(char *str)
{
    size_t len = strlen(str);
    if (len > 0 && (str[len-1] == '\n' || str[len-1] == '\r'))
        str[len-1] = '\0';
    if (len > 1 && str[len-2] == '\r')
        str[len-2] = '\0';
}

static int find_team_index(server_config_t *conf, const char *team_name)
{
    int i = 0;

    while (i < conf->team_count) {
        if (strcmp(conf->team_names[i], team_name) == 0)
            return i;
        i++;
    }
    return -1;
}

static void send_auth_success(int fd, server_config_t *conf)
{
    char msg[64];

    snprintf(msg, sizeof(msg), "%d\n", conf->clients_nb);
    send(fd, msg, strlen(msg), 0);
    snprintf(msg, sizeof(msg), "%d %d\n", conf->width, conf->height);
    send(fd, msg, strlen(msg), 0);
}

static int count_team_members(client_t *clients, const char *team_name)
{
    int count = 0;

    while (clients) {
        if (clients->team_name &&
            strcmp(clients->team_name, team_name) == 0 &&
            !clients->is_graphic)
            count++;
        clients = clients->next;
    }
    return count;
}

static void send_ebo_to_graphics(client_t *clients, int egg_id)
{
    client_t *tmp = clients;

    while (tmp) {
        if (tmp->is_graphic)
            send_ebo(tmp->fd, egg_id);
        tmp = tmp->next;
    }
}

static void handle_player_auth(client_t *client, int fd,
    server_config_t *conf, char *team)
{
    char msg[128];
    int team_idx = find_team_index(conf, team);
    egg_t *egg = NULL;

    if (team_idx == -1 || conf->team_slots[team_idx] <= 0) {
        send(fd, "ko\n", 3, 0);
        remove_client(&conf->clients, fd);
        return;
    }
    conf->team_slots[team_idx]--;
    client->is_graphic = false;
    snprintf(msg, sizeof(msg),
        "Bienvenue joueur de l'équipe %s\n%d\n%d %d\n",
        team, conf->clients_nb, conf->width, conf->height);
    send(fd, msg, strlen(msg), 0);
    client->state = AUTHENTICATED;
    egg = get_unused_egg_for_team(conf, team_idx);
    if (egg) {
        egg->used = 1;
        send_ebo_to_graphics(conf->clients, egg->id);
    }
}

bool handle_auth(auth_context_t *ctx, char *buffer)
{
    strip_newline(buffer);
    if (strcmp(buffer, "GRAPHIC") == 0) {
        handle_graphic_auth(ctx->client, ctx->client->fd, ctx->conf);
        ctx->client->is_graphic = true;
        ctx->client->state = AUTHENTICATED;
        return true;
    }
    handle_player_auth(ctx->client, ctx->client->fd, ctx->conf, buffer);
    return true;
}

bool handle_client_data(client_t **clients, int fd, server_config_t *conf)
{
    char buffer[1024];
    ssize_t r;
    client_t *client = *clients;
    auth_context_t ctx = {clients, NULL, conf};

    while (client && client->fd != fd)
        client = client->next;
    if (!client)
        return true;
    ctx.client = client;
    r = read(fd, buffer, sizeof(buffer) - 1);
    if (r <= 0) {
        remove_client(clients, fd);
        return true;
    }
    buffer[r] = '\0';
    printf("[DEBUG] Data from fd %d: %s\n", fd, buffer);
    if (client->state == WAITING_NAME)
        handle_auth(&ctx, buffer);
    return false;
}
