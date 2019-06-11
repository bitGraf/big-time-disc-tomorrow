#ifndef GJK_H
#define GJK_H

#define GJK_MAX_ITERATIONS 20

#include "Vector.h"

struct gjk_support {
    int aid, bid;
    vec3 a;
    vec3 b;
};
struct gjk_vertex {
    vec3 a;
    vec3 b;
    vec3 p;
    int aid, bid;
};
struct gjk_simplex {
    int max_iter, iter;
    int hit, cnt;
    gjk_vertex v[4];
    float bc[4], D;
};
struct gjk_result {
    int hit;
    vec3 p0;
    vec3 p1;
    float distance_squared;
    int iterations;
};
extern int gjk(struct gjk_simplex *s, const struct gjk_support *sup, vec3 *dv);
extern void gjk_analyze(struct gjk_result *res, const struct gjk_simplex *s);
extern void gjk_quad(struct gjk_result *res, float a_radius, float b_radius);

#endif