/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** main
*/

#include "flag.h"
#include "server.h"

void print_help(void)
{
    printf("USAGE: ./zappy_server -p port -x width -y height");
    printf(" -n name1 name2 ... -c clientsNb -f freq\n");
    printf("\n");
    printf("DESCRIPTION:\n");
    printf("  -p port        is the port number\n");
    printf("  -x width       is the width of the world\n");
    printf("  -y height      is the height of the world\n");
    printf("  -n name1 name2 ... is the name of the teams\n");
    printf("  -c clientsNb   is the number of authorized clients per team\n");
    printf("  -f freq        is the reciprocal of time unit");
    printf(" for execution of actions\n");
}

int main(int argc, char **argv)
{
    server_config_t conf = {0};
    int server_fd;

    if (argc < 2) {
        print_help();
        return 84;
    }
    if (parse(argc, argv, &conf) == 84)
        return 84;
    server_fd = create_server_socket(conf.port);
    if (server_fd < 0)
        return 84;
    accept_clients_loop(server_fd, &conf);
    close(server_fd);
    return 0;
}
