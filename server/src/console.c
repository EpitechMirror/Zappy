/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** main
*/

/* Attention: compiler avec le flag -pthread */

#include "console.h"

#include <stdio.h>
#include <time.h>
#include <pthread.h>

static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

static const char *level_to_string(log_level_t level)
{
    switch (level) {
        case LOG_LEVEL_INFO:
            return"\033[32mINFO\033[0m";
        case LOG_LEVEL_WARNING:
            return "\033[33mWARN\033[0m";
        case LOG_LEVEL_ERROR:
            return "\033[31mERROR\033[0m";
        case LOG_LEVEL_DEBUG:
            return "\033[36mDEBUG\033[0m";
        default:
            return "\033[35mUNKNOWN\033[0m";
    }
}

static void console_log(const char* message, log_level_t level)
{
    time_t now;
    struct tm *local_time;

    time(&now);
    local_time = localtime(&now);
    pthread_mutex_lock(&log_mutex);
        printf("\033[1m[%02d:%02d:%02d] %s\033[0m: %s\n",
            local_time->tm_hour,
            local_time->tm_min,
            local_time->tm_sec,
            level_to_string(level),
            message);
    pthread_mutex_unlock(&log_mutex);
}

void console_info(const char *message)
{
    console_log(message, LOG_LEVEL_INFO);
}

void console_warning(const char *message)
{
    console_log(message, LOG_LEVEL_WARNING);
}

void console_error(const char *message)
{
    console_log(message, LOG_LEVEL_ERROR);
}

void console_debug(const char *message)
{
    console_log(message, LOG_LEVEL_DEBUG);
}
