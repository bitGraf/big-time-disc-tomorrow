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
        EntityBase* ent = Entity::createNewEntity(ENT_Static);
        ent->position = this->position;
        ent->mesh = Resources::manager.getTriMeshResource("sphere");
        ent->baseColor = Resources::manager.getTextureResource("sample");
        ent->Color = {1, 5, 5};
        ent->scale = {.1f, .3f, .1f};
	}

    if (grounded && (key == GLFW_KEY_E) && (action == GLFW_PRESS)) {
		printf("Acquire Fentanyl I must...\n");
        EntityBase* ent = Entity::createNewEntity(ENT_Static);
        ent->mesh = Resources::manager.getTriMeshResource("cube");
        ent->baseColor = Resources::manager.getTextureResource("sample");
        ent->Color = {.2f, 3, 10};
        ent->scale = {.6f, .6f, .6f};
        ent->position = this->position;
	}

    if (grounded && (key == GLFW_KEY_O) && (action == GLFW_PRESS)) {
		printf("load pillar...\n");
        Resources::manager.loadTriMeshResource("pillar", ".modl");
        Entity::manager.pointRender = !Entity::manager.pointRender;

        if (Entity::manager.pointRender) {
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
            glDisable(GL_CULL_FACE);
        } else {
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
            glEnable(GL_CULL_FACE);
        }
	}

    if (grounded && (key == GLFW_KEY_I) && (action == GLFW_PRESS)) {
        Resources::manager.printAllResources();
        Entity::printAllEntities();
	}

    if (grounded && (key == GLFW_KEY_U) && (action == GLFW_PRESS)) {
        printf("nut...\n");
        EntityBase* ent = Entity::createNewEntity(ENT_Temporary);
        ent->position = this->position;
        ent->mesh = Resources::manager.getTriMeshResource("sphere");
        ent->baseColor = Resources::manager.getTextureResource("sample");
        ent->Color = {1, 5, 5};
        ent->scale = {.1f, .3f, .1f};
	}

    if (!levelLoaded && (key == GLFW_KEY_M) && (action == GLFW_PRESS)) {
        printf("load level...\n");
        //Level::loadLevel(" ");

        Resources::manager.loadTerrainResource("smallMap", ".png");
        EntityBase* terr = Entity::createNewEntity(ENT_Static);
        terr->mesh = Resources::manager.getTriMeshResource("smallMap");
        terr->baseColor = Resources::manager.getTextureResource("wall");
        //terr->Remove = true;
        levelLoaded = true;
	}
}

void PlayerEnt::update(double dt) {
	angle += 9 * dt;

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