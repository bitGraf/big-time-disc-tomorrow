#pragma once
#include "Entity.h"

struct PhysicsEnt : public EntityBase {
    float mass = 1;
    float I1 = 1, I2 = 1, I3 = 1;

    vec3 acceleration;
    vec3 velocity;

    vec3 appliedForce;

    vec3 angularAcceleration;
    vec3 angularVelocity;

    bool applyGravity = true;

    void update(double dt) override;
    void postRender() override;
    void handleInput(int key, int scancode, int action, int mods) override;
    void onCreate() override;
};