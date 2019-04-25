#include "Physics.h"

void PhysicsEnt::update(double dt) {
    vec3 gravForce = {0, applyGravity ? -9.81f*mass : 0.0f};

    vec3 totalForce = gravForce + appliedForce;

    acceleration = totalForce* (1/mass);
    velocity = velocity + acceleration * dt;
    position = position + velocity * dt;

    if (position.y < 0)
        position.y = 0;

    EntityBase::update(dt);
}

void PhysicsEnt::postRender() {

}

void PhysicsEnt::handleInput(int ket, int scancode, int action, int mods) {

}

void PhysicsEnt::onCreate() {
    
}