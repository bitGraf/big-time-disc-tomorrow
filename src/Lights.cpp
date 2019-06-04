#include "Lights.h"

PointLight::PointLight() {
    position = { 0,0,0 };
    color = { 1,1,1,1 };
    strength = 1;
}
PointLight::PointLight(vec3 v) : position(v) {
    color = { 1,1,1,1 };
    strength = 1;
}
PointLight::PointLight(vec3 v, vec4 col, float str) : position(v), color(col), strength(str) {}

DirectionalLight::DirectionalLight() {
    direction = { 0,0,0 };
    color = { 1,1,1,1 };
    strength = 1;
}
DirectionalLight::DirectionalLight(vec3 v) : direction(v) {
    color = { 1,1,1,1 };
    strength = 1;
}
DirectionalLight::DirectionalLight(vec3 v, vec4 col, float str) : direction(v), color(col), strength(str) {}

SpotLight::SpotLight() {
    position = { 0,0,0 };
    direction = { 0,0,0 };
    color = { 0,0,0,0 };
    strength= 0;
    cutoff = 0;
}
SpotLight::SpotLight(vec3 p, vec3 d, vec4 col, float str, float ang) : 
        position(p), direction(d), color(col), strength(str), cutoff(ang) {}