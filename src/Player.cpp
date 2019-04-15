#include "Player.h"
#include "EntityManager.h"

void PlayerEnt::handleInput(int key, int scancode, int action, int mods) {
    EntityBase::handleInput(key, scancode, action, mods);

    if (grounded && (key == GLFW_KEY_SPACE) && (action == GLFW_PRESS)) {
        printf("Jumping...\n");
        vel.y += 5;
        grounded = false;
    }

	if (grounded && (key == GLFW_KEY_P) && (action == GLFW_PRESS)) {
		printf("Laying an egg... Yoshey..\n");
		int id;
		Entity::loadEntityFromFile("../data/entities/cube2.ent", &id);
		Entity::lookup_entity_by_id(id)->position = this->position;
	}
}

void PlayerEnt::update(double dt) {
	angle += 90 * dt;

    if (angle > 360)
        angle = 0;

    Quaternion::buildFromAxisAngleD(orientation, {0, 1, 0}, angle);
    
    float vf = Input::manager.move_forward.value;
    float vb = Input::manager.move_backward.value;
    float vl = Input::manager.move_left.value;
    float vr = Input::manager.move_right.value;

    float forwardBackward = vf - vb;
    float rightLeft = vr - vl;

	acc = { 0, !grounded ? -9.81f : 0, 0 };
	vel = { rightLeft, (vel.y + acc.y * float(dt)), -forwardBackward };
    position = position + vel * dt;
    if (position.y < 0) {
        position.y = 0;
		vel.y = 0;
		acc.y = 0;
        grounded = true;
    }


    EntityBase::update(dt);
}

void PlayerEnt::preRender() {
    EntityBase::preRender();
}