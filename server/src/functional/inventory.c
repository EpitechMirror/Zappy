/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** inventory
*/

#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "client_info.h"
#include "flag.h"
#include "ressources.h"
#include "server.h"

static char* parse_resource_name(char *client_message)
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

void take_object(client_t *client, server_config_t *conf, char *client_message)
{
    char *resource = parse_resource_name(client_message);

    if (try_take_resource(client, conf, resource)) {
        send(client->fd, "ok\n", 3, 0);
    } else {
        send(client->fd, "ko\n", 3, 0);
    }
}

void set_object(client_t *client, server_config_t *conf, char *client_message, int fd)
{
    char *resource = client_message + 4;
    tile_t *tile = &conf->map[client->y][client->x];

    if (strcmp(resource, "linemate") == 0 && client->inventory.linemate > 0) {
        tile->linemate++; client->inventory.linemate--; send(fd, "ok\n", 3, 0);
    } else if (strcmp(resource, "deraumere") == 0 && client->inventory.deraumere > 0) {
        tile->deraumere++; client->inventory.deraumere--; send(fd, "ok\n", 3, 0);
    } else if (strcmp(resource, "sibur") == 0 && client->inventory.sibur > 0) {
        tile->sibur++; client->inventory.sibur--; send(fd, "ok\n", 3, 0);
    } else if (strcmp(resource, "mendiane") == 0 && client->inventory.mendiane > 0) {
        tile->mendiane++; client->inventory.mendiane--; send(fd, "ok\n", 3, 0);
    } else if (strcmp(resource, "phiras") == 0 && client->inventory.phiras > 0) {
        tile->phiras++; client->inventory.phiras--; send(fd, "ok\n", 3, 0);
    } else if (strcmp(resource, "thystame") == 0 && client->inventory.thystame > 0) {
        tile->thystame++; client->inventory.thystame--; send(fd, "ok\n", 3, 0);
    } else if (strcmp(resource, "food") == 0 && client->inventory.food > 0) {
        tile->food++; client->inventory.food--; send(fd, "ok\n", 3, 0);
    } else {
        send(fd, "ko\n", 3, 0);
    }
    notify_graphics_player_update(client, conf);
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

int try_drop_ressource(client_t *client, server_config_t *conf, char *resource)
{
    tile_t *tile = &conf->map[client->y][client->x];

    if (strcmp(resource, "linemate") == 0 && client->inventory.linemate > 0) {
        tile->linemate++; client->inventory.linemate--; return 1;
    } else if (strcmp(resource, "deraumere") == 0 && client->inventory.deraumere > 0) {
        tile->deraumere++; client->inventory.deraumere--; return 1;
    } else if (strcmp(resource, "sibur") == 0 && client->inventory.sibur > 0) {
        tile->sibur++; client->inventory.sibur--; return 1;
    } else if (strcmp(resource, "mendiane") == 0 && client->inventory.mendiane > 0) {
        tile->mendiane++; client->inventory.mendiane--; return 1;
    } else if (strcmp(resource, "phiras") == 0 && client->inventory.phiras > 0) {
        tile->phiras++; client->inventory.phiras--; return 1;
    } else if (strcmp(resource, "thystame") == 0 && client->inventory.thystame > 0) {
        tile->thystame++; client->inventory.thystame--; return 1;
    } else if (strcmp(resource, "food") == 0 && client->inventory.food > 0) {
        tile->food++; client->inventory.food--; return 1;
    }
    return 0;
}

void drop_object(client_t *client, server_config_t *conf,
    char *client_message, int fd)
{
    char *resource = parse_resource_name(client_message);
    tile_t *tile = &conf->map[client->y][client->x];

    if (strcmp(resource, "linemate") == 0 && client->inventory.linemate > 0) {
        tile->linemate++; client->inventory.linemate--; send(fd, "ok\n", 3, 0);
    } else if (strcmp(resource, "deraumere") == 0 && client->inventory.deraumere > 0) {
        tile->deraumere++; client->inventory.deraumere--; send(fd, "ok\n", 3, 0);
    } else if (strcmp(resource, "sibur") == 0 && client->inventory.sibur > 0) {
        tile->sibur++; client->inventory.sibur--; send(fd, "ok\n", 3, 0);
    } else if (strcmp(resource, "mendiane") == 0 && client->inventory.mendiane > 0) {
        tile->mendiane++; client->inventory.mendiane--; send(fd, "ok\n", 3, 0);
    } else if (strcmp(resource, "phiras") == 0 && client->inventory.phiras > 0) {
        tile->phiras++; client->inventory.phiras--; send(fd, "ok\n", 3, 0);
    } else if (strcmp(resource, "thystame") == 0 && client->inventory.thystame > 0) {
        tile->thystame++; client->inventory.thystame--; send(fd, "ok\n", 3, 0);
    } else if (strcmp(resource, "food") == 0 && client->inventory.food > 0) {
        tile->food++; client->inventory.food--; send(fd, "ok\n", 3, 0);
    } else {
        send(fd, "ko\n", 3, 0);
    }
    notify_graphics_player_update(client, conf);
}
