// cluster-entries: spline_objects_resolve_pair course_spline_objects_update
/* Installed cluster spline_objects_resolve_pair (tools/cluster_install.py from output/cluster_pilot/spline_objects_resolve_pair.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster spline_objects_resolve_pair: the three objects that run along the course spline; objects that get too close are pushed apart. */
typedef struct SplineObject {
    char unknown_00[8];
    float pos;           /* position along the spline, 0..1 */
    float vel;           /* speed along the spline */
    float margin_a;      /* clearance towards the spline end (pos < 1 - margin_a) */
    float margin_b;      /* clearance towards the spline start (pos > margin_b) */
    char unknown_18[0x40];
} SplineObject;          /* 0x58 bytes */

extern SplineObject g_course_spline_8c5500[3];
extern void spline_object_update(SplineObject *obj, float seconds);

/* Bounces one object back from the spline ends: steps back by its speed and reverses at half speed when it left the allowed range. */
static void spline_object_bounce(SplineObject *o)
{
    float p = o->pos - o->vel;

    o->pos = p;
    if (1.0f - o->margin_a <= p || p <= o->margin_b) {
        o->pos = p - (o->vel + o->vel);
        o->vel = o->vel * -0.5f;
    }
}

/* When two objects overlap (distance < lo.margin_a + hi.margin_b) both bounce and exchange half their speed.  (was: EAX unused) */
void spline_objects_resolve_pair(SplineObject *a, SplineObject *b)
{
    SplineObject *lo = a;
    SplineObject *hi = b;
    float dist;
    float lo_vel;

    if (b->pos < a->pos) {
        lo = b;
        hi = a;
    }
    dist = hi->pos - lo->pos;
    if (dist < 0.0f) {
        dist = -dist;
    }
    if (dist < lo->margin_a + hi->margin_b) {
        spline_object_bounce(hi);
        spline_object_bounce(lo);
        lo_vel = lo->vel;
        lo->vel = lo->vel * -0.5f;
        lo->vel = hi->vel * 0.5f + lo->vel;
        hi->vel = lo_vel * 0.5f - hi->vel * 0.5f;
    }
}

/* Per frame: separates every pair of the three objects, then advances them by ticks / 50 seconds. */
void course_spline_objects_update(uint ticks)
{
    float seconds = (float)ticks * 0.02f;
    int i, j;

    for (i = 0; i < 3; i++) {
        for (j = i + 1; j < 3; j++) {
            spline_objects_resolve_pair(&g_course_spline_8c5500[i], &g_course_spline_8c5500[j]);
        }
    }
    for (i = 0; i < 3; i++) {
        spline_object_update(&g_course_spline_8c5500[i], seconds);
    }
}

