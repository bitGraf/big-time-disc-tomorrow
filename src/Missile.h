#pragma once
#include "Physics.h"

struct MissileEnt : public PhysicsEnt {
    float thrust = 2200;
    float timeOfFlight = 0;
    float separationTime = 3.5f;
    float detonateTime = 35;

    vec3 target;

    void update(double dt) override;
    void onCreate() override;
};