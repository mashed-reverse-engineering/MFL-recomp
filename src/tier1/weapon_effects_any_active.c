#include "mfl_tier2.h"
/*006a7c7c*/ extern undefined4 g_weapon_6a7c7c;
/*006a7d28*/ extern undefined1 g_weapon_6a7d28;
/*006a7cbc*/ extern char g_weapon_6a7cbc[];


#define WEAPON_EFFECT_FLAG_COUNT 2   /* leading ints at g_weapon_6a7d28 checked first */

/* Returns 1 if any weapon effect slot is non-zero, 0 otherwise.
   Checks the flag ints at g_weapon_6a7d28, then the 16-byte-strided entries
   from g_weapon_6a7c7c up to 0x6a7cbc (meaning of the fields: unknown). */
undefined4 weapon_effects_any_active(void)
{
  int i;
  int *slot_ptr;

  i = 0;
  do {
    if (*(int *)(&g_weapon_6a7d28 + i * 4) != 0) {
      return 1;
    }
    i = i + 1;
  } while (i < WEAPON_EFFECT_FLAG_COUNT);
  if (i < 4) {
    slot_ptr = &g_weapon_6a7c7c + i * 4;
    do {
      if (*slot_ptr != 0) {
        return 1;
      }
      slot_ptr = slot_ptr + 4;
    } while ((int)slot_ptr < ((int)&g_weapon_6a7cbc));
  }
  return 0;
}
