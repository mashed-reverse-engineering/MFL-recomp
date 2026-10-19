#include "mfl_tier2.h"
/*0040e680*/ extern int race_setup_get_course();
/*004162f0*/ extern int trail_set_param();
/*00426670*/ extern undefined __cdecl player_stat_add_turbo_used(undefined4 p0, undefined4 p1);
/*00434bb0*/ extern int get_event_type();
/*00435ec0*/ extern int event_type_uses_standard_menu();
/*00435f10*/ extern int get_career_row();
/*004379a0*/ extern int menu_event_course_index_get();
/*00474230*/ extern undefined course_object_pair_build_clump(undefined4 p0, undefined4 p1, int p2, int p3);
/*0047a100*/ extern int car_reset_state();
/*0047bb00*/ extern int car_world_collision_query();
/*0047fc10*/ extern undefined hud_label_draw_text(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5, undefined4 p6, float p7, int p8);
/*0048e410*/ extern int rwp_object_find_index_by_id();
/*0048e650*/ extern int rwp_object_set_in_world();
/*00490c10*/ extern int __cdecl physics_body_get_position(undefined4 p0, undefined4 p1);
/*00491d70*/ extern int car_reset_physics();
/*00492b50*/ extern undefined __cdecl car_add_velocity_impl(undefined4 p0, undefined4 p1);
/*00492d10*/ extern int rwp_object_set_active();
/*004a5da0*/ extern int object_destroy_frames();
/*004b2b48*/ extern int _ftol();
/*004b3850*/ extern char * __cdecl _strstr(char *p0, char *p1);
/*004b4ba7*/ extern int entry();
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d56d0*/ extern struct RwV3d * RwV3dTransformVectors(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*004f44d0*/ extern struct RpWorld * __cdecl RpWorldRemoveAtomic(struct RpWorld *p0, struct RpAtomic *p1);
/*004f4560*/ extern struct RpWorld * __cdecl RpAtomicGetWorld(struct RpAtomic *p0);
/*004f6a60*/ extern RwBool RpAtomicDestroy(struct RpAtomic *p0);
/*005dd340*/ extern float k_f_0_5;
/*005dd370*/ extern float k_f_0_3;
/*005dd61c*/ extern float k_f_0_001;
/*005ddafc*/ extern float k_f_0_05;
/*005ddc28*/ extern float k_f_0;
/*005de1b4*/ extern float k_f_0_0625;
/*005de47c*/ extern float k_f_n0_25;
/*005deb34*/ extern float k_f_300;
/*005df300*/ extern float k_f_0_333333;
/*005dfd60*/ extern float k_f_160;
/*006292ec*/ extern float g_car_default_speed_scale;
/*006292f0*/ extern undefined4 car_handling_table;
/*006292f8*/ extern undefined4 g_car_6292f8;
/*0062ac80*/ extern undefined g_weapon_62ac80;
/*006af5c0*/ extern undefined1 g_car_6af5c0;
/*006af5e4*/ extern undefined4 g_car_6af5e4;
/*006af6e4*/ extern undefined4 g_car_6af6e4;
/*006af6ec*/ extern undefined4 g_car_6af6ec;
/*006c2658*/ extern undefined4 g_car_6c2658;
/*006c2680*/ extern undefined4 g_course_object_count;
/*006c2684*/ extern undefined4 g_course_object_pair_count;
/*006c288c*/ extern undefined4 g_car_6c288c;
/*006c2894*/ extern float g_car_6c2894;
/*006c2898*/ extern undefined4 g_course_open_6c2898;
/*006c289c*/ extern float g_car_6c289c;
/*006c28b8*/ extern float g_car_6c28b8;
/*006c28bc*/ extern float g_car_6c28bc;
/*006c2944*/ extern undefined4 g_car_6c2944;
/*006c29f4*/ extern undefined4 g_car_6c29f4;
/*006c29f8*/ extern undefined g_app_state_6c29f8;
/*006c2ee0*/ extern undefined4 g_rwp_trigger_group_count;
/*006c2ee4*/ extern undefined4 g_car_6c2ee4;
/*006c2f24*/ extern undefined g_car_6c2f24;
/*006c2f64*/ extern undefined4 g_car_6c2f64;
/*006c2f68*/ extern undefined4 g_gfx_6c2f68;
/*006c3348*/ extern undefined1 g_text_pages_6c3348;
/*006c33c8*/ extern undefined4 g_gfx_6c33c8;
/*006c3d9c*/ extern undefined4 g_text_pages_6c3d9c;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007ef7bc*/ extern undefined4 g_cars_7ef7bc;
/*007ef7c4*/ extern undefined4 g_cars_7ef7c4;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*00888640*/ extern struct RwpObjectDesc g_rwp_objects[32];
/*0089de20*/ extern undefined1 g_physics_scenarios;
/*0089de60*/ extern undefined4 g_gfx_89de60;
/*008b06e0*/ extern struct Car player_car[16];
/*008bd720*/ extern undefined4 g_car_8bd720;
/*008bd74c*/ extern undefined4 g_collision_triangle_count;
/*008bd764*/ extern undefined4 g_car_8bd764;
/* course_object_pair_counts_clear */

/* "g_car_6c2680 = g_car_6c2684 = 0" */

extern void course_object_pair_counts_clear(void);


/* course_object_pairs_build_clumps */

/* "builds a clump for every object pair entry" */

extern void __cdecl course_object_pairs_build_clumps (char *param_1,char *param_2,char *param_3,char *param_4, char *param_5,char *param_6,char *param_7,char *param_8, char *param_9,char *param_10,char *param_11,char *param_12, char *param_13,char *param_14,char *param_15,char *param_16);


/* course_objects_reset_transforms */

/* "sets the active bit of each entry and applies its matrix to the frame" */

extern void course_objects_reset_transforms(void);


/* course_objects_destroy_atomics */

/* "removes and destroys the atomic of each entry" */

extern void course_objects_destroy_atomics(void);


/* course_cell_hash_pack */

/* "packs four float values into a 7/9/7/7 bit key" */

extern uint __cdecl course_cell_hash_pack (char *param_1,char *param_2,char *param_3,char *param_4,char *param_5, char *param_6,char *param_7);


/* course_cell_bounds_from_position */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "computes cell indices and fills the cell min/max bounds" */

extern void course_cell_bounds_from_position(char *param_1,float *param_2,int *param_3,int *param_4);


/* car_get_car_6c2944 */

/* "return g_car_6c2944;" */

extern undefined4 car_get_car_6c2944(void);


/* car_hit_by_leader */

/* called when the leader car touches the car in game mode 1 */

extern void __cdecl car_hit_by_leader(char *param_1);


/* car_set_bounds */

/* fills the 8 corner points (+0x90) and the derived edge/face points of a car from its min/max box
    */

extern undefined4 car_set_bounds(char *param_1,float *param_2);


/* car_get_wheel_average */

/* "(out, car, wheel): average of the 16 per wheel floats at Car + 0x890 (stride 0xC4)" */

extern undefined4 car_get_wheel_average(float *param_1,char *param_2,char *param_3);


/* car_add_velocity */

/* "(delta vec, car): adds to Car.velocity, or applies an impulse in RWP mode" */

extern undefined4 car_add_velocity(float *param_1,char *param_2);


/* car_reset_full */

/* "car_fn_491d70 + car_reset_state, returns 1" */

extern undefined4 __cdecl car_reset_full(char *param_1);

#define MAX_CAR_INDEX (0xf) /* last valid index into player_car (16 cars) */


/* car_get_velocity_any_mode */

/* "(out, car): Car.velocity or the RWP velocity scaled by 300" */

/* Writes the velocity of a car to out_velocity (3 floats, x y z) and returns 1. The velocity is
   Car.velocity while the car is not simulated by the physics library (rwp_object == 0), otherwise
   the physics body velocity scaled by 300 (inferred: per tick to per second). Returns 0 and leaves
   out_velocity alone when car_index is not a valid car or the physics body query fails. */

/* TIER2: validated 2026-10-18 authority=diffemu */
undefined4 car_get_velocity_any_mode(float *out_velocity,uint car_index)
{
  int got_position;

  /* car_index is out of range (player_car has MAX_CAR_INDEX + 1 entries) */
  if (MAX_CAR_INDEX < car_index) {
    return 0;
  }
  /* no physics body: copy the velocity stored in the car */
  if (player_car[car_index].rwp_object == 0) {
    *out_velocity = player_car[car_index].velocity[0];
    out_velocity[1] = player_car[car_index].velocity[1];
    out_velocity[2] = player_car[car_index].velocity[2];
    return 1;
  }
  /* physics body present: the physics library writes the velocity, scale it */
  got_position = physics_body_get_position(out_velocity,car_index);
  if (got_position != 0) {
    *out_velocity = *out_velocity * k_f_300;
    out_velocity[1] = out_velocity[1] * k_f_300;
    out_velocity[2] = out_velocity[2] * k_f_300;
    return 1;
  }
  return 0;
}


/* car_get_int_from_float_field */

/* "_ftol of a car float field into the output" */

extern undefined4 car_get_int_from_float_field(undefined4 *param_1,char *param_2);


/* car_set_matrices_from_floats */

/* "(car, float[...]) copies 16 floats to player_car[car].matrices and more" */

extern undefined4 car_set_matrices_from_floats(char *param_1,float *param_2);


/* car_get_forward */

/* "(&out, car): transforms (0,0,1) by the current matrix, stored at +0x9D4" */

extern undefined4 car_get_forward(float *param_1,char *param_2);


/* car_turbo_spend */

/* "(car): if the turbo charge is above 1000 spends it (state 2), else converts the charge (state 1,
   +1000); adds the used charge to the player's turbo_used statistic" */

extern undefined4 car_turbo_spend(char *param_1);


/* car_turbo_charge_update */

/* "(car, dt): turbo_charge += 2 * dt while the accelerate input is above 160, then -= dt, clamped
   to 0..3000" */

extern undefined4 __cdecl car_turbo_charge_update(char *param_1,char *param_2,char *param_3);


/* car_resolve_triangle_contacts */

/* "(car, triangle list at 0x843EA0 collected by car_world_collision_query): contact response of the
   car body points against the triangles (inferred)" */

extern int car_resolve_triangle_contacts(char *param_1,char *param_2);


/* car_get_6292ec */

/* "float g_car_6292ec" */

extern float10 car_get_6292ec(void);

#define EVENT_TYPE_4_ROW_OFFSET (0xf)
#define EVENT_TYPE_5_ROW_OFFSET (0x1e)
#define EVENT_TYPE_14_COURSE_ID (0x48) /* course id used instead of the race setup course (inferred) */
#define TABLE_END_KEY (-1)


/* career_row_for_event */

/* "(was career_row_for_event) looks up the handling-table entry of the career event's course, 4 outputs" */

/* Finds the handling-table entry for the course of the current career event and writes its
   values to out_a..out_d (out_a, out_b: table fields as floats; out_c, out_d: field / 1000)
   (inferred). The course comes from the career row via race_setup_get_course. Outputs are
   left unchanged when no entry matches. */

/* TIER2: unvalidated 2026-10-16 authority=diffemu */
void career_row_for_event(float *out_a,float *out_b,float *out_c,float *out_d)
{
  int *next_key_ptr;
  int career_row;
  int event_type;
  int *entry;
  int *key_ptr;

  /* row from the career table; replaced by the menu course index when the event does not
     use the standard menu, and for event types 11-13 */
  career_row = get_career_row();
  event_type = event_type_uses_standard_menu();
  if (event_type == 0) {
    career_row = menu_event_course_index_get();
  }
  /* event types 11, 12, 13 also take the row from the menu course index */
  event_type = get_event_type();
  if (((event_type == 0xb) || (event_type = get_event_type(), event_type == 0xc)) ||
     (event_type = get_event_type(), event_type == 0xd)) {
    career_row = menu_event_course_index_get();
  }
  /* event types 4 and 5 shift the row by a fixed offset (inferred: next block of rows) */
  event_type = get_event_type();
  if (event_type == 4) {
    career_row = career_row + EVENT_TYPE_4_ROW_OFFSET;
  }
  event_type = get_event_type();
  if (event_type == 5) {
    career_row = career_row + EVENT_TYPE_5_ROW_OFFSET;
  }
  /* from here on event_type holds the course id (reused variable); career_row is reused for
     the event type */
  event_type = race_setup_get_course(career_row);
  career_row = get_event_type();
  if (career_row == 0xe) {
    event_type = EVENT_TYPE_14_COURSE_ID;
  }
  /* walk the handling table (5-dword records, key first) until the end key; copy the
     record whose key matches the course */
  if (car_handling_table != TABLE_END_KEY) {
    key_ptr = &car_handling_table;
    entry = &g_car_6292f8;
    do {
      if (*key_ptr == event_type) {
        *out_a = (float)entry[-1];
        *out_b = (float)*entry;
        *out_c = (float)entry[1] * k_f_0_001;
        *out_d = (float)entry[2] * k_f_0_001;
      }
      next_key_ptr = entry + 3;
      key_ptr = entry + 3;
      entry = entry + 5;
    } while (*next_key_ptr != TABLE_END_KEY);
  }
  return;
}


#define LABEL_COUNT (8)
#define LABEL_STRIDE_DWORDS (0x2c) /* 0xB0 bytes per label record */
#define FADE_STEP (4)

/* hud_labels_age_and_fade */

/* "subtracts the elapsed ticks from the 8 labels and fades their colour bytes" */

/* TIER2: validated 2026-10-18 authority=diffemu */
void __cdecl hud_labels_age_and_fade(int elapsed_ticks)
{
  int *label_ticks;
  int new_value;
  int labels_left;

  label_ticks = &g_car_6c29f4;
  labels_left = LABEL_COUNT;
  do {
    /* only active labels (ticks != 0): count the ticks down, clamp at 0 */
    if (*label_ticks != 0) {
      new_value = *label_ticks - elapsed_ticks;
      *label_ticks = new_value;
      if (new_value < 0) {
        *label_ticks = 0;
      }
      /* fade the colour bytes at +0x7 and +0xb by FADE_STEP each, clamp at 0 */
      new_value = *(byte *)((char *)label_ticks + 0xb) - FADE_STEP;
      *(float *)&label_ticks[3] = *(float *)&label_ticks[4] + *(float *)&label_ticks[3];      /* diffemu: the dwords are floats, the (int) / (float) casts converted */
      if (new_value < 0) {
        new_value = 0;
      }
      *(char *)((char *)label_ticks + 0xb) = (char)new_value;
      new_value = *(byte *)((char *)label_ticks + 7) - FADE_STEP;
      if (new_value < 0) {
        new_value = 0;
      }
      *(char *)((char *)label_ticks + 7) = (char)new_value;
      /* move the two float pairs by their velocity: position += velocity (inferred) */
      *(float *)&label_ticks[-4] = *(float *)&label_ticks[-2] + *(float *)&label_ticks[-4];
      *(float *)&label_ticks[-3] = *(float *)&label_ticks[-1] + *(float *)&label_ticks[-3];
    }
    label_ticks = label_ticks + LABEL_STRIDE_DWORDS;
    labels_left = labels_left + -1;
  } while (labels_left != 0);
  return;
}


/* hud_labels_render_all */

/* "blending setup then renders each 0xB0 byte label (8)" */

extern void hud_labels_render_all(void);

#define SCENARIO_ID_MAX (1999) /* ids up to this index g_physics_scenarios; larger ids are stored as string ids (inferred) */
#define NO_STRING_ID (0xffffffff)
#define TRIGGER_GROUP_STRIDE_BYTES (0x8c) /* 35 dwords per trigger group */
#define TRIGGER_GROUP_STRIDE_DWORDS (0x23)
#define PHYSICS_SCENARIO_STRIDE (0x2510) /* bytes per g_physics_scenarios entry */


/* text_page_add_string_id */

/* "adds a string id (>= 2000) to the current page; smaller ids add a scenario entry" */

/* Adds string_id to the current trigger group (g_rwp_trigger_group_count) and returns 1. Ids above
   SCENARIO_ID_MAX are stored as string ids. Smaller ids (inferred: scenario indices) are appended to
   the group as a pointer into g_physics_scenarios, and the group's entry count goes up by one. */

/* TIER2: validated 2026-10-18 authority=diffemu */
undefined4 text_page_add_string_id(int string_id)
{
  char *rec;

  rec = (char *)&g_car_6c2ee4 + g_rwp_trigger_group_count * TRIGGER_GROUP_STRIDE_BYTES;
  if (string_id >= SCENARIO_ID_MAX + 1) {
    *(int *)(rec + *(int *)(rec + 0x80) * 4 + 0x3c) = string_id;
    return 1;
  }
  *(undefined4 *)(rec + *(int *)(rec + 0x80) * 4 + 0x40) = NO_STRING_ID;
  *(char **)(rec + *(int *)(rec + 0x80) * 4) = (char *)&g_physics_scenarios + string_id * PHYSICS_SCENARIO_STRIDE;
  *(int *)(rec + 0x80) = *(int *)(rec + 0x80) + 1;
  return 1;
}


/* text_pages_clear */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "clears the 0x84 byte records at 0x6C33C8" */

extern void text_pages_clear(void);


/* text_pages_find_string */

/* "strstr over the text pages, returns the page index" */

extern int text_pages_find_string(char *_SubStr);


/* rwp_trigger_groups_resolve_indices */

/* "for each trigger group resolves the rwp object base_label + n to object indices (g_rwp_objects)"
    */

extern void rwp_trigger_groups_resolve_indices(void);


/* gfx_groups_reset */

#define GROUP_ENTRY_SLOT_BASE (-0x21) /* dword offset from a group to its entry slot 0 */
#define ENTRY_OBJECT_COUNT_OFFSET (0x144)
#define ENTRY_OBJECT_ID_OFFSET (0xc4)
#define ENTRY_RECORD_COUNT_OFFSET (0x2428)
#define ENTRY_RECORD_OFFSET (0x14c)
#define ENTRY_RECORD_STRIDE_DWORDS (0x117)
#define ENTRY_RECORD_TAIL_DWORD (0x115) /* dword index inside a record, written as zero */


/* gfx_groups_reset */

/* "per group clears counters; sets in-world and active on the objects of the first entry, clears them on the others" */

/* Resets all trigger groups (count in g_rwp_trigger_group_count, group records at g_gfx_6c2f68 with
   TRIGGER_GROUP_STRIDE_DWORDS dwords each). Takes no parameters and returns nothing. For each group
   the counter words are cleared and every entry's record words are zeroed; the objects of the
   group's first entry get in-world and active set, the objects of the other entries get them
   cleared (inferred: the first entry is the active one). */

/* TIER2: validated 2026-10-18 authority=diffemu */
void gfx_groups_reset(void)
{
  char *entry_addr;
  undefined4 *cursor;
  int first_entry;
  int object_index;
  int entry_index;
  int group_index;
  int *group;

  group_index = 0;
  if (0 < (int)g_rwp_trigger_group_count) {      /* diffemu: undefined4 global, the original compares signed (jle) */
    group = &g_gfx_6c2f68;
    /* each trigger group: clear its counter words (inferred), then walk its entries */
    do {
      group[-1] = 0;
      group[1] = 0;
      entry_index = 0;
      if (0 < *group) {
        /* each entry of the group; entry slot 0 is the group's first entry */
        do {
          object_index = 0;
          entry_addr = group[entry_index + GROUP_ENTRY_SLOT_BASE];
          first_entry = group[GROUP_ENTRY_SLOT_BASE];
          /* in-world and active on each object of the entry: true only for the first entry's objects */
          if (0 < *(int *)(entry_addr + ENTRY_OBJECT_COUNT_OFFSET)) {
            cursor = (undefined4 *)(entry_addr + ENTRY_OBJECT_ID_OFFSET);
            do {
              rwp_object_set_in_world(*cursor,first_entry == entry_addr);
              rwp_object_set_active(*cursor,first_entry == entry_addr);
              object_index = object_index + 1;
              cursor = cursor + 1;
            } while (object_index < *(int *)(entry_addr + ENTRY_OBJECT_COUNT_OFFSET));
          }
          /* zero the words of each record of the entry; first_entry is reused here as the record index */
          first_entry = 0;
          if (0 < *(int *)(entry_addr + ENTRY_RECORD_COUNT_OFFSET)) {
            cursor = (undefined4 *)(entry_addr + ENTRY_RECORD_OFFSET);
            do {
              cursor[ENTRY_RECORD_TAIL_DWORD] = 0;
              *cursor = 0;
              first_entry = first_entry + 1;
              cursor = cursor + ENTRY_RECORD_STRIDE_DWORDS;
            } while (first_entry < *(int *)(entry_addr + ENTRY_RECORD_COUNT_OFFSET));
          }
          entry_index = entry_index + 1;
        } while (entry_index < *group);
      }
      group_index = group_index + 1;
      group = group + TRIGGER_GROUP_STRIDE_DWORDS;
    } while (group_index < (int)g_rwp_trigger_group_count);
  }
  return;
}


/* material_set_alpha_callback */

/* "RpMaterial callback: colour alpha = 0xFF - (int)value" */

extern int material_set_alpha_callback(char *param_1);



