#include "mfl_tier2.h"
/*004757a0*/ extern char *get_viewport_camera_matrix();
/*0049e000*/ extern int fx_ptank_create();
/*0076df18*/ extern undefined4 g_fx_ptank_76df18;
/*0076df24*/ extern undefined4 g_particles_76df24;
/*00886980*/ extern float g_app_886980;
/*00886984*/ extern float g_app_886984;
/* layout of *iVar1 in course_training_cb0_sub_49e170, reconstructed from the byte offsets it is accessed at (explicit padding: the offsets are the original ones, every field sits at its natural alignment; fields are named by offset) */
typedef struct course_training_cb0_sub_49e170_iVar1_layout {
    char pad_0x0[48];
    float f_0x30;
    float f_0x34;
    float f_0x38;
} course_training_cb0_sub_49e170_iVar1_layout;

/*00886988*/ extern float g_data_886988;
/* course_training_cb0_sub_49e170 */

/* single caller course_training_cb0 */

void course_training_cb0_sub_49e170(void)

{
  course_training_cb0_sub_49e170_iVar1_layout *iVar1;
  
  fx_ptank_create(g_particles_76df24,&g_fx_ptank_76df18,0x20);
  iVar1 = (course_training_cb0_sub_49e170_iVar1_layout *)(get_viewport_camera_matrix(0));
  g_app_886980 = iVar1->f_0x30;
  g_app_886984 = iVar1->f_0x34;
  g_data_886988 = iVar1->f_0x38;
  return;
}


