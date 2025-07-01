/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** handle_client_data
*/

#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#include "../../include/client_info.h"
#include "../../include/flag.h"
#include "../../include/ressources.h"
#include "../../include/server.h"

void get_tile_contents(client_t *client, char *inventory)
{
    snprintf(inventory, 256,
        "linemate %d, deraumere %d, sibur %d, mendiane %d,"
        " phiras %d, thystame %d, food %d\n",
        client->inventory.linemate,
        client->inventory.deraumere,
        client->inventory.sibur,
        client->inventory.mendiane,
        client->inventory.phiras,
        client->inventory.thystame,
        client->inventory.food
    );
}

static char *parse_resource_name(char *client_message)
{
    char *resourc = client_message + 5;
    char *end = strchr(resourc, '\n');
    int len;

    if (end)
        *end = '\0';
    end = strchr(resourc, '\r');
    if (end)
        *end = '\0';
    while (*resourc == ' ' || *resourc == '\t')
        resourc++;
    len = strlen(resourc);
    while (len > 0 && (resourc[len - 1] == ' ' || resourc[len - 1] == '\t')) {
        len--;
        resourc[len] = '\0';
    }
    return resourc;
}

static int try_take_resource(client_t *client, server_config_t *conf,
    char *resource)
{
    tile_t *tile = &conf->map[client->y][client->x];
    struct { const char *name; int *tile_count; int *inventory_count; }
        resources[] = {
        {"linemate", &tile->linemate, &client->inventory.linemate},
        {"deraumere", &tile->deraumere, &client->inventory.deraumere},
        {"sibur", &tile->sibur, &client->inventory.sibur},
        {"mendiane", &tile->mendiane, &client->inventory.mendiane},
        {"phiras", &tile->phiras, &client->inventory.phiras},
        {"thystame", &tile->thystame, &client->inventory.thystame},
        {"food", &tile->food, &client->inventory.food}
    };

    for (int i = 0; i < 7; i++)
        if (strcmp(resource, resources[i].name) == 0 &&
            *resources[i].tile_count > 0) {
            (*resources[i].inventory_count)++;
            (*resources[i].tile_count)--;
            return 1;
        }
    return 0;
}

void take_object(client_t *client, server_config_t *conf, char *client_message)
{
    char *resource = parse_resource_name(client_message);

    if (try_take_resource(client, conf, resource))
        send(client->fd, "ok\n", 3, 0);
    else
        send(client->fd, "ko\n", 3, 0);
}

void ppo_graphics(client_t *client, server_config_t *conf)
{
    char msg[128];

    snprintf(msg, sizeof(msg), "ppo #%d %d %d %d\n", client->fd, client->x,
        client->y, client->direction + 1);
    for (int i = 0; i < conf->nb_graphics; i++)
        send(conf->graphic_fds[i], msg, strlen(msg), 0);
}

void add_resource_to_buffer(char *buffer, char *name, int count, int *first)
{
    for (int i = 0; i < count; i++) {
        if (!*first)
            strcat(buffer, " ");
        strcat(buffer, name);
        *first = 0;
    }
}
