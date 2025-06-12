#ifndef INCLUDED_CAMERA_HPP
    #define INCLUDED_CAMERA_HPP

#include <iostream>
#include "raylib.h"
#include "raymath.h"

class CameraController
{
    private:
        Camera _camera;
    protected:
    public:
        CameraController();
        void update();
        Camera& getCamera();
        void zoom(float delta);
};

#endif
