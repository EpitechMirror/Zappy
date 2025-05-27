#ifndef INCLUDED_RENDERER_HPP
    #define INCLUDED_RENDERER_HPP

#include <iostream>
#include "raylib.h"
#include "Camera/Camera.hpp"


class Renderer
{
    private:
        int _screenWidth;
        int _screenHeight;
        CameraController _cameraController;
    protected:
    public:
        Renderer(int width, int height);
        void renderWindow();
        void gameLoop();
};

#endif
