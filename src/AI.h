#pragma once

#include "Entity.h"
#include "ResourceManager.h"
#include "Input.h"


struct AIEnt : EntityBase {
    // Init Vals
    vec3 velocity = { 0, 0, 0 };
    vec3 targetPosition;
    vec3 returnSpot;
    vec3* patrolPoints; //NULL terminated?
    quat rot180;
    quat targetOrientation;
    float distanceFromPlayer;
    float rate = 1.25;
    float timer = NULL;
    float speed = 212.5;
    float patrolTolerance = 2;
    // State
    int state = 0;
    int currentPatrolGoal = 0;


    void onCreate() override;
    void update(double dt) override;
    void onDestroy() override;

    void preRender();
    vec3 returnPoint(vec3, vec3);
    quat lookTowards(vec3 target, bool away = false);
};