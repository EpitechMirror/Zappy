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

void handle_take_command(client_t *client, server_config_t *conf,
    const char *cmd)
{
    take_object(client, conf, (char *)cmd);
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
