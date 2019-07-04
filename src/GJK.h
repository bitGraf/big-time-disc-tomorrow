#ifndef GJK_H
#define GJK_H

#define GJK_MAX_ITERATIONS 40
#define GJK_FLT_MAX 1e30

#include "Vector.h"
#include <math.h>

struct GJK_SupportPoint {
    vec3 a; //point on shape 1
    vec3 b; //point on shape 2
    vec3 P; //point on Minkowski sum A-B
    int aid;
    int bid;
};

struct GJK_Struct {
    //Working data
    GJK_SupportPoint simplex[4];
    float bc[4];
    int cnt = 0;
    float D;

    //Result
    bool hit = false;

    //Contact points
    vec3 P0;
    vec3 P1;
    float distance;

    //Convergence Criteria
    int iteration = 0;
};

int gjk_iteration(GJK_Struct* res, GJK_SupportPoint p, vec3* search_dir, bool verbose = false);
float f3box(vec3 a, vec3 b, vec3 c);
void gjk_processResults(GJK_Struct* res);
void gjk_processRadius(GJK_Struct* res, float r1, float r2);

#endif