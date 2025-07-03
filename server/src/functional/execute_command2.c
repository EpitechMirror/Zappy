/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** handle_client_data
*/

#include "../../include/client_info.h"
#include "../../include/flag.h"
#include "../../include/ressources.h"
#include "../../include/server.h"

int find_resource_index(char *resource_name) {
    static const char *resources[] = {
        "linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame", "food"
    };

    for (int i = 0; i < RESOURCE_TYPES; i++) {
        if (strcmp(resource_name, resources[i]) == 0) {
            return i;
        }
    }
    return -1;
}

// void send_pgt(int client_id, int resource_index, server_config_t *conf)
// {
//     char buffer[64];
//     int len = snprintf(buffer, sizeof(buffer), "pgt %d %d\n", client_id, resource_index);

//     for (int i = 0; i < conf->nb_graphics; i++) {
//         if (conf->graphic_fds[i] != -1) {
//             send(conf->graphic_fds[i], buffer, len, 0);
//         }
//     }
// }

void handle_take_command(client_t *client, server_config_t *conf,
    const char *cmd)
{
    take_object(client, conf, (char *)cmd);
    // send_pgt(client->id, find_resource_index((char *)cmd + 5), conf);
    pin_graphics(client, conf);
}

void handle_look_command(client_t *client, server_config_t *conf,
    const char *cmd)
{
    (void)cmd;
    look_around(client, conf);
}

void handle_fork_command(client_t *client, server_config_t *conf,
    const char *cmd)
{
    (void)cmd;
    create_egg_fork(conf, client);
    send(client->fd, "ok\n", 3, 0);
}
