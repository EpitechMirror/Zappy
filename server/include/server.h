/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** server
*/

#ifndef SERVER_H_
#define SERVER_H_
#include "flag.h"

#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include "Client_Info.h"
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <stdbool.h>

//int init_server(server_config_t *conf);

int create_server_socket(int port);
void handle_new_connection(int server_fd, client_t **clients, server_config_t *conf);
void remove_client(client_t **clients, int fd);
void accept_clients_loop(int server_fd, server_config_t *conf);

#endif /* !SERVER_H_ */
