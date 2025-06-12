/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** create_server_socket
*/

#include "server.h"

static int setup_socket_options(int server_fd)
{
    int opt = 1;

    if (setsockopt(server_fd, SOL_SOCKET,
        SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(server_fd);
        return -1;
    }
    return 0;
}

static int bind_and_listen(int server_fd,
    struct sockaddr_in *address, int port)
{
    if (bind(server_fd, (struct sockaddr *)address, sizeof(*address)) < 0) {
        perror("bind failed");
        close(server_fd);
        return -1;
    }
    if (listen(server_fd, 10) < 0) {
        perror("listen");
        close(server_fd);
        return -1;
    }
    printf("Server started on port %d\n", port);
    return 0;
}

int create_server_socket(int port)
{
    int server_fd;
    struct sockaddr_in address;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        return -1;
    }
    if (setup_socket_options(server_fd) < 0)
        return -1;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if (bind_and_listen(server_fd, &address, port) < 0)
        return -1;
    return server_fd;
}
