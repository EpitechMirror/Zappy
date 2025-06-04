/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** init_server
*/

#include "server.h"

int init_server(server_config_t *conf)
{
    int server_fd;
    int new_socket;
    ssize_t valread;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t arrlen = sizeof(address);
    char buffer[1024] = {0};
    char hello[] = "hello world!";

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        return 84;
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(server_fd);
        return 84;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(conf->port);
    printf("Server started on port %d\n", conf->port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        return 84;
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        close(server_fd);
        return 84;
    }
    new_socket = accept(server_fd, (struct sockaddr *)&address, &arrlen);
    if (new_socket < 0) {
        perror("accept");
        close(server_fd);
        return 84;
    }
    valread = read(new_socket, buffer, 1023);
    buffer[valread > 0 ? valread : 0] = '\0';
    printf("%s\n", buffer);
    send(new_socket, hello, strlen(hello), 0);
    close(new_socket);
    close(server_fd);
    return 0;
}