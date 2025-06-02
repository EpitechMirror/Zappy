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

#pragma once

class Map {
    public:
        Map();
        ~Map();

        void setSize(int width, int height);
        int getWidth() const;
        int getHeight() const;

        int getFoodCount() const { return 0; }
        int getLinemateCount() const { return 0; }
        int getDeraumereCount() const { return 0; }
        int getSiburCount() const { return 0; }
        int getMendianeCount() const { return 0; }
        int getPhirasCount() const { return 0; }
        int getThystameCount() const { return 0; }

        bool isFullyInitialized() const;

    private:
        int _width;
        int _height;
        std::vector<std::vector<int>> _tiles;
};

#endif /* !MAP_HPP_ */
