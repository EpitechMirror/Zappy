/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Map
*/

#ifndef MAP_HPP_
#define MAP_HPP_
#include <iostream>
#include <vector>
#include "../ressources/Ressources.hpp"
#include "../renderer/Player/Player.hpp"
#include "raylib.h"
#include <algorithm>

#pragma once

struct Egg {
    int id;
    int x;
    int y;
};

class Map {
    public:
        Map();
        ~Map();

        void setSize(int width, int height);
        int getWidth() const;
        int getHeight() const;

        void setTileResources(int x, int y, const Resources& resources);
        const Resources& getTileResources(int x, int y) const;

        bool isFullyInitialized() const;

        int getFoodCount() const;
        int getLinemateCount() const;
        int getDeraumereCount() const;
        int getSiburCount() const;
        int getMendianeCount() const;
        int getPhirasCount() const;
        int getThystameCount() const;

        int getEggsCount() const;
        void addEgg(int eggId, int x, int y);
        void removeEgg(int eggId);
        const std::vector<Egg> &getEggs() const;

        const std::vector<Player> &getPlayers() const;
        Player *getPlayerById(int id);
        void addPlayer(const Player &player);
        void removePlayerById(int id);
        void updatePlayerPosition(int id, Vector3 pos, int orientation);
        void updatePlayerLevel(int id, int level);
        void updatePlayerInventory(int id, const int inventory[RESOURCE_COUNT]);

    private:
        int _width;
        int _height;
        std::vector<Resources> _tiles;
        std::vector<Egg> _eggs;
        std::vector<Player> _players;
};

#endif /* !MAP_HPP_ */
