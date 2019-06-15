#include "CollisionDetection.h"

vec3 SphereHull::supportPoint(vec3 d) {
    vec3 v = Vector::normalized(d);

    return v;
}

vec3 PolyHull::supportPoint(vec3 d) {
    vec3 vFarthestAlongD = {0,0,0};
    float distFarthestAlongD = 0;

    for (GLuint i = 0; i < numVerts; i++) {
        float dist = Vector::dot(vertices[i], d);
        if (dist > distFarthestAlongD) {
            vFarthestAlongD = vertices[i];
            distFarthestAlongD = dist;
        }
    }

    return vFarthestAlongD;
}

vec3 CapsuleHull::supportPoint(vec3 d) {
    //wrong
    if (Vector::dot(a, d) < Vector::dot(b, d)) {
        return b;
    } else  {
        return a;
    }
}

GJK_SupportPoint entitySupport(vec3 search_dir, CollisionEntity* e1, CollisionEntity* e2) {
    vec3 tDir1 =  search_dir;
    vec3 tDir2 = -search_dir;

    GJK_SupportPoint p;
    p.a = e1->collisionHull->supportPoint(tDir1) + e1->position;
    p.b = e2->collisionHull->supportPoint(tDir2) + e2->position;
    p.P = p.a - p.b;
    return p;
}

CollisionEvent Collision::collisionTest(CollisionEntity* e1, CollisionEntity* e2) {
    GJK_Result res;
    vec3 d = e1->position - e2->position;
    res.simplex[1] = entitySupport( d, e1, e2);
    res.simplex[0] = entitySupport(-d, e1, e2);
    d = Vector::cross(res.simplex[0].P, res.simplex[1].P-res.simplex[0].P);
    res.simplex[2] = entitySupport( d, e1, e2);

    GJK_SupportPoint p;
    while(gjk_iteration(&res, p, &d)) {
        printf("GJK Iteration [%-2d]\n", res.iterations);
        p = entitySupport(d, e1, e2);
    }
    printf("GJK Iteration [%-2d]\n", res.iterations);

    if (res.hit) {
        printf("Intersecting. Continue to EPA\n");
        // continue to EPA
        /*vec3 displacement = EPA(&res, &e1->collisionHull->supportPoint, &e2->collisionHull->supportPoint);
        displacement.print("Correction vector: ");*/
    }
    CollisionEvent event;
    event.entity1 = (CollisionEntity*)e1;
    event.entity1 = (CollisionEntity*)e1;

    return event;
}