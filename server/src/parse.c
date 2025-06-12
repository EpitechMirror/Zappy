/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** parse
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "flag.h"

static int is_number(const char *str)
{
    int i = 0;

    if (!str)
        return 0;
    if (*str == '\0')
        return 0;
    if (str[0] == '-')
        return 0;
    for (i = 0; str[i]; i++) {
        if (!isdigit(str[i]))
            return 0;
    }
    return 1;
}

int handle_int(int *field, int *i, char **argv)
{
    (*i)++;
    if (!argv[*i])
        return 84;
    if (!is_number(argv[*i])) {
        fprintf(stderr, "Invalid value for %s: %s\n",
            argv[*i - 1], argv[*i]);
        return 84;
    }
    *field = atoi(argv[*i]);
    if (*field <= 0) {
        fprintf(stderr, "Value for %s must be positive: %d\n",
            argv[*i - 1], *field);
        return 84;
    }
    return 0;
}

static int fill_team_names(server_config_t *conf,
    int start, int count, char **argv)
{
    int j = 0;

    conf->team_names = malloc(sizeof(char *) * (count + 1));
    if (!conf->team_names)
        return 84;
    for (j = 0; j < count; j++) {
        conf->team_names[j] = strdup(argv[start + j]);
    }
    conf->team_names[count] = NULL;
    return 0;
}

int handle_teams(server_config_t *conf, int *i, char **argv)
{
    int start = 0;
    int count = 0;
    int ret = 0;

    (*i)++;
    start = *i;
    while (argv[*i] && argv[*i][0] != '-') {
        count++;
        (*i)++;
    }
    conf->team_count = count;
    ret = fill_team_names(conf, start, count, argv);
    (*i)--;
    return ret;
}

static int call_flag_handler(flag_entry_t *entry,
    server_config_t *conf, int *i, char **argv)
{
    if (entry->field)
        return entry->handler(entry->field, i, argv);
    return entry->handler(conf, i, argv);
}

static int handle_flag_match(flag_entry_t *entry, server_config_t *conf,
    int *i, char **argv)
{
    int ret = call_flag_handler(entry, conf, i, argv);

    if (ret == -1 || ret == 84)
        return -1;
    return 1;
}

static int strcmp_flag(char **argv, server_config_t *conf,
    int *i, flag_entry_t *flag_table)
{
    int j = 0;
    int ret = 0;

    for (j = 0; flag_table[j].flag; j++) {
        if (strcmp(argv[*i], flag_table[j].flag) == 0) {
            ret = handle_flag_match(&flag_table[j], conf, i, argv);
            return ret;
        }
    }
    return 0;
}

static void print_server_config(server_config_t *conf)
{
    int i = 0;

    printf("===============Zappy Server===============\n");
    printf("Port: %d\n", conf->port);
    printf("Width: %d\n", conf->width);
    printf("Height: %d\n", conf->height);
    printf("Clients per team: %d\n", conf->clients_nb);
    printf("Frequency: %d\n", conf->freq);
    printf("Teams: ");
    for (i = 0; i < conf->team_count; i++) {
        printf("%s ", conf->team_names[i]);
    }
    printf("\n");
    printf("==========================================\n");
}

int parse_flags(int argc, char **argv, server_config_t *conf,
    flag_entry_t *flag_table)
{
    int i = 0;
    int ret = 0;

    for (i = 1; i < argc; i++) {
        ret = strcmp_flag(argv, conf, &i, flag_table);
        if (ret == -1)
            return 84;
        if (ret == 0)
            return 84;
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
    int ret = parse_flags(argc, argv, conf, flag_table);

    if (ret != 0)
        return 84;
    print_server_config(conf);
    return 0;
}
