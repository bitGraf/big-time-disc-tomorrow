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
        p1 = Entity::createNewEntity(ENT_Static);
        p1->mesh = Resources::manager.getTriMeshResource("plane");
        p1->baseColor = Resources::manager.getTextureResource("wall");
        p1->position = {0, 5, -5};
        Quaternion::buildFromAxisAngleD(p1->orientation, {1, 0, 0}, 90);
        Quaternion::normalize(p1->orientation);
        StaticEnt* se = (StaticEnt*)p1;
        se->Rainbow = false;

        panel1 = new Panel;
        panel1->position = p1->position;
        panel1->orientation = p1->orientation;
        panel1->update();


        p2 = Entity::createNewEntity(ENT_Static);
        p2->mesh = Resources::manager.getTriMeshResource("plane");
        p2->baseColor = Resources::manager.getTextureResource("wall");
        p2->position = {5, 5, 0};
        Quaternion::buildFromAxisAngleD(p2->orientation, {0, 0, 1}, 90);
        //Quaternion::normalize(p2->orientation);
        se = (StaticEnt*)p2;
        se->Rainbow = false;

        panel2 = new Panel;
        panel2->position = p2->position;
        panel2->orientation = p2->orientation;
        panel2->update();


        loaded = true;
	}

    if (loaded && (key == GLFW_KEY_E) && (action == GLFW_PRESS)) {
        printf("toggle attachment panel...\n");

        float d = 0;
        if (d1 < d2) {
            currentPanel = panel1;
            d = d1;
        } else {
            currentPanel = panel2;
            d = d2;
        }

        if (attached == 0 && d < attachRadius) {
            //on ground -> panel
            vec3 worldPos = position;
            localPos = worldPos - currentPanel->position;
            quat invQ = {-currentPanel->orientation.x, -currentPanel->orientation.y, -currentPanel->orientation.z, currentPanel->orientation.w};
            localPos = Quaternion::transformVector(invQ, localPos);
            localPos.y = 0;
            //localOrientation = {0, 0, 0, 1};
            attached = 1;
            grounded = true;
        } else if (attached == 1) {

            bool swap = false;
            Panel* next;
            if (currentPanel == panel1 && d2 < attachRadius) {
                //switch to panel 2
                swap = true;
                next = panel2;
            } else if (currentPanel == panel2 && d1 < attachRadius) {
                //switch to panel 1
                swap = true;
                next = panel1;
            }

            if (swap) {
                currentPanel = next;
                vec3 worldPos = position;
                localPos = worldPos - currentPanel->position;
                quat invQ = {-currentPanel->orientation.x, -currentPanel->orientation.y, -currentPanel->orientation.z, currentPanel->orientation.w};
                localPos = Quaternion::transformVector(invQ, localPos);
                localPos.y = 0;
                //localOrientation = {0, 0, 0, 1};
            attached = 1;
            grounded = true;
            } else {
                localPos = position;
                //localOrientation = orientation;
                grounded = false;
                attached = 0;
            }
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
        position = currentPanel->position + Quaternion::transformVector(currentPanel->orientation, localPos);
        orientation = Quaternion::mul(currentPanel->orientation, localOrientation);
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

    if (loaded) {
        d1 = panel1->distanceToPoint(position);
        if (d1 < attachRadius) {
            p1->Color = {1, 1.5, 1};
        } else {
            p1->Color = {1.5, 1, 1};
        }
        d2 = panel2->distanceToPoint(position);
        if (d2 < attachRadius) {
            p2->Color = {1, 1.5, 1};
        } else {
            p2->Color = {1.5, 1, 1};
        }
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
    if (attached == 0)
        sprintf(text, "Panel: Not attached");
    if (attached == 1)
        sprintf(text, "Panel: 1");
    if (attached == 2)
        sprintf(text, "Panel: 2");
    Font::drawText(Entity::manager.font, 0, 112, {1, 1, 0, 1}, text);
    sprintf(text, "   [%5.2f %5.2f]", d1, d2);
    Font::drawText(Entity::manager.font, 0, 132, {1, 1, 0, 1}, text);

    //sprintf(text, "Left:      [%5.2f %5.2f %5.2f]", Left.x, Left.y, Left.z);
    //Font::drawText(Entity::manager.font, 0, 112, {1, 1, 0, 1}, text);
    //sprintf(text, "Up:        [%5.2f %5.2f %5.2f]", Up.x, Up.y, Up.z);
    //Font::drawText(Entity::manager.font, 0, 132, {1, 1, 0, 1}, text);
    //sprintf(text, "Forward:   [%5.2f %5.2f %5.2f]", Forward.x, Forward.y, Forward.z);
    //Font::drawText(Entity::manager.font, 0, 152, {1, 1, 0, 1}, text);

    EntityBase::preRender();
}