#ifndef INCLUDED_RENDERER_HPP
    #define INCLUDED_RENDERER_HPP

#include <iostream>
#include "raylib.h"
#include "Camera/Camera.hpp"
#include "../map/Map.hpp"
#include "../client/Client.hpp"
#include "../renderer/Shaders/ShadersManager.hpp"
#include "../renderer/Player/Player.hpp"
#include "../renderer/Light/Light.hpp"


class Renderer
{
    public:
        Renderer(int width, int height, const Map & map);
        void renderWindow(Client &client);
        void gameLoop(Client &client);
        void InfoBoard();
        void DrawGrid();
        void drawItems();
        void loadShaders();
        void unloadShaders();
        void loadModels();
        void loadTextures();
        void RenderPlayers();
        void applyShaders();
        void initLights();
        void unloadModels();
        void drawFloor();

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
};

#endif
