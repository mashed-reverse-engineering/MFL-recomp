#include "mfl_tier2.h"
/*004b2b48*/ extern int _ftol();
/*005dd330*/ extern float float_1_0;
/*005ddc28*/ extern float k_f_0;
/*006c60d0*/ extern undefined4 g_gfx_6c60d0[0x1000];
/*006f1e20*/ extern undefined1 g_gfx_6f1e20;
/*007ef7f4*/ extern float g_tick_seconds;
static int effect_slot_free_r(int reg_esi);
extern void effect_slot_free(void);
__declspec(naked) int effect_slot_free_r(int reg_esi)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call effect_slot_free
        pop esi
        ret
    }
}

/* effect_trails_update_all */

/* "per active trail advances time and rewrites the vertex alpha" */

void effect_trails_update_all(void)

{
  float time;
  int count;
  uint colour;
  uint *vertex;
  int index;
  int slot;
  int *trail;
  
  slot = 0;
  trail = g_gfx_6c60d0;
  do {
    if (((&g_gfx_6f1e20)[slot >> 3] & (byte)(1 << ((byte)slot & 7))) != 0) {
      count = *trail;
      time = g_tick_seconds + *(float *)&trail[2];
      *(float *)&trail[2] = time;
      if ((count == 0) && (time > float_1_0)) {
        effect_slot_free_r(slot);
      }
      else {
        /* from here the fraction of the lifetime replaces the elapsed time */
        time = time / *(float *)&trail[3];
        if (time > float_1_0) {
          effect_slot_free_r(slot);
        }
        else if (((count != 0) && (*(float *)&trail[-1] != k_f_0)) && (index = 0, 0 < count)) {
          colour = ((int)((1.0f - time) * 48.0f)) << 0x18 | 0xffffff;
          vertex = (uint *)(trail + -0x9d);
          do {
            vertex[-9] = colour;
            *vertex = colour;
            index = index + 1;
            vertex = vertex + 0x12;
          } while (index < *trail);
        }
      }
    }
    trail = trail + 0xb0;
    slot = slot + 1;
  } while ((char *)trail < (char *)&g_gfx_6f1e20 + 0x2b0);
  return;
}
