/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Map
*/

#ifndef MAP_HPP_
#define MAP_HPP_
#pragma once

class Map {
    public:
        Map();
        ~Map();

        void setSize(int width, int height);
        int getWidth() const;
        int getHeight() const;

    private:
        int _width;
        int _height;
};

#endif /* !MAP_HPP_ */
