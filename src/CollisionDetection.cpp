#include "CollisionDetection.h"

vec3 SphereHull::supportPoint(vec3 d, int* id) {
    vec3 v = Vector::normalized(d);
    if (id) *id = 0;

    return v;
}

vec3 PolyHull::supportPoint(vec3 d, int* id) {
    vec3 vFarthestAlongD = {0,0,0};
    float distFarthestAlongD = 0;
    if (id) *id = 0;

    for (GLuint i = 0; i < numVerts; i++) {
        float dist = Vector::dot(vertices[i], d);
        if (dist > distFarthestAlongD) {
            vFarthestAlongD = vertices[i];
            distFarthestAlongD = dist;
            if (id) *id = i;
        }
    }

    return vFarthestAlongD;
}

vec3 CapsuleHull::supportPoint(vec3 d, int* id) {
    if (Vector::dot(a, d) < Vector::dot(b, d)) {
        if (id) *id = 1;
        return b;
    } else  {
        if (id) *id = 0;
        return a;
    }
}

CollisionEvent Collision::collisionTest(const CollisionEntity* e1, const CollisionEntity* e2) {
    CollisionEvent event;
    event.entity1 = (CollisionEntity*)e1;
    event.entity1 = (CollisionEntity*)e1;

    return event;
}