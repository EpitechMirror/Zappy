/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** remove_client
*/

#include "../../include/server.h"

static void free_client(client_t *client)
{
    close(client->fd);
    free(client->team_name);
    free(client);
}

void remove_client(client_t **clients, int fd)
{
    client_t *prev = NULL;
    client_t *cur = *clients;

    while (cur && cur->fd != fd) {
        prev = cur;
        cur = cur->next;
    }
    if (!cur)
        return;
    if (prev)
        prev->next = cur->next;
    else
        *clients = cur->next;
    printf("Client disconnected: fd=%d\n", fd);
    free_client(cur);
}

int remove_client_fd(struct pollfd *fds, int count, int idx)
{
    for (int i = idx; i < count - 1; i++)
        fds[i] = fds[i + 1];
    return count - 1;
}

int add_client_fd(struct pollfd *fds, int client_fd, int count)
{
    if (count >= MAX_CLIENTS)
        return count;
    fds[count].fd = client_fd;
    fds[count].events = POLLIN;
    return count + 1;
}
