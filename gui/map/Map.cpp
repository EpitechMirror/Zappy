/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Map
*/

#include "Map.hpp"

Map::Map() : _width(0), _height(0) {}

Map::~Map() {}

void Map::setSize(int width, int height) {
    std::cout << "Map::setSize called with " << width << "x" << height << std::endl;
    _width = width;
    _height = height;
}

int Map::getWidth() const {
    return _width;
}

int Map::getHeight() const {
    return _height;
}
