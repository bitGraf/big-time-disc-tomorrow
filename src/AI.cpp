#include "AI.h"
#include "Physics.h"
#include "EntityManager.h"

void AIEnt::onCreate() {
	printf("Spawning AI...\n");
	position = { 0, 0, 0 };
}

void AIEnt::update(double dt) {

	position = position + (velocity * dt);

	switch (state) {
	case 0: // Patrol
		
		if (position.x >= 10) {
			velocity.x -= 1;
			orientation = { 0, -.7071f, 0, .7071f };
		}
		
		if (position.x <= 0) {
			velocity.x += 1;
			orientation = { 0, .7071f, 0, .7071f };

		}
		break;
	case 1:

		break;
	default:
		state = 0;
	}
	EntityBase::update(dt);
}