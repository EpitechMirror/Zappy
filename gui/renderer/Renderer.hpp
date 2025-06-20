/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Renderer
*/

#ifndef INCLUDED_RENDERER_HPP
    #define INCLUDED_RENDERER_HPP

#include <iostream>
#include <random>
#include "raylib.h"
#include "Camera/Camera.hpp"
#include "../map/Map.hpp"
#include "../client/Client.hpp"
#include "../renderer/Shaders/ShadersManager.hpp"
#include "../renderer/Player/Player.hpp"
#include "../renderer/Light/Light.hpp"
#include "../renderer/Player/Player.hpp"

class Renderer
{
    public:
        Renderer(int width, int height, const Map & map);
        void renderWindow(Client &client);
        void gameLoop(Client &client);
        void InfoItemsBoard();
        void InfoTeamsBoard();
        void InfoPlayersBoard();
        void DrawGrid();
        void drawItems();
        void DrawEggs();
        void loadShaders();
        void unloadShaders();
        void loadModels();
        void loadTextures();
        void unloadTextures();
        void DrawPlayers();
        void applyShaders();
        void initLights();
        void unloadModels();
        void drawFloor();
        void showLoadingScreen(const std::string &message);
        Color getColorForResource(ResourceType type);

        const std::vector<Player>& getPlayers() const { return _players; }

    private:
        int _screenWidth;
        int _screenHeight;
        const Map &_map;
        CameraController _cameraController;
        std::vector<Player> _players;
        std::vector<Light> _lights;
        Model _floorModel;
        Model _playerModel;
        ShadersManager _shaders;
        bool _mapInitialized = false;
};

#endif
