/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Player
*/

#include "Player.hpp"

std::vector<std::string> Player::_teamNames;

Player::Player(int id, Vector3 pos, int orientation, int level, const std::string& team)
    : _id(id), _position(pos), _orientation(orientation), _level(level), _team(team) {}


void Player::addTeamName(const std::string &name) {
    _teamNames.push_back(name);
}

const std::vector<std::string> &Player::getTeamNames() {
    return _teamNames;
}

int Player::getId() const {
    return _id;
}

Vector3 Player::getPosition() const {
    return _position;
}

int Player::getOrientation() const {
    return _orientation;
}

int Player::getLevel() const {
    return _level;
}

const std::string& Player::getTeam() const {
    return _team;
}

void Player::setPosition(Vector3 pos) {
    _position = pos;
}

void Player::setOrientation(int orientation) {
    _orientation = orientation;
}

void Player::setLevel(int level) {
    _level = level;
}

const int *Player::getInventory() const {
    return _inventory;
}

void Player::setInventory(const int inventory[RESOURCE_COUNT]) {
    for (int i = 0; i < RESOURCE_COUNT; ++i)
        _inventory[i] = inventory[i];
}
