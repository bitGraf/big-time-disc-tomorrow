#pragma once

#include "Entity.h"
#include "InputBindings.h"

struct PlayerEnt : EntityBase {
    int angle = 0;
    float speed = 8;
    vec3 vel;

    void handleInput(int key, int scancode, int action, int mods);

    void update(double dt);
};

struct StaticEnt : EntityBase {
    float value2 = 69.0f;

    void update() {
        
    }
};
