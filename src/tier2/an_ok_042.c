#include "mfl_tier2.h"
/*0040fdb0*/ extern int get_player_count();
/*0042a3f0*/ extern int course_get_rpworld();
/*00452b90*/ extern undefined __cdecl viewport_camera_sync_to_main(undefined4 p0, undefined4 p1, undefined4 p2);
/*0045b730*/ extern int course_clumps_destroy();
/*0045c7f0*/ extern int course_atomics_render_3();
/*0045d560*/ extern int course_clump_render_with_physics();
/*0045e440*/ extern int weapon_pickups_clear();
/*0045f330*/ extern undefined __cdecl explosion_trigger_callback(struct RwV3d *p0, float *p1);
/*00467a60*/ extern int weapons_update_frame();
/*00467e90*/ extern void * course_get_rpworld_00467e90();
/*00472340*/ extern int sfx_play_player_set_at();
/*00475730*/ extern int get_viewport_camera();
/*004757a0*/ extern int get_viewport_camera_matrix();
/*0047a470*/ extern int car_set_wheel_contact();
/*0047b520*/ extern int car_current_matrix();
/*00481ea0*/ extern int player_marker_set_clump();
/*00481ee0*/ extern int player_marker_set_enabled();
/*00481fd0*/ extern double random_range();
/*00484660*/ extern undefined __cdecl clump_set_flag_on_atomics(struct RpClump *p0);
/*00484ad0*/ extern int pool_list_destroy();
/*00495c30*/ extern int viewport_get_camera();
/*00496850*/ extern int trigger_sphere_register();
/*004980c0*/ extern int fx_trail_particle_spawn_interpolated();
/*00499b40*/ extern int explosion_decal();
/*0049bd90*/ extern int fx_system_destroy();
/*0049bdd0*/ extern int fx_system_render();
/*0049c5f0*/ extern undefined __cdecl fx_snow_ptank_update(int p0, struct RwCamera *p1, undefined1 *p2, float p3, float p4);
/*0049c8f0*/ extern int particle_slot_call_render_method();
/*004a5d70*/ extern int object_render();
/*004a5da0*/ extern int object_destroy_frames();
/*004b4ba7*/ extern int entry();
/*004b5a47*/ extern int positive();
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d1160*/ extern struct RwFrame * RwFrameAddChild(struct RwFrame *p0, struct RwFrame *p1);
/*004d1330*/ extern struct RwFrame * RwFrameRemoveChild(struct RwFrame *p0);
/*004d1d40*/ extern struct RwCamera * RwCameraClear(struct RwCamera *p0, struct RwRGBA *p1, RwInt32 p2);
/*004d1e10*/ extern struct RwCamera * RwCameraSetViewWindow(struct RwCamera *p0, struct RwV2d *p1);
/*004d4490*/ extern RwMatrix * RwMatrixInvert(RwMatrix *p0, RwMatrix *p1);
/*004d49c0*/ extern RwMatrix * RwMatrixTransform(RwMatrix *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*004f4440*/ extern struct RpWorld * RpWorldAddCamera(struct RpWorld *p0, struct RwCamera *p1);
/*004f4470*/ extern struct RpWorld * RpWorldRemoveCamera(struct RpWorld *p0, struct RwCamera *p1);
/*004f44a0*/ extern struct RpWorld * RpWorldAddAtomic(struct RpWorld *p0, struct RpAtomic *p1);
/*004f4570*/ extern struct RpWorld * RpWorldAddClump(struct RpWorld *p0, struct RpClump *p1);
/*004f46d0*/ extern struct RpWorld * RpWorldRemoveClump(struct RpWorld *p0, struct RpClump *p1);
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*004f6a60*/ extern RwBool RpAtomicDestroy(struct RpAtomic *p0);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*005dd340*/ extern float k_f_0_5;
/*005dd378*/ extern float k_f_0_1;
/*005dda20*/ extern float k_f_4_2949673e9;
/*005ddc1c*/ extern float k_f_0_33333334;
/*005e64a4*/ extern float k_f_0;
/*0060d9c4*/ extern undefined g_car_60d9c4;
/*0060d9d8*/ extern undefined4 g_course_player_60d9d8;
/*006a26c4*/ extern float g_course_6a26c4;
/*006a26cc*/ extern undefined4 g_course_clumps_6a26cc;
/*006a2724*/ extern float g_course_6a2724;
/*006a272c*/ extern undefined4 g_course_clumps_6a272c;
/*006a2780*/ extern undefined4 g_course_sands_6a2780;
/*006a2794*/ extern float g_course_superg_6a2794;
/*006a2888*/ extern undefined g_course_6a2888;
/*006a291c*/ extern undefined4 g_course_get_6a291c;
/*006a2950*/ extern float g_course_get_6a2950;
/*006a29a4*/ extern undefined4 g_course_warzone_6a29a4;
/*006a29c0*/ extern undefined4 g_weapon_6a29c0;
/*006a29c4*/ extern undefined g_weapon_mortar_6a29c4;
/*006a29c8*/ extern undefined4 g_weapon_6a29c8;
/*006a29cc*/ extern undefined4 g_weapon_6a29cc;
/*006a29d0*/ extern undefined4 g_weapon_6a29d0;
/*006a2a70*/ extern undefined4 g_weapon_6a2a70;
/*006a2a80*/ extern undefined4 g_weapon_6a2a80;
/*006a4c28*/ extern struct ExplosionEntry g_explosion_queue[100];
/*006a5a38*/ extern undefined4 g_explosion_queue_6a5a38;
/*006a5a6c*/ extern undefined4 g_weapon_drum_6a5a6c;
/*006a5a78*/ extern undefined4 g_weapon_drum_6a5a78;
/*006a5c60*/ extern undefined g_weapon_6a5c60;
/*006a5c6c*/ extern undefined4 g_weapon_drum_6a5c6c;
/*006a5c70*/ extern undefined4 g_weapon_drum_6a5c70;
/*006a5c74*/ extern char *g_weapon_drum_6a5c74;
/*006a5c80*/ extern char *g_weapon_drum_6a5c80;
/*006a5d10*/ extern undefined4 g_weapon_6a5d10;
/*006a5d14*/ extern undefined4 g_weapon_drum_6a5d14;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*008bf050*/ extern undefined4 g_car_8bf050;
/*008bf060*/ extern undefined4 g_course_warzone_8bf060;
/*008bf084*/ extern undefined4 g_course_training_8bf084;
/*008bf088*/ extern undefined4 g_course_sands_8bf088;
/*008d8b30*/ extern undefined4 player_count;

extern void course_sands_cb1(char *param_1,char *param_2);


extern void course_sands_cb2(char *param_1);


extern void course_sands_cb5(char *param_1);


extern void course_superg_cb1(void);


extern void course_superg_cb4(char *param_1);


extern void course_training_cb1(char *param_1,char *param_2);


extern void course_training_cb2(char *param_1);


extern void course_training_cb10(char *param_1);


extern void __cdecl course_particle_init_random(undefined4 *param_1);


extern void anim_set_callback(char *param_1,char *param_2);


extern void course_zone_players_transform_update(char *param_1);


/* course_zone_contains_matrix */

/* "transforms a matrix into zone space and tests x/z against the zone bounds" */

extern undefined4 course_zone_contains_matrix(char *param_1,RwMatrix *param_2,RwReal *param_3);


/* anim_stop */

/* "state 0 and calls the callback with 0" */

extern void anim_stop(char *param_1);


/* course_zone_corners_to_world */

/* "transforms the zone corner points with the frame LTM and derives step sizes (0.1)" */

extern void course_zone_corners_to_world(char *param_1);


/* course_zone_contains_matrix_in_zone_6a2888 */

/* "course_zone_contains_matrix on the zone record at g_course_6a2888" */

extern void course_zone_contains_matrix_in_zone_6a2888(char *param_1,char *param_2);


/* course_get_matrix_pos_6a291c */

/* "g_course_6a291c + 0x30" */

extern int course_get_matrix_pos_6a291c(void);


/* course_player_markers_setup */

/* "assigns the 8 marker clumps of the world record and enables them" */

extern void __cdecl course_player_markers_setup (char *param_1,char *param_2,char *param_3,char *param_4,char *param_5, char *param_6);


/* course_warzone_cb2 */

extern void course_warzone_cb2(char *param_1);


/* course_get_8bf050 */

/* "g_car_8bf050[index]" */

extern undefined4 __cdecl course_get_8bf050(char *param_1);


/* course_get_6a2950 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "float g_course_6a2950" */

extern double course_get_6a2950(void);


/* weapon_mortar_free_assets */

extern void weapon_mortar_free_assets(void);


/* weapon_mortar_is_finished */

extern undefined4 weapon_mortar_is_finished(WeaponSlot *param_1);


/* weapon_mortar_unequip */

extern void weapon_mortar_unequip(WeaponSlot *param_1);


/* weapon_mortar_ammo_fraction */

extern double weapon_mortar_ammo_fraction(WeaponSlot *param_1);


/* weapon_mortar_clear_effects */

extern void weapon_mortar_clear_effects(void);


/* weapon_mortar_reset_all */

extern void weapon_mortar_reset_all(void);

#define EXPLOSION_QUEUE_LAST (99) /* last index of the g_explosion_queue ring (100 entries) */


/* explosion_queue */

/* Queues an explosion in the ring of 100 g_explosion_queue entries (next index at
   g_explosion_queue_6a5a38). position is the xyz centre plus radius in position[0..3], damage
   is multiplied by the distance falloff later, push_horizontal and push_vertical are the
   knock-back strengths, frames is how long it stays active. Then plays the flash/sound
   (sfx_play_player_set_at) and a ground decal (explosion_decal). The damage itself comes later
   from the type 0x19 trigger sphere that weapons_update_frame registers for every frame the
   entry's countdown is positive. Returns nothing. */

/* TIER2: unvalidated 2026-10-17 authority=diffemu */
typedef struct Vec4F { float v[4]; } Vec4F;
extern void explosion_queue(float *position,float damage,float push_horizontal,float push_vertical,int frames);


/* explosion_queue_clear_frames */

/* "sets frames = -1 for every ExplosionEntry (0x24 byte stride)" */

extern void explosion_queue_clear_frames(void);


/* explosion_queue_update */

/* "per frame: for each g_explosion_queue entry with frames_left > 0: decrement and
   trigger_sphere_register({x,y,z,radius}, type 0x19, owner = the entry, callback
   explosion_trigger_callback)" */

extern void explosion_queue_update(void);
extern char g_weapon_drum_6a5c8c[];
extern char g_weapon_drum_6a5d20[];

#define DRUM_SLOT_SIZE (0x2c) /* bytes per drum slot (stride of the drum slot table) */
#define DRUM_SLOT_WORDS (0xb) /* same stride in int words */
#define DRUM_RECORD_WORDS (0x11) /* stride of the part record table, in int words */
#define DRUM_TABLE_END (((LONG_PTR)&g_weapon_drum_6a5d20)) /* end address of the drum slot table */
#define DRUM_RECORD_END (((LONG_PTR)&g_weapon_drum_6a5c8c)) /* end address of the part record table */
#define DRUM_SLOT_ATTACHED (((LONG_PTR)&g_weapon_6a5c60 + 0x8)) /* slot_offset + this = attached count of that slot */
#define DRUM_PARTS (2) /* parts per drum: records claimed and atomics added */
#define DRUM_RECORD_ATOMIC (0x30) /* offset of the RpAtomic * inside a part record */


/* weapon_drum_equip */

/* type 0x0A; state 0x6A5C60 stride 0x2C (DRUM_SLOT_SIZE) */

/* Equips a drum for weapon_slot (inferred): takes the first free slot of the drum table at
   g_weapon_6a5c60, claims up to DRUM_PARTS free part records (table at g_weapon_drum_6a5a6c),
   attaches each part's frame under the slot's mount hook (side 1, then side 2), stores
   weapon_slot in the slot (+0xC, inferred), then adds the slot's clump and both part atomics to
   the course RpWorld. Returns the drum slot.
   No free-slot check: with all slots in use it returns a pointer past the table. If fewer than
   DRUM_PARTS records are free, the atomic loop reads stale record pointers (inferred). */

/* TIER2: unvalidated 2026-10-17 authority=diffemu */
undefined * weapon_drum_equip(WeaponSlot *weapon_slot)
{
  int *cursor;
  RpWorld *world;
  int attached_count;
  WeaponSlot *mount_side;
  int drum_index;
  int slot_offset;
  undefined *drum_slot;
  RpClump *clump;
  RpAtomic *atomic;
  
  /* find the first free drum slot (in-use flag at slot +0x10) */
  drum_index = 0;
  cursor = &g_weapon_drum_6a5c70;
  do {
    if (*cursor == 0) break;
    cursor = cursor + DRUM_SLOT_WORDS;
    drum_index = drum_index + 1;
  } while ((LONG_PTR)cursor < DRUM_TABLE_END);
  slot_offset = drum_index * DRUM_SLOT_SIZE;
  drum_slot = &g_weapon_6a5c60 + slot_offset;
  /* claim up to DRUM_PARTS free part records for this slot: each gets the slot as back-pointer
     and its in-use flag set, and its frame is attached to mount side 1, then side 2 */
  cursor = &g_weapon_drum_6a5a6c;
  mount_side = weapon_slot;
  do {
    if (*cursor == 0) {
      *(int **)(drum_slot + *(int *)(slot_offset + DRUM_SLOT_ATTACHED) * 4) = cursor + -0xb;
      attached_count = *(int *)(slot_offset + DRUM_SLOT_ATTACHED);
      *cursor = 1;
      cursor[-1] = (int)drum_slot;
      /* mount_side (typed WeaponSlot * by Ghidra) holds the side index: 1 for the first part,
         2 for the second (inferred) */
      if (attached_count == 0) {
        mount_side = (WeaponSlot *)0x1;
      }
      else if (attached_count == 1) {
        mount_side = (WeaponSlot *)0x2;
      }
      RwFrameAddChild(*(RwFrame **)(*(char **)((char *)&g_weapon_drum_6a5c74 + (LONG_PTR)mount_side * 4 + slot_offset) + 4),
                      (RwFrame *)cursor[2]);
      attached_count = *(int *)(slot_offset + DRUM_SLOT_ATTACHED) + 1;
      *(int *)(slot_offset + DRUM_SLOT_ATTACHED) = attached_count;
      if (attached_count == DRUM_PARTS) break;
    }
    cursor = cursor + DRUM_RECORD_WORDS;
  } while ((LONG_PTR)cursor < DRUM_RECORD_END);
  /* mark the drum slot in use, store its owner and add its clump to the world */
  clump = (RpClump *)(&g_weapon_drum_6a5c80)[drum_index * DRUM_SLOT_WORDS];
  (&g_weapon_drum_6a5c70)[drum_index * DRUM_SLOT_WORDS] = 1;
  (&g_weapon_drum_6a5c6c)[drum_index * DRUM_SLOT_WORDS] = weapon_slot;
  world = (RpWorld *)course_get_rpworld_00467e90();
  RpWorldAddClump(world,clump);
  /* add both part atomics to the world (drum_index is reused as the 0..DRUM_PARTS-1 counter) */
  drum_index = 0;
  do {
    atomic = *(RpAtomic **)(*(char **)(drum_slot + drum_index * 4) + DRUM_RECORD_ATOMIC);
    world = (RpWorld *)course_get_rpworld_00467e90();
    RpWorldAddAtomic(world,atomic);
    drum_index = drum_index + 1;
  } while (drum_index < DRUM_PARTS);
  return drum_slot;
}


/* weapon_drum_ammo_fraction */

extern double weapon_drum_ammo_fraction(WeaponSlot *param_1);


/* weapon_drum_free_assets */

extern void weapon_drum_free_assets(void);


