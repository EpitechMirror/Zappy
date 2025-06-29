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

void execute_command(client_t *client, server_config_t *conf, const char *cmd)
{
    char inventory[256];
    int eject_direction;
    char *message;
    int direction_tile;
    char broadcast_msg[512];
    char response[32];

    if (strncmp(cmd, "Forward", 7) == 0) {
        move_player(client, conf, client->direction);
        send(client->fd, "ok\n", 3, 0);
        ppo_graphics(client, conf);
        pin_graphics(client, conf);
        return;
    }
    if (strncmp(cmd, "Right", 5) == 0) {
        client->direction = (client->direction + 1) % 4;
        send(client->fd, "ok\n", 3, 0);
        ppo_graphics(client, conf);
        pin_graphics(client, conf);
        return;
    }
    if (strncmp(cmd, "Left", 4) == 0) {
        client->direction = (client->direction + 3) % 4;
        send(client->fd, "ok\n", 3, 0);
        ppo_graphics(client, conf);
        pin_graphics(client, conf);
        return;
    }
    if (strncmp(cmd, "Inventory", 9) == 0) {
        get_tile_contents(client, inventory);
        snprintf(inventory, sizeof(inventory), "[linemate %d, deraumere %d, "
        "sibur %d, mendiane %d, phiras %d, thystame %d, food %d]\n",
            client->inventory.linemate,
            client->inventory.deraumere,
            client->inventory.sibur,
            client->inventory.mendiane,
            client->inventory.phiras,
            client->inventory.thystame,
            client->inventory.food
        );
        send(client->fd, inventory, strlen(inventory), 0);
        return;
    }
    if (strncmp(cmd, "Take", 4) == 0) {
        take_object(client, conf, (char *)cmd);
        pin_graphics(client, conf);
        return;
    }
    if (strncmp(cmd, "Look", 4) == 0) {
        look_around(client, conf);
        return;
    }
    if (strncmp(cmd, "Fork", 4) == 0) {
        create_egg_fork(conf, client);
        send(client->fd, "ok\n", 3, 0);
        return;
    }
    if (strncmp(cmd, "Broadcast", 9) == 0) {
        message = (char *)cmd + 10;
        for (client_t *c = conf->clients; c != NULL; c = c->next) {
            if (c != client) {
                direction_tile = calculate_direction_tile(client, c);
                snprintf(broadcast_msg, sizeof(broadcast_msg),
                    "message %d, %s\n", direction_tile, message);
                send(c->fd, broadcast_msg, strlen(broadcast_msg), 0);
            }
        }
        send(client->fd, "ok\n", 3, 0);
        return;
    }
    if (strncmp(cmd, "Connect_nbr", 11) == 0) {
        snprintf(response, sizeof(response), "%d\n", conf->team_count *
            conf->clients_nb - find_nb_teams(client) + 1);
        send(client->fd, response, strlen(response), 0);
        return;
    }
    if (strncmp(cmd, "Set", 3) == 0) {
        set_object(client, conf, (char *)cmd, client->fd);
        return;
    }
    if (strncmp(cmd, "Eject", 5) == 0) {
        for (client_t *c = conf->clients; c != NULL; c = c->next) {
            if (c != client && c->x == client->x && c->y == client->y) {
                eject_direction = (client->direction + 2) % 4;
                move_player(c, conf, eject_direction);
                notify_graphics_player_update(c, conf);
            }
        }
        send(client->fd, "ok\n", 3, 0);
        return;
    }
    if (strncmp(cmd, "Incantation", 11) == 0) {
        handle_incantation(client, conf, client->fd);
        return;
    }
    send(client->fd, "ko\n", 3, 0);
}
