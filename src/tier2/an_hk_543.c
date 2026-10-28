#include "mfl_tier2.h"
/*0040d0d0*/ extern char *__cdecl find_texture_dict_b(RwChar *p0);
/*0042f780*/ extern char *__cdecl clump_load_by_name(char *p0, undefined4 p1, undefined4 p2);
/*00484660*/ extern undefined __cdecl clump_set_flag_on_atomics(struct RpClump *p0, undefined4 p1);
/*00486b00*/ extern int ptank_set_blend_modes();
/*00486f10*/ extern undefined __cdecl particle_system_setup_ptank(uint *p0, uint p1, uint p2, struct RwTexture *p3);
/*004c3f90*/ extern int clump_get_atomics(RpClump * p0, RpAtomic ** p1);
/*004c52b0*/ extern int FUN_004c52b0(undefined4 p0, undefined4 p1, undefined4 p2);
/*004d1500*/ extern struct RwFrame * RwFrameScale(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d5e10*/ extern struct RwTexDictionary * RwTexDictionarySetCurrent(struct RwTexDictionary *p0);
/*004f6bf0*/ extern int rpclump_clone(void *p0);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*006a6090*/ extern char *g_weapon_rocket_6a6090;
/*006a60a0*/ extern char *g_weapon_rocket_6a60a0;
/*006a60b4*/ extern char *g_missile_load_6a60b4;
/*006a60b8*/ extern char *g_missile_load_6a60b8;
/*006a60bc*/ extern char *g_weapon_6a60bc;
/*006a60c0*/ extern char *g_weapon_6a60c0;
/*006a60c4*/ extern undefined4 g_missile_load_6a60c4;
/*006a60cc*/ extern char *g_weapon_rocket_6a60cc;
/*006a61f0*/ extern undefined g_weapon_6a61f0;

#define GATLING_GUN_STRIDE (0x48)
#define GATLING_GUN_START_OFFSET (0x120)

/* gatling_load_models */

/* "'flash' texture and GatlingGun.dff" */

/* Loads GatlingGun.dff and sets up the gatling gun particle system with the
 * "flash" texture, then fills the global gun slot tables.  Takes no parameters
 * and returns nothing.
 * For each gun slot (GATLING_GUN_STRIDE apart, from GATLING_GUN_START_OFFSET
 * down to 0) it takes a clump from rpclump_clone (inferred: clone of the gun
 * clump), stores it, walks its atomics and stores each atomic in the slot
 * picked by its part id, then sets the clump's atomic flags and scales the
 * frame of one slot by (2, 2, 4).  The loaded clump is destroyed at the end. */
void gatling_load_models(void)

{
  int atomic_idx;
  RwTexture *texture_dict;
  RpClump *clump;
  undefined4 uVar2;
  int atomics_left;
  int next_gun_offset;
  struct {
    RwV3d scale;
    undefined4 atomics [10];
  } buf;
  int gun_offset;

  texture_dict = (RwTexture *)find_texture_dict_b("flash");
  buf.scale.x = 2.0;
  buf.scale.y = 2.0;
  buf.scale.z = 4.0;
  particle_system_setup_ptank((uint *)&g_weapon_6a61f0,0x817,0x80,texture_dict);
  ptank_set_blend_modes(&g_weapon_6a61f0,5,6);
  RwTexDictionarySetCurrent((RwTexDictionary *)0x0);
  clump = (RpClump *)clump_load_by_name("GatlingGun.dff", 0x0, 0x0);
  /* One pass per gun slot: gun_offset walks the slot tables from
     GATLING_GUN_START_OFFSET down to 0 in GATLING_GUN_STRIDE steps. */
  gun_offset = GATLING_GUN_START_OFFSET;
  do {
    next_gun_offset = gun_offset + -GATLING_GUN_STRIDE;
    *(undefined4 *)((char *)&g_weapon_rocket_6a60a0 + gun_offset) = 0;
    uVar2 = rpclump_clone(clump);
    *(undefined4 *)((char *)&g_weapon_rocket_6a6090 + gun_offset) = uVar2;
    atomics_left = clump_get_atomics(uVar2,buf.atomics);
    /* Visit the clump's atomics from last to first. */
    while (atomics_left != 0) {
      atomic_idx = atomics_left + -1;
      atomics_left = atomics_left + -1;
      uVar2 = FUN_004c52b0(buf.atomics[atomic_idx],0,0);
      /* Store the atomic in the slot for its part id (inferred). */
      switch(uVar2) {
      case 0:
        *(undefined4 *)((char *)&g_weapon_6a60bc + gun_offset) = buf.atomics[atomics_left];
        break;
      case 1:
        *(undefined4 *)((char *)&g_weapon_6a60c0 + gun_offset) = buf.atomics[atomics_left];
        break;
      case 2:
        *(undefined4 *)((char *)&g_missile_load_6a60c4 + gun_offset) = buf.atomics[atomics_left];
        break;
      case 3:
        *(undefined4 *)((char *)&g_weapon_rocket_6a60cc + gun_offset) = buf.atomics[atomics_left];
        break;
      case 4:
        *(undefined4 *)((char *)&g_missile_load_6a60b8 + gun_offset) = buf.atomics[atomics_left];
        break;
      case 5:
        *(undefined4 *)(gun_offset + ((LONG_PTR)&g_missile_load_6a60c4 + 0x4)) = buf.atomics[atomics_left];
        break;
      case 6:
        *(undefined4 *)((char *)&g_missile_load_6a60b4 + gun_offset) = buf.atomics[atomics_left];
      }
    }
    clump_set_flag_on_atomics(clump, 0x0);
    RwFrameScale(*(RwFrame **)(*(char **)((char *)&g_missile_load_6a60b4 + gun_offset) + 4),&buf.scale,
                 rwCOMBINEPRECONCAT);
    gun_offset = next_gun_offset;
  } while (next_gun_offset != 0);
  RpClumpDestroy(clump);
  return;
}


