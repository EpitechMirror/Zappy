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

static void move_north(client_t *client, server_config_t *conf)
{
    client->y = (client->y + conf->height - 1) % conf->height;
}

static void move_east(client_t *client, server_config_t *conf)
{
    client->x = (client->x + 1) % conf->width;
}

static void move_south(client_t *client, server_config_t *conf)
{
    client->y = (client->y + 1) % conf->height;
}

static void move_west(client_t *client, server_config_t *conf)
{
    client->x = (client->x + conf->width - 1) % conf->width;
}

static void get_tile_content_string(server_config_t *conf, int xy[2],
    char *buffer, int buffer_size)
{
    char temp[512] = "";
    int first = 1;
    tile_t *tile;

    if (xy[0] < 0 || xy[0] >= conf->width || xy[1] < 0 ||
        xy[1] >= conf->height) {
        strcpy(buffer, "");
        return;
    }
    tile = &conf->map[xy[1]][xy[0]];
    add_resource_to_buffer(temp, "food", tile->food, &first);
    add_resource_to_buffer(temp, "linemate", tile->linemate, &first);
    add_resource_to_buffer(temp, "deraumere", tile->deraumere, &first);
    add_resource_to_buffer(temp, "sibur", tile->sibur, &first);
    add_resource_to_buffer(temp, "mendiane", tile->mendiane, &first);
    add_resource_to_buffer(temp, "phiras", tile->phiras, &first);
    add_resource_to_buffer(temp, "thystame", tile->thystame, &first);
    strncpy(buffer, temp, buffer_size - 1);
    buffer[buffer_size - 1] = '\0';
}

void add_players_to_tile(int xy[2], char *buffer,
    int buffer_size, client_t *current_client)
{
    char temp[512];

    strcpy(temp, buffer);
    if (xy[0] == current_client->x && xy[1] == current_client->y) {
        if (strlen(temp) > 0)
            strcat(temp, " ");
        strcat(temp, "player");
    }
    strncpy(buffer, temp, buffer_size - 1);
    buffer[buffer_size - 1] = '\0';
}

void process_vision_distance(client_t *client, server_config_t *conf,
    int distance, char *response)
{
    char tile_content[256];
    int x;
    int y;
    int tiles_at_distance = (distance == 0) ? 1 : (2 * distance + 1);
    int start_offset = -distance;
    int side_offset;

    for (int i = 0; i < tiles_at_distance; i++) {
        side_offset = start_offset + i;
        get_look_coordinates(client, (int[]){distance, side_offset},
            (int *[]){&x, &y}, conf);
        get_tile_content_string(conf, (int[]){x, y}, tile_content,
            sizeof(tile_content));
        add_players_to_tile((int[]){x, y}, tile_content,
            sizeof(tile_content), client);
        if (strlen(response) > 1)
            strcat(response, ",");
        strcat(response, tile_content);
    }
}

void look_around(client_t *client, server_config_t *conf)
{
    char response[2048] = "[";
    int vision_range = client->level;

    for (int distance = 0; distance <= vision_range; distance++) {
        process_vision_distance(client, conf, distance, response);
    }
    strcat(response, "]\n");
    send(client->fd, response, strlen(response), 0);
}

void move_player(client_t *client, server_config_t *cnf, direction_t direction)
{
    static void (*move_functions[])(client_t *, server_config_t *) = {
        move_north,
        move_east,
        move_south,
        move_west
    };

    if (direction >= 0 && direction < 4)
        move_functions[direction](client, cnf);
}
