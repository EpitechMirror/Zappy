/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** handle_client_data
*/

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "client_info.h"
#include "flag.h"
#include "ressources.h"
#include "server.h"

static void move_north(client_t *client, server_config_t *conf)
{
    if (client->y > 0)
        client->y -= 1;
}

static void move_east(client_t *client, server_config_t *conf)
{
    if (client->x < conf->width - 1)
        client->x += 1;
}

static void move_south(client_t *client, server_config_t *conf)
{
    if (client->y < conf->height - 1)
        client->y += 1;
}

static void move_west(client_t *client, server_config_t *conf)
{
    if (client->x > 0)
        client->x -= 1;
}

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

char* parse_resource_name(char *client_message)
{
    char *resource = client_message + 5;
    char *end = strchr(resource, '\n');

    if (end)
        *end = '\0';
    end = strchr(resource, '\r');
    if (end)
        *end = '\0';
    while (*resource == ' ' || *resource == '\t') resource++;
    int len = strlen(resource);
    while (len > 0 && (resource[len-1] == ' ' || resource[len-1] == '\t')) {
        resource[--len] = '\0';
    }
    return resource;
}

int try_take_resource(client_t *client, server_config_t *conf, char *resource)
{
    tile_t *tile = &conf->map[client->y][client->x];

    if (strcmp(resource, "linemate") == 0 && tile->linemate > 0) {
        client->inventory.linemate++; tile->linemate--; return 1;
    } else if (strcmp(resource, "deraumere") == 0 && tile->deraumere > 0) {
        client->inventory.deraumere++; tile->deraumere--; return 1;
    } else if (strcmp(resource, "sibur") == 0 && tile->sibur > 0) {
        client->inventory.sibur++; tile->sibur--; return 1;
    } else if (strcmp(resource, "mendiane") == 0 && tile->mendiane > 0) {
        client->inventory.mendiane++; tile->mendiane--; return 1;
    } else if (strcmp(resource, "phiras") == 0 && tile->phiras > 0) {
        client->inventory.phiras++; tile->phiras--; return 1;
    } else if (strcmp(resource, "thystame") == 0 && tile->thystame > 0) {
        client->inventory.thystame++; tile->thystame--; return 1;
    } else if (strcmp(resource, "food") == 0 && tile->food > 0) {
        client->inventory.food++; tile->food--; return 1;
    }
    return 0;
}

void take_object(client_t *client, server_config_t *conf, char *client_message)
{
    char *resource = parse_resource_name(client_message);

    if (try_take_resource(client, conf, resource)) {
        send(client->fd, "ok\n", 3, 0);
    } else {
        send(client->fd, "ko\n", 3, 0);
    }
}

void add_resource_to_buffer(char *buffer, char *name, int count, int *first)
{
    for (int i = 0; i < count; i++) {
        if (!*first) strcat(buffer, " ");
        strcat(buffer, name);
        *first = 0;
    }
}

void get_tile_content_string(server_config_t *conf, int x, int y, char *buffer, int buffer_size)
{
    if (x < 0 || x >= conf->width || y < 0 || y >= conf->height) {
        strcpy(buffer, "");
        return;
    }

    tile_t *tile = &conf->map[y][x];
    char temp[512] = "";
    int first = 1;
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

void add_players_to_tile(server_config_t *conf, int x, int y, char *buffer, int buffer_size, client_t *current_client)
{
    char temp[512];
    strcpy(temp, buffer);
    if (x == current_client->x && y == current_client->y) {
        if (strlen(temp) > 0) {
            strcat(temp, " ");
        }
        strcat(temp, "player");
    }
    strncpy(buffer, temp, buffer_size - 1);
    buffer[buffer_size - 1] = '\0';
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

void move_player(client_t *client, server_config_t *conf, direction_t direction)
{
    static void (*move_functions[])(client_t *, server_config_t *) = {
        move_north,
        move_east,
        move_south,
        move_west
    };

    if (direction >= 0 && direction < 4)
        move_functions[direction](client, conf);
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

void notify_graphics_player_update(client_t *player, server_config_t *conf)
{
    char msg[128];
    // ppo
    snprintf(msg, sizeof(msg), "ppo #%d %d %d %d\n", player->fd, player->x, player->y, player->direction + 1);
    for (int i = 0; i < conf->nb_graphics; i++)
        send(conf->graphic_fds[i], msg, strlen(msg), 0);

    // plv
    snprintf(msg, sizeof(msg), "plv #%d %d\n", player->fd, player->level);
    for (int i = 0; i < conf->nb_graphics; i++)
        send(conf->graphic_fds[i], msg, strlen(msg), 0);

    // pin
    snprintf(msg, sizeof(msg), "pin #%d %d %d %d %d %d %d %d %d %d\n",
        player->fd, player->x, player->y,
        player->inventory.food,
        player->inventory.linemate,
        player->inventory.deraumere,
        player->inventory.sibur,
        player->inventory.mendiane,
        player->inventory.phiras,
        player->inventory.thystame
    );
    for (int i = 0; i < conf->nb_graphics; i++)
        send(conf->graphic_fds[i], msg, strlen(msg), 0);
}

int respond_to_server_fd(int fd, server_config_t *conf, char *client_message, client_t *client)
{
    char inventory[256];

    if (strncmp(client_message, "Forward", 7) == 0) {
    move_player(client, conf, client->direction);
    send(fd, "ok\n", 3, 0);
    notify_graphics_player_update(client, conf);
    return 0;
    }
    if (strncmp(client_message, "Right", 5) == 0) {
        client->direction = (client->direction + 1) % 4;
        send(fd, "ok\n", 3, 0);
        notify_graphics_player_update(client, conf);
        return 0;
    }
    if (strncmp(client_message, "Left", 4) == 0) {
        client->direction = (client->direction + 3) % 4;
        send(fd, "ok\n", 3, 0);
        notify_graphics_player_update(client, conf);
        return 0;
    }
    if (strncmp(client_message, "Inventory", 9) == 0) {
        get_tile_contents(client->x, client->y, client, inventory);
        snprintf(inventory, sizeof(inventory), "[linemate %d, deraumere %d, sibur %d, mendiane %d, phiras %d, thystame %d, food %d]\n",
            client->inventory.linemate,
            client->inventory.deraumere,
            client->inventory.sibur,
            client->inventory.mendiane,
            client->inventory.phiras,
            client->inventory.thystame,
            client->inventory.food
        );
        send(fd, inventory, strlen(inventory), 0);
        return 0;
    }
    if (strncmp(client_message, "Take", 4) == 0) {
        take_object(client, conf, client_message);
        return 0;
    }
    if (strncmp(client_message, "Look", 4) == 0) {
        look_around(client, conf);
        return 0;
    }
    if (strncmp(client_message, "Broadcast", 9) == 0) {
        for (client_t *c = conf->clients; c != NULL; c = c->next) {
            if (c->is_alive && c->fd != fd && !c->is_graphic) {
                char broadcast_msg[512];
                snprintf(broadcast_msg, sizeof(broadcast_msg), "message %d, %s\n",
                         calculate_direction_tile(client, c), client_message + 10);
                send(c->fd, broadcast_msg, strlen(broadcast_msg), 0);
            }
        }
        send(fd, "ok\n", 3, 0);
        return 0;
    }
    return 0;
}