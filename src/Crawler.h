#pragma once

#include "Entity.h"
#include "Input.h"

#include "ResourceManager.h"

#include "Panel.h"

struct CrawlerEnt : EntityBase {
    float max_speed = 3.5f;
    float move_acc  = 9.81f;//1 G
    float mass = 10;
    float F, K;

    float attachRadius = 2.5f;
    float autoAttachRadius = 0.5f;
    float attachCooldown = 0.0f;

    // States
    bool attached = false;
    bool grounded = false;

    PanelEnt* currentPanel = NULL;
    PanelEnt* closestOtherPanel = NULL;

    //Relative to the current panel
    vec3 localPos;
    quat localOrientation;

    vec3 velocity;
    vec3 acceleration;

    // These shouldn't be stored in this class
    PanelEnt** allPanels = NULL;
    int numPanels = 0;


    void update(double dt) override;
    void preRender() override;
    void handleInput(int key, int scancode, int action, int mods) override;
    void onCreate() override;
    void transitionToPanel(PanelEnt*);
    PanelEnt* getClosestPanel(PanelEnt*);
};