/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** remove_client
*/

#include "server.h"


void remove_client(client_t **clients, int fd)
{
    client_t *prev = NULL, *cur = *clients;
    while (cur) {
        if (cur->fd == fd) {
            if (prev)
                prev->next = cur->next;
            else
                *clients = cur->next;
            close(cur->fd);
            free(cur);
            printf("Client disconnected: fd=%d\n", fd);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}
