#include "mfl_tier2.h"
/*004b2b48*/ extern int _ftol();
/*004c40c0*/ extern int FUN_004c40c0(undefined4 p0);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*006aaba0*/ extern undefined4 g_weapon_shotgun_6aaba0[0x1000];
static int weapon_fx_clump_remove_c_r(void *reg_esi);
extern void weapon_fx_clump_remove_c(void);
extern char g_weapon_shotgun_6aac30[];
#define SLOT_WORDS (9) /* 36-byte slot records: 0x6aac30 - 0x6aaba0 = 4 slots */
#define SLOT_FADE_BYTES (0x14) /* byte distance back from slot to the fade counter slot[-5] */
#define SHOTGUN_SPIN_DEGREES (20.0) /* per-frame spin of both frames, degrees (inferred) */
#define SHOTGUN_FADE_STEP (0.125f) /* alpha fraction lost per fade frame, 1/8 (inferred) */
#define ALPHA_MAX (255.0f)
#define FX_TINT_WHITE_RGB (0xffffff)
__declspec(naked) int weapon_fx_clump_remove_c_r(void *reg_esi)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call weapon_fx_clump_remove_c
        pop esi
        ret
    }
}

/* weapon_shotgun_update: per-frame update of the shotgun fx slot table.
 * Walks the 4 slots of SLOT_WORDS words each, from g_weapon_shotgun_6aaba0 up to
 * g_weapon_shotgun_6aac30. `slot` points at each slot's last word, so slot[-n] is
 * field n words before it (inferred layout):
 *   slot[-8]  slot base, passed to weapon_fx_clump_remove_c_r
 *   slot[-7]  object A, its +4 is RwFrame A
 *   slot[-6]  object B, its +4 is RwFrame B
 *   slot[-5]  fade counter, counts down to 0
 *   slot[-3]  matrix for the transform
 *   slot[-2]  state: 0 idle, 1 transform, 2 remove clump
 *   slot[-1]  object whose +4 is the transform target frame
 *   slot[0]   countdown for state 1
 * Takes no parameters, returns nothing.
 * `state` is reused for the FUN_004c40c0 result of object A (inferred). */

void weapon_shotgun_update(void)

{
  RwFrame *frame_a;
  RwFrame *frame_b;
  undefined1 alpha;
  int state;
  int colour_b;
  int *slot;
  undefined4 colour;
  RwV3d spin_axis;

  slot = g_weapon_shotgun_6aaba0;
  do {
    state = slot[-2];
    if (state != 0) {
      /* state 1: apply the transform, then count down slot[0] */
      if (state == 1) {
        RwFrameTransform(*(RwFrame **)(slot[-1] + 4),(RwMatrix *)slot[-3],rwCOMBINEREPLACE);
        if (*slot != 0) {
          *slot = *slot + -1;
        }
      }
      /* state 2: remove the clump */
      else if (state == 2) {
        weapon_fx_clump_remove_c_r((void *)(slot + -8));
      }
      /* fade: while the counter runs, spin both frames and tint both colour blocks */
      if (slot[-5] != 0) {
        frame_a = *(RwFrame **)(slot[-7] + 4);
        frame_b = *(RwFrame **)(slot[-6] + 4);
        slot[-5] = slot[-5] + -1;
        spin_axis.x = 0.0;
        spin_axis.y = 0.0;
        spin_axis.z = 1.0;
        RwFrameRotate(frame_a,&spin_axis,SHOTGUN_SPIN_DEGREES,rwCOMBINEPRECONCAT);
        RwFrameRotate(frame_b,&spin_axis,SHOTGUN_SPIN_DEGREES,rwCOMBINEPRECONCAT);
        state = FUN_004c40c0(slot[-7]);
        colour_b = FUN_004c40c0(slot[-6]);
        /* alpha falls from ALPHA_MAX to 0 over 8 fade steps (inferred); colour is alpha<<24 | white */
        if ((state != 0) && (colour_b != 0)) {
          alpha = ((int)(ALPHA_MAX - (((float)*(int *)((char *)slot - SLOT_FADE_BYTES) * SHOTGUN_FADE_STEP) * ALPHA_MAX)));
          colour = CONCAT13(alpha,FX_TINT_WHITE_RGB);
          *(undefined4 *)(state + 4) = colour;
          *(undefined4 *)(colour_b + 4) = colour;
        }
      }
    }
    slot = slot + SLOT_WORDS;
  } while ((char *)slot < ((char *)&g_weapon_shotgun_6aac30));
  return;
}


