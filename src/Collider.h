#ifndef _COLLIDER_H_
#define _COLLIDER_H_

#include "Vector.h"
#include "Matrix.h"
#include <stdlib.h>

/**
 * Collider and GJK code based on (basically copied) kevinmoran on github
 * https://github.com/kevinmoran/GJK
 * modified to integrate into this project 
 */

struct Collider {
    vec3 position;
    quat orientation;

    mat3 matRS;
    mat3 matRS_inverse;

    virtual vec3 support(vec3 search_dir) = 0;
    virtual void cleanup() = 0;
};

/* Oriented Bounding Box */
struct OBBCol : Collider {
    vec3 min, max; //axis aligned.

    vec3 support(vec3 search_dir) override;
    void cleanup() override;
};

/* Bounding Sphere */
struct SphereCol : Collider {
    float r;

    vec3 support(vec3 search_dir) override;
    void cleanup() override;
};

/* Cylinder */
struct CylinderCol : Collider {
    float r, y_base, y_cap;

    vec3 support(vec3 search_dir) override;
    void cleanup() override;
};

/* Capsule */
struct CapsuleCol : Collider {
    float r, y_base, y_cap;

    vec3 support(vec3 search_dir) override;
    void cleanup() override;
};

/* Polytope */
struct PolyCol : Collider {
    vec3 *points;
    int num_points;

    vec3 support(vec3 search_dir) override;
    void cleanup() override;
};

#endif