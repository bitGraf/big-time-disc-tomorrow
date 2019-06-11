#include "GJK.h"
#include <assert.h>

#define GJK_FLT_MAX 3.40282347E+28F
#define GJK_EPSILON 1.19209290E-07F

static const float f3z[3];

static inline float
f3box(const vec3 a, const vec3 b, const vec3 c)
{
    vec3 n;
    n = Vector::cross(a, b);
    return Vector::dot(n, c);
}
static float
inv_sqrt(float n)
{
    union {unsigned u; float f;} conv; conv.f = n;
    conv.u = 0x5f375A84 - (conv.u >> 1);
    conv.f = conv.f * (1.5f - (n * 0.5f * conv.f * conv.f));
    return conv.f;
}
extern int
gjk(struct gjk_simplex *s, const struct gjk_support *sup, vec3 *dv)
{
    assert(s);
    assert(dv);
    assert(sup);
    if (!s || !sup || !dv) return 0;
    if (s->max_iter > 0 && s->iter >= s->max_iter)
        return 0;

    /* I.) Initialize */
    if (s->cnt == 0) {
        s->D = GJK_FLT_MAX;
        s->max_iter = !s->max_iter ? GJK_MAX_ITERATIONS: s->max_iter;
    }
    /* II.) Check for duplications */
    for (int i = 0; i < s->cnt; ++i) {
        if (sup->aid != s->v[i].aid) continue;
        if (sup->bid != s->v[i].bid) continue;
        return 0;
    }
    /* III.) Add vertex into simplex */
    gjk_vertex *vert = &(s->v[s->cnt]);
    vert->a = sup->a;
    vert->b = sup->b;
    vert->p = *dv;
    vert->aid = sup->aid;
    vert->bid = sup->bid;
    s->bc[s->cnt++] = 1.0f;

    /* IV.) Find closest simplex point */
    switch (s->cnt) {
    case 1: break;
    case 2: {
        /* -------------------- Line ----------------------- */
        vec3 a = s->v[0].p;
        vec3 b = s->v[1].p;

        /* compute barycentric coordinates */
        vec3 ab = a - b;
        vec3 ba = b - a;

        float u = Vector::dot(b, ba);
        float v = Vector::dot(a, ab);
        if (v <= 0.0f) {
            /* region A */
            s->bc[0] = 1.0f;
            s->cnt = 1;
            break;
        }
        if (u <= 0.0f) {
            /* region B */
            s->v[0] = s->v[1];
            s->bc[0] = 1.0f;
            s->cnt = 1;
            break;
        }
        /* region AB */
        s->bc[0] = u;
        s->bc[1] = v;
        s->cnt = 2;
    } break;
    case 3: {
        /* -------------------- Triangle ----------------------- */
        vec3 a = s->v[0].p;
        vec3 b = s->v[1].p;
        vec3 c = s->v[2].p;

        vec3 ab = a - b;
        vec3 ba = b - a;
        vec3 bc = b - c;
        vec3 cb = c - b;
        vec3 ca = c - a;
        vec3 ac = a - c;

        /* compute barycentric coordinates */
        float u_ab = Vector::dot(b, ba);
        float v_ab = Vector::dot(a, ab);

        float u_bc = Vector::dot(c, cb);
        float v_bc = Vector::dot(b, bc);

        float u_ca = Vector::dot(a, ac);
        float v_ca = Vector::dot(c, ca);

        if (v_ab <= 0.0f && u_ca <= 0.0f) {
            /* region A */
            s->bc[0] = 1.0f;
            s->cnt = 1;
            break;
        }
        if (u_ab <= 0.0f && v_bc <= 0.0f) {
            /* region B */
            s->v[0] = s->v[1];
            s->bc[0] = 1.0f;
            s->cnt = 1;
            break;
        }
        if (u_bc <= 0.0f && v_ca <= 0.0f) {
            /* region C */
            s->v[0] = s->v[2];
            s->bc[0] = 1.0f;
            s->cnt = 1;
            break;
        }
        /* calculate fractional area */
        vec3  n = Vector::cross(ba, ca);
        vec3 n1 = Vector::cross(b, c);
        vec3 n2 = Vector::cross(c, a);
        vec3 n3 = Vector::cross(a, b);

        float u_abc = Vector::dot(n1, n);
        float v_abc = Vector::dot(n2, n);
        float w_abc = Vector::dot(n3, n);

        if (u_ab > 0.0f && v_ab > 0.0f && w_abc <= 0.0f) {
            /* region AB */
            s->bc[0] = u_ab;
            s->bc[1] = v_ab;
            s->cnt = 2;
            break;
        }
        if (u_bc > 0.0f && v_bc > 0.0f && u_abc <= 0.0f) {
            /* region BC */
            s->v[0] = s->v[1];
            s->v[1] = s->v[2];
            s->bc[0] = u_bc;
            s->bc[1] = v_bc;
            s->cnt = 2;
            break;
        }
        if (u_ca > 0.0f && v_ca > 0.0f && v_abc <= 0.0f) {
            /* region CA */
            s->v[1] = s->v[0];
            s->v[0] = s->v[2];
            s->bc[0] = u_ca;
            s->bc[1] = v_ca;
            s->cnt = 2;
            break;
        }
        /* region ABC */
        assert(u_abc > 0.0f && v_abc > 0.0f && w_abc > 0.0f);
        s->bc[0] = u_abc;
        s->bc[1] = v_abc;
        s->bc[2] = w_abc;
        s->cnt = 3;
    } break;
    case 4: {
        /* -------------------- Tetrahedron ----------------------- */
        vec3 a = s->v[0].p;
        vec3 b = s->v[1].p;
        vec3 c = s->v[2].p;
        vec3 d = s->v[3].p;

        vec3 ab = a - b;
        vec3 ba = b - a;
        vec3 bc = b - c;
        vec3 cb = c - b;
        vec3 ca = c - a;
        vec3 ac = a - c;

        vec3 db = d - b;
        vec3 bd = b - d;
        vec3 dc = d - c;
        vec3 cd = c - d;
        vec3 da = d - a;
        vec3 ad = a - d;

        /* compute barycentric coordinates */
        float u_ab = Vector::dot(b, ba);
        float v_ab = Vector::dot(a, ab);

        float u_bc = Vector::dot(c, cb);
        float v_bc = Vector::dot(b, bc);

        float u_ca = Vector::dot(a, ac);
        float v_ca = Vector::dot(c, ca);

        float u_bd = Vector::dot(d, db);
        float v_bd = Vector::dot(b, bd);

        float u_dc = Vector::dot(c, cd);
        float v_dc = Vector::dot(d, dc);

        float u_ad = Vector::dot(d, da);
        float v_ad = Vector::dot(a, ad);

        /* check verticies for closest point */
        if (v_ab <= 0.0f && u_ca <= 0.0f && v_ad <= 0.0f) {
            /* region A */
            s->bc[0] = 1.0f;
            s->cnt = 1;
            break;
        }
        if (u_ab <= 0.0f && v_bc <= 0.0f && v_bd <= 0.0f) {
            /* region B */
            s->v[0] = s->v[1];
            s->bc[0] = 1.0f;
            s->cnt = 1;
            break;
        }
        if (u_bc <= 0.0f && v_ca <= 0.0f && u_dc <= 0.0f) {
            /* region C */
            s->v[0] = s->v[2];
            s->bc[0] = 1.0f;
            s->cnt = 1;
            break;
        }
        if (u_bd <= 0.0f && v_dc <= 0.0f && u_ad <= 0.0f) {
            /* region D */
            s->v[0] = s->v[3];
            s->bc[0] = 1.0f;
            s->cnt = 1;
            break;
        }
        /* calculate fractional area */
        vec3  n = Vector::cross(da, ba);
        vec3 n1 = Vector::cross(d, b);
        vec3 n2 = Vector::cross(b, a);
        vec3 n3 = Vector::cross(a, d);

        float u_adb = Vector::dot(n1, n);
        float v_adb = Vector::dot(n2, n);
        float w_adb = Vector::dot(n3, n);

        n  = Vector::cross(ca, da);
        n1 = Vector::cross(c, d);
        n2 = Vector::cross(d, a);
        n3 = Vector::cross(a, c);

        float u_acd = Vector::dot(n1, n);
        float v_acd = Vector::dot(n2, n);
        float w_acd = Vector::dot(n3, n);

        n  = Vector::cross(bc, dc);
        n1 = Vector::cross(b, d);
        n2 = Vector::cross(d, c);
        n3 = Vector::cross(c, b);

        float u_cbd = Vector::dot(n1, n);
        float v_cbd = Vector::dot(n2, n);
        float w_cbd = Vector::dot(n3, n);

        n  = Vector::cross(ba, ca);
        n1 = Vector::cross(b, c);
        n2 = Vector::cross(c, a);
        n3 = Vector::cross(a, b);

        float u_abc = Vector::dot(n1, n);
        float v_abc = Vector::dot(n2, n);
        float w_abc = Vector::dot(n3, n);

        /* check edges for closest point */
        if (w_abc <= 0.0f && v_adb <= 0.0f && u_ab > 0.0f && v_ab > 0.0f) {
            /* region AB */
            s->bc[0] = u_ab;
            s->bc[1] = v_ab;
            s->cnt = 2;
            break;
        }
        if (u_abc <= 0.0f && w_cbd <= 0.0f && u_bc > 0.0f && v_bc > 0.0f) {
            /* region BC */
            s->v[0] = s->v[1];
            s->v[1] = s->v[2];
            s->bc[0] = u_bc;
            s->bc[1] = v_bc;
            s->cnt = 2;
            break;
        }
        if (v_abc <= 0.0f && w_acd <= 0.0f && u_ca > 0.0f && v_ca > 0.0f) {
            /* region CA */
            s->v[1] = s->v[0];
            s->v[0] = s->v[2];
            s->bc[0] = u_ca;
            s->bc[1] = v_ca;
            s->cnt = 2;
            break;
        }
        if (v_cbd <= 0.0f && u_acd <= 0.0f && u_dc > 0.0f && v_dc > 0.0f) {
            /* region DC */
            s->v[0] = s->v[3];
            s->v[1] = s->v[2];
            s->bc[0] = u_dc;
            s->bc[1] = v_dc;
            s->cnt = 2;
            break;
        }
        if (v_acd <= 0.0f && w_adb <= 0.0f && u_ad > 0.0f && v_ad > 0.0f) {
            /* region AD */
            s->v[1] = s->v[3];
            s->bc[0] = u_ad;
            s->bc[1] = v_ad;
            s->cnt = 2;
            break;
        }
        if (u_cbd <= 0.0f && u_adb <= 0.0f && u_bd > 0.0f && v_bd > 0.0f) {
            /* region BD */
            s->v[0] = s->v[1];
            s->v[1] = s->v[3];
            s->bc[0] = u_bd;
            s->bc[1] = v_bd;
            s->cnt = 2;
            break;
        }
        /* calculate fractional volume (volume can be negative!) */
        float denom = f3box(cb, ab, db);
        float volume = (denom == 0) ? 1.0f: 1.0f/denom;
        float u_abcd = f3box(c, d, b) * volume;
        float v_abcd = f3box(c, a, d) * volume;
        float w_abcd = f3box(d, a, b) * volume;
        float x_abcd = f3box(b, a, c) * volume;

        /* check faces for closest point */
        if (x_abcd <= 0.0f && u_abc > 0.0f && v_abc > 0.0f && w_abc > 0.0f) {
            /* region ABC */
            s->bc[0] = u_abc;
            s->bc[1] = v_abc;
            s->bc[2] = w_abc;
            s->cnt = 3;
            break;
        }
        if (u_abcd <= 0.0f && u_cbd > 0.0f && v_cbd > 0.0f && w_cbd > 0.0f) {
            /* region CBD */
            s->v[0] = s->v[2];
            s->v[2] = s->v[3];
            s->bc[0] = u_cbd;
            s->bc[1] = v_cbd;
            s->bc[2] = w_cbd;
            s->cnt = 3;
            break;
        }
        if (v_abcd <= 0.0f && u_acd > 0.0f && v_acd > 0.0f && w_acd > 0.0f) {
            /* region ACD */
            s->v[1] = s->v[2];
            s->v[2] = s->v[3];
            s->bc[0] = u_acd;
            s->bc[1] = v_acd;
            s->bc[2] = w_acd;
            s->cnt = 3;
            break;
        }
        if (w_abcd <= 0.0f && u_adb > 0.0f && v_adb > 0.0f && w_adb > 0.0f) {
            /* region ADB */
            s->v[2] = s->v[1];
            s->v[1] = s->v[3];
            s->bc[0] = u_adb;
            s->bc[1] = v_adb;
            s->bc[2] = w_adb;
            s->cnt = 3;
            break;
        }
        /* region ABCD */
        assert(u_abcd > 0.0f && v_abcd > 0.0f && w_abcd > 0.0f && x_abcd > 0.0f);
        s->bc[0] = u_abcd;
        s->bc[1] = v_abcd;
        s->bc[2] = w_abcd;
        s->bc[3] = x_abcd;
        s->cnt = 4;
    } break;}

    /* V.) Check if origin is enclosed by tetrahedron */
    if (s->cnt == 4) {
        s->hit = 1;
        return 0;
    }
    /* VI.) Ensure closing in on origin to prevent multi-step cycling */
    vec3 pnt;
    float denom = 0;
    for (int i = 0; i < s->cnt; ++i)
        denom += s->bc[i];
    denom = 1.0f / denom;

    switch (s->cnt) {
    case 1: pnt = s->v[0].p; break;
    case 2: {
        /* --------- Line -------- */
        vec3 a = s->v[0].p * denom * s->bc[0];
        vec3 b = s->v[1].p * denom * s->bc[1];
        pnt = a + b;
    } break;
    case 3: {
        /* ------- Triangle ------ */
        vec3 a = s->v[0].p * denom * s->bc[0];
        vec3 b = s->v[1].p * denom * s->bc[1];
        vec3 c = s->v[2].p * denom * s->bc[2];

        pnt = a + b + c;
    } break;
    case 4: {
        /* ----- Tetrahedron ----- */
        vec3 a = s->v[0].p * denom * s->bc[0];
        vec3 b = s->v[1].p * denom * s->bc[1];
        vec3 c = s->v[2].p * denom * s->bc[2];
        vec3 d = s->v[3].p * denom * s->bc[3];

        pnt = a + b + c + d;
    } break;}

    float d2 = Vector::dot(pnt, pnt);
    if (d2 >= s->D) return 0;
    s->D = d2;

    /* VII.) New search direction */
    switch (s->cnt) {
    default: assert(0); break;
    case 1: {
        /* --------- Point -------- */
        *dv = -s->v[0].p;
    } break;
    case 2: {
        /* ------ Line segment ---- */
        vec3 ba = s->v[1].p - s->v[0].p;
        vec3 b0 = -s->v[1].p;
        vec3 t = Vector::cross(ba, b0);
        *dv = Vector::cross(t, ba);
    } break;
    case 3: {
        /* ------- Triangle ------- */
        vec3 ab = s->v[1].p - s->v[0].p;
        vec3 ac = s->v[2].p - s->v[0].p;
        vec3 n = Vector::cross(ab, ac);
        if (Vector::dot(n, s->v[0].p) <= 0.0f)
            *dv = n;
        else *dv -n;
    }}
    if (Vector::dot(*dv,*dv) < GJK_EPSILON * GJK_EPSILON)
        return 0;
    return 1;
}
extern void
gjk_analyze(struct gjk_result *res, const struct gjk_simplex *s)
{
    res->iterations = s->iter;
    res->hit = s->hit;

    /* calculate normalization denominator */
    float denom = 0;
    for (int i = 0; i < s->cnt; ++i)
        denom += s->bc[i];
    denom = 1.0f / denom;

    /* compute closest points */
    switch (s->cnt) {
    default: assert(0); break;
    case 1: {
        /* Point */
        res->p0 = s->v[0].a;
        res->p1 = s->v[0].b;
    } break;
    case 2: {
        /* Line */
        float as = denom * s->bc[0];
        float bs = denom * s->bc[1];

        vec3 a = s->v[0].a * as;
        vec3 b = s->v[1].a * bs;
        vec3 c = s->v[0].b * as;
        vec3 d = s->v[1].b * bs;

        res->p0 = a + b;
        res->p1 = c + d;
    } break;
    case 3: {
        /* Triangle */
        float as = denom * s->bc[0];
        float bs = denom * s->bc[1];
        float cs = denom * s->bc[2];

        vec3 a = s->v[0].a * as;
        vec3 b = s->v[1].a * bs;
        vec3 c = s->v[2].a * cs;

        vec3 d = s->v[0].b * as;
        vec3 e = s->v[1].b * bs;
        vec3 f = s->v[2].b * cs;

        res->p0 = a + b;
        res->p0 = res->p0 + c;

        res->p1 = d + e;
        res->p1 = res->p1 + f;
    } break;
    case 4: {
        /* Tetrahedron */
        vec3 a = s->v[0].a * denom * s->bc[0];
        vec3 b = s->v[1].a * denom * s->bc[1];
        vec3 c = s->v[2].a * denom * s->bc[2];
        vec3 d = s->v[3].a * denom * s->bc[3];

        res->p0 = a + b;
        res->p0 = res->p0 + c;
        res->p0 = res->p0 + d;
        res->p1 = res->p0;
    } break;}

    if (!res->hit) {
        /* compute distance */
        vec3 d = res->p1 - res->p0;
        res->distance_squared = Vector::dot(d, d);
    } else res->distance_squared = 0;
}
extern void
gjk_quad(struct gjk_result *res, float a_radius, float b_radius)
{
    float radius = a_radius + b_radius;
    float radius_squared = radius * radius;
    if (res->distance_squared > GJK_EPSILON &&
        res->distance_squared > radius_squared) {
        res->distance_squared -= radius_squared;

        /* calculate normal */
        vec3 n = res->p1 - res->p0;
        float l2 = Vector::dot(n, n);
        if (l2 != 0.0f) {
            float il = inv_sqrt(l2);
            n = n * il;
        }
        vec3 da = n * a_radius;
        vec3 db = n * b_radius;

        /* calculate new collision points */
        res->p0 = res->p0 + da;
        res->p1 = res->p1 - db;
    } else {
        vec3 p = res->p0 + res->p1;
        res->p0 = p * 0.5f;
        res->p1 = res->p0;
        res->distance_squared = 0;
        res->hit = 1;
    }
}