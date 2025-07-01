/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** main
*/

#ifndef CONSOLE_H_
    #define CONSOLE_H_

typedef enum {
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_DEBUG,
} log_level_t;

void console_info(const char *message);
void console_warning(const char *message);
void console_error(const char *message);
void console_debug(const char *message);

#endif /* !CONSOLE_H_ */
