#include "AI.h"
#include "Physics.h"
#include "EntityManager.h"

void AIEnt::onCreate() {
	printf("Spawning AI...\n");
	patrolPoints = LevelLoader::loadPathFile("../data/paths/guardpath.pth");
}

vec3 AIEnt::returnPoint(vec3 a, vec3 b) {
	const float totalLength = abs(Vector::magnitude(a) - Vector::magnitude(b));
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
	if (away == true) {
		Quaternion::buildFromAxisAngleD(rot180, { 0,1,0 }, 180);
		lookAtTar = Quaternion::mul(lookAtTar, rot180);
	}
	return lookAtTar;
}

void AIEnt::update(double dt) {

	targetPosition = position + (velocity * dt);
	!isnan(targetPosition.x) ? position = Vector::lerp(position, targetPosition, rate * dt) : position = position;
	!isnan(targetOrientation.y) ? orientation = Quaternion::lerp(orientation, targetOrientation, rate * dt) : orientation = orientation;
	distanceFromPlayer = Vector::magnitude(Entity::manager.Player->position - position);

	switch (state) {
	case 0: // Idle
		if (distanceFromPlayer >= 10) {
			state = 1;
			//printf("Switching to state %d \n", state);
		}
		break;
	case 1: // Patrol 2.0
		targetOrientation = AIEnt::lookTowards(patrolPoints[currentPatrolGoal]);
		velocity = Forward*speed;
		if (Vector::magnitude(position - patrolPoints[currentPatrolGoal]) <= patrolTolerance) {
			currentPatrolGoal++;
			if (patrolPoints[currentPatrolGoal].x == NULL) {
				currentPatrolGoal = 0;
			}
		}
		

		// Player Distance Check
		if (distanceFromPlayer <= 10) {
			state = 4;
		}
		else if (distanceFromPlayer <= 3) {
			timer = 5;
			state = 2;
			//printf("Switching to state %d \n", state);
		}
		break;

	case 2: // Run away!
		targetOrientation = AIEnt::lookTowards(Entity::manager.Player->position, TRUE);
		velocity = Forward*speed;
		if (timer > 0) {
			timer -= dt;
		}
		else {
			timer = NULL;
		}
		if (distanceFromPlayer >= 10 && timer == NULL) {
			returnSpot = AIEnt::returnPoint(patrolPoints[currentPatrolGoal], patrolPoints[currentPatrolGoal + 1]);
			//printf("retX: %f, retY: %f, retZ: %f\n", returnSpot.x, returnSpot.y, returnSpot.z);
			state = 3;
			//printf("Switching to state %d \n", state);
		}
			break;
	case 3: // Return to given return spot
		targetOrientation = Quaternion::lookAt(position, returnSpot);
		velocity = Forward*speed;
		if (distanceFromPlayer < 10) {
			timer = 5;
			state = 2;
			//printf("Switching to state %d \n", state);
		}
		else if (int(position.x - returnSpot.x) == 0)
		{
			if (int(position.z - returnSpot.z) == 0)
			{
				state = 1;
				//printf("Switching to state %d \n", state);
			}
		}
		break;
	case 4: // Attack!
		targetOrientation = AIEnt::lookTowards(Entity::manager.Player->position);
		velocity = { 0, 0, 0 };
		if (distanceFromPlayer <= 5) {
			velocity = velocity*-1;
			Entity::manager.Player->health -= .1;
		}
		if (distanceFromPlayer >= 15) {
			state = 1;
		}
		break;
	default:
		state = 0;
	}
	EntityBase::update(dt);
}

void AIEnt::preRender() {
	char text[64];
	sprintf(text, "AI position:    [%5.2f %5.2f %5.2f]", position.x, position.y, position.z);
	Font::drawText(Entity::manager.font, 0, 132, { 1, 1, 0, 1 }, text);
	sprintf(text, "AI orientation: [%5.2f %5.2f %5.2f %5.2f]", orientation.x, orientation.y, orientation.z, orientation.w);
	Font::drawText(Entity::manager.font, 0, 152, { 1, 1, 0, 1 }, text);
	
	EntityBase::preRender();
}
