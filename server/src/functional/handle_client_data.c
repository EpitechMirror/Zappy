/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** handle_client_data
*/

#include "../../include/server.h"

static int find_team_index(server_config_t *conf, const char *team_name)
{
    int i = 0;

    while (i < conf->team_count) {
        if (strcmp(conf->team_names[i], team_name) == 0)
            return i;
        i++;
    }
    return -1;
}

static void send_auth_success(int fd, server_config_t *conf)
{
    char msg[64];

    snprintf(msg, sizeof(msg), "%d\n", conf->clients_nb);
    send(fd, msg, strlen(msg), 0);
    snprintf(msg, sizeof(msg), "%d %d\n", conf->width, conf->height);
    send(fd, msg, strlen(msg), 0);
}

static int count_team_members(client_t *clients, const char *team_name)
{
    int count = 0;
    while (clients) {
        if (clients->team_name && strcmp(clients->team_name, team_name) == 0 && !clients->is_graphic)
            count++;
        clients = clients->next;
    }
    return count;
}

bool handle_auth(client_t **clients, client_t *client, int fd, server_config_t *conf, char *buffer)
{
    buffer[strcspn(buffer, "\r\n")] = 0; // enlever \r\n
    char *team = strdup(buffer);
    if (!team) {
        perror("strdup");
        remove_client(clients, fd);
        return false;
    }
    client->team_name = team;

    if (strcmp(team, "GRAPHIC") == 0) {
        client->is_graphic = true;
        char msg[64];
        snprintf(msg, sizeof(msg), "msz %d %d\n", conf->width, conf->height);
        send(fd, msg, strlen(msg), 0);
        client->state = AUTHENTICATED;
        printf("Client %d authenticated as GRAPHIC\n", fd);
        return true;
    } else {
        client->is_graphic = false;
        char msg[128];
        snprintf(msg, sizeof(msg), "Bienvenue joueur de l'équipe %s\n%d\n%d %d\n",
            team, conf->clients_nb, conf->width, conf->height);
        send(fd, msg, strlen(msg), 0);
        client->state = AUTHENTICATED;
        printf("Client %d authenticated as PLAYER (%s)\n", fd, team);
        return true;
    }
}

bool handle_client_data(client_t **clients, int fd, server_config_t *conf)
{
    char buffer[1024];
    ssize_t r;
    client_t *client = *clients;

    while (client && client->fd != fd)
        client = client->next;
    if (!client)
        return true;

    r = read(fd, buffer, sizeof(buffer) - 1);
    if (r <= 0) {
        remove_client(clients, fd);
        return true;
    }
    buffer[r] = '\0';
    printf("[DEBUG] Data from fd %d: %s\n", fd, buffer);

    if (client->state == WAITING_NAME) {
        handle_auth(clients, client, fd, conf, buffer);
    } 
    return false;
}
