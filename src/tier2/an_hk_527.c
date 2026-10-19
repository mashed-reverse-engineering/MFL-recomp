#include "mfl_tier2.h"
/*0075bfa8*/ extern char *g_particles_75bfa8;
/*0075bfac*/ extern char *g_particles_75bfac;
/*0075bfb0*/ extern char *g_particles_75bfb0;
/*0075bfb4*/ extern char *g_particles_75bfb4;
/*0075bfb8*/ extern char *g_particles_75bfb8;
/*0075bfbc*/ extern char *g_particles_75bfbc;
/*0075bfc0*/ extern char *g_particles_75bfc0;
/*0075bfc4*/ extern char *g_particles_75bfc4;
/*0075bfc8*/ extern char *g_particles_75bfc8;
/*0075bfcc*/ extern char *g_particles_75bfcc;
/*0075bfd0*/ extern char *g_particles_75bfd0;
/*0075bfd4*/ extern char *g_particles_75bfd4;
/*0075bfd8*/ extern char *g_particles_75bfd8;
/*0075bfdc*/ extern char *g_particles_75bfdc;
/*0075cda8*/ extern char *g_particles_75cda8;
/* particles_slot_set_vectors */

/* "stores three vectors into slot (system, index) (max 16 each)" */

undefined4
particles_slot_set_vectors
          (int param_1,undefined4 *param_2,undefined4 param_3,undefined4 *param_4,
          undefined4 *param_5)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  
  iVar1 = *(int *)((char *)&g_particles_75cda8 + param_1 * 4);
  if (0xf < iVar1) {
    return 0;
  }
  iVar3 = (param_1 * 0x10 + iVar1) * 0x38;
  *(undefined4 *)((char *)&g_particles_75bfa8 + iVar3) = *param_2;
  *(undefined4 *)((char *)&g_particles_75bfac + iVar3) = param_2[1];
  *(undefined4 *)((char *)&g_particles_75bfb0 + iVar3) = param_2[2];
  *(undefined4 *)((char *)&g_particles_75bfb8 + iVar3) = *param_4;
  *(undefined4 *)((char *)&g_particles_75bfbc + iVar3) = param_4[1];
  *(undefined4 *)((char *)&g_particles_75bfc0 + iVar3) = param_4[2];
  *(undefined4 *)((char *)&g_particles_75bfc4 + iVar3) = *param_5;
  *(undefined4 *)((char *)&g_particles_75bfc8 + iVar3) = param_5[1];
  *(undefined4 *)((char *)&g_particles_75bfcc + iVar3) = param_5[2];
  *(undefined4 *)((char *)&g_particles_75bfb4 + iVar3) = param_3;
  *(undefined4 *)((char *)&g_particles_75bfd0 + iVar3) = *param_2;
  *(undefined4 *)((char *)&g_particles_75bfd4 + iVar3) = param_2[1];
  uVar2 = param_2[2];
  *(int *)((char *)&g_particles_75cda8 + param_1 * 4) = iVar1 + 1;
  *(undefined4 *)((char *)&g_particles_75bfd8 + iVar3) = uVar2;
  *(undefined4 *)((char *)&g_particles_75bfdc + iVar3) = param_3;
  return 1;
}


