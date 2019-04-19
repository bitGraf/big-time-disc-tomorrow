#include "Crawler.h"


#include "Player.h"
#include "EntityManager.h"

void CrawlerEnt::handleInput(int key, int scancode, int action, int mods) {
    EntityBase::handleInput(key, scancode, action, mods);

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

	acc = { 0, 0, 0 };

    vel = {0, 0, 0};
    vel = vel + Forward * forwardBackward;
    localPos = localPos + vel * dt;

    if (loaded && attached) {
        position = panel->position + Quaternion::transformVector(panel->orientation, localPos);
        orientation = Quaternion::mul(panel->orientation, localOrientation);
    } else {
        position = localPos;
        orientation = localOrientation;
    }

    EntityBase::update(dt);
}

void CrawlerEnt::preRender() {
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