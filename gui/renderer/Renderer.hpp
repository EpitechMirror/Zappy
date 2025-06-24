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
#include <optional>
#include "raylib.h"
#include "rlgl.h"
#include "Camera/Camera.hpp"
#include "../map/Map.hpp"
#include "../client/Client.hpp"
#include "../renderer/Shaders/ShadersManager.hpp"
#include "../renderer/Player/Player.hpp"
#include "../renderer/Light/Light.hpp"
#include "../renderer/Player/Player.hpp"
#include <cmath>

class Renderer
{
    public:
        Renderer(int width, int height, const Map & map);
        void renderWindow(Client &client);
        void gameLoop(Client &client);
        void InfoItemsBoard();
        void InfoTeamsBoard();
        void InfoPlayersBoard();
        void InfoBoxBoard();
        void DrawGrid();
        void drawItems();
        void DrawEggs();
        void loadShaders();
        void unloadShaders();
        void loadModels();
        void loadTextures();
        void loadAudio();
        void unloadAudio();
        void unloadTextures();
        void DrawPlayers();
        void applyShaders();
        void initLights();
        void unloadModels();
        void drawFloor();
        void drawRoomAndy();
        void handleMouseClick();
        bool GetRayGroundIntersection(Ray ray, Vector3 &outPoint);
        void showLoadingScreen(const std::string &message);
        Color getColorForResource(ResourceType type);

        const std::vector<Player>& getPlayers() const { return _players; }

    private:
        Music _mainMusic;
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
        Model _wallLong;
        Model _wallShort;
        std::vector<std::string> _loadingTips = {
            "Use ZQSD to move around and get a better overview ! ",
            "Click on a box to find out more about its contents ! ",
            "Click on a player to find out more about their inventory ! ",
            "Nice is a beautiful city, isn't it ? ",
            "Did you know? Eggs hatch into players ! ",
            "Legend says no one ever reached level 8... Yet. "
        };
        Font _toyFont;
        std::optional<Vector2> _selectedTile;    
        std::optional<int>_selectedPlayerId;
};
#endif
