/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Client_Info
*/

#ifndef CLIENT_INFO_H_
#define CLIENT_INFO_H_
#include <stdbool.h>

typedef struct client_s {
    int fd;
    bool connected;
    bool is_ready;
    bool is_alive;
    int x, y, id;
    char *team_name;
    bool is_graphic;
    struct client_s *next;
} client_t;


#endif /* !CLIENT_INFO_H_ */
