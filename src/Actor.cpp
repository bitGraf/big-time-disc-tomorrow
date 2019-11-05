#include "Actor.h"

void ActorEntity::onCreate() {
    groundNormal = {0,1,0};
}

void ActorEntity::onDestroy() {
    
}

void ActorEntity::update(double dt) {
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

void  ActorEntity::PM_WalkMove() {
    int i;
    vec3 wishvel;
    float fmove, smove;
    vec3 wishdir;
    float wishspeed;
    float scale;
    //usercmd_t cmd;
    float accelerate;
    float vel;

    ///////////////
    int pm_waterlevel = 0;
    ///////////////

    if (pm_waterlevel > 0 && Vector::dot(Forward, groundNormal) > 0) {
        // begin swimming
        PM_WaterMove();
        return;
    }

    if (PM_CheckJump()) {
        // jumped away
        if (pm_waterlevel > 1) {
            PM_WaterMove();
        } else {
            PM_AirMove();
        }
        return;
    }

    PM_Friction();

    float fmove = ((float)(currentAction & MOVEMENT_FORWARD) - (float)(currentAction & MOVEMENT_BACKWARD));
    float smove = ((float)(currentAction & MOVEMENT_STRAFE_RIGHT) - (float)(currentAction & MOVEMENT_STRAFE_LEFT));

    scale = PM_CmdScale();

    // set movementDir
    PM_SetMovementDir();

    //project moves down to flat plane
    vec3 forward = Forward; forward.y = 0;
    vec3 right   = -Left;   right.y   = 0;

    // projecct the forward and right directions onto the ground plane
    #define OVERCLIP 0
    PM_ClipVelocity(forward, groundNormal, forward, OVERCLIP);
    PM_ClipVelocity(right,   groundNormal, right,   OVERCLIP);
    //
    Vector::normalize(forward);
    Vector::normalize(right);

    wishvel = forward*fmove + right*smove;

    wishdir = wishvel;
    wishspeed = Vector::magnitude(wishdir);
    Vector::normalize(wishdir);
    wishspeed *= scale;

    // clamp speed lower if ducking
    if (false) {
        if (wishspeed > pm_speed * pm_duckscale) {
            wishspeed = pm_speed * pm_duckscale;
        }
    }

    // clamp speed lower if wading or walking on the bottom
    if (pm_waterlevel) {
        float waterScale;

        waterScale = pm_waterlevel / 3.0;
        waterScale = 1.0 - (1.0 - pm_swimScale) * waterScale;
        if (wishspeed > pm_speed * waterScale) {
            wishspeed = pm_speed * waterScale;
        }
    }

    // when get hit temporarily lose full control
    if (false) {
        accelerate = pm_airaccelerate;
    } else {
        accelerate = pm_accelerate;
    }

    PM_Accelerate(wishdir, wishspeed, accelerate);

    //
    //

    if (false) {
        velocity.y -= 0; ////////////////////////
    } else {
        //don't reset the z velocity for slopes
        // velocity.y = 0;
    }

    vel = Vector::magnitude(velocity);

    // slide along the ground plane
    PM_ClipVelocity(velocity, groundNormal, velocity, OVERCLIP);

    // don't decrease velocity when going up or down a slope
    Vector::normalize(velocity);
    velocity = velocity * vel;

    //don't do anything if standing still
    if (!velocity.x && !velocity.z) {
        return;
    }

    PM_StepSlideMove(false);
}
void  ActorEntity::PM_Accelerate(vec3 wishdir, float wishspeed, float accelerate) {

}
void  ActorEntity::PM_ClipVelocity(vec3 v1, vec3 v2, vec3 v3, int k) {

}
void  ActorEntity::PM_SetMovementDir() {

}
float ActorEntity::PM_CmdScale() {

}
bool  ActorEntity::PM_CheckJump() {

}
void  ActorEntity::PM_AirMove() {

}
void  ActorEntity::PM_Friction() {

}
void ActorEntity::PM_StepSlideMove(bool gravity) {
    
}