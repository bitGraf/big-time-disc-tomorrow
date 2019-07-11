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
    bool grounded = false;
    bool jumping = false;
    bool jump_button_pressed = false;
    float max_stand_slope = 45;
    float top_speed = 10.0f;
    float time_till_top_speed = 0.25f;
    float acc = top_speed/time_till_top_speed;
    float friction_factor = 0.3f;
    float jump_height = 4.0f;
    float jump_dist_to_peak = 2.0f;
    float g = -2*jump_height*top_speed*top_speed/(jump_dist_to_peak*jump_dist_to_peak);
    float jump_vel = 2*jump_height*top_speed/jump_dist_to_peak;

    vec3 vel;

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
};

#endif