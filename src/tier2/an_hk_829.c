#include "mfl_tier2.h"
typedef int CourseId;
/*0042b110*/ extern int current_course_id();
/*00458170*/ extern int __cdecl course_point_in_water_box_test(struct RwV3d *p0);
/*0045d590*/ extern int course_zone_contains_matrix_in_zone_6a2888();
/*00467fd0*/ extern int __cdecl course_surface_id_is_active_check(undefined4 p0);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/* rocket_surface_hit_test */
#define COURSE_ROUABOUT (2)
#define COURSE_WARZONE (33)
#define SURFACE_TAG_WARZONE_ZONE (-0x7fbfc0) /* (inferred) */
#define ZONE_MATRIX_FLAGS (0x20003) /* (inferred) */

/* Rocket surface hit test: reads the surface under the rocket, checks it via course_surface_id_is_active_check,
   then applies the course specific checks for COURSE_ROUABOUT (water box at position) and COURSE_WARZONE
   (zone test on a matrix moved to position). Returns 1 if the check fires (inferred: hit), 0 otherwise.
   rocket = rocket object (inferred), position = rocket world position.
   surface_entry is reused as scratch for call results after the surface lookup.
   (was: "reads the surface id under a rocket, checks it via course_surface_id_is_active_check and course
   specific zones (2, 0x21)") */

undefined4 __cdecl rocket_surface_hit_test(char *rocket,RwV3d *position)

{
  int surface_tag;
  int surface_entry;
  CourseId course_id;
  RwMatrix zone_matrix;

  /* look up the entry of the rocket's current surface */

  surface_entry = *(int *)(*(int *)(*(char **)(rocket + 0x3c) + 0x10) +
                      ((uint)*(ushort *)
                              (*(int *)(*(char **)(rocket + 0x34) + 4) + 6 +
                              *(int *)(rocket + 0xc) * 8) +
                      (uint)*(ushort *)(*(char **)(rocket + 0x34) + 0x80)) * 4);
  surface_tag = *(int *)(surface_entry + 4);
  /* an active surface counts as a hit straight away */
  surface_entry = course_surface_id_is_active_check(surface_entry);
  if (surface_entry != 0) {
    return 1;
  }
  course_id = current_course_id();
  /* Rouabout: hit if the position is inside a water box */
  if (course_id == COURSE_ROUABOUT) {
    surface_entry = course_point_in_water_box_test(position);
    if (surface_entry != 0) {
      return 1;
    }
  }
  /* Warzone: only for the SURFACE_TAG_WARZONE_ZONE surface; build an identity matrix moved to position
     and test it against zone 6a2888 */
  else if ((course_id == COURSE_WARZONE) && (surface_tag == SURFACE_TAG_WARZONE_ZONE)) {
    zone_matrix.flags = zone_matrix.flags | ZONE_MATRIX_FLAGS;
    zone_matrix.at.z = 1.0;
    zone_matrix.up.y = 1.0;
    zone_matrix.right.x = 1.0;
    zone_matrix.up.x = 0.0;
    zone_matrix.right.z = 0.0;
    zone_matrix.right.y = 0.0;
    zone_matrix.at.y = 0.0;
    zone_matrix.at.x = 0.0;
    zone_matrix.up.z = 0.0;
    zone_matrix.pos.z = 0.0;
    zone_matrix.pos.y = 0.0;
    zone_matrix.pos.x = 0.0;
    RwMatrixTranslate(&zone_matrix,position,rwCOMBINEREPLACE);
    surface_entry = course_zone_contains_matrix_in_zone_6a2888(&zone_matrix,&zone_matrix);
    if (surface_entry == 0) {
      return 1;
    }
  }
  return 0;
}


