/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** client_graphic
*/

#include "server.h"
#include "ressources.h"

void add_egg_to_list(server_config_t *conf, egg_t *egg)
{
    if (!egg || !conf)
        return;
    egg->next = conf->eggs;
    conf->eggs = egg;
}

void send_team_names(int fd, server_config_t *conf)
{
    char msg[128];
    int i = 0;

    for (i = 0; i < conf->team_count; i++) {
        snprintf(msg, sizeof(msg), "tna %s\n", conf->team_names[i]);
        send(fd, msg, strlen(msg), 0);
    }
}

void send_enw(int fd, egg_t *egg, int player_id)
{
    char msg[128];

    snprintf(msg, sizeof(msg), "enw #%d #%d %d %d\n",
        egg->id, player_id, egg->x, egg->y);
    send(fd, msg, strlen(msg), 0);
}

void send_ebo(int fd, int egg_id)
{
    char msg[64];

    snprintf(msg, sizeof(msg), "ebo #%d\n", egg_id);
    send(fd, msg, strlen(msg), 0);
}

egg_t *get_unused_egg_for_team(server_config_t *conf, int team_idx)
{
    egg_t *egg = conf->eggs;

    while (egg) {
        if (egg->team_idx == team_idx && !egg->used)
            return egg;
        egg = egg->next;
    }
    return NULL;
}

egg_t *create_egg(server_config_t *conf, int team_idx, int x, int y)
{
    egg_t *egg = malloc(sizeof(egg_t));

    if (!egg)
        return NULL;
    egg->id = conf->next_egg_id++;
    egg->x = x;
    egg->y = y;
    egg->team_idx = team_idx;
    egg->used = 0;
    egg->next = NULL;
    return egg;
}

egg_t *create_egg_fork(server_config_t *conf, client_t *client)
{
    printf("fork to create an egg");
    egg_t *egg = create_egg(conf, client->team_idx, client->x, client->y);

    if (!egg)
        return NULL;
    egg->used = 1; // Mark as used
    add_egg_to_list(conf, egg);
    for (int i = 0; i < conf->nb_graphics; i++)
        send_enw(conf->graphic_fds[i], egg, client->fd);
    return egg;
}

void handle_graphic_auth(int fd, server_config_t *conf)
{
    char msg[128];
    egg_t *egg = NULL;
    int t = 0;
    int s = 0;

    snprintf(msg, sizeof(msg), "WELCOME\n");
    send(fd, msg, strlen(msg), 0);
    snprintf(msg, sizeof(msg), "msz %d %d\n", conf->width, conf->height);
    send(fd, msg, strlen(msg), 0);
    send_whole_map(fd, conf);
    send_team_names(fd, conf);
    for (t = 0; t < conf->team_count; t++) {
        for (s = 0; s < conf->clients_nb; s++) {
            egg = create_egg(conf, t, rand() % conf->width, rand() % conf->height);
            add_egg_to_list(conf, egg);
            send_enw(fd, egg, -1);
        }
    }
}
