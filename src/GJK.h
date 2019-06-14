#ifndef GJK_H
#define GJK_H

#define GJK_MAX_ITERATIONS 20

#include "Vector.h"
#include <math.h>

struct GJK_Result {
    vec3 simplex[4];
    int simplexSize = 0;
    int iterations = 0;
    bool hit = false;
    float distance = 0;
};

void gjk(GJK_Result* s, vec3 offset);
vec3 sphereSupport(vec3 d, vec3 origin);
vec3 support(vec3 d, vec3 origin1, vec3 origin2);
vec3 planeNormal(vec3 a, vec3 b, vec3 c);
int farthestPoint(vec3 a, vec3 b, vec3 c, vec3 d);
int collapseSimplex(const vec3 a, const vec3 b, const vec3 c, const vec3 d,
    vec3* na, vec3* nb, vec3* nc);
bool containsOrigin(vec3 a, vec3 b, vec3 c, vec3 d);
bool testTri(vec3 a, vec3 b, vec3 c);
bool triContainsPoint(vec3 a, vec3 b, vec3 c, vec3 p);

#endif