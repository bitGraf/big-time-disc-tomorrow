#include "GJK.h"
#include <assert.h>

int gjk_iteration(GJK_Result* res, GJK_SupportPoint p, vec3* search_dir) {
    res->result = p;
    // Add new point to simplex
        res->simplex.d = res->simplex.c;
        res->simplex.c = res->simplex.b;
        res->simplex.b = res->simplex.a;
        res->simplex.a = p;
        res->cnt++;
    // * res->simplex.a is ALWAYS the most recent point to be added

    if (res->cnt > 1) { /* more than one point in simplex */
        //check if converged on a solution
        float da = Vector::dot(*search_dir, res->simplex.a.P);
        float db = Vector::dot(*search_dir, res->simplex.b.P);
        float convergence = da - db;

        convergence = fabs(Vector::magnitude(res->simplex.a.P - res->simplex.b.P));
        //printf("res = %.5f\n", convergence);
        if (convergence < .005) {
            //Solution has converged
            res->converged = true;
            res->hit = false;
            //printf("Converged after %d iteration\n", res->iteration);
            return 0;
        }
    }

    //create new simplex
    res->cnt = buildNewSimplex(&res->simplex, res->cnt);

    //check if origin is contained
    if (res->cnt == 4) {
        res->hit = true;
        res->converged = true;
        return 0;
    }

    //find new search direction
    switch(res->cnt) {
        case 1: {
            /* Point */
            *search_dir = -res->simplex.a.P;
        } break;
        case 2: {
            /* Line Segment */
            vec3 ba = res->simplex.b.P-res->simplex.a.P;
            vec3 t = Vector::cross(ba, -res->simplex.b.P);
            *search_dir = Vector::cross(t, ba);
        } break;
        case 3: {
            /* Triangle */
            vec3 ab = res->simplex.b.P - res->simplex.a.P;
            vec3 ac = res->simplex.c.P - res->simplex.a.P;
            vec3 n = Vector::cross(ab, ac);
            if (Vector::dot(n, res->simplex.a.P) <= 0)
                *search_dir = n;
            else
                *search_dir = -n;
        } break;
        default:
            // ! This should never occur...
            break;
    }

    if (Vector::dot(*search_dir, *search_dir) < (1e-7)*(1e-7)) {
        /* Both shapes are just touching */
        res->hit = true;
        res->converged = true;
        return 0;
    }

    res->iteration++;
    res->converged = false;
    res->hit = false;
    if (res->iteration == GJK_MAX_ITERATIONS+1) {
        printf("Failed to converge after %d iterations.\n", GJK_MAX_ITERATIONS);
        return 0;
    }
    return 1;
}

int buildNewSimplex(GJK_Simplex* simplex, int cnt) {
    switch(cnt) {
        default: {
            return 0;
        } break;
        case 1: {
            /* Point */
            return 1;
        } break;
        case 2: {
            /* Line Segment */
            float u = Vector::dot(simplex->b.P, simplex->b.P-simplex->a.P);
            float v = Vector::dot(simplex->a.P, simplex->a.P-simplex->b.P);

            if (v <= 0) {
                /* Region A */
                simplex->b = {0};
                return 1;
            }

            if (u <= 0) {
                /* Region B */
                simplex->a = simplex->b;
                simplex->b = {0};
                return 1;
            }

            /* Region AB */
            return 2;
        } break;
        case 3: {
            /* Triangle */
            vec3 ab = simplex->a.P - simplex->b.P;
            vec3 ba = simplex->b.P - simplex->a.P;
            vec3 bc = simplex->b.P - simplex->c.P;
            vec3 cb = simplex->c.P - simplex->b.P;
            vec3 ca = simplex->c.P - simplex->a.P;
            vec3 ac = simplex->a.P - simplex->c.P;

            float u_ab = Vector::dot(simplex->b.P, ba);
            float v_ab = Vector::dot(simplex->a.P, ab);

            float u_bc = Vector::dot(simplex->c.P, cb);
            float v_bc = Vector::dot(simplex->b.P, bc);

            float u_ca = Vector::dot(simplex->a.P, ac);
            float v_ca = Vector::dot(simplex->c.P, ca);

            if (v_ab <= 0 && u_ca <= 0) {
                /* Region A */
                simplex->b = {0};
                simplex->c = {0};
                return 1;
            }

            if (u_ab <= 0 && v_bc <= 0) {
                /* Region B */
                simplex->a = simplex->b;
                simplex->b = {0};
                simplex->c = {0};
                return 1;
            }

            if (u_bc <= 0 && v_ca <= 0) {
                /* Region C */
                simplex->a = simplex->c;
                simplex->b = {0};
                simplex->c = {0};
                return 1;
            }

            /* Calculate fractional area */
            vec3 n = Vector::cross(ba, ca);
            vec3 n1 = Vector::cross(simplex->b.P, simplex->c.P);
            vec3 n2 = Vector::cross(simplex->c.P, simplex->a.P);
            vec3 n3 = Vector::cross(simplex->a.P, simplex->b.P);

            float u_abc = Vector::dot(n1, n);
            float v_abc = Vector::dot(n2, n);
            float w_abc = Vector::dot(n3, n);

            if (u_ab > 0 && v_ab > 0 && w_abc < 0) {
                /* Region AB */
                simplex->c = {0};
                return 2;
            }

            if (u_bc > 0 && v_bc > 0 && u_abc < 0) {
                /* Region BC */
                simplex->a = simplex->b;
                simplex->b = simplex->c;
                simplex->c = {0};
                return 2;
            }

            if (u_ca > 0 && v_ca > 0 && w_abc < 0) {
                /* Region CA */
                simplex->b = simplex->c;
                simplex->c = {0};
                return 2;
            }

            /* Region ABC */
            return 3;
        } break;
        case 4: {
            /* Tetrahedron */
            float u_ab = Vector::dot(simplex->b.P, simplex->b.P-simplex->a.P);
            float v_ab = Vector::dot(simplex->a.P, simplex->a.P-simplex->b.P);

            float u_bc = Vector::dot(simplex->c.P, simplex->c.P-simplex->b.P);
            float v_bc = Vector::dot(simplex->b.P, simplex->b.P-simplex->c.P);

            float u_ca = Vector::dot(simplex->a.P, simplex->a.P-simplex->c.P);
            float v_ca = Vector::dot(simplex->c.P, simplex->c.P-simplex->a.P);

            float u_dc = Vector::dot(simplex->c.P, simplex->c.P-simplex->d.P);
            float v_dc = Vector::dot(simplex->d.P, simplex->d.P-simplex->c.P);

            float u_bd = Vector::dot(simplex->d.P, simplex->d.P-simplex->b.P);
            float v_bd = Vector::dot(simplex->b.P, simplex->b.P-simplex->d.P);

            float u_ad = Vector::dot(simplex->d.P, simplex->d.P-simplex->a.P);
            float v_ad = Vector::dot(simplex->a.P, simplex->a.P-simplex->d.P);

            /* Check vertices as closest point */
            if (v_ab <= 0 && u_ca <= 0 && v_ad <= 0) {
                /* Region A */
                simplex->b = {0};
                simplex->c = {0};
                simplex->d = {0};
                return 1;
            }

            if (v_ab <= 0 && v_bc <= 0 && v_bd <= 0) {
                /* Region B */
                simplex->a = simplex->b;
                simplex->b = {0};
                simplex->c = {0};
                simplex->d = {0};
                return 1;
            }

            if (u_bc <= 0 && v_ca <= 0 && u_dc <= 0) {
                /* Region C */
                simplex->a = simplex->c;
                simplex->b = {0};
                simplex->c = {0};
                simplex->d = {0};
                return 1;
            }

            if (u_bd <= 0 && v_dc <= 0 && u_ad <= 0) {
                /* Region D */
                simplex->a = simplex->d;
                simplex->b = {0};
                simplex->c = {0};
                simplex->d = {0};
                return 1;
            }

            /* calculate fractional area */

            vec3 n  = Vector::cross(simplex->d.P-simplex->a.P, simplex->b.P-simplex->a.P);
            vec3 n1 = Vector::cross(simplex->d.P, simplex->b.P);
            vec3 n2 = Vector::cross(simplex->b.P, simplex->a.P);
            vec3 n3 = Vector::cross(simplex->a.P, simplex->d.P);

            float u_adb = Vector::dot(n1, n);
            float v_adb = Vector::dot(n2, n);
            float w_adb = Vector::dot(n3, n);

            n  = Vector::cross(simplex->c.P-simplex->a.P, simplex->d.P-simplex->a.P);
            n1 = Vector::cross(simplex->c.P, simplex->d.P);
            n2 = Vector::cross(simplex->d.P, simplex->a.P);
            n3 = Vector::cross(simplex->a.P, simplex->c.P);

            float u_acd = Vector::dot(n1, n);
            float v_acd = Vector::dot(n2, n);
            float w_acd = Vector::dot(n3, n);

            n  = Vector::cross(simplex->b.P-simplex->c.P, simplex->d.P-simplex->c.P);
            n1 = Vector::cross(simplex->b.P, simplex->d.P);
            n2 = Vector::cross(simplex->d.P, simplex->c.P);
            n3 = Vector::cross(simplex->c.P, simplex->b.P);

            float u_cbd = Vector::dot(n1, n);
            float v_cbd = Vector::dot(n2, n);
            float w_cbd = Vector::dot(n3, n);

            n  = Vector::cross(simplex->b.P-simplex->a.P, simplex->c.P-simplex->a.P);
            n1 = Vector::cross(simplex->b.P, simplex->c.P);
            n2 = Vector::cross(simplex->c.P, simplex->a.P);
            n3 = Vector::cross(simplex->a.P, simplex->b.P);

            float u_abc = Vector::dot(n1, n);
            float v_abc = Vector::dot(n2, n);
            float w_abc = Vector::dot(n3, n);

            /* Check edges for closest edge */
            if (w_abc <= 0 && v_adb <= 0 && u_ab > 0 && v_ab > 0) {
                /* Region AB */
                simplex->c = {0};
                simplex->d = {0};
                return 2;
            }

            if (u_abc <= 0 && w_cbd <= 0 && u_bc > 0 && v_bc > 0) {
                /* Region BC */
                simplex->a = simplex->b;
                simplex->b = simplex->c;
                simplex->c = {0};
                simplex->d = {0};
                return 2;
            }

            if (v_abc <= 0 && w_acd <= 0 && u_ca > 0 && v_ca > 0) {
                /* Region CA */
                simplex->b = simplex->c;
                simplex->c = {0};
                simplex->d = {0};
                return 2;
            }

            if (v_cbd <= 0 && u_acd <= 0 && u_dc > 0 && v_dc > 0) {
                /* Region DC */
                simplex->b = simplex->c;
                simplex->c = {0};
                simplex->d = {0};
                return 2;
            }

            if (v_acd <= 0 && w_adb <= 0 && u_ad > 0 && v_ad > 0) {
                /* Region AD */
                simplex->b = simplex->d;
                simplex->c = {0};
                simplex->d = {0};
                return 2;
            }

            if (u_cbd <= 0 && u_adb <= 0 && u_bd > 0 && v_bd > 0) {
                /* Region BD */
                simplex->a = simplex->b;
                simplex->b = simplex->d;
                simplex->c = {0};
                simplex->d = {0};
                return 2;
            }

            /* calculate fractional volume (can be negative) */
            float denom = f3box(simplex->c.P-simplex->b.P, simplex->a.P-simplex->b.P, simplex->d.P-simplex->d.b);
            float volume = (denom == 0) ? 1.0f: 1.0f/denom;
            float u_abcd = f3box(simplex->c.P, simplex->d.P, simplex->b.P) * volume;
            float v_abcd = f3box(simplex->c.P, simplex->a.P, simplex->d.P) * volume;
            float w_abcd = f3box(simplex->d.P, simplex->a.P, simplex->b.P) * volume;
            float x_abcd = f3box(simplex->b.P, simplex->a.P, simplex->c.P) * volume;

            /* check faces for closest point */
            if (x_abcd <= 0 && u_abc > 0 && v_abc > 0 && w_abc > 0) {
                /* Region ABC */
                simplex->d = {0};
                return 3;
            }
            
            if (u_abcd <= 0 && u_cbd > 0 && v_cbd > 0 && w_cbd > 0) {
                /* Region CBD */
                simplex->a = simplex->b;
                simplex->b = simplex->c;
                simplex->c = simplex->d;
                simplex->d = {0};
                return 3;
            }

            if (v_abcd <= 0 && u_acd > 0 && v_acd > 0 && w_acd > 0) {
                /* Region ACD */
                simplex->b = simplex->c;
                simplex->c = simplex->d;
                simplex->d = {0};
                return 3;
            }

            if (w_abcd <= 0 && u_adb > 0 && v_adb > 0 && w_adb > 0) {
                /* Region ADB */
                simplex->c = simplex->d;
                simplex->d = {0};
                return 3;
            }

            /* Region ABCD */
            return 4;
        } break;
    }
}

float f3box(vec3 a, vec3 b, vec3 c) {
    vec3 n = Vector::cross(a, b);
    return Vector::dot(n, c);
}

int findSmallest(float a, float b, float c, float d) {
    float v[4] = {a, b, c, d};

    int id = -1;
    float vs = 1e32;

    for (int i = 0; i < 4; i++) {
        if (v[i] < vs) {
            vs = v[i];
            id = i;
        }
    }

    return id;
}

// Vector normal to ABC that points away from s
vec3 planeNormal(vec3 a, vec3 b, vec3 c, vec3 s) {
    vec3 n = Vector::normalized(Vector::cross(c-a, b-a));
    if (Vector::dot(a-s, n) < 0)
        n = -n;
    return n;
}

// Vector normal to AB that points away from s
vec3 lineSegmentNormal(vec3 a, vec3 b, vec3 s) {
    vec3 n_ab = Vector::normalized(Vector::cross(b-a, Vector::cross(s-a, b-a)));
    if (Vector::dot(s-a, n_ab) > 0) {
        n_ab = -n_ab;
    }
    return n_ab;
}

int findClosestFace(GJK_Simplex* simplex, vec3 center) {
    float val[4];

    vec3 a = simplex->a.P;
    vec3 b = simplex->b.P;
    vec3 c = simplex->c.P;
    vec3 d = simplex->d.P;

    //ABC
    val[0] = Vector::dot(a, getOuterFaceNormal(a, b, c, center));
    //ABD
    val[1] = Vector::dot(a, getOuterFaceNormal(a, b, d, center));
    //BCD
    val[2] = Vector::dot(b, getOuterFaceNormal(b, c, d, center));
    //CAD
    val[3] = Vector::dot(c, getOuterFaceNormal(c, a, d, center));

    float goodVal = 1e20;
    int goodID = -1;
    for (int i = 0; i < 4; i++) {
        if (val[i] < goodVal) {
            goodVal = val[i];
            goodID = i;
        }
    }

    return goodID;
}

vec3 getOuterFaceNormal(vec3 a, vec3 b, vec3 c, vec3 center) {
    vec3 n = Vector::cross(b- a, c- a);
    vec3 k = (a + b + c) * (1.0f/3.0f);

    if (Vector::dot(n, k-center) < 0) {
        n = -n;
    }

    return n;
}

/*int gjk_iteration(GJK_Result* res, GJK_SupportPoint p, vec3* search_dir) {
    res->Rp = res->R;
    res->R = p;

    float differ = Vector::magnitude(res->Rp.P - res->R.P);
    if (differ < 0.15f) {
        //converged
        res->hit = false;
        res->distance = Vector::magnitude(res->R.P);
        res->converge = true;
        res->search_dir = *search_dir;
        return 0;
    }

    if (containsOrigin(res->simplex[0].P, res->simplex[1].P, res->simplex[2].P, p.P)) {
        res->hit = true;
        res->distance = 0;
        res->simplex[3] = p;
        res->converge = true;
        res->search_dir = *search_dir;
        return 0;
    }

    int ret = collapseSimplex(res->simplex[0], res->simplex[1], res->simplex[2], p, 
            &res->simplex[0], &res->simplex[1], &res->simplex[2]);
    if (ret == 0) {
        res->numNoConverge++;
        if (res->numNoConverge >= 10) {
            printf("Not converging\n");
            res->hit = false;
            res->distance =0;
            res->converge = false;
            res->search_dir = *search_dir;
            return 0;
        }
    }

    *search_dir = planeNormal(res->simplex[0].P, res->simplex[1].P, res->simplex[2].P);

    res->iterations++;
    return 1;
}
*/

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

/*
int collapseSimplex(const GJK_SupportPoint a, const GJK_SupportPoint b, const GJK_SupportPoint c, const GJK_SupportPoint d,
    GJK_SupportPoint* na, GJK_SupportPoint* nb, GJK_SupportPoint* nc) {

    int f = farthestPoint(a, b, c, d); //this is not correct
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

int farthestPoint(GJK_SupportPoint a, GJK_SupportPoint b, GJK_SupportPoint c, GJK_SupportPoint d) {
    float m[4] = { Vector::magnitude(a.P),Vector::magnitude(b.P),
        Vector::magnitude(c.P), Vector::magnitude(d.P) };

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
*/