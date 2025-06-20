/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Map
*/

#include "Map.hpp"

Map::Map() : _width(0), _height(0) {}

Map::~Map() {}

int Map::getWidth() const {
    return _width;
}

int Map::getHeight() const {
    return _height;
}

bool Map::isFullyInitialized() const {
    return static_cast<int>(_tiles.size()) == _width * _height;
}

void Map::setSize(int width, int height) {
    _width = width;
    _height = height;
    _tiles.resize(width * height);
}

void Map::setTileResources(int x, int y, const Resources& resources) {
    if (x < 0 || x >= _width || y < 0 || y >= _height)
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

int Map::getEggsCount() const {
    return static_cast<int>(_eggs.size());
}

void Map::addEgg(int eggId, int x, int y) {
    _eggs.push_back({eggId, x, y});
}

void Map::removeEgg(int eggId) {
    auto it = std::find_if(_eggs.begin(), _eggs.end(), [eggId](const Egg &egg) {
        return egg.id == eggId;
    });

    if (it != _eggs.end()) {
        _eggs.erase(it);
    } else {
        std::cerr << "Warning: Tried to remove non-existent egg #" << eggId << "\n";
    }
}

const std::vector<Egg> &Map::getEggs() const {
    return _eggs;
}

Player *Map::getPlayerById(int id) {
    for (auto &player : _players) {
        if (player.getId() == id)
            return &player;
    }
    return nullptr;
}

void Map::addPlayer(const Player &player) {
    _players.push_back(player);
}

void Map::removePlayerById(int id) {
    _players.erase(std::remove_if(_players.begin(), _players.end(),
        [id](const Player &p) { return p.getId() == id; }), _players.end());
}

void Map::updatePlayerPosition(int id, Vector3 pos, int orientation) {
    Player *p = getPlayerById(id);
    if (p) {
        p->setPosition(pos);
        p->setOrientation(orientation);
    }
}

void Map::updatePlayerLevel(int id, int level) {
    Player *p = getPlayerById(id);
    if (p) {
        p->setLevel(level);
    }
}

void Map::updatePlayerInventory(int id, const int inventory[RESOURCE_COUNT]) {
    Player *p = getPlayerById(id);
    if (p) {
        p->setInventory(inventory);
    }
}

const std::vector<Player>& Map::getPlayers() const {
    return _players;
}
