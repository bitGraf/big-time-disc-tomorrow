#ifndef _ACTOR_H_
#define _ACTOR_H_

#include "CollisionEntity.h"
#include "Input.h"

#define MOVEMENT_NONE 0
#define MOVEMENT_FORWARD 1
#define MOVEMENT_BACKWARD 2
#define MOVEMENT_STRAFE_LEFT 4
#define MOVEMENT_STRAFE_RIGHT 8
#define MOVEMENT_ROTATE_LEFT 16
#define MOVEMENT_ROTATE_RIGHT 32
#define MOVEMENT_JUMP 64

struct ActorEntity : CollisionEntity {
    vec3 velocity;
    vec3 groundNormal;

    float pm_speed = 10.0f;
    float pm_duckscale = 0.25f;
    float pm_swimScale = 0.50f;
    float pm_wadeScale = 0.70f;

    float	pm_accelerate = 10.0f;
    float	pm_airaccelerate = 1.0f;
    float	pm_wateraccelerate = 4.0f;
    float	pm_flyaccelerate = 8.0f;

    void onCreate() override;
    void update(double dt) override;
    void onDestroy() override;

    // movement commands
    unsigned char currentAction = MOVEMENT_NONE;
    void moveForward();
    void moveBackward();
    void strafeLeft();
    void strafeRight();
    void rotateLeft();
    void rotateRight();
    void jump();

    // Quake style moving
    void  PM_Accelerate(vec3 wishdir, float wishspeed, float accelerate);
    void  PM_ClipVelocity(vec3 v1, vec3 v2, vec3 v3, int k);
    void  PM_SetMovementDir();
    float PM_CmdScale();
    bool  PM_CheckJump();
    void  PM_WalkMove();
    void  PM_WaterMove();
    void  PM_AirMove();
    void  PM_Friction();
    void PM_StepSlideMove(bool gravity);
};

#endif