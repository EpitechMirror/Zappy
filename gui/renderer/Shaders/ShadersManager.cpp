/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** ShadersManager, gère la création et le chargement de textures
*/

#include "ShadersManager.hpp"

ShadersManager::~ShadersManager() {
    unloadAll();
}

void ShadersManager::unloadAll() {
    for (auto& [_, shader] : _shaders) {
        if (shader.id != 0)
            UnloadShader(shader);
    }
    _shaders.clear();
}

void ShadersManager::loadPBR() {
    Shader shader = LoadShader("../../resources/shaders/pbr.vs", "../../resources/shaders/pbr.fs");

    if (shader.id == 0) {
        std::cerr << "Failed to load PBR shader" << std::endl;
        return;
    }

    _shaders["pbr"] = shader;
}

Shader& ShadersManager::getPBR() {
    if (_shaders.find("pbr") == _shaders.end())
        throw std::runtime_error("PBR shader not loaded");
    return _shaders.at("pbr");
}
