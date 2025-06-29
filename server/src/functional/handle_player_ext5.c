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

static const elevation_req_t elevation_requirements[] = {
    {1, 1, 0, 0, 0, 0, 0},  // 1->2
    {2, 1, 1, 1, 0, 0, 0},  // 2->3
    {2, 2, 0, 1, 0, 2, 0},  // 3->4
    {4, 1, 1, 2, 0, 1, 0},  // 4->5
    {4, 1, 2, 1, 3, 0, 0},  // 5->6
    {6, 1, 2, 3, 0, 1, 0},  // 6->7
    {6, 2, 2, 2, 2, 2, 1}   // 7->8
};

static void update_coords_north(int *x, int *y, int dist_and_sideoff[2])
{
    *y -= dist_and_sideoff[0];
    *x += dist_and_sideoff[1];
}

static void update_coords_east(int *x, int *y, int dist_and_sideoff[2])
{
    *x += dist_and_sideoff[0];
    *y += dist_and_sideoff[1];
}

static void update_coords_south(int *x, int *y, int dist_and_sideoff[2])
{
    *y += dist_and_sideoff[0];
    *x -= dist_and_sideoff[1];
}

static void update_coords_west(int *x, int *y, int dist_and_sideoff[2])
{
    *x -= dist_and_sideoff[0];
    *y -= dist_and_sideoff[1];
}

void get_look_coordinates(client_t *client, int dist_and_sideoff[2],
    int **coords, server_config_t *conf)
{
    static void (*update_functions[])(int *, int *, int *) = {
        update_coords_north, update_coords_east,
        update_coords_south, update_coords_west
    };

    *coords[0] = client->x;
    *coords[1] = client->y;
    if (client->direction >= 0 && client->direction < 4)
        update_functions[client->direction](coords[0], coords[1],
            dist_and_sideoff);
    *coords[0] = (*coords[0] + conf->width) % conf->width;
    *coords[1] = (*coords[1] + conf->height) % conf->height;
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

int respond_to_server_fd(server_config_t *conf, char *c_message,
    client_t *c)
{
    double now = get_current_time();
    double delay = get_command_delay(c_message, conf);
    double exec_time = c->last_command_end > now ? c->last_command_end : now;
    command_t *new_cmd = malloc(sizeof(command_t));
    command_t *tmp;

    exec_time += delay;
    new_cmd->cmd = strdup(c_message);
    new_cmd->execute_at = exec_time;
    new_cmd->next = NULL;
    if (!c->command_queue)
        c->command_queue = new_cmd;
    else {
        tmp = c->command_queue;
        while (tmp->next)
            tmp = tmp->next;
        tmp->next = new_cmd;
    }
    c->last_command_end = exec_time;
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
