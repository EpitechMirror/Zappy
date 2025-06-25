/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Camera
*/

#include "Camera.hpp"

CameraController::CameraController(float mapWidth, float mapDepth)
  : _angleDeg(45.0f), _distance(15.0f), _height(10.0f)
{
    _target = { mapWidth * 0.5f, 0.0f, mapDepth * 0.5f };

    _camera.target     = _target;
    _camera.up         = { 0.0f, 1.0f, 0.0f };
    _camera.fovy       = 45.0f;
    _camera.projection = CAMERA_PERSPECTIVE;

    float rad = _angleDeg * DEG2RAD;
    _camera.position.x = _target.x + cosf(rad) * _distance;
    _camera.position.z = _target.z + sinf(rad) * _distance;
    _camera.position.y = _height;
}

void CameraController::update() {
    float dt = GetFrameTime();

    // Tourner autour du centre : Q/A ←, D →
    if (IsKeyDown(KEY_Q) || IsKeyDown(KEY_A)) _angleDeg -= _angleSpeed * dt;
    if (IsKeyDown(KEY_D))                     _angleDeg += _angleSpeed * dt;

    // Zoom avant/arrière : Z/W ←, S →
    if (IsKeyDown(KEY_Z) || IsKeyDown(KEY_W)) _distance -= _zoomSpeed * dt;
    if (IsKeyDown(KEY_S))                     _distance += _zoomSpeed * dt;

    if (_distance < _minDist) _distance = _minDist;
    if (_distance > _maxDist) _distance = _maxDist;

    float rad = _angleDeg * DEG2RAD;
    _camera.position.x = _target.x + cosf(rad) * _distance;
    _camera.position.z = _target.z + sinf(rad) * _distance;
    _camera.position.y = _height;

    _camera.target = _target;
}

Camera& CameraController::getCamera() {
    return _camera;
}
