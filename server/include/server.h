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
    #include <stdlib.h>
    #include <string.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <unistd.h>
    #include <stdio.h>
    #include "Client_Info.h"
    #include <sys/time.h>
    #include <sys/select.h>
    #include <stdbool.h>
    #include <poll.h>
    #include <fcntl.h>
    #include <errno.h>
    #define MAX_CLIENTS 1024
    #define MAX_BUFFER_SIZE 1024


typedef struct poll_context {
    struct pollfd fds[MAX_CLIENTS];
    client_t *clients;
    int *client_count;
    int server_fd;
    server_config_t *conf;
} poll_context_t;

typedef struct {
    client_t **clients;
    client_t *client;
    server_config_t *conf;
} auth_context_t;


int create_server_socket(int port);
void handle_new_connection(poll_context_t *ctx);
void remove_client(client_t **clients, int fd);
void accept_clients_loop(int server_fd, server_config_t *conf);
int remove_client_fd(struct pollfd *fds, int count, int idx);
int add_client_fd(struct pollfd *fds, int client_fd, int count);
bool handle_client_data(client_t **clients, int fd, server_config_t *conf);
void poll_clients(poll_context_t *ctx);
void accept_clients_loop(int server_fd, server_config_t *conf);
int respond_to_server_fd(int fd, server_config_t *conf, char *client_message, client_t *client);

#endif /* !SERVER_H_ */
