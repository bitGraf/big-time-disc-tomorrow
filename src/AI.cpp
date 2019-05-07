#include "AI.h"
#include "Physics.h"
#include "EntityManager.h"

void AIEnt::onCreate() {
	printf("Spawning AI...\n");
	patrolPoints = Level::loadPathFile("../data/paths/guardpath.pth");
	position = { 10, 0, 10 };
}

vec3 AIEnt::distanceToPatrol(vec3 a, vec3 b) {
	const float totalLength = Vector::magnitude(a) - Vector::magnitude(b);
	if (totalLength == 0.0) return a - position;
	const float t = max(0, min(1, Vector::dot(b - position, a - position) / totalLength));
	const vec3 projection = position + (b - a)*t;
	//printf("aX: %f, aY: %f, aZ: %f, bX: %f, bY: %f, bZ: %f\n", a.x, a.y, a.z, b.x, b.y, b.z);
	return projection - position;
}

quat AIEnt::lookTowards(vec3 target, bool away) {
	quat lookAtTar = Quaternion::lookAt(position, target);
	lookAtTar = { 0, lookAtTar.y, 0, lookAtTar.w };
	Quaternion::normalize(lookAtTar);
	if (away = TRUE) {
		Quaternion::buildFromAxisAngleD(rot180, { 0,1,0 }, 180);
		lookAtTar = Quaternion::mul(lookAtTar, rot180);
	}
	return lookAtTar;
}

void AIEnt::update(double dt) {

	position = position + (velocity * dt);
	distanceFromPlayer = Vector::magnitude(Entity::manager.Player->position - position);

	switch (state) {
	/*case 0: // Patrol
		// Thot Patrol Loop
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
		break;*/
	case 0: // Patrol 2.0
		orientation = AIEnt::lookTowards(patrolPoints[currentPatrolGoal]);
		velocity = Forward*speed;
		if (int(position.x - patrolPoints[currentPatrolGoal].x) == 0) {
			currentPatrolGoal++;
			printf("New Patrol Goal is: %i\n", currentPatrolGoal);
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
		orientation = AIEnt::lookTowards(Entity::manager.Player->position, TRUE);
		velocity = Forward*speed;
		if (distanceFromPlayer >= 10) {
			velocity = { 0, 0, 0 };
			returnSpot = AIEnt::distanceToPatrol(patrolPoints[currentPatrolGoal], patrolPoints[currentPatrolGoal + 1]);
			//printf("retX: %f, retY: %f, retZ: %f\n", returnSpot.x, returnSpot.y, returnSpot.z);
			state = 3;
			printf("Switching to state %d \n", state);
		}
			break;
	case 3: // Return
		orientation = Quaternion::lookAt(position, returnSpot);
		velocity = Forward*speed;
		if (distanceFromPlayer < 10) {
			velocity = { 0, 0, speed*1.f };
			state = 2;
			printf("Switching to state %d \n", state);
		}
		else if (int(position.x - returnSpot.x) == 0)
		{
			printf("X complete\n");
			if (int(position.z - returnSpot.z) == 0)
			{
				printf("Z complete\n");
				state = 0;
				printf("Switching to state %d \n", state);
			}
		}
		break;
	default:
		state = 0;
	}
	EntityBase::update(dt);
}

