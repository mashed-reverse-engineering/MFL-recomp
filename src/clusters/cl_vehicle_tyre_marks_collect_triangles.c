// cluster-entries: vehicle_tyre_marks_collect_triangles
/* Installed cluster vehicle_tyre_marks_collect_triangles (tools/cluster_install.py from output/cluster_pilot_j1/vehicle_tyre_marks_collect_triangles.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
#pragma auto_inline(off)      /* the verified (diffemu, /Ob0) and the shipped (/O2) code must have the same call structure */
/* One collected triangle of a tyre mark: its three corners and the screen positions (x, y) of the corners. */
typedef struct TyreMark {
    float corner[3][3];
    float screen[3][2];
} TyreMark;

/* Tyre marks of one vehicle (0xf54 bytes): up to 64 triangles that lie under the car, plus the state of the last query. */
typedef struct TyreMarkSet {
    TyreMark mark[64];
    char pad_f00[0x10];
    float pos[3];                 /* position of the last query: the vehicle shifted by half its offset */
    float width;                  /* 1.5 until the vehicle size is known */
    char pad_f20[0x1b];
    unsigned char alpha;          /* strength of the marks (0..153) */
    char pad_f3c[4];
    int count;                    /* number of triangles in mark[] */
    char pad_f44[4];
    unsigned char *view;          /* object whose matrix (at +0x20) transforms the triangles to the screen */
    float lift;                   /* the corners are lifted by this to keep the marks above the road */
    float base_width;
} TyreMarkSet;

/* A triangle found by the world query (0x40 bytes). */
typedef struct TriHit {
    int pad_0;
    float facing;                 /* z of the face normal: only faces that look up enough get a mark */
    int pad_8;
    int index;                    /* triangle index in the mesh */
    float corner[3][3];
    unsigned char *mesh;
    int pad_38;
    unsigned char *world;
} TriHit;

/*006605a0*/ extern TyreMarkSet g_game_6605a0[];
/*00660570*/ extern float g_vehicle_render_660570[][3];       /* position of the last query per vehicle */
/*006642f0*/ extern unsigned char *g_game_6642f0;             /* camera object (position at +0x30) */
/*005e64a4*/ extern float k_f_0;
/*00481e80*/ extern int player_marker_get_colour(int player);
/*00487ee0*/ extern void __cdecl triangle_normal_cross(RwV3d *normal, float *p0, float *p1, float *p2);
/*00487fd0*/ extern int plane_classify_point(RwV3d *normal, float *point, float *distance);
/*004c4be0*/ extern int world_collect_triangles(void *query, float *pos, TriHit *hits, int max);
/*004d5670*/ extern RwV3d *RwV3dTransformPoints(RwV3d *out, RwV3d *in, int count, RwMatrix *matrix);

void vehicle_tyre_marks_collect_triangles(int index, float *obj, void *query, float scale)
{
    TyreMarkSet *set = &g_game_6605a0[index];
    RwMatrix view;
    TriHit hits[64];
    RwV3d normal, out[3];
    float dist, new_x, new_y, dx, dy;
    double offset, half, new_z, dz, scaled;
    int *last = (int *)g_vehicle_render_660570[index];
    int n, i, k, cam_x, cam_y, cam_z, colour, sum;
    unsigned char *material;
    float *cam;

    set->alpha = (unsigned char)(int)(scale * 153.0f);
    for (i = 0; i < 16; i++) ((int *)&view)[i] = ((int *)(set->view + 0x20))[i];
    for (i = 0; i < 3; i++) ((int *)set->pos)[i] = ((int *)(obj + 12))[i];
    set->width = 1.5f;
    cam = (float *)(g_game_6642f0 + 0x30);
    cam_x = ((int *)cam)[0];
    cam_y = ((int *)cam)[1];
    cam_z = ((int *)cam)[2];
    offset = cam[1] * obj[5] + obj[4] * cam[0] + cam[2] * obj[6] + 1.0f;
    scaled = offset * set->base_width;      /* computed first so that offset stays in the x87 register (cl spills it as a double otherwise); a separate statement: cl would factor base_width out of the sum (different sign of zero) */
    set->width = (float)(scaled * 1.5f + set->base_width);
    half = 0.5f * offset;
    new_x = *(float *)&cam_x * half + set->pos[0];
    set->pos[0] = new_x;
    new_y = k_f_0 * half + set->pos[1];
    set->pos[1] = new_y;
    new_z = half * *(float *)&cam_z + set->pos[2];
    set->pos[2] = (float)new_z;
    dx = *(float *)&last[0] - new_x;
    dy = *(float *)&last[1] - new_y;
    dz = *(float *)&last[2] - new_z;
    n = 0;
    if (dy * dy + dx * dx + dz * dz > 0.03125) {
        for (i = 0; i < 3; i++) last[i] = ((int *)set->pos)[i];
        set->count = 0;
        n = world_collect_triangles(query, set->pos, hits, 0x40);
        if (n == 0) return;
    }
    set->lift = 0.01f;
    for (i = 0; i < n; i++) {
        TriHit *hit = &hits[i];
        sum = *(unsigned short *)(*(unsigned char **)(hit->mesh + 4) + hit->index * 8 + 6) + *(unsigned short *)(hit->mesh + 0x80);
        material = (unsigned char *)(*(int **)(hit->world + 0x10))[sum];
        colour = *(int *)(material + 4);
        if (colour == (int)0xff010101 || colour == (int)0xff0000ff || colour == (int)0xffff0080) continue;
        if (colour == player_marker_get_colour(0)) continue;
        if (colour == player_marker_get_colour(1)) continue;
        if (colour == player_marker_get_colour(2)) continue;
        if (colour == player_marker_get_colour(3)) continue;
        if (colour == player_marker_get_colour(4)) continue;
        if (hit->facing < 0.75f) continue;
        triangle_normal_cross(&normal, hit->corner[0], hit->corner[1], hit->corner[2]);
        k = plane_classify_point(&normal, obj + 12, &dist);
        if (k == 0) { if (dist > 1.0f) continue; }
        else if (k == 2) { if (dist > 0.1f) continue; }
        {
            TyreMark *mark = &set->mark[set->count];
            set->count++;
            for (k = 0; k < 9; k++) ((int *)mark)[k] = ((int *)hit->corner)[k];
            mark->corner[0][1] = mark->corner[0][1] + set->lift;
            mark->corner[1][1] = mark->corner[1][1] + set->lift;
            mark->corner[2][1] = mark->corner[2][1] + set->lift;
        }
    }
    for (i = 0; i < set->count; i++) {
        TyreMark *mark = &set->mark[i];
        RwV3dTransformPoints(out, (RwV3d *)mark->corner, 3, &view);
        ((int *)mark->screen)[0] = ((int *)&out[0])[0];
        ((int *)mark->screen)[1] = ((int *)&out[0])[1];
        ((int *)mark->screen)[2] = ((int *)&out[1])[0];
        ((int *)mark->screen)[3] = ((int *)&out[1])[1];
        ((int *)mark->screen)[4] = ((int *)&out[2])[0];
        ((int *)mark->screen)[5] = ((int *)&out[2])[1];
    }
}

