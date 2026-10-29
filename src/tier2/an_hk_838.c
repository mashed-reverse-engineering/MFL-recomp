#include "mfl_tier2.h"
/*0040d0d0*/ extern char *__cdecl find_texture_dict_b(RwChar *p0);
/*0042f780*/ extern char *__cdecl clump_load_by_name(char *p0, undefined4 p1, undefined4 p2);
/*0049be00*/ extern int __cdecl fx_system_create(int p0, struct RwTexture *p1, uint p2, int p3, undefined4 *p4, undefined4 p5, undefined4 p6);
/*004c3f90*/ extern int clump_get_atomics(RpClump * p0, RpAtomic ** p1);
/*004c52b0*/ extern int atomic_geometry_user_data_int(undefined4 p0, undefined4 p1, undefined4 p2);
/*004d1500*/ extern struct RwFrame * RwFrameScale(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d5e10*/ extern struct RwTexDictionary * RwTexDictionarySetCurrent(struct RwTexDictionary *p0);
/*004f6bf0*/ extern int rpclump_clone(void *p0);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*006a5dc8*/ extern undefined4 g_weapon_6a5dc8;
/*006a5df8*/ extern undefined g_weapon_rocket_6a5df8;
/*006a5e20*/ extern undefined g_rockets;
/*006a5e30*/ extern undefined g_weapon_rocket_6a5e30;
/*006a5fd0*/ extern undefined4 g_weapon_6a5fd0;
/*006a5fd8*/ extern undefined4 g_weapon_rocket_6a5fd8;
/*006a5fdc*/ extern undefined4 g_weapon_6a5fdc;
/*006a5fe0*/ extern undefined4 g_weapon_6a5fe0;
/*006a5fe4*/ extern undefined4 g_weapon_6a5fe4;
/*006a5fe8*/ extern undefined4 g_weapon_6a5fe8;
/*006a5db0*/ extern char g_weapon_flashbang_6a5db0[];
/*006a5fbc*/ extern char g_weapon_rocket_6a5fbc[];

#define MISSILE_COUNT (5) /* records loaded per call (inferred) */
#define MISSILE_RECORD_STRIDE (0x6c) /* bytes per record in g_weapon_6a5dc8 / g_rockets (inferred) */
#define MISSILE_SLOT_STRIDE (0x2c) /* bytes per entry in g_weapon_6a5fd0..6a5fe8 (inferred) */
#define ATOMIC_BUFFER_SIZE (10) /* clump_get_atomics output buffer (inferred) */
#define SMOKE_TINT (0xa0) /* smoke colour, RGB bytes (inferred) */
#define SMOKE_ALPHA (0x40) /* smoke colour, alpha byte (inferred) */
#define SMOKE_FX_FLAGS (0x817) /* fx_system_create flags (inferred) */
/* missile_load_assets
 * Loads MissileLauncher.dff and missile.dff, the "smoke" texture and a smoke-trail fx,
 * then fills MISSILE_COUNT records of the global weapon tables (inferred: one launcher
 * instance, one missile instance and one fx per record). Destroys both template clumps
 * at the end. Takes no parameters, returns nothing.
 * (was: "MissileLauncher.dff + smoke trail particles", which left out missile.dff)
 */

void missile_load_assets(void)

{
  undefined4 handle;
  int atomics_left;
  int scratch;
  int record_offset;
  int slot_offset;
  int next_record_offset;
  /* fx_block: bytes 0..3 are the smoke colour passed to fx_system_create (inferred);
     iStack_44 is the record counter, counted down from MISSILE_COUNT */
  struct {
    undefined1 uStack_48;
    undefined1 uStack_47;
    undefined1 uStack_46;
    undefined1 uStack_45;
    int iStack_44;
  } fx_block;
  RpClump *launcher_clump;
  RpClump *missile_clump;
  RwTexture *smoke_texture;
  RwV3d scale;
  int atomics [ATOMIC_BUFFER_SIZE];
  
  fx_block.iStack_44 = MISSILE_COUNT;
  smoke_texture = (RwTexture *)find_texture_dict_b("smoke");
  fx_block.uStack_48 = SMOKE_TINT;
  fx_block.uStack_47 = SMOKE_TINT;
  fx_block.uStack_46 = SMOKE_TINT;
  fx_block.uStack_45 = SMOKE_ALPHA;
  launcher_clump = (RpClump *)clump_load_by_name("MissileLauncher.dff", 0x0, 0x0);
  missile_clump = (RpClump *)clump_load_by_name("missile.dff", 0x0, 0x0);
  RwTexDictionarySetCurrent((RwTexDictionary *)0x0);
  slot_offset = MISSILE_COUNT * MISSILE_SLOT_STRIDE;
  record_offset = MISSILE_COUNT * MISSILE_RECORD_STRIDE;
  /* One pass per record: record_offset runs MISSILE_COUNT*stride down to one stride
     (the loop ends when next_record_offset reaches 0). */
  do {
    fx_block.iStack_44 = fx_block.iStack_44 + -1;
    next_record_offset = record_offset + -MISSILE_RECORD_STRIDE;
    handle = rpclump_clone(launcher_clump);
    *(undefined4 *)((char *)&g_weapon_6a5fdc + slot_offset) = handle;
    atomics_left = clump_get_atomics((RpClump *)handle,(RpAtomic **)atomics);
    /* Launcher atomics, last to first: the one atomic_geometry_user_data_int accepts (returns 1)
       goes into the slot table at g_weapon_6a5fe4 (inferred). */
    while (atomics_left != 0) {
      scratch = atomics_left + -1;
      atomics_left = atomics_left + -1;
      scratch = atomic_geometry_user_data_int(atomics[scratch],0,0);
      if (scratch == 1) {
        *(int *)((char *)&g_weapon_6a5fe4 + slot_offset) = atomics[atomics_left];
      }
    }
    *(undefined4 *)(slot_offset + ((char *)&g_weapon_rocket_6a5fbc + 0xc)) = 0;
    *(undefined4 *)((char *)&g_weapon_rocket_6a5fd8 + slot_offset) = 0x3e99999a; /* 0.3f */
    *(int *)((char *)&g_weapon_6a5fd0 + slot_offset) = fx_block.iStack_44;
    *(undefined4 *)((char *)&g_weapon_6a5fe0 + slot_offset) = 0;
    *(undefined4 *)((char *)&g_weapon_6a5fe8 + slot_offset) = 0;
    handle = rpclump_clone(missile_clump);
    *(undefined4 *)(record_offset + ((char *)&g_weapon_flashbang_6a5db0 + 0x10)) = handle;
    handle = fx_system_create(10,smoke_texture,SMOKE_FX_FLAGS,1,(undefined4 *)&fx_block.uStack_48,0x3f000000 /* 0.5f */,0x40000000 /* 2.0f */);
    *(undefined4 *)(record_offset + ((char *)&g_weapon_6a5dc8 + 0x4)) = handle;
    atomics_left = clump_get_atomics((RpClump *)(*(char **)(record_offset + ((char *)&g_weapon_flashbang_6a5db0 + 0x10))),(RpAtomic **)atomics);
    /* Missile atomics, last to first: the one atomic_geometry_user_data_int accepts (returns 1) is
       stored in the record at g_weapon_6a5dc8 and its frame is scaled by (3, 3, 9). */
    while (atomics_left != 0) {
      scratch = atomics_left + -1;
      atomics_left = atomics_left + -1;
      scratch = atomic_geometry_user_data_int(atomics[scratch],0,0);
      if (scratch == 1) {
        scratch = atomics[atomics_left];
        scale.x = 3.0;
        scale.y = 3.0;
        scale.z = 9.0;
        *(int *)((char *)&g_weapon_6a5dc8 + record_offset) = scratch;
        RwFrameScale(*(RwFrame **)(scratch + 4),&scale,rwCOMBINEPRECONCAT);
      }
    }
    /* Clear the rocket fields of the record. Index comes from iStack_44 (4..0), so this
       range is one lower than the 1..5 range used above (inferred, check table sizes). */
    record_offset = fx_block.iStack_44 * MISSILE_RECORD_STRIDE;
    *(undefined4 *)((char *)&g_rockets + record_offset) = 0;
    *(undefined4 *)((char *)&g_weapon_rocket_6a5e30 + record_offset) = 0;
    *(undefined4 *)((char *)&g_weapon_rocket_6a5df8 + record_offset) = 0;
    slot_offset = slot_offset + -MISSILE_SLOT_STRIDE;
    record_offset = next_record_offset;
  } while (next_record_offset != 0);
  RpClumpDestroy(launcher_clump);
  RpClumpDestroy(missile_clump);
  return;
}


