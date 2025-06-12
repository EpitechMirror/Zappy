/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Player
*/

#include "raylib.h"
#include <string>

class Player {
public:
    Player(int id, Vector3 pos, int orientation, int level, const std::string& team);
    int getId() const;
    Vector3 getPosition() const;
    int getOrientation() const;
    int getLevel() const;
    const std::string& getTeam() const;
    void setPosition(Vector3 pos);
    void setOrientation(int orientation);
    void setLevel(int level);
private:
    int _id;
    Vector3 _position;
    int _orientation;
    int _level;
    std::string _team;
};