#include "Crawler.h"


#include "Player.h"
#include "EntityManager.h"

void CrawlerEnt::handleInput(int key, int scancode, int action, int mods) {
    EntityBase::handleInput(key, scancode, action, mods);

    // Jump
    if (grounded && (key == GLFW_KEY_SPACE) && (action == GLFW_PRESS)) {
        printf("Jumping...\n");
        
        if (!attached) {
            velocity.y += 10;
            grounded = false;
        }
    }

    //Reset all states
    if ((key == GLFW_KEY_R) && (action == GLFW_PRESS)) {
        localPos = {0, 1, 0};
        velocity = {0, 0, 0};
        acceleration = {0, 0, 0};
        grounded = false;
        attached = false;
        localOrientation = {0, 0, 0, 1};
        currentPanel = NULL;

        position = localPos;
        orientation = localOrientation;
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
    //float C12 = 2*(localOrientation.x*localOrientation.y - localOrientation.z*localOrientation.w);
    float C13 = 2*(localOrientation.z*localOrientation.x + localOrientation.y*localOrientation.w);
    float C21 = 2*(localOrientation.x*localOrientation.y + localOrientation.z*localOrientation.w);
    //float C22 = 1 - 2*localOrientation.z*localOrientation.z - 2*localOrientation.x*localOrientation.x;
    float C23 = 2*(localOrientation.y*localOrientation.z - localOrientation.x*localOrientation.w);
    float C31 = 2*(localOrientation.z*localOrientation.x - localOrientation.y*localOrientation.w);
    //float C32 = 2*(localOrientation.y*localOrientation.z + localOrientation.x*localOrientation.w);
    float C33 = 1 - 2*localOrientation.x*localOrientation.x - 2*localOrientation.y*localOrientation.y;
    
    vec3 localLeft    = {C11, C21, C31};
    //vec3 localUp      = {C12, C22, C32};
    vec3 localForward = {C13, C23, C33};

    vec3 friction;
    if (grounded)
        friction = -velocity * K;
    friction.y = 0;
    vec3 propulsion;
    if (grounded) {
        propulsion = localForward*forwardBackward*F;
        if (Input::manager.move_strafe.value > 0.5f) {
            propulsion = propulsion - localLeft*rightLeft*F;
        }
    }
    vec3 gravity = { 0, !grounded ? -9.81f : 0, 0 };
	acceleration = gravity + (propulsion + friction) * (1/mass);

    velocity = velocity + acceleration*dt;
    localPos = localPos + velocity*dt;

    if (attached) {
        position = currentPanel->position + Quaternion::transformVector(currentPanel->orientation, localPos);
        orientation = Quaternion::mul(currentPanel->orientation, localOrientation);
    } else {
        position = localPos;
        orientation = localOrientation;
    }

    if (!attached && position.y < 1) {
        position.y = 1;
		velocity.y = 0;
		acceleration.y = 0;
        grounded = true;
    }

    bool onAPanel = false;
    for (int i = 0; i < numPanels; i++) {
        allPanels[i]->distanceToPoint(position);
        if (allPanels[i]->inVolume) {
            onAPanel = true;
            allPanels[i]->Color = {1, 2.5, 1};
            if ((currentPanel == NULL) || (allPanels[i]->K2 < currentPanel->K2)) {
                transitionToPanel(allPanels[i]);
            }
        } else {
            allPanels[i]->Color = {2.5, 1, 1};
        }
    }
    if (currentPanel != NULL && !onAPanel) {
        grounded = false;
        attached = false;
        transitionToPanel(NULL);
    }

    quat ttt;
    Quaternion::buildFromAxisAngleD(ttt, {0, 1, 0}, 180);
    Entity::manager.camera.targetOrientation = Quaternion::mul(orientation, ttt);
    Quaternion::buildFromAxisAngleD(ttt, {1, 0, 0}, -30);
    Entity::manager.camera.targetOrientation = Quaternion::mul(Entity::manager.camera.targetOrientation, ttt);
    Entity::manager.camera.targetPosition = position - Forward*10.0f + Up*10.0f;

    EntityBase::update(dt);
}

void CrawlerEnt::postRender() {
    char text[64];
    
    sprintf(text, "Position:     [%5.2f %5.2f %5.2f]", position.x, position.y, position.z);
    Font::drawText(Entity::manager.font, 0, 32, {1, 1, 0, 1}, text);
    sprintf(text, "Velocity:     [%5.2f %5.2f %5.2f]", velocity.x, velocity.y, velocity.z);
    Font::drawText(Entity::manager.font, 0, 52, {1, 1, 0, 1}, text);
    sprintf(text, "Acceleration: [%5.2f %5.2f %5.2f]", acceleration.x, acceleration.y, acceleration.z);
    Font::drawText(Entity::manager.font, 0, 72, {1, 1, 0, 1}, text);

    for (int i = 0; i < numPanels; i++) {
        sprintf(text, "Panel %d: [K = %5.2f, In Volume: %s]%s (%5.2f, %5.2f, %5.2f)", 
            i+1, sqrt(allPanels[i]->K2), 
            allPanels[i]->inVolume ? "True " : "False",
            allPanels[i] == currentPanel ? "*" : " ",
            allPanels[i]->u.x, allPanels[i]->u.y, allPanels[i]->u.z);
        Font::drawText(Entity::manager.font, 0, 92 + 20*i, {1, 1, 0, 1}, text);
    }

    EntityBase::preRender();
}

void CrawlerEnt::onCreate() {
    printf("loading panel...\n");

    //Panel resources
    Resources::manager.loadTriMeshResource("plane", ".modl");
    Resources::manager.loadTextureResource("wall", ".jpg");

    numPanels = 4;
    allPanels = (PanelEnt**)malloc(numPanels * sizeof(PanelEnt*));

    for (int i = 0; i < numPanels; i ++) {
        allPanels[i] = (PanelEnt*)Entity::createNewEntity(ENT_Panel);
        allPanels[i]->mesh = Resources::manager.getTriMeshResource("plane");
        allPanels[i]->baseColor = Resources::manager.getTextureResource("wall");
        allPanels[i]->update(0);
        allPanels[i]->scale = {allPanels[i]->length, 1, allPanels[i]->width};
    }

    allPanels[0]->position = {-7, 5, 0};
    Quaternion::buildFromAxisAngleD(allPanels[0]->orientation, {0, 0, 1}, -90);

    allPanels[1]->position = {0, 5, -7};
    Quaternion::buildFromAxisAngleD(allPanels[1]->orientation, {1, 0, 0}, 90);
    quat q2;
    Quaternion::buildFromAxisAngleD(q2, {0, 1, 0}, -90);
    allPanels[1]->orientation = Quaternion::mul(allPanels[1]->orientation, q2);

    allPanels[2]->position = {-1.4645f - 2.0f + 7.0710678f, 13.536f + 7.0710678f, 0.0f};
    Quaternion::buildFromAxisAngleD(allPanels[2]->orientation, {0, 0, 1}, -135);
    allPanels[2]->length = 30;
    allPanels[2]->scale = {allPanels[2]->length, 1, allPanels[2]->width};

    allPanels[3]->position = {-1.4645f + 6.0f + 7.0710678f, 13.536f + 7.0710678f + 8.0f, 14.0f};
    Quaternion::buildFromAxisAngleD(allPanels[3]->orientation, {0, 0, 1}, -160);
    allPanels[3]->length = 30;
    allPanels[3]->scale = {allPanels[3]->length, 1, allPanels[3]->width};

    F = move_acc * mass;
    K = F / max_speed;

    printf("F = %f\nK = %f\n", F, K);
}

void CrawlerEnt::transitionToPanel(PanelEnt* newPanel) {
    vec3 tUp = {0, 1, 0};
    if (currentPanel != NULL) {
        tUp = currentPanel->Up;
    }

    PanelEnt* oldPanel = currentPanel;
    currentPanel = newPanel;

    vec3 oldLocal = localPos;
    localPos = position;
    if (currentPanel != NULL) {
        localPos = oldLocal;
        float theta = acos(Vector::dot(newPanel->Up, tUp)) * 180 / 3.14159f;
        vec3 rotAxis = Vector::normalized(Vector::cross(tUp, newPanel->Up));
        //printf("Transition angle: %f\n", theta);
        //rotAxis.print("Rotation Axis: ");

        vec3 worldPos = position;
        localPos = worldPos - currentPanel->position;
        quat invQ = {-currentPanel->orientation.x, -currentPanel->orientation.y, -currentPanel->orientation.z, currentPanel->orientation.w};
        localPos = Quaternion::transformVector(invQ, localPos);
        localPos.y = 1;
        attached = true;
        grounded = true;

        //localOrientation = newLocal;  //in the end just leave localOrientation the same
    }
}