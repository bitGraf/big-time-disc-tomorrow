#include "CollisionDetection.h"

CollisionManager Collision::manager;

void CollisionEntity::update(double dt) {
    if (moveable) {
            position = position + velocity*dt;
    }

    EntityBase::update(dt);
}

void Collision::Update() {
    for (int i = 0; i < manager.cEntList.size(); i++) {
        CollisionEntity* e1 = manager.cEntList[i];
        for (int j = i+1; j < manager.cEntList.size(); j++) {
            CollisionEntity* e2 = manager.cEntList[j];

            CollisionEvent e = Collision::collisionTest(e1, e2);

            if (e.GJK_Converged) {
                if (e.intersect) {
                    printf("Shapes %d and %d are intersecting.\n", i, j);
                    if (e.EPA_Converged) {
                        printf("EPA succesful\n");
                        printf("Shapes are %.4f units intersecting along ", e.distance);
                        e.response.print();

                        //Shift to resolve the collison;
                        float fudge = 1.01;
                        vec3 halfShift = e.response*e.distance*.5*fudge;

                        if (e1->moveable && e2->moveable) {
                            //shift both in opposite direction
                            e1->position = e1->position - halfShift;
                            e2->position = e2->position + halfShift;

                            /*
                            vec3 nv = e1->velocity - e.response*(fudge*Vector::dot(e1->velocity, e.response));
                            printf("%.3f -> %.3f\n", Vector::magnitude(e1->velocity),Vector::magnitude(nv));
                            e1->velocity = nv;
                            nv = e2->velocity + e.response*(fudge*Vector::dot(e2->velocity, e.response));
                            printf("%.3f -> %.3f\n", Vector::magnitude(e2->velocity),Vector::magnitude(nv));
                            e2->velocity = nv;
                            */
                        } else if (e1->moveable && !e2->moveable) {
                            //shift only 1
                            e1->position = e1->position - (halfShift*2.0f);

                            /*
                            vec3 nv = e1->velocity - e.response*(fudge*Vector::dot(e1->velocity, e.response));
                            printf("%.3f -> %.3f\n", Vector::magnitude(e1->velocity),Vector::magnitude(nv));
                            e1->velocity = nv;
                            */
                        } else if (!e1->moveable && e2->moveable) {
                            //shift only 2
                            e2->position = e2->position + (halfShift*2.0f);

                            /*
                            vec3 nv = e2->velocity + e.response*(fudge*Vector::dot(e2->velocity, e.response));
                            printf("%.3f -> %.3f\n", Vector::magnitude(e2->velocity),Vector::magnitude(nv));
                            e2->velocity = nv;
                            */
                        } else {
                            //shift neither -> How did we end up in this situation?
                            printf("Two immoveable objects are colliding...\n");
                        }
                    }
                }/* else {
                    if (e1->moveable) {
                        e1->position = e1->wishPosition;
                        e1->velocity = e1->wishVelocity;
                    }

                    if (e2->moveable) {
                        e2->position = e2->wishPosition;
                        e2->velocity = e2->wishVelocity;
                    }
                }*/
            } else {
                printf("GJK algorithm failed to converge.\n");
            }
        }
    }
}

void Collision::track(EntityBase* ent) {
    manager.cEntList.push_back((CollisionEntity*)ent);

    //not sure how robust this is...
    ((CollisionEntity*)ent)->collisionID = manager.cEntList.size()-1;
}

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

vec3 CylinderHull::supportPoint(vec3 d) {
    float sig = (d.x*d.x + d.z*d.z);
    vec3 v = Vector::normalized(d);
    if (sig > .003) { //point is on curved surface
        v.x *= radius/sig;
        v.y *= halfHeight/abs(v.y);
        v.z *= radius/sig;
    } else { //point is either of the endcaps
        v.x = 0;
        v.y *= halfHeight/abs(v.y);
        v.z = 0;
    }
    return v;
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
    CollisionEvent event;
    event.entity1 = (CollisionEntity*)e1;
    event.entity2 = (CollisionEntity*)e2;

    /* First check if spheres are even close */
    float boundingDist = Vector::magnitude(e1->position - e2->position) - (e1->boundingRadius + e2->boundingRadius);
    if (boundingDist < 0) {
        /* Bounding spheres intersect, MIGHT be a collision */
        GJK_Result gjk_res;
        vec3 d = { 1,2,3 };// e1->position - e2->position;
        GJK_SupportPoint p = entitySupport(d, e1, e2);
        while(gjk_iteration(&gjk_res, p, &d)) {
            p = entitySupport(d, e1, e2);
        }

        /*gjk_res.simplex[1] = entitySupport( d, e1, e2);
        gjk_res.simplex[0] = entitySupport(-d, e1, e2);
        d = Vector::cross(gjk_res.simplex[0].P, gjk_res.simplex[1].P- gjk_res.simplex[0].P);
        gjk_res.simplex[2] = entitySupport( d, e1, e2);

        GJK_SupportPoint p;
        d = planeNormal(gjk_res.simplex[0].P, gjk_res.simplex[1].P, gjk_res.simplex[2].P);
        p = entitySupport(d, e1, e2);

        while(gjk_iteration(&gjk_res, p, &d)) {
            p = entitySupport(d, e1, e2);
        }*/

        event.intersect = gjk_res.hit;
        event.GJK_Converged = gjk_res.converged;
        event.distance = Vector::magnitude(gjk_res.result.P);
        //printf("Distance: %.4f\n", event.distance);
        event.response = Vector::normalized(gjk_res.result.P);

        if (gjk_res.hit) {
            printf("Intersecting. Continue to EPA\n");
            // continue to EPA
            EPA_Result epa_res;
            EPA_Face face;
            epa_seed(&epa_res, &gjk_res, &face);
            p = entitySupport(face.normal, e1, e2);
            while (epa_iteration(&epa_res, p, &face)) {
                printf("  EPA Iteration: %d [%d]\n", epa_res.iteration, epa_res.simplex.numFaces);
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

    } else {
        event.intersect = false;
        event.distance = boundingDist;
        event.response = Vector::normalized(e1->position - e2->position);
        event.GJK_Converged = true; //idk
        event.EPA_Converged = true; //idk
        printf("Not bothering with GJK (%.4f)\n", event.distance);
    }

    return event;
}