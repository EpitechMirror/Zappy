/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** handle_client_data
*/

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>

#include "../../include/flag.h"
#include "../../include/ressources.h"
#include "../../include/server.h"


double get_current_time(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

double get_command_delay(const char *cmd, server_config_t *conf)
{
    if (strncmp(cmd, "Forward", 7) == 0 ||
        strncmp(cmd, "Right", 5) == 0 ||
        strncmp(cmd, "Left", 4) == 0 ||
        strncmp(cmd, "Look", 4) == 0 ||
        strncmp(cmd, "Broadcast", 9) == 0 ||
        strncmp(cmd, "Take", 4) == 0 ||
        strncmp(cmd, "Set", 3) == 0 ||
        strncmp(cmd, "Eject", 5) == 0)
        return 7.0 / conf->freq;
    if (strncmp(cmd, "Inventory", 9) == 0)
        return 1.0 / conf->freq;
    if (strncmp(cmd, "Fork", 4) == 0)
        return 42.0 / conf->freq;
    if (strncmp(cmd, "Incantation", 11) == 0)
        return 300.0 / conf->freq;
    return 0.0;
}
