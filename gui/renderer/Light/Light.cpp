/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Light
*/

#include "Light.hpp"

Light::Light(LightType type, Vector3 pos, Vector3 target, Color color, float intensity, Shader& shader, int idx)
    : _type(type), _enabled(true), _position(pos), _target(target), _intensity(intensity), _index(idx)
{
    _color[0] = color.r / 255.0f;
    _color[1] = color.g / 255.0f;
    _color[2] = color.b / 255.0f;
    _color[3] = color.a / 255.0f;
    // Récupère les locations dans le shader
    _enabledLoc = GetShaderLocation(shader, TextFormat("lights[%i].enabled", idx));
    _typeLoc = GetShaderLocation(shader, TextFormat("lights[%i].type", idx));
    _positionLoc = GetShaderLocation(shader, TextFormat("lights[%i].position", idx));
    _targetLoc = GetShaderLocation(shader, TextFormat("lights[%i].target", idx));
    _colorLoc = GetShaderLocation(shader, TextFormat("lights[%i].color", idx));
    _intensityLoc = GetShaderLocation(shader, TextFormat("lights[%i].intensity", idx));
    updateShader(shader);
}

void Light::updateShader(Shader& shader) {
    SetShaderValue(shader, _enabledLoc, &_enabled, SHADER_UNIFORM_INT);
    SetShaderValue(shader, _typeLoc, &_type, SHADER_UNIFORM_INT);
    float pos[3] = {_position.x, _position.y, _position.z};
    SetShaderValue(shader, _positionLoc, pos, SHADER_UNIFORM_VEC3);
    float tgt[3] = {_target.x, _target.y, _target.z};
    SetShaderValue(shader, _targetLoc, tgt, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, _colorLoc, _color, SHADER_UNIFORM_VEC4);
    SetShaderValue(shader, _intensityLoc, &_intensity, SHADER_UNIFORM_FLOAT);
}

Vector3 Light::getPosition() const {
    return _position;
}

void Light::setEnabled(bool enabled) { _enabled = enabled; }
