#include "Player.h"
#include "EntityManager.h"

void PlayerEnt::handleInput(int key, int scancode, int action, int mods) {
    //printf("Key event: %d -> action: %d [mods: %d]\n",
    //    key, action, mods);

    if (grounded && (key == GLFW_KEY_SPACE) && (action == GLFW_PRESS)) {
        printf("Jumping...\n");
        vel.y += 5;
        grounded = false;
    }

	if (grounded && (key == GLFW_KEY_P) && (action == GLFW_PRESS)) {
		printf("Laying an egg... Yoshey..\n");
		int id;
		Entity::loadEntityFromFile("../data/entities/cube2.ent", &id);
		Entity::lookup_entity_by_id(id, 0)->position = this->position;
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

    /*if (!grounded) {
        printf("falling...\n");
    }*/
	acc = { 0, !grounded ? -9.81f : 0, 0 };
	vel = { rightLeft, (vel.y + acc.y * float(dt)), -forwardBackward };
    position = position + vel * dt;
    if (position.y < 0) {
        position.y = 0;
		vel.y = 0;
		acc.y = 0;
        grounded = true;
    }

    /*vec3 Forward = {0, 0, -1};
    vec3 Right = {1, 0, 0};
    vec3 vel = (Forward * forwardBackward) + 
                (Right   * rightLeft);
    if (!grounded) {
        vel.y -= 9.81 * dt;
        printf("falling...\n");
    }
    position = position + (vel * dt * speed);

    if (position.y <= 0) {
        position.y = 0;
        //printf("on ground...\n");
        grounded = true;
    }*/

    Matrix::buildFromTRS(&modelMatrix, position, orientation, scale);
    //vel = {0, 0, 0};
}
