#include "mfl_tier2.h"
/*0075cde0*/ extern float g_particles_75cde0;
/*0075cde4*/ extern float g_particles_75cde4;
/*0075cde8*/ extern float g_particles_75cde8;
/* course_training_cb1_sub_49c180 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* single caller course_training_cb1 */

void course_training_cb1_sub_49c180(undefined4 *param_1)

{
  *(undefined4 *)&g_particles_75cde0 = *param_1;
  *(undefined4 *)&g_particles_75cde4 = param_1[1];
  *(undefined4 *)&g_particles_75cde8 = param_1[2];
  return;
}


