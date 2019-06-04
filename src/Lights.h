#ifndef _LIGHTS_H_
#define _LIGHTS_H_

#include "Vector.h"
#include "Quaternion.h"

typedef quat vec4;

struct PointLight {
    vec3 position;
    vec4 color;
    float strength;

    PointLight();
    PointLight(vec3, vec4, float);
    PointLight(vec3);
};

struct DirectionalLight {
    vec3 direction;
    vec4 color;
    float strength;

    DirectionalLight();
    DirectionalLight(vec3, vec4, float);
    DirectionalLight(vec3);
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec4 color;
    float strength;
    float cutoff;

    SpotLight();
    SpotLight(vec3, vec3, vec4, float, float);
};

#endif