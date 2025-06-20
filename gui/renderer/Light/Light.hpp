/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Light
*/

#include "raylib.h"

enum LightType { LIGHT_DIRECTIONAL = 0, LIGHT_POINT, LIGHT_SPOT };

class Light {
public:
    Light(LightType type, Vector3 pos, Vector3 target, Color color, float intensity, Shader& shader, int idx);
    void updateShader(Shader& shader);
    void setEnabled(bool enabled);
    Vector3 getPosition() const;
private:
    LightType _type;
    bool _enabled;
    Vector3 _position;
    Vector3 _target;
    float _color[4];
    float _intensity;
    // Shader locations
    int _typeLoc, _enabledLoc, _positionLoc, _targetLoc, _colorLoc, _intensityLoc;
    int _index;
};
