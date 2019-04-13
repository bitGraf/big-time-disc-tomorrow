#pragma once

#include "Entity.h"
#include "InputBindings.h"
#include "Input.h"

struct PlayerEnt : EntityBase {
    int angle = 0;
    float speed = 3;
    bool grounded = false;

    vec3 vel = {0, 10, 0};
    vec3 acc;
    vec3 impulse;

    void handleInput(int key, int scancode, int action, int mods);

    void update(double dt);
};

struct StaticEnt : EntityBase {
    float value2 = 69.0f;

    void update() {
        
    }
};
