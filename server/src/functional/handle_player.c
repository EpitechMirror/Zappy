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
#include "client_info.h"
#include "flag.h"
#include "ressources.h"
#include "server.h"

void get_tile_contents(int x, int y, client_t *client, char *inventory)
{
    snprintf(inventory, 256,
        "linemate %d, deraumere %d, sibur %d, mendiane %d, phiras %d, thystame %d, food %d\n",
        client->inventory.linemate,
        client->inventory.deraumere,
        client->inventory.sibur,
        client->inventory.mendiane,
        client->inventory.phiras,
        client->inventory.thystame,
        client->inventory.food
    );
}


void get_look_coordinates(client_t *client, int distance, int side_offset, int *x, int *y, server_config_t *conf)
{
    *x = client->x;
    *y = client->y;

    switch (client->direction) {
        case NORTH:
            *y -= distance;
            *x += side_offset;
            break;
        case EAST:
            *x += distance;
            *y += side_offset;
            break;
        case SOUTH:
            *y += distance;
            *x -= side_offset;
            break;
        case WEST:
            *x -= distance;
            *y -= side_offset;
            break;
    }
    *x = (*x + conf->width) % conf->width;
    *y = (*y + conf->height) % conf->height;
}

void look_around(client_t *client, server_config_t *conf)
{
    char response[2048] = "[";
    char tile_content[256];
    int vision_range = client->level;

    for (int distance = 0; distance <= vision_range; distance++) {
        int tiles_at_distance = (distance == 0) ? 1 : (2 * distance + 1);
        int start_offset = -distance;
        for (int i = 0; i < tiles_at_distance; i++) {
            int side_offset = start_offset + i;
            int x, y;
            get_look_coordinates(client, distance, side_offset, &x, &y, conf);
            get_tile_content_string(conf, x, y, tile_content, sizeof(tile_content));
            add_players_to_tile(conf, x, y, tile_content, sizeof(tile_content), client);
            if (strlen(response) > 1) {
                strcat(response, ",");
            }
            strcat(response, tile_content);
        }
    }
    strcat(response, "]\n");
    send(client->fd, response, strlen(response), 0);
}

int calculate_direction_tile(client_t *sender, client_t *receiver)
{
    int dx = sender->x - receiver->x;
    int dy = sender->y - receiver->y;

    if (dx == 0 && dy == 0)
        return 0;

    if (dy < 0) {
        if (dx < 0) return 1; // North-East
        if (dx == 0) return 2; // North
        return 3; // North-West
    } else if (dy == 0) {
        if (dx < 0) return 8; // East
        return 4; // West
    } else {
        if (dx < 0) return 7; // South-East
        if (dx == 0) return 6; // South
        return 5; // South-West
    }
}

int find_nb_teams(client_t *client)
{
    int team_count = 0;
    for (client_t *c = client; c != NULL; c = c->next) {
        team_count++;
    }
    return team_count;
}
