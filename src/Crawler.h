#pragma once

#include "Entity.h"
#include "Input.h"

#include "ResourceManager.h"

#include "Panel.h"

struct CrawlerEnt : EntityBase {
    float speed = 3.5f;
    float attachRadius = 2.5f;

    // States
    bool attached = false;
    bool grounded = false;

    PanelEnt* currentPanel = NULL;
    PanelEnt* closestOtherPanel = NULL;

    //Relative to the current panel
    vec3 localPos;
    quat localOrientation;

    vec3 vel;
    vec3 acc;

    // These shouldn't be stored in this class
    PanelEnt** allPanels = NULL;
    int numPanels = 0;
    //EntityBase* p1 = NULL;
    //EntityBase* p2 = NULL;


    void update(double dt) override;
    void preRender() override;
    void handleInput(int key, int scancode, int action, int mods) override;
    void onCreate() override;
    void transitionToPanel(PanelEnt*);
    PanelEnt* getClosestPanel(PanelEnt*);
};