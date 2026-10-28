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

/* "adds the mortar shell smoke trail particles (position / size / colour) to the fx system and
   flushes it" */

/* shell+0x58 = particle batch handle, shell+0x14 = current position (vec3), shell+0x5c = trail of 15 previous positions (vec3 each, inferred). Adds the 15 trail points as fading particles, flushes, then shifts the trail by one and stores the current position as newest. */
void weapon_mortar_trail_particles_add_n(int shell)

{
  int dst_entry;
  int i;
  int trail_cursor_or_index;
  struct {
    unsigned char col[4];   /* r g b a */
    float t;
    float uv[2];
  } frame;
  
  frame.col[0] = 0x80;
  frame.col[1] = 0x80;
  frame.col[2] = 0x80;
  trail_cursor_or_index = shell + 0x5c;
  for (i = 0; i < 0xf; i++) {
    frame.t = (float)i * k_f_0_06666667;
    frame.col[3] = (unsigned char)(int)(100.0f - frame.t * 90.0f);
    frame.uv[0] = (k_f_1_4 - frame.t) * k_f_0_3;
    frame.uv[1] = frame.uv[0];
    particle_batch_add(*(int *)(shell + 0x58),trail_cursor_or_index,frame.col);
    particle_batch_set_uv_a(*(int *)(shell + 0x58),frame.uv);
    particle_batch_set_size(*(int *)(shell + 0x58),frame.t * k_f_3_1415927);
    trail_cursor_or_index = trail_cursor_or_index + 0xc;
  }
  particle_batch_mark_current_active(*(undefined4 *)(shell + 0x58));
  fx_system_flush(*(undefined4 *)(shell + 0x58));
  trail_cursor_or_index = 0xf;
  while (trail_cursor_or_index = trail_cursor_or_index + -1, trail_cursor_or_index != 0) {
    dst_entry = shell + trail_cursor_or_index * 0xc;
    *(undefined4 *)(dst_entry + 0x5c) = *(undefined4 *)(dst_entry + 0x50);
    *(undefined4 *)(dst_entry + 0x60) = *(undefined4 *)(dst_entry + 0x54);
    *(undefined4 *)(dst_entry + 100) = *(undefined4 *)(dst_entry + 0x58);
  }
  *(undefined4 *)(shell + 0x5c) = *(undefined4 *)(shell + 0x14);
  *(undefined4 *)(shell + 0x60) = *(undefined4 *)(shell + 0x18);
  *(undefined4 *)(shell + 100) = *(undefined4 *)(shell + 0x1c);
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
