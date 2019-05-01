#include "AI.h"
#include "Physics.h"
#include "EntityManager.h"

void AIEnt::onCreate() {
	printf("Spawning AI...\n");
	position = { 0, 0, 0 };
}

void AIEnt::update(double dt) {

	position.y = position.y + (velocity.y * dt);

	switch (state) {
	case 0:
		velocity.y += .1;
		if (position.y >= 10) {
			velocity.y = 0;
			state = 1;
		}
		break;
	case 1:
		velocity.y -= .1;
		if (position.y <= 0) {
			velocity.y = 0;
			state = 0;
		}
		break;
	}
	EntityBase::update(dt);
}