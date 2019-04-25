#include "Missile.h"

void MissileEnt::update(double dt) {
    timeOfFlight += dt;

    if (timeOfFlight < separationTime) {
        appliedForce = {0, thrust, 0};
    } else {
        appliedForce = {0, 0, 0};
        orientation = Quaternion::lookAt(position, target);
        vec3 toTarg = (target - position);
        if (Vector::magnitude(toTarg) < 0.1f)
            Remove = true;
        appliedForce = Vector::normalized(toTarg) * thrust;
        vec3 ag = {0, .35f*9.81f*mass, 0};
        appliedForce = appliedForce + ag;
        if (timeOfFlight > detonateTime) {
            Remove = true;
        }
    }

    PhysicsEnt::update(dt);
}

void MissileEnt::onCreate() {
    velocity = {0, 2, 0};

    mass = 200;

    float L = 3.25f;
    float r = 0.1f;

    I3 = .5f * mass * r*r;
    I1 = I2 = mass * (3*r*r + L*L) / 12.0f;
}