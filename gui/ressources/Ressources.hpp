/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Ressources
*/

#ifndef RESSOURCES_HPP_
    #define RESSOURCES_HPP_

#pragma once

enum ResourceType {
    FOOD = 0,
    LINEMATE,
    DERAUMERE,
    SIBUR,
    MENDIANE,
    PHIRAS,
    THYSTAME,
    EGG,
    RESOURCE_COUNT
};

struct Resources {
    int quantities[RESOURCE_COUNT] = {0};
};

#endif /* !RESSOURCES_HPP_ */
