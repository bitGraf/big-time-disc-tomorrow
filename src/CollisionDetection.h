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

    bool GJK_Converged = false;
    bool EPA_Converged = false;

    bool intersect = false;
    //If not intersecting, distance apart. 
    //If intersecting, penetration depth.
    float distance = 0;
    //if intersecting, penetration normal.
    vec3 response;
};

namespace Collision {
    CollisionEvent collisionTest(CollisionEntity* e1, CollisionEntity* e2);
}

#endif