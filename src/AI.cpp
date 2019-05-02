#include "AI.h"
#include "Physics.h"
#include "EntityManager.h"

void AIEnt::onCreate() {
	printf("Spawning AI...\n");
	position = { 0, 0, 0 };
}

vec3 AIEnt::distanceToPatrol(vec3 a, vec3 b) {
	const float totalLength = Vector::magnitude(a) - Vector::magnitude(b);
	if (totalLength == 0.0) return a - position;
	const float t = max(0, min(1, Vector::dot(b - position, a - position) / totalLength));
	const vec3 projection = position + (b - a)*t;
	return projection - position;
}

void AIEnt::update(double dt) {

	position = position + (velocity * dt);
	distanceFromPlayer = Vector::magnitude(Entity::manager.Player->position - position);

	switch (state) {
	case 0: // Patrol
		// Patrol Loop
		if (position.x > 10) {
			velocity.x = -speed;
			orientation = { 0, -.7071f, 0, .7071f };
		}
		
		if (position.x <= 0) {
			velocity.x = speed;
			orientation = { 0, .7071f, 0, .7071f };
		}

		// Player Distance Check
			if (distanceFromPlayer <= 10) {
				velocity = { 0, 0, 0 };
				state = 2;
				printf("Switching to state %d \n", state);
		}
		break;
	case 1: // Idle
		if (distanceFromPlayer >= 10) {
			velocity.x = speed*Forward.x;
			state = 0;
			printf("Switching to state %d \n", state);
		}
		break;
	case 2: // Run away!
		orientation = Quaternion::lookAt(position, Entity::manager.Player->position);
		orientation = { 0, orientation.y, 0, orientation.w };
		Quaternion::buildFromAxisAngleD(rot180, { 0,1,0 }, 180);
		orientation = Quaternion::mul(orientation, rot180);
		velocity = Forward*speed;
		if (distanceFromPlayer >= 10) {
			velocity.z = 0;
			state = 3;
			printf("Switching to state %d \n", state);
		}
			break;
	case 3: // Return
		returnSpot = AIEnt::distanceToPatrol({ 0,0,0 }, { 10,0,0 });
		orientation = Quaternion::lookAt(returnSpot, position);
		Quaternion::buildFromAxisAngleD(rot180, { 0,1,0 }, 180);
		orientation = Quaternion::mul(orientation, rot180);
		velocity = Forward*speed;
		if (distanceFromPlayer < 10) {
			velocity = { 0, 0, speed*1.f };
			state = 2;
			printf("Switching to state %d \n", state);
		}
		else if (position.z <= 0.5 && position.z >= -0.5 && position.x >=0 && position.x <= 10) {
			orientation = { 0, -.7071f, 0, .7071f };
			velocity = { -speed*1.f, 0, 0 };
			state = 0;
			printf("Switching to state %d \n", state);
		}
		break;
	default:
		state = 0;
	}
	EntityBase::update(dt);
}

