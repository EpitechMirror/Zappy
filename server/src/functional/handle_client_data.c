/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** handle_client_data
*/

#include <stdbool.h>
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

void send_pnw_to_graphics(client_t *player, server_config_t *conf)
{
    char msg[256];

    snprintf(msg, sizeof(msg), "pnw #%d %d %d %d %d %s\n",
        player->fd, player->x, player->y, player->direction + 1, player->level, player->team_name);

    for (int i = 0; i < conf->nb_graphics; i++)
        send(conf->graphic_fds[i], msg, strlen(msg), 0);
}

void send_ebo_to_graphics(int egg_id, server_config_t *conf)
{
    char msg[64];

    snprintf(msg, sizeof(msg), "ebo #%d\n", egg_id);
    for (int i = 0; i < conf->nb_graphics; i++)
        send(conf->graphic_fds[i], msg, strlen(msg), 0);
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
        client->is_graphic = false;
        client->state = AUTHENTICATED;
        client->is_alive = true;
        client->inventory.food = 10;
        client->hunger_tick = 0;
        client->x = egg->x;
        client->y = egg->y;
        client->direction = NORTH;
        client->level = 1;
        client->team_name = strdup(team);
        client->inventory.food = 10;
        client->next = conf->clients;
        conf->clients = client;

        send_pnw_to_graphics(client, conf);
        send_ebo_to_graphics(egg->id, conf);
    }
}

bool handle_auth(auth_context_t *ctx, char *buffer)
{
    strip_newline(buffer);
    if (strcmp(buffer, "GRAPHIC") == 0) {
        ctx->client->is_graphic = true;
        ctx->client->state = AUTHENTICATED;
        ctx->conf->graphic_fds[ctx->conf->nb_graphics++] = ctx->client->fd;
        handle_graphic_auth(ctx->client->fd, ctx->conf);
        return true;
    }
    handle_player_auth(ctx->client, ctx->client->fd, ctx->conf, buffer);
    return true;
}

static bool process_client_request(client_t *client, int fd,
server_config_t *conf, char *buffer)
{
    auth_context_t ctx = {&conf->clients, client, conf};
    //printf("[DEBUG] fd=%d is_graphic=%d state=%d is_alive=%d\n", client->fd, client->is_graphic, client->state, client->is_alive);
    
    if (client->state == WAITING_NAME) {
        handle_auth(&ctx, buffer);
    } else if (client->state == AUTHENTICATED) {
        if (!client->is_graphic) {
            if (!conf->game_started) {
                conf->game_started = true;
                pthread_create(&conf->game_thread, NULL, game_tick_thread, conf);
                // printf("[DEBUG] Game loop started.\n");
            }
            respond_to_server_fd(fd, conf, buffer, client);
        }
    }
    
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
