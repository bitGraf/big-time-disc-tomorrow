#include "Actor.h"

void ActorEntity::onCreate() {
    groundFriction = 20;
    acceleration = 2;
    airAcceleration = 1;
    maxSpeed = 10;
    jumpPower = 5;
    rotateSpeed = 30;

    commandFlag = 0;
    grounded = false;
}

void ActorEntity::update(double dt) {
    categorizeMovement();

    vec3 wVelocity;

    //do this before we test for jumping, so jumping doesnt 
    //become falling on first update
    wVelocity = velocity;
    if (!grounded) {
        wVelocity.y = wVelocity.y - (9.81f*dt);
    }

    if(moveable && commandFlag) {
        //There is a command to process

        //determine wish conditions
        if (grounded) {
            if (commandFlag & am_MOVE_FORWARD) {
                wVelocity = wVelocity + Forward * ( acceleration);
            }
            if (commandFlag & am_MOVE_BACKWARD) {
                //moving diagonally will be faster
                wVelocity = wVelocity + Forward * (-acceleration);
            }
            if (commandFlag & am_STRAFE_LEFT) {
                wVelocity = wVelocity + Left * ( acceleration);
            }
            if (commandFlag & am_STRAFE_RIGHT) {
                wVelocity = wVelocity + Left * (-acceleration);
            }
            if (commandFlag & am_JUMP) {
                wVelocity = wVelocity + Up * (jumpPower);
                grounded = false;
            }
        } else {
        }
    }

    //normalize XZ velocity
    float fallVel = wVelocity.y;
    vec3 xzVel = wVelocity;
    xzVel.y = 0;
    float speed = Vector::magnitude(xzVel);
    if (speed > maxSpeed) {
        //cap at maxSpeed
        Vector::normalize(xzVel);
        xzVel = xzVel * maxSpeed;
    }

    //add friction if gounded
    if (grounded && commandFlag==0) {
        xzVel = xzVel - (Vector::normalized(xzVel) * groundFriction*dt);
    }

    //restore y velocity
    xzVel.y = fallVel;
    velocity = xzVel;
    
    //resolve collisions
    CollisionEntity* me = (CollisionEntity*)this;
    for (int i = 0; i < Collision::manager.cEntList.size(); i++) {
        if (i != collisionID) { //dont test against itself
            CollisionEntity* other = Collision::manager.cEntList[i];
            CollisionEvent event = Collision::collisionTest(me, other);

            if (event.intersect) {
                //they are already intersecting, resolve it
                if (moveable) {
                    //move us out of the other object
                    position = position - (event.response*event.distance*1.03f);
                }
            } else {
                //check if they will intersect after this update

                //how far i will move in the direction of other entity
                vec3 relVel = wVelocity - other->velocity;
                float dist = Vector::dot(relVel, event.response);

                if ((dist > 0) && (dist > event.distance)) {
                    //moving in the direction of the other entity AND
                    //in one frame we will intersect

                    //new velocity will only move use 97% of the way there
                    //hopefully preventing a collision in the first place.
                    velocity = Vector::normalized(relVel) * (event.distance * .97f);
                }
            }
        }
    }

    //reset all commands
    commandFlag = 0;

    //update using new velocity
    CollisionEntity::update(dt);
}

void ActorEntity::onDestroy() {

}

void ActorEntity::command(ActorCommands com) {
    commandFlag |= com;
}

void ActorEntity::categorizeMovement() {
    //just as an example...
    if (position.y < 0.15f && velocity.y < 0) {
        position.y = 0.0f;
        velocity.y = 0;
        grounded = true;
    }
}