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
        Vector3 _target;       // point fixe : centre de la map

        float   _angleDeg;     // angle actuel autour de Y (degrés)
        float   _distance;     // distance actuelle au target
        float   _height;       // hauteur fixe (Y) de la caméra

        // paramètres de vitesse
        const float _angleSpeed = 60.0f;  // °/s
        const float _zoomSpeed  =  5.0f;  // unités/s

        // limites de zoom
        const float _minDist =  2.0f;
        const float _maxDist = 50.0f;

    public:
        CameraController();

        CameraController(float mapWidth, float mapDepth);
        void update();
        Camera& getCamera();
};

#endif
