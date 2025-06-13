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

static int alloc_team_slots(server_config_t *conf)
{
    int i = 0;

    conf->team_slots = malloc(sizeof(int) * conf->team_count);
    if (!conf->team_slots)
        return 84;
    for (i = 0; i < conf->team_count; i++)
        conf->team_slots[i] = conf->clients_nb;
    return 0;
}

static int alloc_map(server_config_t *conf)
{
    int y = 0;

    conf->map = malloc(sizeof(tile_t *) * conf->height);
    if (!conf->map)
        return 84;
    for (y = 0; y < conf->height; y++) {
        conf->map[y] = calloc(conf->width, sizeof(tile_t));
        if (!conf->map[y])
            return 84;
    }
    return 0;
}

static int setup_server(int argc, char **argv, server_config_t *conf)
{
    if (argc < 2) {
        print_help();
        return 84;
    }
    if (parse(argc, argv, conf) == 84)
        return 84;
    if (alloc_team_slots(conf) == 84)
        return 84;
    if (alloc_map(conf) == 84)
        return 84;
    generate_map_resources(conf);
    conf->clients = calloc(MAX_CLIENTS, sizeof(client_t));
    if (!conf->clients)
        return 84;
    return 0;
}

int main(int argc, char **argv)
{
    server_config_t conf = {0};
    int server_fd;

    if (setup_server(argc, argv, &conf) == 84)
        return 84;
    server_fd = create_server_socket(conf.port);
    if (server_fd < 0)
        return 84;
    accept_clients_loop(server_fd, &conf);
    close(server_fd);
    free(conf.team_slots);
    return 0;
}
