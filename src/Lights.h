#ifndef _LIGHTS_H_
#define _LIGHTS_H_

#include "Vector.h"
#include "Quaternion.h"

typedef quat Color;

struct PointLight {
    vec3 position;
    Color color;
    float strength;

    PointLight();
    PointLight(vec3, Color, double);
    PointLight(vec3);
};

struct DirectionalLight {
    vec3 direction;
    Color color;
    float strength;

    DirectionalLight();
    DirectionalLight(vec3, Color, double);
    DirectionalLight(vec3);
};

#endif