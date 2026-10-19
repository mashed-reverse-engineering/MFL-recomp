// cluster-entries: weapon_mortar_trail_particles_add
/* Installed cluster weapon_mortar_trail_particles_add (tools/cluster_install.py from output/cluster_pilot11/weapon_mortar_trail_particles_add.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0049b870*/ extern int particle_batch_mark_current_active(int batch);
/*0049b890*/ extern int particle_batch_add(int batch, int pos, void *colour);
/*0049b910*/ extern int particle_batch_set_uv_a(int batch, void *uv);
/*0049b940*/ extern int particle_batch_set_size(int batch, float size);
/*0049bd50*/ extern int fx_system_flush(int batch);
/*005dd370*/ extern float k_f_0_3;
/*005ddb08*/ extern float k_f_1_4;
/*005de1f8*/ extern float k_f_3_1415927;
/*005de23c*/ extern float k_f_0_06666667;
/* weapon_mortar_trail_particles_add */

/* WARNING: Unknown calling convention */
/* "adds the mortar shell smoke trail particles (position / size / colour) to the fx system and
   flushes it" */

void weapon_mortar_trail_particles_add_n(int in_esi)

{
  int iVar1;
  int i;
  int iVar3;
  struct {
    unsigned char col[4];   /* r g b a */
    float t;
    float uv[2];
  } frame;
  
  frame.col[0] = 0x80;
  frame.col[1] = 0x80;
  frame.col[2] = 0x80;
  iVar3 = in_esi + 0x5c;
  for (i = 0; i < 0xf; i++) {
    frame.t = (float)i * k_f_0_06666667;
    frame.col[3] = (unsigned char)(int)(100.0f - frame.t * 90.0f);
    frame.uv[0] = (k_f_1_4 - frame.t) * k_f_0_3;
    frame.uv[1] = frame.uv[0];
    particle_batch_add(*(int *)(in_esi + 0x58),iVar3,frame.col);
    particle_batch_set_uv_a(*(int *)(in_esi + 0x58),frame.uv);
    particle_batch_set_size(*(int *)(in_esi + 0x58),frame.t * k_f_3_1415927);
    iVar3 = iVar3 + 0xc;
  }
  particle_batch_mark_current_active(*(undefined4 *)(in_esi + 0x58));
  fx_system_flush(*(undefined4 *)(in_esi + 0x58));
  iVar3 = 0xf;
  while (iVar3 = iVar3 + -1, iVar3 != 0) {
    iVar1 = in_esi + iVar3 * 0xc;
    *(undefined4 *)(iVar1 + 0x5c) = *(undefined4 *)(iVar1 + 0x50);
    *(undefined4 *)(iVar1 + 0x60) = *(undefined4 *)(iVar1 + 0x54);
    *(undefined4 *)(iVar1 + 100) = *(undefined4 *)(iVar1 + 0x58);
  }
  *(undefined4 *)(in_esi + 0x5c) = *(undefined4 *)(in_esi + 0x14);
  *(undefined4 *)(in_esi + 0x60) = *(undefined4 *)(in_esi + 0x18);
  *(undefined4 *)(in_esi + 100) = *(undefined4 *)(in_esi + 0x1c);
  return;
}

/* entry thunk at the original address of weapon_mortar_trail_particles_add: register inputs (esi) -> natural C weapon_mortar_trail_particles_add_n; ret 0 */
__declspec(naked) void weapon_mortar_trail_particles_add(void)
{
    __asm {
        push esi
        call weapon_mortar_trail_particles_add_n
        add esp, 4
        ret
    }
}
