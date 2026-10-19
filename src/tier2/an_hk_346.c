#include "mfl_tier2.h"
/*0041b970*/ extern int hudpanel_reset_all();
/*0041be10*/ extern int hudpanel_slots_clear();
/*0041be30*/ extern int hudpanel_values_init();
/*00658450*/ extern undefined g_hudpanel_658450;
/*00658538*/ extern undefined g_hudpanel_658538;
static int hudpanel_counters_clear_r(int reg_eax);
extern void hudpanel_counters_clear(void);
__declspec(naked) int hudpanel_counters_clear_r(int reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call hudpanel_counters_clear
        ret
    }
}

static int hudpanel_slot_reset_r(undefined4 * reg_eax);
extern void hudpanel_slot_reset(void);
extern char g_hudpanel_6584f8[];
extern char g_load_6586e8[];
#define HUD_SLOT_STRIDE (0x6c)
#define HUD_COUNTER_STRIDE (0x54)
__declspec(naked) int hudpanel_slot_reset_r(undefined4 * reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call hudpanel_slot_reset
        ret
    }
}

/* hudpanel_reset_all_b */

/* "second variant of hudpanel_reset_all" */

/* Resets the HUD panel state. Takes no parameters, returns nothing.
   Walks the slot table from g_hudpanel_658538 up to g_load_6586e8 in HUD_SLOT_STRIDE steps,
   resetting each slot; then clears the slots and re-initialises the values; then walks the
   counter table from g_hudpanel_658450 up to g_hudpanel_6584f8 in HUD_COUNTER_STRIDE steps,
   clearing each counter. (inferred: the wrappers return the address of the next entry) */
void hudpanel_reset_all_b(void)

{
  undefined *cursor;
  int next;

  /* slot table: reset every entry until the end of the table */
  cursor = &g_hudpanel_658538;
  do {
    next = hudpanel_slot_reset_r(cursor);
    cursor = (undefined *)(next + HUD_SLOT_STRIDE);
  } while ((LONG_PTR)cursor < ((LONG_PTR)&g_load_6586e8));
  /* clear the slots, then re-initialise the values (inferred) */
  hudpanel_slots_clear();
  hudpanel_values_init();
  /* counter table: clear every entry until the end of the table */
  cursor = &g_hudpanel_658450;
  do {
    next = hudpanel_counters_clear_r(cursor);
    cursor = (undefined *)(next + HUD_COUNTER_STRIDE);
  } while ((LONG_PTR)cursor < ((LONG_PTR)&g_hudpanel_6584f8));
  return;
}


