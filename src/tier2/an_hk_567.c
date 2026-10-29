#include "mfl_tier2.h"
/*00465470*/ extern int weapon_find_targets_in_cone();
/*00465f40*/ extern int weapon_effect_get_slot_value_b();
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*005e64a4*/ extern float k_f_0;
/*0062ac80*/ extern undefined g_weapon_62ac80;
/*006a29bc*/ extern undefined4 g_weapon_mortar_6a29bc[0x1000];
/*006a2a28*/ extern undefined g_weapon_6a2a28[0x1000];
/*007ef7f4*/ extern float g_tick_seconds;
static int weapon_mortar_register_trigger_r(int reg_eax);
extern void weapon_mortar_register_trigger(void);
__declspec(naked) int weapon_mortar_register_trigger_r(int reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call weapon_mortar_register_trigger
        ret
    }
}

static int weapon_mortar_shell_update_r(int reg_eax);
extern void weapon_mortar_shell_update(void);
extern char g_explosion_queue[];
extern char g_weapon_6a2a2c[];

#define MORTAR_ENTRY_WORDS (7)
#define MATRIX_POS_OFFSET (0x30)
#define MORTAR_SPIN_DEG (30.0)
#define SHELL_STRIDE (0x110)
__declspec(naked) int weapon_mortar_shell_update_r(int reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call weapon_mortar_shell_update
        ret
    }
}

/* weapon_mortar_update */
/* Per-frame update of the mortar weapon (inferred).
 * Pass 1 walks the mortar slots: an active slot gets its slot value stored, its frame
 * transformed, and a search of the firing cone for targets. While the slot timer is above
 * k_f_0 the turret frame is spun and the timer is counted down by g_tick_seconds.
 * Pass 2 walks the shell array and updates every active shell and its trigger.
 * Takes no parameters, returns nothing. */

void weapon_mortar_update(void)

{
  undefined4 slot_value;
  undefined4 *mortar_entry;
  undefined *shell_entry;
  int mortar_index;
  undefined4 target_id;
  
  /* Pass 1: mortar slots, MORTAR_ENTRY_WORDS words each, up to g_weapon_6a2a2c. */
  mortar_index = 0;
  mortar_entry = g_weapon_mortar_6a29bc;
  do {
    target_id = 0xffffff00;
    /* Active slot (word 1 non-zero). */
    if (mortar_entry[1] != 0) {
      slot_value = weapon_effect_get_slot_value_b(mortar_index);
      *mortar_entry = slot_value;
      RwFrameTransform(*(RwFrame **)(mortar_entry[4] + 4),(RwMatrix *)mortar_entry[-1],rwCOMBINEREPLACE);
      weapon_find_targets_in_cone(mortar_index,mortar_entry[-1] + MATRIX_POS_OFFSET,0x41700000,0x41a00000,&target_id);
      /* Spin the turret and count the timer down while it is above k_f_0 (inferred). */
      if (k_f_0 < *(float *)&mortar_entry[3]) {
        RwFrameRotate(*(RwFrame **)(mortar_entry[5] + 4),(RwV3d *)&g_weapon_62ac80,MORTAR_SPIN_DEG,rwCOMBINEPRECONCAT
                     );
        *(float *)&mortar_entry[3] = *(float *)&mortar_entry[3] - g_tick_seconds;
      }
    }
    mortar_entry = mortar_entry + MORTAR_ENTRY_WORDS;
    mortar_index = mortar_index + 1;
  } while ((LONG_PTR)mortar_entry < ((LONG_PTR)&g_weapon_6a2a2c));
  /* Pass 2: shell array, SHELL_STRIDE bytes each, up to g_explosion_queue. */
  shell_entry = g_weapon_6a2a28;
  do {
    /* Active shell (word 1 non-zero). */
    if (*(int *)(shell_entry + 4) != 0) {
      weapon_mortar_shell_update_r(shell_entry);
      weapon_mortar_register_trigger_r(shell_entry);
    }
    shell_entry = shell_entry + SHELL_STRIDE;
  } while ((LONG_PTR)shell_entry < ((LONG_PTR)&g_explosion_queue));
  return;
}


