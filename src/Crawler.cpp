#include "Crawler.h"


#include "Player.h"
#include "EntityManager.h"

void CrawlerEnt::handleInput(int key, int scancode, int action, int mods) {
    EntityBase::handleInput(key, scancode, action, mods);

    // Jump
    if (grounded && !attached && (key == GLFW_KEY_SPACE) && (action == GLFW_PRESS)) {
        printf("Jumping...\n");
        vel.y += 5;
        grounded = false;
    }

    if ((key == GLFW_KEY_E) && (action == GLFW_PRESS)) {
        // Get the closest panel
        PanelEnt* clp = getClosestPanel(currentPanel);
        bool noOtherPanelsNear = (clp->d > attachRadius);

        if (attached) {
            printf("Currently on a panel:  ");

            if (noOtherPanelsNear) {
                printf("There are no nearby panels. Detaching\n");

                currentPanel = NULL;
                localPos = position;
                //localOrientation = orientation;
                grounded = false;
                attached = false;
            } else {
                printf("Transitioning to a new panel.\n");
                transitionToPanel(clp);
            }
        } else {
            printf("Not on a panel:        ");
            if (noOtherPanelsNear) {
                printf("There are no nearby panels.\n");
                attached = false;
            } else {
                printf("Attaching to a panel\n");
                transitionToPanel(clp);
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

    float delAngle = -50*dt*rightLeft;
    quat transQ;

    Quaternion::buildFromAxisAngleD(transQ, {0, 1, 0}, delAngle);
    localOrientation = Quaternion::mul(localOrientation, transQ);

    float C11 = 1 - 2*localOrientation.y*localOrientation.y - 2*localOrientation.z*localOrientation.z;
    float C12 = 2*(localOrientation.x*localOrientation.y - localOrientation.z*localOrientation.w);
    float C13 = 2*(localOrientation.z*localOrientation.x + localOrientation.y*localOrientation.w);
    float C21 = 2*(localOrientation.x*localOrientation.y + localOrientation.z*localOrientation.w);
    float C22 = 1 - 2*localOrientation.z*localOrientation.z - 2*localOrientation.x*localOrientation.x;
    float C23 = 2*(localOrientation.y*localOrientation.z - localOrientation.x*localOrientation.w);
    float C31 = 2*(localOrientation.z*localOrientation.x - localOrientation.y*localOrientation.w);
    float C32 = 2*(localOrientation.y*localOrientation.z + localOrientation.x*localOrientation.w);
    float C33 = 1 - 2*localOrientation.x*localOrientation.x - 2*localOrientation.y*localOrientation.y;
    
    vec3 localLeft    = {C11, C21, C31};
    vec3 localUp      = {C12, C22, C32};
    vec3 localForward = {C13, C23, C33};

	acc = { 0, !grounded ? -9.81f : 0, 0 };

    vel = {0, vel.y + acc.y * (float)dt, 0};
    vel = vel + localForward * forwardBackward * speed;
    localPos = localPos + vel * dt;

    if (attached) {
        position = currentPanel->position + Quaternion::transformVector(currentPanel->orientation, localPos);
        orientation = Quaternion::mul(currentPanel->orientation, localOrientation);
        //orientation = localOrientation;
    } else {
        position = localPos;
        orientation = localOrientation;
    }

    if (!attached && position.y < 0) {
        position.y = 0;
		vel.y = 0;
		acc.y = 0;
        grounded = true;
    }

    for (int i = 0; i < numPanels; i++) {
        allPanels[i]->distanceToPoint(position);
        if (allPanels[i]->d < attachRadius) {
            allPanels[i]->Color = {1, 2.5, 1};
        } else {
            allPanels[i]->Color = {2.5, 1, 1};
        }
    }

    PanelEnt* clp = getClosestPanel(currentPanel);
    if (clp->d < attachRadius) {
        clp->Color = {1, 4, 1};
    }

    EntityBase::update(dt);
}

void CrawlerEnt::preRender() {
    char text[64];
    sprintf(text, "Velocity: [%5.2f %5.2f %5.2f]", vel.x, vel.y, vel.z);
    Font::drawText(Entity::manager.font, 0, 32, {1, 1, 0, 1}, text);
    sprintf(text, "World position:    [%5.2f %5.2f %5.2f]", position.x, position.y, position.z);
    Font::drawText(Entity::manager.font, 0, 52, {1, 1, 0, 1}, text);
    sprintf(text, "Local position:    [%5.2f %5.2f %5.2f]", localPos.x, localPos.y, localPos.z);
    Font::drawText(Entity::manager.font, 0, 72, {1, 1, 0, 1}, text);
    sprintf(text, "World orientation: [%5.2f %5.2f %5.2f %5.2f]", orientation.x, orientation.y, orientation.z, orientation.w);
    Font::drawText(Entity::manager.font, 0, 92, {1, 1, 0, 1}, text);
    sprintf(text, "Local orientation: [%5.2f %5.2f %5.2f %5.2f]", localOrientation.x, localOrientation.y, localOrientation.z, localOrientation.w);
    Font::drawText(Entity::manager.font, 0, 112, {1, 1, 0, 1}, text);
    sprintf(text, "   [%5.2f %5.2f]", allPanels[0]->d, allPanels[1]->d);
    Font::drawText(Entity::manager.font, 0, 132, {1, 1, 0, 1}, text);

    EntityBase::preRender();
}

void CrawlerEnt::onCreate() {
    printf("loading panel...\n");

    //Panel resources
    Resources::manager.loadTriMeshResource("plane", ".modl");
    Resources::manager.loadTextureResource("wall", ".jpg");

    numPanels = 3;
    allPanels = (PanelEnt**)malloc(numPanels * sizeof(PanelEnt*));

    for (int i = 0; i < numPanels; i ++) {
        allPanels[i] = (PanelEnt*)Entity::createNewEntity(ENT_Panel);
        allPanels[i]->mesh = Resources::manager.getTriMeshResource("plane");
        allPanels[i]->baseColor = Resources::manager.getTextureResource("wall");
        allPanels[i]->update(0);
    }

    allPanels[0]->position = {-5, 5, 0};
    Quaternion::buildFromAxisAngleD(allPanels[0]->orientation, {0, 0, 1}, -90);
    allPanels[0]->orientation.print();

    allPanels[1]->position = {0, 5, -5};
    Quaternion::buildFromAxisAngleD(allPanels[1]->orientation, {1, 0, 0}, 90);
    quat q2;
    Quaternion::buildFromAxisAngleD(q2, {0, 1, 0}, -90);
    allPanels[1]->orientation = Quaternion::mul(allPanels[1]->orientation, q2);
    allPanels[1]->orientation.print();

    allPanels[2]->position = {-1.4645f, 13.536f, 0.0f};
    Quaternion::buildFromAxisAngleD(allPanels[2]->orientation, {0, 0, 1}, -135);
    allPanels[2]->orientation.print();
}

void CrawlerEnt::transitionToPanel(PanelEnt* newPanel) {
    vec3 tUp = {0, 1, 0};
    if (currentPanel != NULL) {
        tUp = currentPanel->Up;
    }

    float theta = acos(Vector::dot(newPanel->Up, tUp)) * 180 / 3.14159f;
    vec3 rotAxis = Vector::normalized(Vector::cross(tUp, newPanel->Up));
    printf("Transition angle: %f\n", theta);
    rotAxis.print("Rotation Axis: ");

    currentPanel = newPanel;

    vec3 worldPos = position;
    localPos = worldPos - currentPanel->position;
    quat invQ = {-currentPanel->orientation.x, -currentPanel->orientation.y, -currentPanel->orientation.z, currentPanel->orientation.w};
    localPos = Quaternion::transformVector(invQ, localPos);
    localPos.y = 0;
    attached = true;
    grounded = true;

    // Set the new local orientation
    quat currentWorld = orientation;

    //This is still wrong...
    quat panel2panel;
    Quaternion::buildFromAxisAngleD(panel2panel, rotAxis, theta);
    quat newWorld = Quaternion::mul(panel2panel, currentWorld);
    quat newLocal = Quaternion::mul(currentWorld, invQ);

    //localOrientation = newLocal;  //in the end just leave localOrientation the same
}

PanelEnt* CrawlerEnt::getClosestPanel(PanelEnt* curr) {
    float closest = realmax;
    PanelEnt* ret;

    for (int i = 0; i < numPanels; i++) {
        if (allPanels[i]->d < closest) {
            closest = allPanels[i]->d;
            ret = allPanels[i];

            //printf("Closest panel is %f away\n", closest);
        }
    }

    if ((curr != NULL) && (ret == curr)) {
        // the current panel IS the closest
        //printf("  recursing\n");
        float temp = closest;
        ret->d = realmax; //push to back of list and resort
        PanelEnt* newRet = getClosestPanel(curr);
        ret->d = temp;
        ret = newRet;
        //printf("  Next closest panel found %f\n", ret->d);
    }

    return ret;
}