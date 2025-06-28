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

void get_tile_contents(int x, int y, client_t *client, char *inventory)
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

char *parse_resource_name(char *client_message)
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

int try_take_resource(client_t *client, server_config_t *conf, char *resource)
{
    tile_t *tile = &conf->map[client->y][client->x];

    if (strcmp(resource, "linemate") == 0 && tile->linemate > 0) {
        client->inventory.linemate++;
        tile->linemate--;
        return 1;
    }
    if (strcmp(resource, "deraumere") == 0 && tile->deraumere > 0) {
        client->inventory.deraumere++;
        tile->deraumere--;
        return 1;
    }
    if (strcmp(resource, "sibur") == 0 && tile->sibur > 0) {
        client->inventory.sibur++;
        tile->sibur--;
        return 1;
    }
    if (strcmp(resource, "mendiane") == 0 && tile->mendiane > 0) {
        client->inventory.mendiane++;
        tile->mendiane--;
        return 1;
    }
    if (strcmp(resource, "phiras") == 0 && tile->phiras > 0) {
        client->inventory.phiras++;
        tile->phiras--;
        return 1;
    }
    if (strcmp(resource, "thystame") == 0 && tile->thystame > 0) {
        client->inventory.thystame++;
        tile->thystame--;
        return 1;
    }
    if (strcmp(resource, "food") == 0 && tile->food > 0) {
        client->inventory.food++;
        tile->food--;
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

void add_resource_to_buffer(char *buffer, char *name, int count, int *first)
{
    for (int i = 0; i < count; i++) {
        if (!*first)
            strcat(buffer, " ");
        strcat(buffer, name);
        *first = 0;
    }
}

void get_tile_content_string(server_config_t *conf, int x, int y, char *buffer,
    int buffer_size)
{
    char temp[512] = "";
    int first = 1;
    tile_t *tile;

    if (x < 0 || x >= conf->width || y < 0 || y >= conf->height) {
        strcpy(buffer, "");
        return;
    }
    tile = &conf->map[y][x];
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

void add_players_to_tile(server_config_t *conf, int x, int y, char *buffer,
    int buffer_size, client_t *current_client)
{
    char temp[512];

    strcpy(temp, buffer);
    if (x == current_client->x && y == current_client->y) {
        if (strlen(temp) > 0)
            strcat(temp, " ");
        strcat(temp, "player");
    }
    strncpy(buffer, temp, buffer_size - 1);
    buffer[buffer_size - 1] = '\0';
}

void get_look_coordinates(client_t *client, int distance, int side_offset,
    int *x, int *y, server_config_t *conf)
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
    int x;
    int y;
    int tiles_at_distance;
    int start_offset;
    int side_offset;

    for (int distance = 0; distance <= vision_range; distance++) {
        tiles_at_distance = (distance == 0) ? 1 : (2 * distance + 1);
        start_offset = -distance;
        for (int i = 0; i < tiles_at_distance; i++) {
            side_offset = start_offset + i;
            get_look_coordinates(client, distance, side_offset, &x, &y, conf);
            get_tile_content_string(conf, x, y, tile_content,
                sizeof(tile_content));
            add_players_to_tile(conf, x, y, tile_content, sizeof(tile_content),
                client);
            if (strlen(response) > 1)
                strcat(response, ",");
            strcat(response, tile_content);
        }
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

void ppo_graphics(client_t *client, server_config_t *conf)
{
    char msg[128];

    snprintf(msg, sizeof(msg), "ppo #%d %d %d %d\n", client->fd, client->x,
        client->y, client->direction + 1);
    for (int i = 0; i < conf->nb_graphics; i++)
        send(conf->graphic_fds[i], msg, strlen(msg), 0);
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

void set_object(client_t *client, server_config_t *conf, char *client_message,
    int fd)
{
    char *resourc = client_message + 4;
    tile_t *tile = &conf->map[client->y][client->x];

    if (strcmp(resourc, "linemate") == 0 && client->inventory.linemate > 0) {
        tile->linemate++;
        client->inventory.linemate--;
        send(fd, "ok\n", 3, 0);
    }
    if (strcmp(resourc, "deraumere") == 0 && client->inventory.deraumere > 0) {
        tile->deraumere++;
        client->inventory.deraumere--;
        send(fd, "ok\n", 3, 0);
    }
    if (strcmp(resourc, "sibur") == 0 && client->inventory.sibur > 0) {
        tile->sibur++;
        client->inventory.sibur--;
        send(fd, "ok\n", 3, 0);
    }
    if (strcmp(resourc, "mendiane") == 0 && client->inventory.mendiane > 0) {
        tile->mendiane++;
        client->inventory.mendiane--;
        send(fd, "ok\n", 3, 0);
    }
    if (strcmp(resourc, "phiras") == 0 && client->inventory.phiras > 0) {
        tile->phiras++;
        client->inventory.phiras--;
        send(fd, "ok\n", 3, 0);
    }
    if (strcmp(resourc, "thystame") == 0 && client->inventory.thystame > 0) {
        tile->thystame++;
        client->inventory.thystame--;
        send(fd, "ok\n", 3, 0);
    }
    if (strcmp(resourc, "food") == 0 && client->inventory.food > 0) {
        tile->food++;
        client->inventory.food--;
        send(fd, "ok\n", 3, 0);
    } else
        send(fd, "ko\n", 3, 0);
    notify_graphics_player_update(client, conf);
}

// Elevation requirements table (level 1->2, 2->3, etc.)
static const elevation_req_t elevation_requirements[] = {
    {1, 1, 0, 0, 0, 0, 0},  // 1->2
    {2, 1, 1, 1, 0, 0, 0},  // 2->3
    {2, 2, 0, 1, 0, 2, 0},  // 3->4
    {4, 1, 1, 2, 0, 1, 0},  // 4->5
    {4, 1, 2, 1, 3, 0, 0},  // 5->6
    {6, 1, 2, 3, 0, 1, 0},  // 6->7
    {6, 2, 2, 2, 2, 2, 1}   // 7->8
};

int count_players_on_tile(server_config_t *conf, int x, int y, int level)
{
    int count = 0;

    for (client_t *c = conf->clients; c != NULL; c = c->next)
        if (c->x == x && c->y == y && c->level == level)
            count++;
    return count;
}

int check_tile_resources(server_config_t *conf, int x, int y,
    const elevation_req_t *req)
{
    tile_t *tile = &conf->map[y][x];

    return (tile->linemate >= req->linemate &&
            tile->deraumere >= req->deraumere &&
            tile->sibur >= req->sibur &&
            tile->mendiane >= req->mendiane &&
            tile->phiras >= req->phiras &&
            tile->thystame >= req->thystame);
}

void consume_tile_resources(server_config_t *conf, int x, int y,
    const elevation_req_t *req)
{
    tile_t *tile = &conf->map[y][x];

    tile->linemate -= req->linemate;
    tile->deraumere -= req->deraumere;
    tile->sibur -= req->sibur;
    tile->mendiane -= req->mendiane;
    tile->phiras -= req->phiras;
    tile->thystame -= req->thystame;
}

void elevate_players_on_tile(server_config_t *conf, int x, int y, int level)
{
    char level_msg[32];

    for (client_t *c = conf->clients; c != NULL; c = c->next) {
        if (c->x == x && c->y == y && c->level == level) {
            c->level++;
            notify_graphics_player_update(c, conf);
            send(c->fd, "Current level: ", 15, 0);
            snprintf(level_msg, sizeof(level_msg), "%d\n", c->level);
            send(c->fd, level_msg, strlen(level_msg), 0);
        }
    }
}

void handle_incantation(client_t *client, server_config_t *conf, int fd)
{
    int players_on_tile;
    const elevation_req_t *req;

    if (client->level >= 8) {
        send(fd, "ko\n", 3, 0);
        return;
    }
    req = &elevation_requirements[client->level - 1];
    players_on_tile = count_players_on_tile(conf, client->x, client->y,
        client->level);
    if (players_on_tile < req->nb_players) {
        send(fd, "ko\n", 3, 0);
        return;
    }
    if (!check_tile_resources(conf, client->x, client->y, req)) {
        send(fd, "ko\n", 3, 0);
        return;
    }
    consume_tile_resources(conf, client->x, client->y, req);
    elevate_players_on_tile(conf, client->x, client->y, client->level);
}

int respond_to_server_fd(int fd, server_config_t *conf, char *client_message,
    client_t *client)
{
    double now = get_current_time();
    double delay = get_command_delay(client_message, conf);
    double exec_time = client->last_command_end > now ?
        client->last_command_end : now;
    command_t *new_cmd = malloc(sizeof(command_t));
    command_t *tmp;

    exec_time += delay;
    new_cmd->cmd = strdup(client_message);
    new_cmd->execute_at = exec_time;
    new_cmd->next = NULL;
    if (!client->command_queue)
        client->command_queue = new_cmd;
    else {
        tmp = client->command_queue;
        while (tmp->next)
            tmp = tmp->next;
        tmp->next = new_cmd;
    }
    client->last_command_end = exec_time;
    return 0;
}

void execute_pending_commands(server_config_t *conf)
{
    double now = get_current_time();
    command_t *cmd;

    for (client_t *c = conf->clients; c; c = c->next) {
        if (!c->is_alive || !c->command_queue)
            continue;
        cmd = c->command_queue;
        if (cmd->execute_at <= now) {
            execute_command(c, conf, cmd->cmd);
            c->command_queue = cmd->next;
            free(cmd->cmd);
            free(cmd);
        }
    }
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
        get_tile_contents(client->x, client->y, client, inventory);
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
