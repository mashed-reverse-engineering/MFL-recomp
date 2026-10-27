#include "mfl_tier2.h"
/*00472890*/ extern int sfx_play_voice_at();
/*00484660*/ extern undefined __cdecl clump_set_flag_on_atomics(struct RpClump *p0, int p1);      /* diffemu: the original takes (clump, flag), the flag is read from the stack slot */
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004f4570*/ extern struct RpWorld * RpWorldAddClump(struct RpWorld *p0, struct RpClump *p1);
/* layout of *rec in weapon_flamethrower_equip, reconstructed from the byte offsets it is accessed at (explicit padding: the offsets are the original ones, every field sits at its natural alignment; fields are named by offset) */
typedef struct weapon_flamethrower_equip_rec_layout {
    undefined4 f_0x0;
    WeaponSlot * f_0x4;
    undefined4 f_0x8;
    char pad_0xc[20];
    RpClump * f_0x20;
    char * f_0x24;
} weapon_flamethrower_equip_rec_layout;

/*006a9588*/ extern char *g_weapon_6a9588;
#define WEAPON_FLAME_RECORD_STRIDE (0x68)     /* bytes per player's flame record */
#define SFX_VOICE_FLAMETHROWER (0x16)         /* voice id passed to sfx_play_voice_at (inferred) */
#define FLAME_FLOAT_0_02_BITS (0x3ca3d70a)    /* IEEE bits of float 0.02f, stored into the record (inferred use) */
/* weapon_flamethrower_equip */
/* type 0x10; state 0x6A9588 stride 0x68 */
/*
 * Equips the flamethrower for the player in slot->player: clears that player's flame fx records, marks the
 * record active, attaches the flame clump to the course world and plays the flame voice at the flame frame's
 * position (inferred). Parameter: slot (WeaponSlot *). Returns the player's flame record (undefined4 *,
 * first dword = active flag, record + 4 = slot, record + 8 = float 0.02f).
 */
undefined4 * weapon_flamethrower_equip(WeaponSlot *slot)
{
  uint player;
  RwMatrix *ltm;
  weapon_flamethrower_equip_rec_layout *rec;
  RwFrame *frame;
  RwV3d voice_pos;

  player = slot->player;
  rec = (weapon_flamethrower_equip_rec_layout *)((char *)&g_weapon_6a9588 + player * WEAPON_FLAME_RECORD_STRIDE);
  weapon_fx_grid_clear(rec);
  rec->f_0x0 = 1;
  rec->f_0x4 = slot;
  RpWorldAddClump((RpWorld *)course_get_rpworld_00467e90(),rec->f_0x20);
  clump_set_flag_on_atomics(rec->f_0x20,1);
  rec->f_0x8 = FLAME_FLOAT_0_02_BITS;
  frame = *(RwFrame **)(rec->f_0x24 + 4);
  ltm = RwFrameGetLTM(frame);
  voice_pos = ltm->pos;
  sfx_play_voice_at(SFX_VOICE_FLAMETHROWER,&voice_pos,slot->player);
  return &rec->f_0x0;
}
/* course_get_rpworld_00467e90 */

/* duplicate name: was course_get_rpworld (first address 0042a3f0) */

extern uint course_get_rpworld_00467e90(void);
