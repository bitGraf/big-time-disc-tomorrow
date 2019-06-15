#ifndef GJK_H
#define GJK_H

#define GJK_MAX_ITERATIONS 20

#include "Vector.h"
#include <math.h>

struct GJK_SupportPoint {
    vec3 a; //point on shape 1
    vec3 b; //point on shape 2
    vec3 P; //poont on Minkowski sum A-B
};

struct GJK_Result {
    GJK_SupportPoint simplex[4];
    int simplexSize = 0;
    int iterations = 0;
    bool hit = false;
    float distance = 0;

    vec3 R;
    vec3 Rp;
    int numNoConverge = 0;
};

void gjk(GJK_Result* s, vec3 initial_guess);
int gjk_iteration(GJK_Result* res, GJK_SupportPoint p, vec3* search_dir);
vec3 gjk_support(vec3 d, vec3, vec3);
vec3 planeNormal(vec3 a, vec3 b, vec3 c);
int farthestPoint(vec3 a, vec3 b, vec3 c, vec3 d);
int collapseSimplex(const vec3 a, const vec3 b, const vec3 c, const vec3 d,
    vec3* na, vec3* nb, vec3* nc);
bool containsOrigin(vec3 a, vec3 b, vec3 c, vec3 d);
bool testTri(vec3 a, vec3 b, vec3 c);
bool triContainsPoint(vec3 a, vec3 b, vec3 c, vec3 p);

#endif