// cluster-entries: fx_objects_update_all
/* Installed cluster fx_objects_update_all (tools/cluster_install.py from output/cluster_pilot_kb/fx_objects_update_all.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0049cc20*/ extern undefined __cdecl fx_object_update_frame(undefined4 p0, undefined4 p1);
/*005dd330*/ extern float float_1_0;
/*005e64a4*/ extern float k_f_0;
/*0075f828*/ extern undefined4 g_particles_75f828[0x1000];
/*007ef7f4*/ extern float g_tick_seconds;
/* fx_objects_update_all */

/* "updates each active fx object and fades the block" */

static int fx_object_apply_transform_r(int * p0);
extern void fx_object_apply_transform(void);
void fx_objects_update_all(void)

{
  float fade;
  int *object;
  int count;
  int *block;
  
  block = g_particles_75f828;
  do {
    if (*block != 0) {
      count = block[0xcb];
      if (count != 0) {
        object = block + count * 0x14 + 3;
        do {
          object = object + -0x14;
          fx_object_update_frame(object,block[2]);
          count = count + -1;
        } while (count != 0);
      }
      *(float *)&block[2] = (float_1_0 - *(float *)&block[1]);
      fade = *(float *)&block[1] - g_tick_seconds;
      *(float *)&block[1] = fade;
      if (fade < k_f_0) {
        count = block[0xcb];
        *block = 0;
        if (count != 0) {
          object = block + count * 0x14 + 3;
          do {
            object = object + -0x14;
            fx_object_apply_transform_r(object);
            count = count + -1;
          } while (count != 0);
        }
      }
    }
    block = block + 0xcc;
  } while ((int)block < (int)g_particles_75f828 + 0x9f60);
  return;
}

/* fx_object_apply_transform_r: C -> original fx_object_apply_transform (registers esi) */
__declspec(naked) int fx_object_apply_transform_r(int * p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call fx_object_apply_transform
        pop esi
        ret
    }
}
