#ifndef _ACTOR_H_
#define _ACTOR_H_

#include "CollisionEntity.h"
#include "Input.h"

struct ActorEntity : CollisionEntity {
    bool grounded = false;
    bool jumping = false;
    bool jump_button_pressed = false;
    float max_stand_slope = 45;
    float top_speed = 10.0f;
    float time_till_top_speed = 0.25f;
    float acc = top_speed/time_till_top_speed;
    float friction_factor = 0.3f;
    float jump_height = 2.0f;
    float jump_dist_to_peak = 2.0f;
    float g = -2*jump_height*top_speed*top_speed/(jump_dist_to_peak*jump_dist_to_peak);
    float jump_vel = 2*jump_height*top_speed/jump_dist_to_peak;

    vec3 vel;

    void onCreate() override;
    void update(double dt) override;
    void onDestroy() override;
};

#endif