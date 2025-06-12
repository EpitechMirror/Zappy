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
    _width = width;
    _height = height;
    _tiles.resize(_width * _height); 
}

int Map::getWidth() const {
    return _width;
}

int Map::getHeight() const {
    return _height;
}

bool Map::isFullyInitialized() const {
    return static_cast<int>(_tiles.size()) == _width * _height;
}

void Map::setTileResources(int x, int y, const Resources& resources) {
    if (x < 0 || y < 0 || x >= _width || y >= _height)
        return;
    _tiles[y * _width + x] = resources;
}

const Resources& Map::getTileResources(int x, int y) const {
    static Resources dummy;
    if (x < 0 || y < 0 || x >= _width || y >= _height)
        return dummy;
    return _tiles[y * _width + x];
}

int Map::getFoodCount() const {
    int count = 0;
    for (const auto& resources : _tiles) {
        count += resources.quantities[FOOD];
    }
    return count;
}

int Map::getLinemateCount() const {
    int count = 0;
    for (const auto& resources : _tiles) {
        count += resources.quantities[LINEMATE];
    }
    return count;
}

int Map::getDeraumereCount() const {
    int count = 0;
    for (const auto& resources : _tiles) {
        count += resources.quantities[DERAUMERE];
    }
    return count;
}

int Map::getSiburCount() const {
    int count = 0;
    for (const auto& resources : _tiles) {
        count += resources.quantities[SIBUR];
    }
    return count;
}

int Map::getMendianeCount() const {
    int count = 0;
    for (const auto& resources : _tiles) {
        count += resources.quantities[MENDIANE];
    }
    return count;
}

int Map::getPhirasCount() const {
    int count = 0;
    for (const auto& resources : _tiles) {
        count += resources.quantities[PHIRAS];
    }
    return count;
}

int Map::getThystameCount() const {
    int count = 0;
    for (const auto& resources : _tiles) {
        count += resources.quantities[THYSTAME];
    }
    return count;
}
