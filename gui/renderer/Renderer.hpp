#ifndef INCLUDED_RENDERER_HPP
    #define INCLUDED_RENDERER_HPP

#include <iostream>
#include "raylib.h"
#include "Camera/Camera.hpp"
#include "../map/Map.hpp"


class Renderer
{
    public:
        Renderer(int width, int height, const Map & map);
        void renderWindow();
        void gameLoop();

    private:
        int _screenWidth;
        int _screenHeight;
        const Map &_map;
        CameraController _cameraController;
};

#endif
