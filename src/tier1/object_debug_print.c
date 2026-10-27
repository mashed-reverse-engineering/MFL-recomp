#include "mfl_tier2.h"
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0);
/* course_sands_cb1 */
/* course_sands_cb2 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* course_sands_cb5 */
/* course_superg_cb1 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* course_superg_cb4 */
/* course_training_cb1 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* course_training_cb2 */
/* course_training_cb10 */
/* course_particle_init_random */
/* "initialises one particle (colour 0xffc0c0c0, flags 0x1120) with random velocity" */
/* anim_set_callback */
/* "stores the callback at +0xC4" */
/* course_zone_players_transform_update */
/* "per player: inverts the zone matrix and transforms the car matrix into zone space" */
/* object_debug_print */
/* "debug print of name / pos / matrix (wprintf)" */
/* Prints one object's debug block with wprintf: a "%s" line, then the pos, at, up and right
   rows of the matrix. Parameters: name (inferred: the string for the %s line; this decompile
   never reads it, so the %s gets no argument), matrix (float[16] in RwMatrix layout: right at
   [0..2], up at [4..6], at at [8..10], pos at [12..14]). Returns nothing. */
void object_debug_print(undefined4 name,float *matrix)
{
  FID_conflict__wprintf("%s\n",name);
  FID_conflict__wprintf
            ("pos    (%f, %f, %f)\n",(double)matrix[0xc],(double)matrix[0xd],(double)matrix[0xe])
  ;
  FID_conflict__wprintf
            ("at     (%f, %f, %f)\n",(double)matrix[8],(double)matrix[9],(double)matrix[10]);
  FID_conflict__wprintf
            ("up     (%f, %f, %f)\n",(double)matrix[4],(double)matrix[5],(double)matrix[6]);
  FID_conflict__wprintf
            ("right  (%f, %f, %f)\n\n",(double)*matrix,(double)matrix[1],(double)matrix[2]);
  return;
}
