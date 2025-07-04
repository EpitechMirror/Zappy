/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** AssetsManager
*/

#pragma once
#include "raylib.h"
#include "../Shaders/ShadersManager.hpp"
#include "../../map/Map.hpp"

#ifndef ASSETSMANAGER_HPP_
#define ASSETSMANAGER_HPP_

class AssetsManager {
public:
    AssetsManager(int mapWidth, int mapHeight);
    ~AssetsManager();

    void loadAllResources();
    void unloadAllResources();
    void applyShaders();

    Model floorModel;
    Model playerModel;
    Model deskModel;
    Model wallLong;
    Model wallShort;
    Font toyFont;
    Music mainMusic;
    Music loadingMusic;
    Texture2D wallpaperTexture;
    ShadersManager shaders;

    bool isModelLoaded(const std::string &modelPath) const;
    bool isTextureLoaded(const std::string &texturePath) const;
    bool isShaderLoaded(const std::string &shaderPath) const;
    bool isFontLoaded(const std::string &fontPath) const;
    bool isAudioLoaded(const std::string &audioPath) const;


    void loadFonts();
    void loadAudio();
    void loadModels();
    void loadTextures();
    void loadShaders();

private:
    int mapWidth;
    int mapHeight;

};

#endif /* !ASSETSMANAGER_HPP_ */
