#ifndef _COLLISION_DETECTION_H_
#define _COLLISION_DETECTION_H_

#include "Entity.h"
#include "GJK.h"
#include "EPA.h"

struct ConvexHull {
    float radius = 0;
    virtual vec3 supportPoint(vec3 d) { return {0,0,0};}
};

struct SphereHull : public ConvexHull {
    vec3 supportPoint(vec3 d) override;
};

struct PolyHull : public ConvexHull {
    vec3* vertices = NULL;
    int numVerts = 0;

    vec3 supportPoint(vec3 d) override;
};

struct CapsuleHull : public ConvexHull {
    vec3 a, b;

    vec3 supportPoint(vec3 d) override;
};

struct CollisionEntity : public EntityBase {
    ConvexHull* collisionHull;
};

struct CollisionEvent {
    CollisionEntity* entity1 = NULL;
    CollisionEntity* entity2 = NULL;

    bool intersect = false;
    float distance = 0;

    vec3 response1;
    vec3 response2;
};

namespace Collision {
    CollisionEvent collisionTest(CollisionEntity* e1, CollisionEntity* e2);
}

#endif