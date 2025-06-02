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

/* void Map::setTile(int x, int y, const Tile& tile) {
    if (x < 0 || y < 0 || x >= _width || y >= _height)
        return;
    _tiles[y * _width + x] = tile;
}
 */