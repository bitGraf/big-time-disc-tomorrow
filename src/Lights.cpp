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
PointLight::PointLight(vec3 v, Color col, double str) : position(v), color(col), strength(str) {}

DirectionalLight::DirectionalLight() {
    direction = { 0,0,0 };
    color = { 1,1,1,1 };
    strength = 1;
}
DirectionalLight::DirectionalLight(vec3 v) : direction(v) {
    color = { 1,1,1,1 };
    strength = 1;
}
DirectionalLight::DirectionalLight(vec3 v, Color col, double str) : direction(v), color(col), strength(str) {}