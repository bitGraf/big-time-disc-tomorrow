#include "Crawler.h"


#include "Player.h"
#include "EntityManager.h"

void CrawlerEnt::handleInput(int key, int scancode, int action, int mods) {
    EntityBase::handleInput(key, scancode, action, mods);

    if (grounded && !attached && (key == GLFW_KEY_SPACE) && (action == GLFW_PRESS)) {
        printf("Jumping...\n");
        vel.y += 5;
        grounded = false;
    }

    if ((key == GLFW_KEY_F) && (action == GLFW_PRESS)) {
        printf("toggle frame rendering...\n");

        Entity::manager.showFrames = !Entity::manager.showFrames;
	}

    if (!loaded && (key == GLFW_KEY_P) && (action == GLFW_PRESS)) {
        printf("loading panel...\n");

        Resources::manager.loadTriMeshResource("plane", ".modl");
        Resources::manager.loadTextureResource("wall", ".jpg");
        EntityBase* flo = Entity::createNewEntity(ENT_Static);
        flo->mesh = Resources::manager.getTriMeshResource("plane");
        flo->baseColor = Resources::manager.getTextureResource("wall");
        flo->position = {0, 10, 0};
        flo->orientation = {1, 2, 10, 4};
        Quaternion::normalize(flo->orientation);

        panel = new Panel;
        panel->position = flo->position;
        panel->orientation = flo->orientation;

        loaded = true;
	}

    if (loaded && (key == GLFW_KEY_E) && (action == GLFW_PRESS)) {
        printf("toggle attachment panel...\n");

        attached = !attached;

        if (attached) {
            localPos = {0, 0, 0};
            localOrientation = {0, 0, 0, 1};
        } else {
            localPos = position;
            localOrientation = orientation;
            grounded = false;
        }
	}
}

void CrawlerEnt::update(double dt) {
    float vf = Input::manager.move_forward.value;
    float vb = Input::manager.move_backward.value;
    float vl = Input::manager.move_left.value;
    float vr = Input::manager.move_right.value;

    float forwardBackward = vf - vb;
    float rightLeft = vr - vl;

    angle -= 50*dt*rightLeft;

    if (angle > 360)
        angle = 0;

    Quaternion::buildFromAxisAngleD(localOrientation, {0, 1, 0}, angle);

    float C11 = 1 - 2*localOrientation.y*localOrientation.y - 2*localOrientation.z*localOrientation.z;
    float C12 = 2*(localOrientation.x*localOrientation.y - localOrientation.z*localOrientation.w);
    float C13 = 2*(localOrientation.z*localOrientation.x + localOrientation.y*localOrientation.w);
    float C21 = 2*(localOrientation.x*localOrientation.y + localOrientation.z*localOrientation.w);
    float C22 = 1 - 2*localOrientation.z*localOrientation.z - 2*localOrientation.x*localOrientation.x;
    float C23 = 2*(localOrientation.y*localOrientation.z - localOrientation.x*localOrientation.w);
    float C31 = 2*(localOrientation.z*localOrientation.x - localOrientation.y*localOrientation.w);
    float C32 = 2*(localOrientation.y*localOrientation.z + localOrientation.x*localOrientation.w);
    float C33 = 1 - 2*localOrientation.x*localOrientation.x - 2*localOrientation.y*localOrientation.y;
    
    vec3 localLeft    = { C11,  C12, -C13};
    vec3 localUp      = { C21,  C22,  C23};
    vec3 localForward = {-C31,  C32,  C33};

	acc = { 0, !grounded ? -9.81f : 0, 0 };

    vel = {0, vel.y + acc.y * (float)dt, 0};
    vel = vel + localForward * forwardBackward;
    localPos = localPos + vel * dt;

    if (loaded && attached) {
        position = panel->position + Quaternion::transformVector(panel->orientation, localPos);
        orientation = Quaternion::mul(panel->orientation, localOrientation);
        //orientation = localOrientation;
    } else {
        position = localPos;
        orientation = localOrientation;
    }

    if (position.y < 0) {
        position.y = 0;
		vel.y = 0;
		acc.y = 0;
        grounded = true;
    }

    EntityBase::update(dt);
}

void CrawlerEnt::preRender() {
    char text[64];
    sprintf(text, "World position:    [%5.2f %5.2f %5.2f]", position.x, position.y, position.z);
    Font::drawText(Entity::manager.font, 0, 32, {1, 1, 0, 1}, text);
    sprintf(text, "Local position:    [%5.2f %5.2f %5.2f]", localPos.x, localPos.y, localPos.z);
    Font::drawText(Entity::manager.font, 0, 52, {1, 1, 0, 1}, text);
    sprintf(text, "World orientation: [%5.2f %5.2f %5.2f %5.2f]", orientation.x, orientation.y, orientation.z, orientation.w);
    Font::drawText(Entity::manager.font, 0, 72, {1, 1, 0, 1}, text);
    sprintf(text, "Local orientation: [%5.2f %5.2f %5.2f %5.2f]", localOrientation.x, localOrientation.y, localOrientation.z, localOrientation.w);
    Font::drawText(Entity::manager.font, 0, 92, {1, 1, 0, 1}, text);

    //sprintf(text, "Left:      [%5.2f %5.2f %5.2f]", Left.x, Left.y, Left.z);
    //Font::drawText(Entity::manager.font, 0, 112, {1, 1, 0, 1}, text);
    //sprintf(text, "Up:        [%5.2f %5.2f %5.2f]", Up.x, Up.y, Up.z);
    //Font::drawText(Entity::manager.font, 0, 132, {1, 1, 0, 1}, text);
    //sprintf(text, "Forward:   [%5.2f %5.2f %5.2f]", Forward.x, Forward.y, Forward.z);
    //Font::drawText(Entity::manager.font, 0, 152, {1, 1, 0, 1}, text);

    EntityBase::preRender();
}