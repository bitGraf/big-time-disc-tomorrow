#pragma once

#include "Entity.h"
#include "InputBindings.h"
#include "Input.h"

#include "ResourceManager.h"

struct PlayerEnt : EntityBase {
    int angle = 0;
    float speed = 3;
    bool grounded = false;

    vec3 vel = {0, 10, 0};
    vec3 acc;
    vec3 impulse;


    void update(double dt) override;
    void preRender() override;
    void handleInput(int key, int scancode, int action, int mods) override;
};

struct StaticEnt : EntityBase {
    void update(double dt) override {
      EntityBase::update(dt);
    };
    void preRender() override {
      EntityBase::preRender();
    }
    void handleInput(int key, int scancode, int action, int mods) override {
      EntityBase::handleInput(key, scancode, action, mods);
    }
};
