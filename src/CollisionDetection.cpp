#include "CollisionDetection.h"

vec3 SphereHull::supportPoint(vec3 d) {
    vec3 v = Vector::normalized(d)*radius;

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
    GJK_Result gjk_res;
    vec3 d = { 1,2,3 };// e1->position - e2->position;
    gjk_res.simplex[1] = entitySupport( d, e1, e2);
    gjk_res.simplex[0] = entitySupport(-d, e1, e2);
    d = Vector::cross(gjk_res.simplex[0].P, gjk_res.simplex[1].P- gjk_res.simplex[0].P);
    gjk_res.simplex[2] = entitySupport( d, e1, e2);

    GJK_SupportPoint p;
    d = planeNormal(gjk_res.simplex[0].P, gjk_res.simplex[1].P, gjk_res.simplex[2].P);
    p = entitySupport(d, e1, e2);

    while(gjk_iteration(&gjk_res, p, &d)) {
        p = entitySupport(d, e1, e2);
    }

    CollisionEvent event;
    event.entity1 = (CollisionEntity*)e1;
    event.entity1 = (CollisionEntity*)e1;
    event.intersect = gjk_res.hit;
    event.GJK_Converged = gjk_res.converge;
    event.distance = gjk_res.distance;

    if (gjk_res.hit) {
        //printf("Intersecting. Continue to EPA\n");
        // continue to EPA
        EPA_Result epa_res;
        EPA_Face face;
        epa_seed(&epa_res, &gjk_res, &face);
        p = entitySupport(face.normal, e1, e2);
        while (epa_iteration(&epa_res, p, &face)) {
            d = face.normal;
            p = entitySupport(d, e1, e2);
        }

        if (epa_res.converged) {
            //printf("EPA converged after %d iteration.\n", epa_res.iteration);
            //epa_res.penetrationNormal.print("penVec = ");
            //printf("penDepth = %.4f\n", epa_res.penetrationDepth);

            event.EPA_Converged = true;
            event.distance = epa_res.penetrationDepth;
            event.response = epa_res.penetrationNormal;
        }

    }

    return event;
}