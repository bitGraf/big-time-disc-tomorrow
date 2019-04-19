#pragma once

#include "Entity.h"
#include "Input.h"

#include "ResourceManager.h"

#include "Panel.h"

struct CrawlerEnt : EntityBase {
    float angle = 0;
    float speed = 3;

    int attached = 0;
    bool loaded = false;
    bool grounded = false;
    Panel* panel1 = NULL;
    Panel* panel2 = NULL;
    Panel* currentPanel = NULL;
    EntityBase* p1 = NULL;
    EntityBase* p2 = NULL;
    float d1 = -1;
    float d2 = -1;
    float attachRadius = 2;

    vec3 localPos = {0, 0, 0};
    quat localOrientation;

    vec3 vel = {0, 10, 0};
    vec3 acc;
    vec3 impulse;


    void update(double dt) override;
    void preRender() override;
    void handleInput(int key, int scancode, int action, int mods) override;
};