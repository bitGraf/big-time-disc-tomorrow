#pragma once

#include "Entity.h"
#include "ResourceManager.h"
#include "Input.h"


struct AIEnt : EntityBase {
	// Init Vals
	vec3 velocity = { 0, 0, 0 };
	vec3 returnSpot;
	vec3* patrolPoints;
	quat rot180;
	float distanceFromPlayer;
	int speed = 1;
	// State
	int state = 0;
	int currentPatrolGoal = 0;


	void onCreate() override;
	void update(double dt) override;
	vec3 distanceToPatrol(vec3, vec3);
	quat lookTowards(vec3 target, bool away = FALSE);
};