/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** flag
*/

#ifndef FLAG_H_
    #define FLAG_H_

typedef struct server_config_s {
    int port;
    int width;
    int height;
    int clients_nb;
    int freq;
    int team_count;
    char **team_names;
} server_config_t;

typedef struct {
    char *flag;
    int (*handler)(void *, int *, char **);
    void *field;
} flag_entry_t;

int parse(int argc, char **argv, server_config_t *conf);
int handle_int(int *field, int *i, char **argv);
int handle_teams(server_config_t *conf, int *i, char **argv);


#endif /* !FLAG_H_ */
