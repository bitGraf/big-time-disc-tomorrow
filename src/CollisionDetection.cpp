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
        *id = 1;
        return b;
    } else  {
        *id = 0;
        return a;
    }
}

CollisionEvent Collision::collisionTest(const CollisionEntity* e1, const CollisionEntity* e2) {
    /* initial guess */
    vec3 d = {0,1,0};
    gjk_support s = {0};
    s.a = e1->collisionHull->supportPoint( d, NULL) + e1->position;
    s.b = e2->collisionHull->supportPoint(-d, NULL) + e2->position;
    d = s.b - s.a;
    int numIts = 0;

    /* run gjk algorithm */
    gjk_simplex gsx = {0};
    while (gjk(&gsx, &s, &d)) {
        vec3 n = -d;
        s.a = e1->collisionHull->supportPoint( d, &s.aid) + e1->position;
        s.b = e2->collisionHull->supportPoint(-d, &s.bid) + e2->position;
        s.a.print(" a = ");
        s.b.print(" b = ");
        d = s.b - s.a;
        numIts++;
    }
    /* check distance between closest points */
    gjk_result res;
    gjk_analyze(&res, &gsx);
    gjk_quad(&res, e1->collisionHull->radius, e2->collisionHull->radius);
    printf("%d iteration\n", numIts);

    CollisionEvent event;
    event.entity1 = (CollisionEntity*)e1;
    event.entity1 = (CollisionEntity*)e1;
    event.intersect = res.hit;
    event.distance = sqrt(res.distance_squared);
    event.response1 = res.p0;
    event.response2 = res.p1;

    return event;
}