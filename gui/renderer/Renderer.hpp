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
#include "../map/Map.hpp"
#include "Camera/Camera.hpp"
#include "../client/Client.hpp"
#include "../renderer/Shaders/ShadersManager.hpp"
#include "../player/Player.hpp"
#include "../renderer/Light/Light.hpp"
#include <cmath>
#include "AssetsManager/AssetsManager.hpp"

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
        void DrawPlayers();
        void initLights();
        // void drawFloor();
        void drawRoomAndy();        
        void handleMouseClick();
        bool GetRayGroundIntersection(Ray ray, Vector3 &outPoint);
        void showLoadingScreen(const std::string &message);
        Color getColorForResource(ResourceType type);

        float getDesktopY();

        const std::vector<Player>& getPlayers() const { return _players; }

        void handleServerDisconnect();

    private:
        AssetsManager _assets;
        int _screenWidth;
        int _screenHeight;
        const Map &_map;
        CameraController _cameraController;
        std::vector<Player> _players;
        std::vector<Light> _lights;
        bool _mapInitialized = false;
        std::vector<std::string> _loadingTips = {
            "Use ZQSD to move around and get a better overview ! ",
            "Click on a box to find out more about its contents ! ",
            "Click on a player to find out more about their inventory ! ",
            "Nice is a beautiful city, isn't it ? ",
            "Did you know? Eggs hatch into players ! ",
            "Legend says no one ever reached level 8... Yet. "
        };
        std::optional<Vector2> _selectedTile;    
        std::optional<int>_selectedPlayerId;
        static bool firstCall;
        static bool disconnected;
        float _disconnectTimer;
        void notifyServerDisconnect();
};
#endif
