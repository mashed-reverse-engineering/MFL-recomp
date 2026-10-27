// cluster-entries: car_world_collision_query
/* Installed cluster car_world_collision_query (tools/cluster_install.py from output/cluster_pilot_zx/car_world_collision_query.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
extern char LAB_00477170[];
/*0047b900*/ extern int car_resolve_triangle_contacts();
/*00492100*/ extern int rwp_car_apply_forces();
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*00549290*/ extern int RpCollisionWorldForAllIntersections();
/*005dfd80*/ extern float k_f_0_2778;
/*00843ea0*/ extern undefined4 g_collision_triangles;
/*008b06a0*/ extern undefined4 g_car_8b06a0;
/*008b06a4*/ extern float g_car_8b06a4[0x1000];
/*008b06e0*/ extern struct Car player_car[16];
/*008bd720*/ extern undefined4 g_car_8bd720;
/*008bd724*/ extern undefined4 g_car_8bd724;
/*008bd728*/ extern undefined4 g_car_8bd728;
/*008bd72c*/ extern undefined4 g_car_world_8bd72c;
/*008bd740*/ extern undefined g_car_8bd740;
/*008bd74c*/ extern undefined4 g_collision_triangle_count;
/*008bd760*/ extern undefined4 g_car_8bd760;
/*008bd778*/ extern undefined g_car_8bd778;
/*008bd784*/ extern undefined g_car_8bd784;
/*008bd794*/ extern undefined4 g_car_8bd794;
extern char g_car_world_8b06d4[];
#define RW_MATRIX_FLOATS (16)
#define WHEEL_STRIDE_FLOATS (0x31)

/* car_world_collision_query */

/* transforms the wheel anchors and a bounding sphere by the car's previous-frame matrix
   (matrix_previous) and runs RpCollisionWorldForAllIntersections against the track.
   car_index: index into player_car.
   collision_ctx: pointer whose +0x8 holds the collision world (inferred); stored in g_car_8bd794.
   Returns the result of car_resolve_triangle_contacts. Also runs rwp_car_apply_forces(car_index, 8). */

static int triangle_normal_r(float * p0, float * p1, float * p2, float * p3);
extern void triangle_normal(void);
undefined4 car_world_collision_query(int car_index,int collision_ctx)

{
  RwMatrix *prev_matrix;
  float scaled_x;
  undefined4 *anchor_cursor;
  undefined4 *next_anchor;
  undefined4 contact_result;
  float *wheel_pos;
  struct {
    RwV3d origin;
    RwReal sphere_x;
    RwReal sphere_y;
    RwReal sphere_z;
    float sphere_radius;
    undefined1 pad_c[8];
    undefined4 intersect_type;
  } query;

  /* reset the three global counters/flags used by the collision callback */
  g_car_8bd720 = 0;
  g_car_8bd724 = 0;
  g_car_8bd728 = 0;
  prev_matrix =(RwMatrix *)(player_car[car_index].matrices + player_car[car_index].matrix_previous * RW_MATRIX_FLOATS);
  query.origin.x = 0.0;
  query.origin.y = 0.0;
  query.origin.z = 0.0;
  g_car_world_8bd72c = 0;
  /* for each of the 4 wheels: store (pos_x * k, 0, pos_z * k) as a triple in g_car_8b06a4
     (k = k_f_0_2778, ~1/3.6 (inferred)),
     and clear the 4 gap bytes at wheel +0x34 */
  anchor_cursor = g_car_8b06a4;
  wheel_pos = &player_car[car_index].wheels[0].pos_x;
  do {
    scaled_x = *wheel_pos;
    *anchor_cursor = 0;
    scaled_x = scaled_x * k_f_0_2778;
    wheel_pos[0xc] = 0.0;
    next_anchor = anchor_cursor + 3;
    *(float *)&anchor_cursor[-1] = scaled_x;
    *(float *)&anchor_cursor[1] = wheel_pos[2] * k_f_0_2778;
    anchor_cursor = next_anchor;
    wheel_pos = wheel_pos + WHEEL_STRIDE_FLOATS;
  } while ((LONG_PTR)next_anchor < ((LONG_PTR)&g_car_world_8b06d4));
  /* car origin -> g_car_8bd740; the 4 wheel anchors -> g_car_8bd760 */
  RwV3dTransformPoints((RwV3d *)&g_car_8bd740,&query.origin,1,prev_matrix);
  RwV3dTransformPoints((RwV3d *)&g_car_8bd760,(RwV3d *)&g_car_8b06a0,4,prev_matrix);
  /* &player_car[car_index].up (sizeof(struct Car) = 0xd04) */
  triangle_normal_r(&g_car_8bd760,&g_car_8bd778,&g_car_8bd784,car_index * 0xd04 + 0x8b10a8);
  query.intersect_type = 3;
  /* bounding sphere: centre = car position in world, radius = bounds_radius */
  query.sphere_x = (prev_matrix->pos).x;
  query.sphere_y = (prev_matrix->pos).y;
  query.sphere_radius = player_car[car_index].bounds_radius;
  query.sphere_z = (prev_matrix->pos).z;
  g_collision_triangle_count = 0;
  /* collect every track triangle the sphere touches, then resolve the contacts for this car */
  g_car_8bd794 = collision_ctx;
  RpCollisionWorldForAllIntersections
            (*(undefined4 *)(collision_ctx + 8),&query.sphere_x,&LAB_00477170,&g_collision_triangles);
  contact_result = car_resolve_triangle_contacts(player_car + car_index,&g_collision_triangles);
  rwp_car_apply_forces(car_index,8);
  return contact_result;
}

/* triangle_normal_r: C -> original triangle_normal (registers eax,edx,ecx,esi) */
__declspec(naked) int triangle_normal_r(float * p0, float * p1, float * p2, float * p3)
{
    __asm {
        push esi
        mov eax, dword ptr [esp + 8]
        mov edx, dword ptr [esp + 12]
        mov ecx, dword ptr [esp + 16]
        mov esi, dword ptr [esp + 20]
        call triangle_normal
        pop esi
        ret
    }
}
