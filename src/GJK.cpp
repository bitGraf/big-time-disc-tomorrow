#include "GJK.h"
#include <assert.h>

int gjk_iteration(GJK_Struct* res, GJK_SupportPoint p, vec3* search_dir, bool verbose) {
    if (verbose) {
        printf("\n\n***GJK iteration [%-2d]***\n", res->iteration);
        search_dir->print("   Search direction: ");
        p.P.print("   p = ");
        p.a.print("    .a = ");
        p.b.print("    .b = ");
        printf("    aid = %d\n    bid = %d\n", p.aid, p.bid);
    }

    if (res->iteration >= GJK_MAX_ITERATIONS) {
        printf("Failed to converge after %d iterations.\n", res->iteration);
    }

    /* Initialize */
    if (res->cnt == 0) {
        res->D = GJK_FLT_MAX;
        res->hit = false;
    }

    /* Check for duplicates */
    float dc = Vector::dot(*search_dir, p.P);
    for (int i = 0; i < res->cnt; i++) {
        //if (p.aid != res->simplex[i].aid) continue;
        //if (p.bid != res->simplex[i].bid) continue;
        float di = Vector::dot(*search_dir, res->simplex[i].P);
        if (fabs(di-dc) > .01) continue;
        if (verbose) printf("Duplicate found.\n");
        return 0;
    }

    /* Add new point to simplex */
    res->simplex[res->cnt] = p;
    res->bc[res->cnt++] = 1.0f;

    /* Find closest element of simplex */
    switch(res->cnt) {
        case 1: {
            /* Point */
            if (verbose) printf("   1 point in simplex\n    Taking point A\n");
        } break;
        case 2: {
            /* Line Segment */
            if (verbose) printf("   2 points in simplex\n");
            vec3 a = res->simplex[0].P;
            vec3 b = res->simplex[1].P;

            float u = Vector::dot(b, b-a);
            float v = Vector::dot(a, a-b);

            if (v <= 0) {
                /* Region A */
                if (verbose) printf("    Taking point A\n");
                res->bc[0] = 1.0f;
                res->cnt = 1;
                break;
            }

            if (u <= 0) {
                /* Region B */
                if (verbose) printf("    Taking point B\n");
                res->simplex[0] = res->simplex[1];
                res->bc[0] = 1.0f;
                res->cnt = 1;
                break;
            }
            /* Region AB */
            if (verbose) printf("    Taking line AB\n");
            res->bc[0] = u;
            res->bc[1] = v;
            res->cnt = 2;
        } break;
        case 3: {
            /* Triangle */
            if (verbose) printf("   3 Points in simplex\n");
            vec3 a = res->simplex[0].P;
            vec3 b = res->simplex[1].P;
            vec3 c = res->simplex[2].P;

            vec3 ab = a - b;
            vec3 ba = b - a;
            vec3 bc = b - c;
            vec3 cb = c - b;
            vec3 ca = c - a;
            vec3 ac = a - c;

            float u_ab = Vector::dot(b, ba);
            float v_ab = Vector::dot(a, ab);

            float u_bc = Vector::dot(c, cb);
            float v_bc = Vector::dot(b, bc);

            float u_ca = Vector::dot(a, ac);
            float v_ca = Vector::dot(c, ca);

            if (v_ab <= 0 && u_ca <= 0) {
                /* Region A */
                if (verbose) printf("    Taking point A\n");
                res->bc[0] = 1.0f;
                res->cnt = 1;
                break;
            }
            if (u_ab <= 0 && v_bc <= 0) {
                /* Region B */
                if (verbose) printf("    Taking point B\n");
                res->simplex[0] = res->simplex[1];
                res->bc[0] = 1.0f;
                res->cnt = 1;
                break;
            }

            if (u_bc <= 0 && v_ca <= 0) {
                /* Region C */
                if (verbose) printf("    Taking point C\n");
                res->simplex[0] = res->simplex[2];
                res->bc[0] = 1.0f;
                res->cnt = 1;
                break;
            }

            /* Calculate fractional area */
            vec3 n  = Vector::cross(ba, ca);
            vec3 n1 = Vector::cross(b, c);
            vec3 n2 = Vector::cross(c, a);
            vec3 n3 = Vector::cross(a, b);

            float u_abc = Vector::dot(n1, n);
            float v_abc = Vector::dot(n2, n);
            float w_abc = Vector::dot(n3, n);

            if (u_ab > 0 && v_ab > 0 && w_abc <= 0) {
                /* Region AB */
                if (verbose) printf("    Taking line AB\n");
                res->bc[0] = u_ab;
                res->bc[1] = v_ab;
                res->cnt = 2;
                break;
            }
            if (u_bc > 0 && v_bc > 0 && u_abc <= 0) {
                /* Region BC */
                if (verbose) printf("    Taking line BC\n");
                res->simplex[0] = res->simplex[1];
                res->simplex[1] = res->simplex[2];
                res->bc[0] = u_bc;
                res->bc[1] = v_bc;
                res->cnt = 2;
                break;
            }

            if (u_ca > 0 && v_ca > 0 && v_abc <= 0) {
                /* Region CA */
                if (verbose) printf("    Taking line CA\n");
                res->simplex[1] = res->simplex[0];
                res->simplex[0] = res->simplex[2];
                res->bc[0] = u_ca;
                res->bc[1] = v_ca;
                res->cnt = 2;
                break;
            }

            /* Region ABC */
            if (verbose) printf("    Taking triangle ABC\n");
            if (!(u_abc > 0.0f && v_abc > 0.0f && w_abc > 0.0f)) printf("Wuh oh\n");
            res->bc[0] = u_abc;
            res->bc[1] = v_abc;
            res->bc[2] = w_abc;
            res->cnt = 3;
        } break;
        case 4: {
            /* Tetrahedron */
            if (verbose) printf("   4 Points in simplex\n");
            vec3 a = res->simplex[0].P;
            vec3 b = res->simplex[1].P;
            vec3 c = res->simplex[2].P;
            vec3 d = res->simplex[3].P;

            float u_ab = Vector::dot(b, b-a);
            float v_ab = Vector::dot(a, a-b);

            float u_bc = Vector::dot(c, c-b);
            float v_bc = Vector::dot(b, b-c);

            float u_ca = Vector::dot(a, a-c);
            float v_ca = Vector::dot(c, c-a);

            float u_dc = Vector::dot(c, c-d);
            float v_dc = Vector::dot(d, d-c);

            float u_bd = Vector::dot(d, d-b);
            float v_bd = Vector::dot(b, b-d);

            float u_ad = Vector::dot(d, d-a);
            float v_ad = Vector::dot(a, a-d);

            /* Check vertices as closest point */
            if (v_ab <= 0 && u_ca <= 0 && v_ad <= 0) {
                /* Region A */
                if (verbose) printf("    Taking point A\n");
                res->bc[0] = 1.0f;
                res->cnt = 1;
                break;
            }

            if (u_ab <= 0 && v_bc <= 0 && v_bd <= 0) {
                /* Region B */
                if (verbose) printf("    Taking point B\n");
                res->simplex[0] = res->simplex[1];
                res->bc[0] = 1.0f;
                res->cnt = 1;
                break;
            }

            if (u_bc <= 0 && v_ca <= 0 && u_dc <= 0) {
                /* Region C */
                if (verbose) printf("    Taking point C\n");
                res->simplex[0] = res->simplex[2];
                res->bc[0] = 1.0f;
                res->cnt = 1;
                break;
            }

            if (u_bd <= 0 && v_dc <= 0 && u_ad <= 0) {
                /* Region D */
                if (verbose) printf("    Taking point D\n");
                res->simplex[0] = res->simplex[3];
                res->bc[0] = 1.0f;
                res->cnt = 1;
                break;
            }

            /* calculate fractional area */

            vec3 n  = Vector::cross(d-a, b-a);
            vec3 n1 = Vector::cross(d, b);
            vec3 n2 = Vector::cross(b, a);
            vec3 n3 = Vector::cross(a, d);

            float u_adb = Vector::dot(n1, n);
            float v_adb = Vector::dot(n2, n);
            float w_adb = Vector::dot(n3, n);

            n  = Vector::cross(c-a, d-a);
            n1 = Vector::cross(c, d);
            n2 = Vector::cross(d, a);
            n3 = Vector::cross(a, c);

            float u_acd = Vector::dot(n1, n);
            float v_acd = Vector::dot(n2, n);
            float w_acd = Vector::dot(n3, n);

            n  = Vector::cross(b-c, d-c);
            n1 = Vector::cross(b, d);
            n2 = Vector::cross(d, c);
            n3 = Vector::cross(c, b);

            float u_cbd = Vector::dot(n1, n);
            float v_cbd = Vector::dot(n2, n);
            float w_cbd = Vector::dot(n3, n);

            n  = Vector::cross(b-a, c-a);
            n1 = Vector::cross(b, c);
            n2 = Vector::cross(c, a);
            n3 = Vector::cross(a, b);

            float u_abc = Vector::dot(n1, n);
            float v_abc = Vector::dot(n2, n);
            float w_abc = Vector::dot(n3, n);

            /* Check edges for closest edge */
            if (w_abc <= 0 && v_adb <= 0 && u_ab > 0 && v_ab > 0) {
                /* Region AB */
                if (verbose) printf("    Taking line AB\n");
                res->bc[0] = u_ab;
                res->bc[1] = v_ab;
                res->cnt = 2;
                break;
            }

            if (u_abc <= 0 && w_cbd <= 0 && u_bc > 0 && v_bc > 0) {
                /* Region BC */
                if (verbose) printf("    Taking line BC\n");
                res->simplex[0] = res->simplex[1];
                res->simplex[1] = res->simplex[2];
                res->bc[0] = u_bc;
                res->bc[1] = v_bc;
                res->cnt = 2;
                break;
            }

            if (v_abc <= 0 && w_acd <= 0 && u_ca > 0 && v_ca > 0) {
                /* Region CA */
                if (verbose) printf("    Taking line CA\n");
                res->simplex[1] = res->simplex[0];
                res->simplex[0] = res->simplex[2];
                res->bc[0] = u_ca;
                res->bc[1] = v_ca;
                res->cnt = 2;
                break;
            }

            if (v_cbd <= 0 && u_acd <= 0 && u_dc > 0 && v_dc > 0) {
                /* Region DC */
                if (verbose) printf("    Taking line DC\n");
                res->simplex[0] = res->simplex[3];
                res->simplex[1] = res->simplex[2];
                res->bc[0] = u_dc;
                res->bc[1] = v_dc;
                res->cnt = 2;
                break;
            }

            if (v_acd <= 0 && w_adb <= 0 && u_ad > 0 && v_ad > 0) {
                /* Region AD */
                if (verbose) printf("    Taking line AD\n");
                res->simplex[1] = res->simplex[3];
                res->bc[0] = u_ad;
                res->bc[1] = v_ad;
                res->cnt = 2;
                break;
            }

            if (u_cbd <= 0 && u_adb <= 0 && u_bd > 0 && v_bd > 0) {
                /* Region BD */
                if (verbose) printf("    Taking line BD\n");
                res->simplex[0] = res->simplex[1];
                res->simplex[1] = res->simplex[3];
                res->bc[0] = u_bd;
                res->bc[1] = v_bd;
                res->cnt = 2;
                break;
            }

            /* calculate fractional volume (can be negative) */
            float denom = f3box(c-b, a-b, d-b);
            float volume = (denom == 0) ? 1.0f: 1.0f/denom;
            float u_abcd = f3box(c, d, b) * volume;
            float v_abcd = f3box(c, a, d) * volume;
            float w_abcd = f3box(d, a, b) * volume;
            float x_abcd = f3box(b, a, c) * volume;

            /* check faces for closest point */
            if (x_abcd <= 0 && u_abc > 0 && v_abc > 0 && w_abc > 0) {
                /* Region ABC */
                if (verbose) printf("    Taking triangle ABC\n");
                res->bc[0] = u_abc;
                res->bc[1] = v_abc;
                res->bc[2] = w_abc;
                res->cnt = 3;
                break;
            }
            if (u_abcd <= 0 && u_cbd > 0 && v_cbd > 0 && w_cbd > 0) {
                /* Region CBD */
                if (verbose) printf("    Taking triangle CBD\n");
                res->simplex[0] = res->simplex[2];
                res->simplex[2] = res->simplex[3];
                res->bc[0] = u_cbd;
                res->bc[1] = v_cbd;
                res->bc[2] = w_cbd;
                res->cnt = 3;
                break;
            }
            if (v_abcd <= 0 && u_acd > 0 && v_acd > 0 && w_acd > 0) {
                /* Region ACD */
                if (verbose) printf("    Taking triangle ACD\n");
                res->simplex[1] = res->simplex[2];
                res->simplex[2] = res->simplex[3];
                res->bc[0] = u_acd;
                res->bc[1] = v_acd;
                res->bc[2] = w_acd;
                res->cnt = 3;
                break;
            }
            if (w_abcd <= 0 && u_adb > 0 && v_adb > 0 && w_adb > 0) {
                /* Region ADB */
                if (verbose) printf("    Taking triangle ADB\n");
                res->simplex[2] = res->simplex[1];
                res->simplex[1] = res->simplex[3];
                res->bc[0] = u_adb;
                res->bc[1] = v_adb;
                res->bc[2] = w_adb;
                res->cnt = 3;
                break;
            }
            /* Region ABCD */
            if (verbose) printf("    Taking tetrahedron ABCD\n");
            res->bc[0] = u_abcd;
            res->bc[1] = v_abcd;
            res->bc[2] = w_abcd;
            res->bc[3] = x_abcd;
            res->cnt = 4;
        } break;
    }

    /* check if origin is contained */
    if (res->cnt == 4) {
        res->hit = true;
        printf("Origin contained.\n");
        return 0;
    }

    /* Ensure closing in on origin */
    vec3 pnt;
    float denom = 0;
    for (int i = 0; i < res->cnt; ++i) {
        denom += res->bc[i];
    }
    denom = 1.0f/denom;
    switch(res->cnt) {
        case 1: {
            /* Single point */
            pnt = res->simplex[0].P;
        } break;
        case 2: {
            /* Line Segment */
            vec3 a = res->simplex[0].P*denom*res->bc[0];
            vec3 b = res->simplex[1].P*denom*res->bc[1];
            pnt = a + b;
        } break;
        case 3: {
            /* Triangle */
            vec3 a = res->simplex[0].P*denom*res->bc[0];
            vec3 b = res->simplex[1].P*denom*res->bc[1];
            vec3 c = res->simplex[2].P*denom*res->bc[2];
            pnt = a + b + c;
        } break;
        case 4: {
            /* Tetrahedron */
            printf("Pretty cringe tbh...\n");
            vec3 a = res->simplex[0].P*denom*res->bc[0];
            vec3 b = res->simplex[1].P*denom*res->bc[1];
            vec3 c = res->simplex[2].P*denom*res->bc[2];
            vec3 d = res->simplex[3].P*denom*res->bc[3];
            pnt = a + b + c + d;
        } break;
    }

    float d2 = Vector::dot(pnt, pnt);
    if (d2 >= res->D) {
        printf("Leaving the origin?\n");
        //return 0;
    }
    res->D = d2;

    /* Find new search direction */
    switch(res->cnt) {
        case 1: {
            /* Point */
            if (verbose) printf("point\n    Taking opposite of point\n");
            *search_dir = -res->simplex[0].P;
        } break;
        case 2: {
            /* Line Segment */
            if (verbose) printf("line\n    Taking normal to line\n");
            vec3 ba = res->simplex[1].P-res->simplex[0].P;
            vec3 t = Vector::cross(ba, -res->simplex[1].P);
            *search_dir = Vector::cross(t, ba);
        } break;
        case 3: {
            /* Triangle */
            if (verbose) printf("triangle\n    Taking normal to triangle\n");
            vec3 ab = res->simplex[1].P - res->simplex[0].P;
            vec3 ac = res->simplex[2].P - res->simplex[0].P;
            vec3 n = Vector::cross(ab, ac);
            if (Vector::dot(n, res->simplex[0].P) <= 0)
                *search_dir = n;
            else
                *search_dir = -n;
        } break;
        default:
            // ! This should never occur...
            break;
    }

    vec3 nd = *search_dir;
    if (verbose) nd.print("    New search direction: ");
    if (Vector::dot(nd, nd) < ((1e-7)*(1e-7))) {
        /* Both shapes are just touching */
        res->hit = true;
        printf("Shapes are touching\n");
        return 0;
    }

    res->iteration++;
    return 1;
}

float f3box(vec3 a, vec3 b, vec3 c) {
    vec3 n = Vector::cross(a, b);
    return Vector::dot(n, c);
}

void gjk_processResults(GJK_Struct* res) {
    /* calculate normalization denominator */
    float denom = 0;
    for (int i = 0; i < res->cnt; i++) {
        denom += res->bc[i];
    }
    denom = 1.0f / denom;

    /* Find closest points */
    switch(res->cnt) {
        case 1: {
            /* Single point */
            res->P0 = res->simplex[0].a;
            res->P1 = res->simplex[0].b;
        } break;
        case 2: {
            /* Line segment */
            float as = denom * res->bc[0];
            float bs = denom * res->bc[1];

            vec3 a = res->simplex[0].a * as;
            vec3 b = res->simplex[1].a * bs;

            vec3 c = res->simplex[0].b * as;
            vec3 d = res->simplex[1].b * bs;

            res->P0 = a + b;
            res->P1 = c + d;
        } break;
        case 3: {
            /* Triangle */
            float as = denom * res->bc[0];
            float bs = denom * res->bc[1];
            float cs = denom * res->bc[2];

            vec3 a = res->simplex[0].a * as;
            vec3 b = res->simplex[1].a * bs;
            vec3 c = res->simplex[2].a * cs;

            vec3 d = res->simplex[0].b * as;
            vec3 e = res->simplex[1].b * bs;
            vec3 f = res->simplex[2].b * cs;

            res->P0 = a + b + c;
            res->P1 = d + e + f;
        } break;
        case 4: {
            vec3 a = res->simplex[0].a*(denom*res->bc[0]);
            vec3 b = res->simplex[1].a*(denom*res->bc[1]);
            vec3 c = res->simplex[2].a*(denom*res->bc[2]);
            vec3 d = res->simplex[3].a*(denom*res->bc[3]);

            res->P0 = a + b + c + d;
            res->P1 = res->P0;
        } break;
    }
    /* If not hit, find distance */
    if (!res->hit) {
        vec3 d = res->P1 - res->P0;
        res->distance = sqrt(Vector::dot(d,d));
    } else {
        res->distance = 0;
    }
}

void gjk_processRadius(GJK_Struct* res, float r1, float r2) {
    float combinedRadius = r1 + r2;
    float signedDistance = res->distance;
    if (signedDistance > combinedRadius) {
        signedDistance -= combinedRadius; //the distance between the expanded points

        res->distance = signedDistance;

        /* calculate normal */
        vec3 n = res->P1 - res->P0;
        Vector::normalize(n);

        vec3 da = n*r1;
        vec3 db = n*r2;

        /* New collision points */
        res->P0 = res->P0 + da;
        res->P1 = res->P1 - db;
    } else {
        vec3 p1 = res->P0;
        vec3 p2 = res->P1;
        res->P0 = p2*(r1/(r1+r2)) + p1*(r2/(r1+r2));
        res->P1 = res->P0;
        res->distance = 0;
        res->hit = true;
    }
}