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

            if (e.intersect) {
                printf("Shapes %d and %d are intersecting.\n", i, j);
                printf("EPA succesful\n");
                printf("Shapes are %.4f units intersecting along ", e.distance);
                vec3 response = (e.P1 - e.P0);
                response.print("Response: ");

                //Shift to resolve the collison;
                float fudge = 1.01;
                vec3 halfShift = response*e.distance*.5*fudge;

                if (e1->moveable && e2->moveable) {
                    //shift both in opposite direction
                    e1->position = e1->position - halfShift;
                    e2->position = e2->position + halfShift;
                } else if (e1->moveable && !e2->moveable) {
                    //shift only 1
                    e1->position = e1->position - (halfShift*2.0f);
                } else if (!e1->moveable && e2->moveable) {
                    //shift only 2
                    e2->position = e2->position + (halfShift*2.0f);
                } else {
                    //shift neither -> How did we end up in this situation?
                    printf("Two immoveable objects are colliding...\n");
                }
            }
        }
    }
}

void Collision::track(EntityBase* ent) {
    manager.cEntList.push_back((CollisionEntity*)ent);

    //not sure how robust this is...
    ((CollisionEntity*)ent)->collisionID = manager.cEntList.size()-1;
}

int SphereHull::supportPoint(vec3 d, vec3* p) {
    *p = {0,0,0};

    return 0;
}

int PolyHull::supportPoint(vec3 d, vec3* p) {
    assert(vertices != NULL);

    int imax = 0;
    float dmax = Vector::dot(vertices[0], d);

    for (int i = 1; i < numVerts; i++) {
        float dot = Vector::dot(vertices[i], d);
        if (dot < dmax) continue;
        imax = i;
        dmax = dot;
    }

    *p = vertices[imax];
    return imax;
}

void PolyHull::cleanup() {
    if (vertices) {
        free(vertices);
        vertices = NULL;
    }
}

int CapsuleHull::supportPoint(vec3 d, vec3* p) {
    int i = 0;
    if (Vector::dot(va, d) < Vector::dot(vb, d)) {
        *p = vb;
        i = 1;
    } else {
        *p = va;
    }
    return i;
}

GJK_SupportPoint entitySupport(vec3 search_dir, CollisionEntity* e1, CollisionEntity* e2) {
    vec3 tDir1 =  search_dir;
    vec3 tDir2 = -search_dir;

    GJK_SupportPoint p;
    p.aid = e1->collisionHull->supportPoint(tDir1, &p.a);
    p.a = p.a + e1->position;
    p.bid = e2->collisionHull->supportPoint(tDir2, &p.b);
    p.b = p.b + e2->position;
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
        GJK_Struct gjk_struct;
        vec3 d = e1->position - e2->position;
        GJK_SupportPoint p = entitySupport(d, e1, e2);
        while(gjk_iteration(&gjk_struct, p, &d)) {
            p = entitySupport(d, e1, e2);
        }
        gjk_processResults(&gjk_struct);

        event.intersect = gjk_struct.hit;
        event.distance = gjk_struct.distance;
        //printf("Distance: %.4f\n", event.distance);
        event.P0 = gjk_struct.P0;
        event.P1 = gjk_struct.P1;

        if (event.intersect) {
            printf("Intersecting. Continue to EPA\n");
            // continue to EPA
            EPA_Struct epa_struct;
            EPA_Face face;
            /*
            epa_seed(&epa_struct, &gjk_struct, &face);
            p = entitySupport(face.normal, e1, e2);
            while (epa_iteration(&epa_struct, p, &face)) {
                printf("  EPA Iteration: %d [%d]\n", epa_struct.iteration, epa_struct.simplex.numFaces);
                d = face.normal;
                p = entitySupport(d, e1, e2);
            }

            epa_processResults(&epa_struct, &gjk_struct);

            //printf("EPA converged after %d iteration.\n", epa_res.iteration);
            //epa_res.penetrationNormal.print("penVec = ");
            //printf("penDepth = %.4f\n", epa_res.penetrationDepth);

            event.distance = epa_struct.penetrationDepth;
            event.P0 = epa_struct.contactPoint.a;
            event.P1 = epa_struct.contactPoint.b;
            */
        }
    } else {
        event.intersect = false;
        event.distance = boundingDist;
        event.P0 =  Vector::normalized(e1->position - e2->position); //wrong
        event.P1 = -Vector::normalized(e1->position - e2->position); //wrong
        printf("Not bothering with GJK (%.4f)\n", event.distance);
    }

    return event;
}