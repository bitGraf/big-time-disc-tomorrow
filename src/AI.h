#pragma once

#include "Entity.h"
#include "ResourceManager.h"
#include "Input.h"


struct AIEnt : EntityBase {
	// Init Vals
	vec3 velocity = { 0, 0, 0 };
	
	// State
	int state = 0;

	void onCreate() override;
	void update(double dt) override;
};