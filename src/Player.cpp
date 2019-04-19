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
		printf("Toggle wireframe...\n");
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

        //Resources::manager.loadTerrainResource("smallMap", ".png");
        Resources::manager.loadTerrainResource("mountains512", ".png");
        EntityBase* terr = Entity::createNewEntity(ENT_Static);
        terr->mesh = Resources::manager.getTriMeshResource("mountains512");
        terr->baseColor = Resources::manager.getTextureResource("wall");
        levelLoaded = true;
	}

    if (grounded && (key == GLFW_KEY_F) && (action == GLFW_PRESS)) {
        printf("toggle frame rendering...\n");

        Entity::manager.showFrames = !Entity::manager.showFrames;
	}

    if (grounded && (key == GLFW_KEY_R) && (action == GLFW_PRESS)) {
        printf("toggle rotation...\n");

        rotate = !rotate;
	}
}

void PlayerEnt::update(double dt) {
    //if (rotate)
	//    angle += 9 * dt;
    
    float vf = Input::manager.move_forward.value;
    float vb = Input::manager.move_backward.value;
    float vl = Input::manager.move_left.value;
    float vr = Input::manager.move_right.value;

    float forwardBackward = vf - vb;
    float rightLeft = vr - vl;

    angle -= 50*dt*rightLeft;

    if (angle > 360)
        angle = 0;

    Quaternion::buildFromAxisAngleD(orientation, {0, 1, 0}, angle);

	acc = { 0, !grounded ? -9.81f : 0, 0 };

    vel = {0, (vel.y + acc.y * float(dt)), 0};
    vel = vel + Forward * forwardBackward;
    //vel = vel + Left    * -rightLeft;
	//vel = { rightLeft, (vel.y + acc.y * float(dt)), -forwardBackward };
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
    char text[64];
    sprintf(text, "player position:    [%5.2f %5.2f %5.2f]", position.x, position.y, position.z);
    Font::drawText(Entity::manager.font, 0, 32, {1, 1, 0, 1}, text);
    sprintf(text, "player orientation: [%5.2f %5.2f %5.2f %5.2f]", orientation.x, orientation.y, orientation.z, orientation.w);
    Font::drawText(Entity::manager.font, 0, 52, {1, 1, 0, 1}, text);

    sprintf(text, "Left:      [%5.2f %5.2f %5.2f]", Left.x, Left.y, Left.z);
    Font::drawText(Entity::manager.font, 0, 72, {1, 1, 0, 1}, text);
    sprintf(text, "Up:        [%5.2f %5.2f %5.2f]", Up.x, Up.y, Up.z);
    Font::drawText(Entity::manager.font, 0, 92, {1, 1, 0, 1}, text);
    sprintf(text, "Forward:   [%5.2f %5.2f %5.2f]", Forward.x, Forward.y, Forward.z);
    Font::drawText(Entity::manager.font, 0, 112, {1, 1, 0, 1}, text);

    EntityBase::preRender();
}