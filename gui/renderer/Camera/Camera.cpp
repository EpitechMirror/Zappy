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

void CameraController::zoom(float delta)
{
    Vector3 dir = Vector3Subtract(_camera.target, _camera.position);
    dir = Vector3Normalize(dir);
    _camera.position = Vector3Add(_camera.position, Vector3Scale(dir, delta));
}

Camera& CameraController::getCamera() {
    return _camera;
}
