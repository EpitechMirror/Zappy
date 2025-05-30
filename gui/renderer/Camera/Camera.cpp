/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Client
*/

#include "Camera.hpp"

CameraController::CameraController()
{
    _camera.position = {0.0f, 10.0f, 10.0f};
    _camera.target = {0.0f, 0.0f, 0.0f};
    _camera.up = {0.0f, 1.0f, 0.0f};
    _camera.fovy = 45.0f;
    _camera.projection = CAMERA_PERSPECTIVE;
}

void CameraController::update() {
    UpdateCamera(&_camera, CAMERA_FIRST_PERSON);
}

Camera& CameraController::getCamera() {
    return _camera;
}
