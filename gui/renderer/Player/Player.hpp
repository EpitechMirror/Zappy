/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Player
*/

#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include "raylib.h"
#include <string>
#include <vector>

class Player {
    public:
        Player(int id, Vector3 pos, int orientation, int level, const std::string& team);
        static void addTeamName(const std::string &name);
        static const std::vector<std::string> &getTeamNames();

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
        static std::vector<std::string> _teamNames;
};

#endif /* !PLAYER_HPP_ */
