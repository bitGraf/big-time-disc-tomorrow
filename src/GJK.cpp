#include "GJK.h"
#include <assert.h>

#define GJK_FLT_MAX 3.40282347E+28F
#define GJK_EPSILON 1.19209290E-07F

int gjk_iteration(GJK_Result* res, GJK_SupportPoint p, vec3* search_dir) {
    res->Rp = res->R;
    res->R = p.P;

    float differ = Vector::magnitude(res->Rp - res->R);
    if (differ < 0.15f) {
        //converged
        res->hit = false;
        res->distance = Vector::magnitude(res->R);
        return 0;
    }

    if (containsOrigin(res->simplex[0].P, res->simplex[1].P, res->simplex[2].P, p.P)) {
        res->hit = true;
        res->distance = 0;
        res->simplex[3] = p;
        return 0;
    }

    int ret = collapseSimplex(res->simplex[0].P, res->simplex[1].P, res->simplex[2].P, p.P, 
            &res->simplex[0].P, &res->simplex[1].P, &res->simplex[2].P);
    if (ret == 0) {
        res->numNoConverge++;
        if (res->numNoConverge >= 10) {
            printf("Not converging\n");
            res->hit = false;
            res->distance =0;
            return 0;
        }
    }

    *search_dir = planeNormal(res->simplex[0].P, res->simplex[1].P, res->simplex[2].P);

    res->iterations++;
    return 1;
}

void gjk(GJK_Result* s, vec3 initial_guess) {
    vec3 search_dir = initial_guess;//{ 1, 2, 3 };
    vec3 f1 = {0,3,0};
    vec3 f2 = {3,5,0};
    vec3 b = gjk_support(search_dir, f1, f2);
    vec3 a = gjk_support(-search_dir, f1, f2);

    search_dir = Vector::cross(a, b - a);
    vec3 c = gjk_support(search_dir, f1, f2);

    int iteration = 0, maxIterations = 30;
    int numNoConverge = 0, maxNoConverge = 5;
    float tolerance = .1f;
    vec3 R;
    vec3 Rprev = { 1000,1000, 1000 };
    vec3 d;

    while (iteration < maxIterations) {
        search_dir = planeNormal(a, b, c);

        d = gjk_support(search_dir, f1, f2);
        Rprev = R;
        R = d;

        float differ = Vector::magnitude(Rprev - R);
        if (differ < 0.15f) {
            //converged
            break;
        }

        if (containsOrigin(a, b, c, d)) {
            s->hit = true;
            break;
        }

        int res = collapseSimplex(a, b, c, d, &a, &b, &c);
        if (res == 0) {
            numNoConverge++;
            if (numNoConverge >= maxNoConverge) {
                printf("Not converging\n");
            }
        }

        iteration++;
    }

    s->simplex[0].P = a;
    s->simplex[1].P = b;
    s->simplex[2].P = c;
    s->simplex[3].P = d;
    s->simplexSize = 4;
    s->iterations = iteration;
    s->distance = s->hit ? 0 : Vector::magnitude(R);
}

vec3 gjk_support(vec3 d, vec3 f1, vec3 f2) {
    vec3 v1 = Vector::normalized( d)*4 + f1;
    vec3 v2 = Vector::normalized(-d)*2 + f2;

    return v1 - v2;
}

int collapseSimplex(const vec3 a, const vec3 b, const vec3 c, const vec3 d,
    vec3* na, vec3* nb, vec3* nc) {

    int f = farthestPoint(a, b, c, d);
    switch (f) {
    case 1: {
        //a farthest
        *na = d;
        *nb = b;
        *nc = c;
        return 1;
    } break;
    case 2: {
        //b farthest
        *na = d;
        *nb = a;
        *nc = c;
        return 2;
    } break;
    case 3: {
        //c farthest
        *na = d;
        *nb = a;
        *nc = b;
        return 3;
    } break;
    case 4: {
        //d farthest
        *na = d;
        *nb = b;
        *nc = c;
        return 0;
    } break;
    }
    return 0;
}

vec3 planeNormal(vec3 a, vec3 b, vec3 c) {
    vec3 n;

    n = Vector::cross(b - a, c - a);
    vec3 k = (a + b + c) * (1.0f / 3.0f);

    if (Vector::dot(k, n) > 0)
        n = -n;

    return n;
}

int farthestPoint(vec3 a, vec3 b, vec3 c, vec3 d) {
    float m[4] = { Vector::magnitude(a),Vector::magnitude(b),
        Vector::magnitude(c), Vector::magnitude(d) };

    float max = 0;
    int im = -1;
    for (int i = 0; i < 4; i++) {
        if (m[i] > max) {
            max = m[i];
            im = i;
        }
    }

    return im + 1;
}

bool containsOrigin(vec3 a, vec3 b, vec3 c, vec3 d) {
    if (!testTri(a, b, c)) return false;
    if (!testTri(a, b, d)) return false;
    if (!testTri(b, c, d)) return false;
    if (!testTri(c, a, d)) return false;

    return true;
}

bool testTri(vec3 a, vec3 b, vec3 c) {
    //returns true if the origin projected 
    //on to triangle abc is contained

    //get a,b,c,p relative to a
    vec3 np = -a;
    vec3 na = a - a;
    vec3 nb = b - a;
    vec3 nc = c - a;

    //build triangle coord frame
    vec3 X = Vector::normalized(nb - na);
    vec3 Y = Vector::cross(nb - na, nc - na);
    vec3 Z = Vector::cross(X, Y);

    vec3 tp = { Vector::dot(X, np), Vector::dot(Y, np), Vector::dot(Z, np) }; tp.y = 0;
    vec3 ta = { Vector::dot(X, na), Vector::dot(Y, na), Vector::dot(Z, na) };
    vec3 tb = { Vector::dot(X, nb), Vector::dot(Y, nb), Vector::dot(Z, nb) };
    vec3 tc = { Vector::dot(X, nc), Vector::dot(Y, nc), Vector::dot(Z, nc) };

    return triContainsPoint(ta, tb, tc, tp);
}

bool triContainsPoint(vec3 a, vec3 b, vec3 c, vec3 p) {
    // returns true if the triangle abc contains the point p
    // only valid if a,b,c,p are coplanar
    vec3 v0 = b - a;
    vec3 v1 = c - a;
    vec3 v2 = p - a;

    float d00 = Vector::dot(v0, v0);
    float d01 = Vector::dot(v0, v1);
    float d11 = Vector::dot(v1, v1);
    float d20 = Vector::dot(v2, v0);
    float d21 = Vector::dot(v2, v1);

    float denom = d00 * d11 - d01 * d01;

    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    //return ((u > 1 || v > 1 || w > 1) || (u < 0 || v < 0 || w < 0));
    return ((u < 1 && v < 1 && w < 1) && (u > 0 && v > 0 && w > 0));
}