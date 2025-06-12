/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Client
*/

#ifndef INCLUDED_SHADERS_HPP
    #define INCLUDED_SHADERS_HPP

#include <iostream>
#include "raylib.h"
#include <string>
#include <unordered_map>

class ShadersManager
{
    private:
        std::unordered_map<std::string, Shader> _shaders;
    protected:
    public:
        ShadersManager()  = default;
        ~ShadersManager();
        void loadPBR();
        //void load(const std::string& name, const std::string& vsPath, const std::string& fsPath);
        Shader& getPBR();
        void unloadAll();
};

#endif
