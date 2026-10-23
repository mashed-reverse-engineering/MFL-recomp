#include "mfl_tier2.h"
extern struct FxSlot g_fx_slots[10];
/*004077d0*/ extern int fx_counters_clear();
/*00408810*/ extern int fx_objects_reset();
/*0040d0d0*/ extern char *__cdecl find_texture_dict_b(RwChar *p0);
/*00429cc0*/ extern int game_open_perm_piz();
/*004608c0*/ extern int weapon_6a5e2c_get_clump();
/*00488230*/ extern int __cdecl clump_load_logged(void *p0, RwChar *p1);
/*0048e390*/ extern int rwp_object_set_radius_squared();
/*0048e4f0*/ extern int rwp_object_set_enabled();
/*0048e6d0*/ extern int rwp_object_get_handle();
/*0048e720*/ extern undefined __cdecl rwp_object_set_matrix(int p0, RwMatrix *p1);
/*00491530*/ extern int __cdecl rwp_object_create_from_atomic(struct RpAtomic *p0, void *p1, float p2, undefined4 p3, undefined4 p4, undefined4 p5, undefined4 p6);
/*0049be00*/ extern int __cdecl fx_system_create(int p0, struct RwTexture *p1, uint p2, int p3, undefined4 *p4, undefined4 p5, undefined4 p6);
/*004a4440*/ extern int game_close_piz();
/*004b2a60*/ extern int sprintf();
/*004c3f60*/ extern RpAtomic * clump_first_atomic(RpClump * p0);
/*004c3f90*/ extern int clump_get_atomics(RpClump * p0, RpAtomic ** p1);
/*004c48b0*/ extern void FUN_004c48b0(void *p0, undefined4 p1, undefined4 p2);
/*004c52b0*/ extern int FUN_004c52b0(undefined4 p0, undefined4 p1, undefined4 p2);
/*004d5e10*/ extern struct RwTexDictionary * RwTexDictionarySetCurrent(struct RwTexDictionary *p0);
/*005ddae0*/ extern undefined g_load_copter_5ddae0;
/*00651268*/ extern undefined4 g_load_651268[0x1000];
/*00651aa0*/ extern undefined4 g_fx_object_count;

/* One copter record in g_load_651268 is COPTER_RECORD_WORDS words; the word offsets below are relative to the record start. */
#define COPTER_RECORD_WORDS (0x3b)
#define COPTER_NAME_WORD (-6)
#define COPTER_CLUMP_WORD (-2)
#define COPTER_ATOMIC_WORD (-1)
#define COPTER_RWP_OBJECT_WORD (0xb)
#define COPTER_SLOT_WORD (0x27)
/* byte offset of the 4-byte copter slot table inside the game state (inferred) */
#define GAME_COPTER_TABLE_OFFSET (0xc9b0)
/* float bit pattern 100.0f, passed as radius squared (inferred) */
#define COPTER_RADIUS_SQUARED (0x42c80000)
/* flags word for fx_system_create (inferred) */
#define FX_SYSTEM_FLAGS (0x817)

/* load_copter_models */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "Copters/ ... %s%s%s.dff, smoke" */

/* Loads the copter models for the copter records in g_load_651268 (g_fx_object_count of them, inferred: current race).
 * Each record gets its clump from "ToastArt/common/perm/Copters/<name>.dff", an RwObject placed at the matrix of
 * the slot in game_state's table (GAME_COPTER_TABLE_OFFSET), and its atomics copied into record words 1..n.
 * Then one smoke FX system is created per g_fx_slots entry and the fx counters are reset.
 * game_state: game state block (inferred). unused_arg: not used.
 */

void __cdecl load_copter_models(char *game_state,char *unused_arg)

{
  int atomic_index;
  undefined4 clump;
  RpAtomic *atomic;
  int remaining_atomics;
  RwTexture *smoke_texture;
  uint temp_handle;
  undefined4 *atomic_slot;
  undefined4 *copter_record;
  FxSlot *fx_slot;
  struct {
    int local_b0;
    undefined4 local_ac [10];
    undefined1 local_84 [64];
    RwMatrix local_44;
  } frame;

  frame.local_b0 = 0;
  game_open_perm_piz();
  if (0 < (int)g_fx_object_count) {
    copter_record = g_load_651268;
    /* one iteration per copter record: load the clump, create its object, place it */
    do {
      sprintf(frame.local_84,"%s%s%s.dff","ToastArt/common/perm/","Copters/",copter_record + COPTER_NAME_WORD);
      RwTexDictionarySetCurrent((RwTexDictionary *)0x0);
      clump = clump_load_logged(frame.local_84,(RwChar *)0x0);
      copter_record[COPTER_CLUMP_WORD] = clump;
      atomic = (RpAtomic *)clump_first_atomic(clump);
      copter_record[COPTER_ATOMIC_WORD] = atomic;
      clump = rwp_object_create_from_atomic
                        (atomic,&g_load_copter_5ddae0,0.0,0x3f000000,0x3f000000,0x41a00000,200000);
      copter_record[COPTER_RWP_OBJECT_WORD] = clump;
      rwp_object_set_radius_squared(clump,COPTER_RADIUS_SQUARED);
      clump = rwp_object_get_handle(copter_record[COPTER_RWP_OBJECT_WORD]);
      *copter_record = clump;
      copter_record[0x2c] = 0;
      clump = *(undefined4 *)(game_state + GAME_COPTER_TABLE_OFFSET + copter_record[COPTER_SLOT_WORD] * 4);
      copter_record[0x2d] = clump;
      FUN_004c48b0(&frame.local_44,clump,0);
      rwp_object_set_matrix(copter_record[COPTER_RWP_OBJECT_WORD],&frame.local_44);
      rwp_object_set_enabled(copter_record[COPTER_RWP_OBJECT_WORD],1);
      remaining_atomics = clump_get_atomics(copter_record[COPTER_CLUMP_WORD],frame.local_ac);
      /* copy the atomic pointers into record words n..1, last atomic first (inferred: atomic 0 ends up in word 1) */
      if (remaining_atomics != 0) {
        atomic_slot = copter_record + remaining_atomics + 1;
        do {
          atomic_index = remaining_atomics + -1;
          remaining_atomics = remaining_atomics + -1;
          atomic_slot = atomic_slot + -1;
          FUN_004c52b0(frame.local_ac[atomic_index],0,0);
          *atomic_slot = frame.local_ac[remaining_atomics];
        } while (remaining_atomics != 0);
      }
      frame.local_b0 = frame.local_b0 + 1;
      copter_record = copter_record + COPTER_RECORD_WORDS;
    } while (frame.local_b0 < (int)g_fx_object_count);
  }
  game_close_piz();
  /* one smoke FX system per g_fx_slots entry; the loop ends at the address of g_fx_object_count, which follows the slot array (inferred) */
  fx_slot = g_fx_slots;
  do {
    smoke_texture = (RwTexture *)find_texture_dict_b("smoke");
    /* frame.local_b0 is reused here as a float argument (bits 0x40a0a0a0) to fx_system_create */
    frame.local_b0 = 0x40a0a0a0;
    /* give the slot a clump if it has none */
    if (fx_slot->clump == 0) {
      temp_handle = weapon_6a5e2c_get_clump();
      fx_slot->clump = temp_handle;
    }
    temp_handle = fx_system_create(8,smoke_texture,FX_SYSTEM_FLAGS,2,&frame.local_b0,0x3e99999a,0x3fc00000);
    fx_slot->fx_system = temp_handle;
    fx_slot = fx_slot + 1;
  } while ((char *)fx_slot < ((char *)&g_fx_object_count));
  fx_objects_reset();
  fx_counters_clear();
  return;
}


