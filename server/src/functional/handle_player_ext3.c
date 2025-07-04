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

static void pic_graphics(client_t *client, server_config_t *conf)
{
    char msg[256];

    snprintf(msg, sizeof(msg), "pic %d %d %d #%d\n", 
             client->x, client->y, client->level, client->fd);
    
    for (int i = 0; i < conf->nb_graphics; i++)
        send(conf->graphic_fds[i], msg, strlen(msg), 0);
}

void pie_graphics(client_t *client, server_config_t *conf, int result)
{
    char msg[256];

    snprintf(msg, sizeof(msg), "pie %d %d %d\n", client->x, client->y, result);
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


void notify_graphics_player_update(client_t *client, server_config_t *conf, int result, bool is_incantation)
{
    ppo_graphics(client, conf);
    pin_graphics(client, conf);
    
    if (is_incantation) {
        pic_graphics(client, conf);
        
        if (result == 1) {
            client->level++;
            char level_msg[64];
            snprintf(level_msg, sizeof(level_msg), "Current level: %d\n", client->level);
            send(client->fd, level_msg, strlen(level_msg), 0);
            send(client->fd, "ok\n", 3, 0);
            
            plv_graphics(client, conf);
        } else {
            send(client->fd, "ko\n", 3, 0);
        }
        
        client->is_incanting = true;
        client->incantation_end_time = get_current_time() + 6.0;
        client->incantation_result = result;
        
        printf("[DEBUG] Client #%d starts incantation, will end in 5 seconds\n", client->fd);
        
    } else {
        plv_graphics(client, conf);
    }
}

void check_incantation_timers(server_config_t *conf)
{
    double now = get_current_time();
    
    for (client_t *client = conf->clients; client; client = client->next) {
        if (client->is_incanting && now >= client->incantation_end_time) {
            printf("[DEBUG] Client #%d incantation finished\n", client->fd);
            
            pie_graphics(client, conf, client->incantation_result);
            
            client->is_incanting = false;
        }
    }
}
