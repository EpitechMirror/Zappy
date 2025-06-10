/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Player
*/

#include "Player.hpp"

Player::Player(int id, Vector3 pos, int orientation, int level, const std::string& team)
    : _id(id), _position(pos), _orientation(orientation), _level(level), _team(team) {}

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