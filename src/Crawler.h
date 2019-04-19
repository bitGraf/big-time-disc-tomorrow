#pragma once

#include "Entity.h"
#include "Input.h"

#include "ResourceManager.h"

#include "Panel.h"

struct CrawlerEnt : EntityBase {
    float angle = 0;
    float speed = 3;

    bool attached = false;
    bool loaded = false;
    Panel* panel = NULL;

    vec3 localPos = {0, 0, 0};
    vec3 vel = {0, 10, 0};
    vec3 acc;
    vec3 impulse;


    void update(double dt) override;
    void preRender() override;
    void handleInput(int key, int scancode, int action, int mods) override;
};