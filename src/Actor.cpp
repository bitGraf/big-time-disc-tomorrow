#include "Actor.h"

void ActorEntity::onCreate() {
    jump_button_pressed = false;
}

void ActorEntity::onDestroy() {
    
}

void ActorEntity::update(double dt) {
    bool moved = false;

    vec3 fwd_xz_proj = Vector::normalized({Forward.x, 0, Forward.z});
    vec3 rgt_xz_proj = Vector::normalized({-Left.x,   0,-Left.z});

    //do some input stuff?
    {
        if (currentAction & MOVEMENT_FORWARD) {
            vel = vel + fwd_xz_proj*acc*dt;
            moved = true;
        }
        else if (Vector::dot(fwd_xz_proj,vel)>0) vel = vel - fwd_xz_proj*acc*dt;
        if (currentAction & MOVEMENT_BACKWARD) {
            vel = vel - fwd_xz_proj*acc*dt;
            moved = true;
        }
        else if (Vector::dot(-fwd_xz_proj,vel)>0) vel = vel + fwd_xz_proj*acc*dt;
        if (currentAction & MOVEMENT_STRAFE_RIGHT) {
            quat tq; Quaternion::buildFromAxisAngleD(tq, Up, -160.0f*dt);
            orientation = Quaternion::mul(orientation, tq);
        }
        if (currentAction & MOVEMENT_STRAFE_LEFT) {
            quat tq; Quaternion::buildFromAxisAngleD(tq, Up,  160.0f*dt);
            orientation = Quaternion::mul(orientation, tq);
        }
    }

    if (grounded) {
        //clamp speed
        if (Vector::magnitude(vel) > top_speed) {
            vel = Vector::normalized(vel) * top_speed;
        }
        //Deceleration
        if (!moved) vel = vel*friction_factor;
        
        //jump test
        if (currentAction & MOVEMENT_JUMP) {
            if (!jump_button_pressed) {
                vel.y += jump_vel;
                grounded = false;
                jumping = true;
                jump_button_pressed = true;
            }
        } else {
            jump_button_pressed = false;
        }
    } else { //not grounded
        if (jumping) {
            //if you dont hold jump you don't go as high
            if (!(currentAction & MOVEMENT_JUMP) && vel.y>0) {
                vel.y += 5*g*dt;
            }
        }

        //clamp xz speed
        vec3 xz_vel = vel;  xz_vel.y = 0;
        if (Vector::magnitude(xz_vel) > top_speed) {
            xz_vel = Vector::normalized(xz_vel) * top_speed;
            vel.x = xz_vel.x;
            vel.z = xz_vel.z;
        }
        vel.y += g*dt;
    }

    //Update player position
    position = position + vel*dt;

    //collided into ground
    if (position.y < 0) {
        position.y = 0;
        vel.y = 0;
        grounded = true;
        jumping = false;
    }

    currentAction = MOVEMENT_NONE;

    CollisionEntity::update(dt);
}

void ActorEntity::moveForward() {
    currentAction |= MOVEMENT_FORWARD;
}
void ActorEntity::moveBackward() {
    currentAction |= MOVEMENT_BACKWARD;
}
void ActorEntity::strafeLeft() {
    currentAction |= MOVEMENT_STRAFE_LEFT;
}
void ActorEntity::strafeRight() {
    currentAction |= MOVEMENT_STRAFE_RIGHT;
}
void ActorEntity::rotateLeft() {
    currentAction |= MOVEMENT_ROTATE_LEFT;
}
void ActorEntity::rotateRight() {
    currentAction |= MOVEMENT_ROTATE_RIGHT;
}
void ActorEntity::jump() {
    currentAction |= MOVEMENT_JUMP;
}