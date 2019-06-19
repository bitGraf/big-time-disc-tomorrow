#ifndef GJK_H
#define GJK_H

#define GJK_MAX_ITERATIONS 40

#include "Vector.h"
#include <math.h>

struct GJK_SupportPoint {
    vec3 a; //point on shape 1
    vec3 b; //point on shape 2
    vec3 P; //point on Minkowski sum A-B
    vec3 N; //contact normal
};

struct GJK_Simplex {
    GJK_SupportPoint a, b, c, d;
};

struct GJK_Result {
    //Working data
    GJK_Simplex simplex;
    int cnt = 0;

    //Result
    bool hit = false;
    GJK_SupportPoint result;

    //Convergence Criteria
    int iteration = 0;
    bool converged = false;
};

/**
 * @brief 
 * 
 * @param res 
 * @param p 
 * @param search_dir 
 * @return int 
 */
int gjk_iteration(GJK_Result* res, GJK_SupportPoint p, vec3* search_dir);
int findClosestFace(GJK_Simplex* simplex, vec3 center);
vec3 getOuterFaceNormal(vec3 a, vec3 b, vec3 c, vec3 center);
int buildNewSimplex(GJK_Simplex* simplex, int cnt);
vec3 lineSegmentNormal(vec3 a, vec3 b, vec3 s);
vec3 planeNormal(vec3 a, vec3 b, vec3 c, vec3 s);
float f3box(vec3 a, vec3 b, vec3 c);
////vec3 planeNormal(vec3 a, vec3 b, vec3 c);
////int farthestPoint(GJK_SupportPoint a, GJK_SupportPoint b, GJK_SupportPoint c, GJK_SupportPoint d);
////int collapseSimplex(const GJK_SupportPoint a, const GJK_SupportPoint b, const GJK_SupportPoint c, const GJK_SupportPoint d,
////    GJK_SupportPoint* na, GJK_SupportPoint* nb, GJK_SupportPoint* nc);
bool containsOrigin(vec3 a, vec3 b, vec3 c, vec3 d);
bool testTri(vec3 a, vec3 b, vec3 c);
bool triContainsPoint(vec3 a, vec3 b, vec3 c, vec3 p);

#endif