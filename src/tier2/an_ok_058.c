#include "mfl_tier2.h"
/*00406b90*/ extern undefined __cdecl ktc_add_pickup(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5, undefined4 p6, undefined4 p7, undefined4 p8, undefined4 p9, undefined4 p10, undefined4 p11, undefined4 p12, undefined4 p13, undefined4 p14, undefined4 p15, undefined4 p16);
/*004357e0*/ extern int __cdecl career_record_ptr(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5, undefined4 p6, undefined4 p7, undefined4 p8, undefined4 p9, undefined4 p10, undefined4 p11, undefined4 p12, undefined4 p13, undefined4 p14, undefined4 p15, undefined4 p16);
/*004492b0*/ extern int lua_register_course_functions();
/*00474340*/ extern undefined __cdecl course_object_pair_add(undefined4 p0, undefined4 p1, undefined4 p2);
/*00480130*/ extern int text_page_reset_line();
/*00480150*/ extern int text_page_next();
/*00480190*/ extern int text_page_add_string_id();
/*0048cf80*/ extern int lua_run_script_file();
/*004c7150*/ extern int lua_gettop();
/*004c7220*/ extern double lua_tonumber();
/*004c72d0*/ extern int lua_pushnumber();
/*006299d8*/ extern undefined1 g_gfx_6299d8;
/*006299dc*/ extern undefined4 g_lua_current_type;
/*006299e0*/ extern float g_lua_current_respawn;
/*006f2adc*/ extern float g_lua_current_pos;
/*006f2ae0*/ extern float g_gfx_6f2ae0;
/*006f2ae4*/ extern float g_gfx_6f2ae4;
/*006f2aec*/ extern struct CourseCounts *g_course_counts;
/*006f2af8*/ extern struct CourseLap *g_course_lap;
/*006f2afc*/ extern undefined4 g_lap_line_count;
/*00888640*/ extern struct RwpObjectDesc g_rwp_objects[32];
/*0089de40*/ extern undefined4 g_gfx_89de40;
/*0089de60*/ extern undefined4 g_gfx_89de60;
/*0089df68*/ extern undefined4 g_gfx_89df68;
/*0089df94*/ extern undefined4 g_gfx_89df94;
/*008a0248*/ extern undefined4 g_gfx_8a0248;
/*008a0250*/ extern undefined4 g_gfx_8a0250;
/*008a025c*/ extern char *g_lua_physics_8a025c;
/*008a032c*/ extern undefined4 g_gfx_8a032c;
/* gfx_set_6299d8 */

/*  */

extern void gfx_set_6299d8(undefined1 param_1);


/* lua_run_course_script */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "resets the lua current pos/type and runs the script with lua_register_course_functions" */

extern void __cdecl lua_run_course_script(char *param_1);


/* lua_Lap_Variations */

/* Lua function Lap_Variations (registered by the script engine) */

extern undefined4 lua_Lap_Variations(char *param_1);


/* lua_Split_Sector */

/* Lua function Split_Sector (registered by the script engine) */

extern undefined4 lua_Split_Sector(char *param_1);


/* lua_Lap_Line */

/* Lua function Lap_Line (registered by the script engine) */

extern undefined4 lua_Lap_Line(char *param_1);


/* lua_Safe_Start_Lines */

/* Lua function Safe_Start_Lines (registered by the script engine) */

extern undefined4 lua_Safe_Start_Lines(char *param_1);


/* lua_Clump_Mts_Pair */

/* Lua function Clump_Mts_Pair (registered by the script engine) */

extern undefined4 lua_Clump_Mts_Pair(char *param_1);


/* lua_Physics_Object */

/* Lua function Physics_Object (registered by the script engine) */

extern undefined4 lua_Physics_Object(char *param_1);


/* lua_Clump_Exclude_From_World */

/* Lua function Clump_Exclude_From_World (registered by the script engine) */

extern undefined4 lua_Clump_Exclude_From_World(char *param_1);


/* lua_Ambient_RGB */

/* Lua function Ambient_RGB (registered by the script engine) */

extern undefined4 lua_Ambient_RGB(char *param_1);


/* lua_Setup_Fog */

/* Lua function Setup_Fog (registered by the script engine) */

extern undefined4 lua_Setup_Fog(char *param_1);


/* lua_Modify_Fog */

/* Lua function Modify_Fog (registered by the script engine) */

extern undefined4 lua_Modify_Fog(char *param_1);


/* lua_Vehicle_Shininess_Range */

/* Lua function Vehicle_Shininess_Range (registered by the script engine) */

extern undefined4 lua_Vehicle_Shininess_Range(char *param_1);


/* lua_Shine_Scale */

/* Lua function Shine_Scale (registered by the script engine) */

extern undefined4 lua_Shine_Scale(char *param_1);


/* lua_Shadow_Scale */

/* Lua function Shadow_Scale (registered by the script engine) */

extern undefined4 lua_Shadow_Scale(char *param_1);


/* lua_Vehicle_Body_Dirt_Texture__Vehicle_Wheel_Dirt_Texture */

/* Lua function Vehicle_Body_Dirt_Texture, Vehicle_Wheel_Dirt_Texture (registered by the script
   engine) */

extern undefined4 lua_Vehicle_Body_Dirt_Texture__Vehicle_Wheel_Dirt_Texture(char *param_1);

#define CLUMP_ANIM_ENTRY_SIZE (0xc)
#define SET_CLUMP_ANIM_ARGS_WITH_ID (3)


/* lua_SetClumpAnim */

/* Lua function SetClumpAnim (registered by the script engine) */
/*
 * Lua: SetClumpAnim([anim_id,] clump_id, mts_id) (inferred from arg order)
 * Appends one clump animation entry to the course's clump anim table: zeroes its first word, stores
 * clump_id and mts_id as bytes in the clump_mts_pairs area, and bumps clump_anim_count. With three
 * arguments the first one is also stored as anim_id. Returns 0.
 */

/* TIER2: unvalidated 2026-10-17 authority=diffemu */
undefined4 lua_SetClumpAnim(undefined4 lua_state)
{
  byte *anim_entry; /* the entry's anim_id word (typed byte * by Ghidra) */
  int entry_offset; /* anim_index * CLUMP_ANIM_ENTRY_SIZE */
  CourseCounts *counts;
  byte pair_byte; /* holds clump_id, then mts_id */
  char anim_id;
  int arg_count;
  int anim_index; /* index of the new entry, then reused as its byte offset */

  /* the last two Lua args are clump_id and mts_id */
  arg_count = lua_gettop(lua_state);
  anim_index = g_course_counts->clump_anim_count;
  entry_offset = anim_index * CLUMP_ANIM_ENTRY_SIZE;
  anim_entry = g_course_counts->clump_mts_pairs + entry_offset + 0x4c;
  anim_entry[0] = 0;
  anim_entry[1] = 0;
  anim_entry[2] = 0;
  anim_entry[3] = 0;
  anim_index = anim_index * CLUMP_ANIM_ENTRY_SIZE;
  pair_byte = (int)lua_tonumber(lua_state,arg_count + -1);
  g_course_counts->clump_mts_pairs[anim_index + 0x44] = pair_byte;
  pair_byte = (int)lua_tonumber(lua_state,arg_count);
  counts = g_course_counts;
  g_course_counts->clump_mts_pairs[anim_index + 0x45] = pair_byte;
  counts->clump_anim_count = counts->clump_anim_count + 1;
  /* 3-argument form: the first Lua arg is the anim_id stored in the entry */
  if (arg_count == SET_CLUMP_ANIM_ARGS_WITH_ID) {
    anim_id = (int)lua_tonumber(lua_state,1);
    *(int *)(g_course_counts->clump_mts_pairs + entry_offset + 0x4c) = (int)anim_id;
  }
  return 0;
}


/* lua_RWP_Object_Box */

/* Lua function RWP_Object_Box (registered by the script engine) */

extern undefined4 lua_RWP_Object_Box(char *param_1);


/* lua_RWP_Object_Cylinder */

/* Lua function RWP_Object_Cylinder (registered by the script engine) */

extern undefined4 lua_RWP_Object_Cylinder(char *param_1);


/* lua_RWP_Object_Sphere */

/* Lua function RWP_Object_Sphere (registered by the script engine) */

extern undefined4 lua_RWP_Object_Sphere(char *param_1);


/* lua_RWP_Object_Capsule */

/* Lua function RWP_Object_Capsule (registered by the script engine) */

extern undefined4 lua_RWP_Object_Capsule(char *param_1);


/* lua_RWP_Object_Properties */

/* Lua function RWP_Object_Properties (registered by the script engine) */

extern undefined4 lua_RWP_Object_Properties(char *param_1);


/* lua_RWP_Object_CentreOfMassOffset */

/* Lua function RWP_Object_CentreOfMassOffset (registered by the script engine) */

extern undefined4 lua_RWP_Object_CentreOfMassOffset(char *param_1);


/* lua_RWP_Object_BaseLabel */

/* Lua function RWP_Object_BaseLabel (registered by the script engine) */

extern undefined4 lua_RWP_Object_BaseLabel(char *param_1);


/* lua_RWP_Object_ShatterVel */

/* Lua function RWP_Object_ShatterVel (registered by the script engine) */

extern undefined4 lua_RWP_Object_ShatterVel(char *param_1);


/* lua_Set_Current_Position */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* Lua function Set_Current_Position (registered by the script engine) */

extern undefined4 lua_Set_Current_Position(char *param_1);


/* lua_Set_Current_Type */

/* Lua function Set_Current_Type (registered by the script engine) */

extern undefined4 lua_Set_Current_Type(char *param_1);


/* lua_Set_Current_Respawn_Time */

/* Lua function Set_Current_Respawn_Time (registered by the script engine) */

extern undefined4 lua_Set_Current_Respawn_Time(char *param_1);


/* lua_Set_Track_Powerups */

/* Lua function Set_Track_Powerups (registered by the script engine) */

extern undefined4 lua_Set_Track_Powerups(char *param_1);


/* lua_Physics_Scenario_Set_Sequence */

/* Lua function Physics_Scenario_Set_Sequence (registered by the script engine) */

extern undefined4 lua_Physics_Scenario_Set_Sequence(char *param_1);


/* lua_Physics_Scenario_Add_Objects */

/* Lua function Physics_Scenario_Add_Objects (registered by the script engine) */

extern undefined4 lua_Physics_Scenario_Add_Objects(char *param_1);

#define SCENARIO_STRIDE_WORDS (0x944)
#define SCENARIO_STRIDE_BYTES (0x2510)
#define ANIM_ENTRY_SIZE (0x1c)
#define ANIM_ENTRY_WORDS (7)


/* lua_Physics_Scenario_Add_Animated_Object */

/* Lua function Physics_Scenario_Add_Animated_Object (registered by the script engine) */
/*
 * Lua: Physics_Scenario_Add_Animated_Object(scenario, rwp_object, anim_value) (inferred from arg order)
 * Appends one animated object entry to a physics scenario: stores anim_value at entry +0xc and the
 * RWP object's base_label at entry +0x0, then increments the scenario's entry count. Returns 0.
 */

/* TIER2: unvalidated 2026-10-16 authority=diffemu */
undefined4 lua_Physics_Scenario_Add_Animated_Object(undefined4 param_1)
{
  int arg_count;
  int scenario_index;
  int object_index;
  undefined4 anim_value;

  /* the last three Lua args, in order: scenario, rwp object, value */
  arg_count = lua_gettop(param_1);
  scenario_index = (int)lua_tonumber(param_1,arg_count + -2);
  object_index = (int)lua_tonumber(param_1,arg_count + -1);
  anim_value = (int)lua_tonumber(param_1,arg_count);
  /* arg_count is reused here: now the base label of the RWP object */
  arg_count = g_rwp_objects[object_index].base_label;
  /* object_index is reused here: now the scenario's current entry count (index of the new entry) */
  object_index = (&g_gfx_8a032c)[scenario_index * SCENARIO_STRIDE_WORDS];
  *(undefined4 *)((char *)&g_lua_physics_8a025c + object_index * ANIM_ENTRY_SIZE + scenario_index * SCENARIO_STRIDE_BYTES) = anim_value;
  (&g_gfx_8a0250)[scenario_index * SCENARIO_STRIDE_WORDS + object_index * ANIM_ENTRY_WORDS] = arg_count;
  /* bump the scenario's entry count */
  (&g_gfx_8a032c)[scenario_index * SCENARIO_STRIDE_WORDS] = (&g_gfx_8a032c)[scenario_index * SCENARIO_STRIDE_WORDS] + 1;
  return 0;
}


/* lua_Physics_Scenario_Create_Trigger */

/* Lua function Physics_Scenario_Create_Trigger (registered by the script engine) */

extern undefined4 lua_Physics_Scenario_Create_Trigger(char *param_1);


/* lua_Physics_Scenario_Trigger_Add_Objects */

/* Lua function Physics_Scenario_Trigger_Add_Objects (registered by the script engine) */

extern undefined4 lua_Physics_Scenario_Trigger_Add_Objects(char *param_1);


/* lua_Physics_Scenario_Trigger_Set_AI_Poly */

/* Lua function Physics_Scenario_Trigger_Set_AI_Poly (registered by the script engine) */

extern undefined4 lua_Physics_Scenario_Trigger_Set_AI_Poly(char *param_1);


/* lua_KTC_AddPickUp */

/* Lua function KTC_AddPickUp (registered by the script engine) */

extern undefined4 lua_KTC_AddPickUp(char *param_1);


/* lua_ExpObj_Clump_Mts_Pair */

/* Lua function ExpObj_Clump_Mts_Pair (registered by the script engine) */

extern undefined4 lua_ExpObj_Clump_Mts_Pair(char *param_1);



