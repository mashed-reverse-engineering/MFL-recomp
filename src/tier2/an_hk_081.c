#include "mfl_tier2.h"
/*004d1a50*/ extern RwBool __cdecl RwBBoxContainsPoint(struct RwBBox *p0, struct RwV3d *p1);
/*00557860*/ extern int __cdecl RtIntersectionLineTriangle(void *p0, void *p1, void *p2, void *p3, void *p4, void *p5);
/*005dd374*/ extern float k_f_0_2;
/*006a2168*/ extern undefined4 g_course_6a2168;
/*006a23e0*/ extern undefined g_course_6a23e0;
/*006a2614*/ extern float g_course_6a2614;
/*006a2634*/ extern undefined4 g_course_6a2634;
/*006a2184*/ extern char g_course_rouabout_6a2184[];

#define LINE_DELTA_Y_BITS (0x3e4ccccd) /* float 0.2f bit pattern: Y component of the upward ray (inferred) */
#define VEC3_SIZE (0xc) /* sizeof(RwV3d) */
#define WATER_BOX_VERTS_STRIDE (0x48) /* bytes per box entry in the vertex table (inferred) */

/* course_point_in_water_box_test */

/*
 * Tests whether a point is inside a water box on the course (inferred from the name and globals).
 *
 * vertex: world-space point to test.
 *
 * Returns 1 if the point is inside the water bounds g_course_6a2614 and no enabled box triangle
 * is hit by a short upward ray from just below the point; returns 0 otherwise (also when the
 * g_course_6a2634 gate is off).
 */
undefined4 __cdecl course_point_in_water_box_test(RwV3d *vertex)

{
  RwBool in_box;
  int hit;
  int *box_flag;
  undefined *box_vertices;
  undefined4 intersect_t;
  RwV3d line_start;       /* start and delta are 12 byte vectors passed by address (RtIntersectionLineTriangle) */
  undefined4 line_delta[3];
  
  line_delta[0] = 0;
  line_delta[2] = 0;
  intersect_t = 0;
  /* gate: water test is only active when g_course_6a2634 is set (inferred) */
  if (g_course_6a2634 != 0) {
    /* ray starts 0.2 below the point (k_f_0_2) and points straight up (line_delta) */
    line_start.x = vertex->x;
    line_start.z = vertex->z;
    line_start.y = vertex->y - k_f_0_2;
    line_delta[1] = LINE_DELTA_Y_BITS;
    in_box = RwBBoxContainsPoint((RwBBox *)&g_course_6a2614,vertex);
    if (in_box != 0) {
      /* walk the boxes: box_flag is the per-box flag table, box_vertices the matching vertex data;
       * a box is tested only while its flag is 0 (inferred: 0 = enabled) */
      box_flag = &g_course_6a2168;
      box_vertices = &g_course_6a23e0;
      do {
        /* each box has two triangles at the vertex offsets below; any hit means outside */
        if (*box_flag == 0) {
          hit = RtIntersectionLineTriangle
                            (&line_start,line_delta,box_vertices + -VEC3_SIZE,box_vertices,box_vertices + VEC3_SIZE,&intersect_t);
          if (hit != 0) {
            return 0;
          }
          hit = RtIntersectionLineTriangle
                            (&line_start,line_delta,box_vertices + 0x18,box_vertices + 0x24,box_vertices + 0x30,&intersect_t)
          ;
          if (hit != 0) {
            return 0;
          }
        }
        box_flag = box_flag + 1;
        box_vertices = box_vertices + WATER_BOX_VERTS_STRIDE;
        /* stop when the flag pointer passes the end of the table (ends at g_course_rouabout_6a2184, inferred) */
        if (((LONG_PTR)&g_course_rouabout_6a2184 + 0x3) < (LONG_PTR)box_flag) {
          return 1;
        }
      } while( true );
    }
  }
  return 0;
}


