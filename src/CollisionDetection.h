#ifndef _COLLISION_DETECTION_H_
#define _COLLISION_DETECTION_H_

#include "Entity.h"
#include "GJK.h"
#include "EPA.h"

#include <vector>
#include <assert.h>

struct ConvexHull {
    float hullRadius = 0.0f;
    virtual int supportPoint(vec3 d, vec3* p) { assert(0);return 0;}
    virtual void cleanup() {}
};

struct SphereHull : public ConvexHull {
    int supportPoint(vec3 d, vec3* p) override;
};

struct PolyHull : public ConvexHull {
    vec3* vertices = NULL;
    int numVerts = 0;

    int supportPoint(vec3 d, vec3* p) override;
    void cleanup() override;
};

struct CapsuleHull : public ConvexHull {
    vec3 va, vb;
    int supportPoint(vec3 d, vec3* p) override;
};

struct CollisionEntity : public EntityBase {
    ConvexHull* collisionHull;
    bool moveable = false;
    vec3 velocity;
    unsigned int collisionID;
    float boundingRadius = 1.0f;

    void update(double dt) override;
};

struct CollisionEvent {
    CollisionEntity* entity1 = NULL;
    CollisionEntity* entity2 = NULL;

    bool intersect = false;
    //If not intersecting, distance apart. 
    //If intersecting, penetration depth.
    float distance = 0;
    //if intersecting, penetration normal.
    vec3 P0;
    vec3 P1;
};

struct CollisionManager {
    std::vector<CollisionEntity*> cEntList;
};

namespace Collision {
    extern CollisionManager manager;
    void Update();
    void track(EntityBase* ent);
    CollisionEvent collisionTest(CollisionEntity* e1, CollisionEntity* e2);
}

#endif