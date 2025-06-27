/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** create_server_socket
*/

#include "server.h"

int create_server_socket(int port)
{
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        return -1;
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(server_fd);
        return -1;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
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
    return server_fd;
}
