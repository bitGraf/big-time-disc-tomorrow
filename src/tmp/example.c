#include "gjk.h"
#include <stdio.h>
#include <math.h>

static const float f3z[3];
#define fop(r,e,a,p,b,i,s) (r) e ((a) p (b)) i (s)
#define f3op(r,e,a,p,b,i,s) do {\
    fop((r)[0],e,(a)[0],p,(b)[0],i,s),\
    fop((r)[1],e,(a)[1],p,(b)[1],i,s),\
    fop((r)[2],e,(a)[2],p,(b)[2],i,s);}while(0)
#define f3cpy(d,s) (d)[0]=(s)[0],(d)[1]=(s)[1],(d)[2]=(s)[2]
#define f3add(d,a,b) f3op(d,=,a,+,b,+,0)
#define f3sub(d,a,b) f3op(d,=,a,-,b,+,0)
#define f3mul(d,a,s) f3op(d,=,a,+,f3z,*,s)
#define f3dot(a,b) ((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])
#define f3cross(d,a,b) do {\
    (d)[0] = ((a)[1]*(b)[2]) - ((a)[2]*(b)[1]),\
    (d)[1] = ((a)[2]*(b)[0]) - ((a)[0]*(b)[2]),\
    (d)[2] = ((a)[0]*(b)[1]) - ((a)[1]*(b)[0]);}while(0)

static int
line_support(float *support, const float *d,
    const float *a, const float *b)
{
    int i = 0;
    if (f3dot(a, d) < f3dot(b, d)) {
        f3cpy(support, b); i = 1;
    } else f3cpy(support, a);
    return i;
}
static int
polyhedron_support(float *support, const float *d,
    const float *verts, int cnt)
{
    int imax = 0;
    float dmax = f3dot(verts, d);
    for (int i = 1; i < cnt; ++i) {
        /* find vertex with max dot product in direction d */
        float dot = f3dot(&verts[i*3], d);
        if (dot < dmax) continue;
        imax = i, dmax = dot;
    } f3cpy(support, &verts[imax*3]);
    return imax;
}
static int
polyhedron_intersect_capsule(const float *verts, int cnt,
    const float *ca, const float *cb, float cr)
{
    /* initial guess */
    float d[3] = {0};
    struct gjk_support s = {0};
    f3cpy(s.a, verts);
    f3cpy(s.b, ca);
    f3sub(d, s.b, s.a);
    f3sub(d, s.b, s.a);

    /* run gjk algorithm */
    struct gjk_simplex gsx = {0};
    while (gjk(&gsx, &s, d)) {
        float n[3]; f3mul(n, d, -1);
        s.aid = polyhedron_support(s.a, n, verts, cnt);
        s.bid = line_support(s.b, d, ca, cb);
        f3sub(d, s.b, s.a);
    }
    /* check distance between closest points */
    struct gjk_result res;
    gjk_analyze(&res, &gsx);
    gjk_quad(&res, 0, cr);
    return res.hit;
}

int main() {
    float verts[] = {
        0,7,0,
        1,6,1,
        1,6,-1,
        -1,6,1,
        -1,6,-1,
        0,3.95f,0
    };
    int numVerts = 6;
    float ca[3] = {0,0,0};
    float cb[3] = {0,3,0};
    float cr = 1;

    int hit = polyhedron_intersect_capsule(verts, numVerts, ca, cb, cr);
    printf("Intersect: %s\n", hit==1 ? "True." : "False.");

    return 0;
}