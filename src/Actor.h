#ifndef _ACTOR_H_
#define _ACTOR_H_

#include "GJK.h"
#include "Entity.h"
#include "Utils.h"

/**
 * @brief ActorEntity is an Entity that has collision associated with it.
 * 
 */
struct ActorEntity : EntityBase {
    Collider* collider; // collider interface

    bool moveable;

    void onCreate() override;
    void update(double dt) override;
    void onDestroy() override;

    bool testCollision(Collider* col, vec3* mtv = NULL, float* slope = NULL);
};

#endif