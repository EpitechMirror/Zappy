/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** handle_client_data
*/

#include <stdbool.h>
#include "server.h"

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

static void handle_graphic_auth(client_t *client,
    int fd, server_config_t *conf)
{
    char msg[64];

    client->is_graphic = true;
    snprintf(msg, sizeof(msg), "msz %d %d\n", conf->width, conf->height);
    send(fd, msg, strlen(msg), 0);
    send_whole_map(fd, conf);
    client->state = AUTHENTICATED;
    printf("Client %d authenticated as GRAPHIC\n", fd);
}

static void handle_player_auth(client_t *client, int fd,
    server_config_t *conf, char *team)
{
    char msg[128];
    int team_idx = find_team_index(conf, team);

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
    client->direction = NORTH;
    client->level = 1;
    printf("Client %d authenticated as PLAYER (%s)\n", fd, team);
}

static bool handle_auth(auth_context_t *ctx, char *buffer)
{
    char *team = NULL;

    buffer[strcspn(buffer, "\r\n")] = 0;
    team = strdup(buffer);
    printf("[DEBUG] team received: '%s'\n", team);
    if (!team) {
        perror("strdup");
        remove_client(ctx->clients, ctx->client->fd);
        return false;
    }
    ctx->client->team_name = team;
    if (strcmp(team, "GRAPHIC") == 0) {
        handle_graphic_auth(ctx->client, ctx->client->fd, ctx->conf);
        return true;
    }
    handle_player_auth(ctx->client, ctx->client->fd, ctx->conf, team);
    return true;
}

static bool process_client_request(client_t *client, int fd,
    server_config_t *conf, char *buffer)
{
    auth_context_t ctx = {&conf->clients, client, conf};

    if (client->is_graphic == false)
        respond_to_server_fd(fd, conf, buffer, client);
    if (client->state == WAITING_NAME)
        handle_auth(&ctx, buffer);
    return false;
}

bool handle_client_data(client_t **clients, int fd,
    server_config_t *conf)
{
    char buffer[1024];
    ssize_t r;
    client_t *client = *clients;

    while (client && client->fd != fd)
        client = client->next;
    if (!client)
        return true;
    r = read(fd, buffer, sizeof(buffer) - 1);
    if (r <= 0) {
        remove_client(clients, fd);
        return true;
    }
    buffer[r] = '\0';
    printf("[DEBUG] Data from fd %d: %s\n", fd, buffer);
    return process_client_request(client, fd, conf, buffer);
}
