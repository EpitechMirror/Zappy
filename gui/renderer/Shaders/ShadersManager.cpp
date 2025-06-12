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
    for (auto& [_, shader] : _shaders) UnloadShader(shader);
    _shaders.clear();
}

void ShadersManager::loadPBR() {
    _shaders["pbr"] = LoadShader("../../resources/shaders/pbr.vs", "../../resources/shaders/pbr.fs");
}
Shader& ShadersManager::getPBR() {
    return _shaders.at("pbr");
}
