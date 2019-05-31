#include "Crawler.h"
//#include "Physics.h"
#include "EntityManager.h"

void CrawlerEnt::handleInput(int key, int scancode, int action, int mods) {
    EntityBase::handleInput(key, scancode, action, mods);

    // Jump
    if (grounded && (key == GLFW_KEY_SPACE) && (action == GLFW_PRESS)) {
        printf("Jumping...\n");
        if (!attached) {
            velocity.y += 10;
            grounded = false;
        } else {
			quat newOrientation;// = { orientation.x, 0.f, orientation.z, orientation.w };
			grounded = false;
            velocity = velocity + currentPanel->Up * 10.0f;
            attachCoolDown = 0.25f;
            transitionToPanel(NULL);
			localOrientation = Quaternion::normalized(newOrientation);
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

        Entity::manager.camera.position = {0, 0, 0};
        Entity::manager.camera.targetPosition = {0, 0, 0};
        Entity::manager.camera.targetOrientation = {0, 0, 0, 1};
        Entity::manager.camera.orientation = {0, 0, 0, 1};
        Entity::manager.camera.update(0);
    }

    if ((key == GLFW_KEY_C) && (action == GLFW_PRESS)) {
        cameraFollow = !cameraFollow;
    }

    if ((key == GLFW_KEY_L) && (action == GLFW_PRESS)) {
        if (currentLevel) {
            for (int i = 0; i < currentLevel->numPanels; i++) {
                currentLevel->panels[i]->Remove = true;
            }
            currentLevel = NULL;
        }
    }

	if ((key == GLFW_KEY_Y) && (action == GLFW_PRESS)) {
		printf("BREAK\n");
	}
}

void CrawlerEnt::update(double dt) {
    if (attachCoolDown > 0)
        attachCoolDown -= dt;
    if (attachCoolDown < 0)
        attachCoolDown = 0.0f;

    float vf = Input::manager.move_forward.value;
    float vb = Input::manager.move_backward.value;
    float vl = Input::manager.move_left.value;
    float vr = Input::manager.move_right.value;
	float vsl = Input::manager.move_strafe_left.value;
	float vsr = Input::manager.move_strafe_right.value;

    float forwardBackward = vf - vb;
    float rightLeft = vr - vl;
	float strafeRightLeft = vsl - vsr;

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
            propulsion = propulsion + localLeft*rightLeft*F*1.5f;
        }
		if (Input::manager.move_strafe_left.value || Input::manager.move_strafe_right.value > 0.5f) {
			propulsion = localLeft*strafeRightLeft*F*1.2f;
			if (Input::manager.move_strafe.value > 0.5f) {
				propulsion = propulsion * .5;
			}
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

    if (!attached && position.y < 1 && velocity.y < 0) {
        position.y = 1;
		velocity.y = 0;
		acceleration.y = 0;
        grounded = true;
    }

    if (currentLevel != NULL) {
        bool onAPanel = false;
        for (int i = 0; i < currentLevel->numPanels; i++) {
            currentLevel->panels[i]->distanceToPoint(position);
            if (currentLevel->panels[i]->inVolume) {
                onAPanel = true;
                currentLevel->panels[i]->Color = {1, 2.5, 1};
                if ((currentPanel == NULL) || (currentLevel->panels[i]->K2 < currentPanel->K2)) {
                    if (attachCoolDown < 0.001f)
                        transitionToPanel(currentLevel->panels[i]);
					attachCoolDown = .25f;
					break;
                }
            } else {
                currentLevel->panels[i]->Color = {2.5, 1, 1};
            }
        }
        if (currentPanel != NULL && !onAPanel) {
            transitionToPanel(NULL);
        }
    }

    if (cameraFollow) {
        quat ttt;
        Quaternion::buildFromAxisAngleD(ttt, {0, 1, 0}, 180);
        Entity::manager.camera.targetOrientation = Quaternion::mul(orientation, ttt);
        Quaternion::buildFromAxisAngleD(ttt, {1, 0, 0}, -30);
        Entity::manager.camera.targetOrientation = Quaternion::mul(Entity::manager.camera.targetOrientation, ttt);
        Entity::manager.camera.targetPosition = position - Forward*10.0f + Up*10.0f;
    } else {
        //Entity::manager.camera.targetPosition = {0, 0, 0};
        quat ttt = Quaternion::lookAt(Entity::manager.camera.position, position);
        quat correc;
        Quaternion::buildFromAxisAngleD(correc, {0, 1, 0}, 180);
        Entity::manager.camera.targetOrientation = Quaternion::mul(ttt, correc);
    }

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
    sprintf(text, "Cooldown: %6.4f", attachCoolDown);
    Font::drawText(Entity::manager.font, 0, 92, {1, 1, 0, 1}, text);
	sprintf(text, "Health: %5.2f,", health);
	Font::drawText(Entity::manager.font, 0, 112, { 1, 1, 0, 1 }, text);
/*
    for (int i = 0; i < numPanels; i++) {
        sprintf(text, "Panel %d: [K = %5.2f, In Volume: %s]%s (%5.2f, %5.2f)", 
            i+1, sqrt(allPanels[i]->K2), 
            allPanels[i]->inVolume ? "True " : "False",
            allPanels[i] == currentPanel ? "*" : " ",
            allPanels[i]->u.x, allPanels[i]->u.z);
        Font::drawText(Entity::manager.font, 0, 112 + 20*i, {1, 1, 0, 1}, text);
    }
*/

    EntityBase::preRender();
}

void CrawlerEnt::onCreate() {
    printf("loading panels...\n");
    currentLevel = NULL;

    //Load level from file
    //allPanels = (PanelEnt**)LevelLoader::loadFromFile("../data/levels/level2.lvl", &numPanels);

    F = move_acc * mass;
    K = F / max_speed;

    printf("F = %f\nK = %f\n", F, K);
}

void CrawlerEnt::transitionToPanel(PanelEnt* newPanel) {
    vec3 currUp = {0, 1, 0};
    if (currentPanel != NULL) {
        currUp = currentPanel->Up;
    }

    if (newPanel == NULL) {
        localPos = position;
        currentPanel = newPanel;
        grounded = false;
        attached = false;
        printf("Panel detach\n");
    } else {
        printf("Panel change\n");
        vec3 Axis = Vector::normalized(Vector::cross(currUp, newPanel->Up));
        float Angle = acos(Vector::dot(currUp, newPanel->Up));
        quat qT;Quaternion::buildFromAxisAngle(qT, Axis, Angle);

        //New local position
        localPos = Quaternion::transformVector(
            Quaternion::inverse(newPanel->orientation),
            position - newPanel->position);
        
        //Current world orientation
        //quat currWorld = orientation;
        //New world orientation
        //quat newWorld  = Quaternion::mul(qT, orientation);
        //Transform into newPanel-space
        localOrientation = Quaternion::mul(
            Quaternion::inverse(newPanel->orientation), 
            Quaternion::mul(qT, orientation));
        
        //Set attachment to new panel
        currentPanel = newPanel;
        localPos.y = 1;
        velocity.y = 0;
        attached = true;
        grounded = true;
    }
}


/*
void CrawlerEnt::transitionToPanel(PanelEnt* newPanel) {
    vec3 tUp = {0, 1, 0};
    if (currentPanel != NULL) {
        tUp = currentPanel->Up;
    }

    PanelEnt* oldPanel = currentPanel;
    currentPanel = newPanel;

    vec3 oldLocal = localPos;
    localPos = position;
    if (newPanel != NULL) {
        localPos = oldLocal;
        float theta = acos(Vector::dot(newPanel->Up, tUp)) * 180 / 3.14159f;
        vec3 rotAxis = Vector::normalized(Vector::cross(tUp, newPanel->Up));
        //printf("Transition angle: %f\n", theta);
        //rotAxis.print("Rotation Axis: ");

        vec3 worldPos = position;
        localPos = worldPos - newPanel->position;
        quat invQ = Quaternion::inverse(newPanel->orientation);
        localPos = Quaternion::transformVector(invQ, localPos);
        localPos.y = 1;
        attached = true;
        grounded = true;

        //localOrientation = newLocal;  //in the end just leave localOrientation the same
    }
}
*/