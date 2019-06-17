#ifndef _ACTOR_H_
#define _ACTOR_H_

#include "CollisionDetection.h"

typedef unsigned char byte;

enum ActorCommands {
    am_NONE = 0,
    am_MOVE_FORWARD = 1,
    am_MOVE_BACKWARD = 2,
    am_STRAFE_LEFT = 4,
    am_STRAFE_RIGHT = 8,
    am_ROTATE_RIGHT = 16,
    am_ROTATE_LEFT = 32,
    am_JUMP = 64
};

struct ActorEntity : public CollisionEntity {
    byte commandFlag;

    float groundFriction;
    float acceleration;
    float airAcceleration;
    float maxSpeed;
    float jumpPower;
    float rotateSpeed;

    bool grounded;

    void onCreate() override;
    void update(double dt) override;
    void onDestroy() override;

    void categorizeMovement();
    void command(ActorCommands com);
};

#endif