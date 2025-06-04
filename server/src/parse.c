/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** parse
*/

#include <stdlib.h>
#include <string.h>
#include "flag.h"
#include <stdio.h>

int handle_int(int *field, int *i, char **argv) {
    *field = atoi(argv[++(*i)]);
    printf("Setting %s to %d\n", argv[*i - 1], *field);
    return 0;
}

int handle_teams(server_config_t *conf, int *i, char **argv) {
    int start = ++(*i);
    int count = 0;

    while (argv[*i] && argv[*i][0] != '-') {
        count++;
        (*i)++;
    }

    conf->team_count = count;
    printf("Setting team count to %d\n", conf->team_count);
    conf->team_names = malloc(sizeof(char *) * (count + 1));
    if (!conf->team_names)
        return 84;

    for (int j = 0; j < count; j++)
        conf->team_names[j] = strdup(argv[start + j]);
    conf->team_names[count] = NULL;
    printf("Teams: ");
    for (int j = 0; j < count; j++) {
        printf("%s ", conf->team_names[j]);
    }
    printf("\n");
    (*i)--;
    return 0;
}

int strcmp_flag(char **argv, server_config_t *conf, int *i, flag_entry_t *flag_table)
{
    for (int j = 0; flag_table[j].flag; j++) {
        if (strcmp(argv[*i], flag_table[j].flag) == 0) {
            if (flag_table[j].field) {
                flag_table[j].handler(flag_table[j].field, i, argv);
            } else {
                flag_table[j].handler(conf, i, argv);
            }
            return 1;
        }
    }
    return 0;
}

int parse(int argc, char **argv, server_config_t *conf)
{
    flag_entry_t flag_table[] = {
        {"-p", (int (*)(void *, int *, char **))handle_int, &conf->port},
        {"-x", (int (*)(void *, int *, char **))handle_int, &conf->width},
        {"-y", (int (*)(void *, int *, char **))handle_int, &conf->height},
        {"-c", (int (*)(void *, int *, char **))handle_int, &conf->clients_nb},
        {"-f", (int (*)(void *, int *, char **))handle_int, &conf->freq},
        {"-n", (int (*)(void *, int *, char **))handle_teams, NULL},
        {NULL, NULL, NULL}
    };

    for (int i = 1; i < argc; i++) {
        if (!strcmp_flag(argv, conf, &i, flag_table))
            return 84;
    }
    return 0;
}
