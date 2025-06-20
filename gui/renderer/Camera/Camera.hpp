#ifndef INCLUDED_CAMERA_HPP
    #define INCLUDED_CAMERA_HPP

#include <iostream>
#include "raylib.h"
#include "raymath.h"
#include <cmath>

class CameraController
{
    private:
        Camera _camera;

        const float _angleSpeed = 60.0f;  // °/s
        const float _zoomSpeed  =  5.0f;  // unités/s

        const float _minDist =  2.0f;
        const float _maxDist = 50.0f;

    public:
        CameraController();

        CameraController(float mapWidth, float mapDepth);
        void update();
        Camera& getCamera();

        float _angleDeg;
        Vector3 _target;
        float _distance;
        float _height;
};

#endif
