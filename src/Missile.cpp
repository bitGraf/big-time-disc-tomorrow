#include "Missile.h"

void MissileEnt::update(double dt) {
    timeOfFlight += dt;

    if (timeOfFlight < separationTime) {
		vec3 toTarg = Vector::normalized(target - position);
		appliedForce = toTarg*thrust;
		if (Vector::magnitude(Entity::manager.Player->position - position) < 3.f) {
			Remove = true;
			Entity::manager.Player->health -= 5;
		}
    } else {
		Remove = true;
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