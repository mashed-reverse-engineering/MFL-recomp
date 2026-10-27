#include "mfl_tier2.h"
/*00422150*/ extern int vehicle_get_flags_10();
/*00422280*/ extern int vehicle_part_frame();
/*0047b520*/ extern int car_current_matrix();
/* course_superg_cb4_sub_45c1f0 */

/* single caller course_superg_cb4 */

extern void course_superg_cb4_sub_45c140(int part_frame, RwMatrix *matrix);

/* The original pushes (matrix, 100, index) for vehicle_part_frame, which only reads two of them, and leaves the third (the matrix pointer) on the stack as
   the second argument of course_superg_cb4_sub_45c140 (cdecl; the caller cleans 8 bytes after it).  The decompiler lost that argument: the 45c140 call had one
   parameter and the callee read stack garbage (NULL, crash in RwMatrixTransform on course 7). */
void course_superg_cb4_sub_45c1f0(int count)

{
  int i;
  RwMatrix *matrix;
  struct {
    undefined1 auStack_8 [4];
    undefined1 auStack_4 [4];
  } frame;
  int part_frame;
  
  for (i = 0; i < count; i = i + 1) {
    vehicle_get_flags_10(i,frame.auStack_4,frame.auStack_8);
    car_current_matrix(&matrix,i);
    part_frame = vehicle_part_frame(i,100);
    course_superg_cb4_sub_45c140(part_frame,matrix);
  }
  return;
}
