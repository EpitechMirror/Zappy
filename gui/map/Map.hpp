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
#include "../player/Player.hpp"
#include "raylib.h"
#include <algorithm>
#include <functional>

#pragma once

struct Egg {
    int id;
    int x;
    int y;
};

struct Incantation {
    int x;
    int y;
    int level;
    std::vector<int> playerIds;
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

        std::vector<Incantation> _activeIncantations;
        void startIncantation(int x, int y, int level, const std::vector<int> &players);
        void clearIncantationAt(int x, int y);
        const std::vector<Incantation>& getActiveIncantations() const;
        void addFallingEgg(int x, int y);

        void setFallingEggCallback(std::function<void(int, int)> callback) {
            _fallingEggCallback = callback;
        }

        void setGameOver(bool gameOver, std::string teamName) {
            _gameOver = gameOver;
            _winningTeam = teamName;
        }

        bool _gameOver = false;
        std::string _winningTeam;
        void setFrequencyTime(int frequency) {
            _frequency = frequency;
        }

        int getFrequency() const {
            return _frequency;
        }

    private:
        int _frequency = 0;
        int _width;
        int _height;
        std::vector<Resources> _tiles;
        std::vector<Egg> _eggs;
        std::vector<Player> _players;
        std::function<void(int, int)> _fallingEggCallback;
};

#endif /* !MAP_HPP_ */
