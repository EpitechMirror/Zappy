/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** handle_client_data
*/

#include "../../include/client_info.h"
#include "../../include/flag.h"
#include "../../include/ressources.h"
#include "../../include/server.h"

int calculate_direction_tile(client_t *sender, client_t *receiver)
{
    int dx = sender->x - receiver->x;
    int dy = sender->y - receiver->y;
    int mx = 0;
    int my = 0;
    static const int dir_map[3][3] = {{7, 6, 5}, {8, 0, 4}, {1, 2, 3}};

    if (dx == 0 && dy == 0)
        return 0;
    if (dx > 0)
        mx = 1;
    else if (dx < 0)
        mx = -1;
    if (dy > 0)
        my = 1;
    else if (dy < 0)
        my = -1;
    return dir_map[my + 1][mx + 1];
}

void plv_graphics(client_t *client, server_config_t *conf)
{
    char msg[64];

    snprintf(msg, sizeof(msg), "plv #%d %d\n", client->fd, client->level);
    for (int i = 0; i < conf->nb_graphics; i++)
        send(conf->graphic_fds[i], msg, strlen(msg), 0);
}

void pin_graphics(client_t *client, server_config_t *conf)
{
    char msg[256];

    snprintf(msg, sizeof(msg), "pin #%d %d %d %d %d %d %d %d %d %d\n",
        client->fd, client->x, client->y,
        client->inventory.food,
        client->inventory.linemate,
        client->inventory.deraumere,
        client->inventory.sibur,
        client->inventory.mendiane,
        client->inventory.phiras,
        client->inventory.thystame
    );
    for (int i = 0; i < conf->nb_graphics; i++)
        send(conf->graphic_fds[i], msg, strlen(msg), 0);
}

int find_nb_teams(client_t *client)
{
    int team_count = 0;

    for (client_t *c = client; c != NULL; c = c->next) {
        team_count++;
    }
    return team_count;
}

void notify_graphics_player_update(client_t *client, server_config_t *conf)
{
    ppo_graphics(client, conf);
    plv_graphics(client, conf);
    pin_graphics(client, conf);
}
