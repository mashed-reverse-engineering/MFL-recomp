#include "mfl_tier2.h"
typedef int CourseId;
/*0040e6a0*/ extern int vehicles_any_active();
/*0040e6f0*/ extern int course_unload_current();
/*0040e9d0*/ extern int vehicle_prepare_parts_0();
/*0040e9e0*/ extern int vehicle_load_from_piz();
/*0040ea40*/ extern int camera_world_attach();
/*0040eac0*/ extern undefined __cdecl course_model_path_copy(undefined4 p0, undefined4 p1);
/*0042b0b0*/ extern int course_get_split_sector_8();
/*0042b3f0*/ extern int course_world_instance_for_viewport();
/*0042b450*/ extern undefined __cdecl course_open_and_select(undefined4 p0);
/*004a8420*/ extern int debug_printf();
/*0050b5e0*/ extern int nop_stub();
/*00843e80*/ extern undefined4 g_course_843e80;
/* race_load_course */
#define COURSE_ARCTIC (0)
#define COURSE_ANGELS (1)
#define COURSE_ROUABOUT (2)
#define COURSE_EGYPT (3)
#define COURSE_EGYPT_R (4)
#define COURSE_NEUST_R (5)
#define COURSE_TIERRA_N (6)
#define COURSE_SUPERG_N (8)
#define COURSE_FOREST_R (9)
#define COURSE_SANDS (11)
#define COURSE_HIGHWAYN (12)
#define COURSE_SANDSNOW (13)
#define COURSE_STORM_R (15)
#define COURSE_NT1 (17)
#define COURSE_DEMR (18)
#define COURSE_NT1_S (20)
#define COURSE_KEYS01 (22)
#define COURSE_KEYS (23)
#define COURSE_DEM (24)
#define COURSE_SUPERG (25)
#define COURSE_CITY (26)
#define COURSE_TRAINING (30)
#define COURSE_WARZONE (33)
#define COURSE_FOREST (34)
#define COURSE_STORM (36)
#define COURSE_DUMP (37)
#define COURSE_HIGHWAY (38)
#define COURSE_NEUSTEIN (39)
#define COURSE_LIST_END (-1)

/*
 * Loads every course in two passes. Takes no parameters and returns nothing.
 * Pass 1 opens and instances COURSE_ARCTIC, then each id in course_list up to COURSE_LIST_END, and saves each one.
 * Pass 2 loads vehicle data and prepares parts for courses 0..89, then unloads.
 * g_course_843e80 is set to 1 while loading and to 2 when done (inferred).
 * Log strings: "instancing course %d" and "saving course... %d" print instance_index (the pass number), not the course id.
 */

void race_load_course(void)

{
  int split_sector;
  int instance_index;
  CourseId course_id;
  CourseId *course_cursor;
  CourseId course_id_copy;
  CourseId course_list [119];
  
  course_list[0] = COURSE_ANGELS;
  course_list[1] = COURSE_ROUABOUT;
  course_list[2] = COURSE_EGYPT;
  course_list[3] = COURSE_EGYPT_R;
  course_list[4] = COURSE_NEUST_R;
  course_list[5] = COURSE_TIERRA_N;
  course_list[6] = COURSE_SUPERG_N;
  course_list[7] = COURSE_FOREST_R;
  course_list[8] = COURSE_SANDS;
  course_list[9] = COURSE_HIGHWAYN;
  course_list[10] = COURSE_SANDSNOW;
  course_list[0xb] = COURSE_STORM_R;
  course_list[0xc] = COURSE_NT1;
  course_list[0xd] = COURSE_NT1_S;
  course_list[0xe] = COURSE_KEYS01;
  course_list[0xf] = COURSE_KEYS;
  course_list[0x10] = COURSE_DEM;
  course_list[0x11] = COURSE_SUPERG;
  course_list[0x12] = COURSE_CITY;
  course_list[0x13] = COURSE_TRAINING;
  course_list[0x14] = COURSE_WARZONE;
  course_list[0x15] = COURSE_FOREST;
  course_list[0x16] = COURSE_STORM;
  course_list[0x17] = COURSE_DUMP;
  course_list[0x18] = COURSE_HIGHWAY;
  course_list[0x19] = COURSE_NEUSTEIN;
  course_list[0x1a] = COURSE_DEMR;
  course_list[0x1b] = COURSE_LIST_END;
  course_list[0x1c] = 0;
  course_list[0x1d] = 1;
  course_list[0x1e] = 2;
  course_list[0x1f] = 3;
  course_list[0x20] = 4;
  course_list[0x21] = 5;
  course_list[0x22] = 6;
  course_list[0x23] = 7;
  course_list[0x24] = 8;
  course_list[0x25] = 9;
  course_list[0x26] = 10;
  course_list[0x27] = 0xb;
  course_list[0x28] = 0xc;
  course_list[0x29] = 0xd;
  course_list[0x2a] = 0xe;
  course_list[0x2b] = 0xf;
  course_list[0x2c] = 0x10;
  course_list[0x2d] = 0x11;
  course_list[0x2e] = 0x12;
  course_list[0x2f] = 0x13;
  course_list[0x30] = 0x14;
  course_list[0x31] = 0x15;
  course_list[0x32] = 0x16;
  course_list[0x33] = 0x17;
  course_list[0x34] = 0x18;
  course_list[0x35] = 0x19;
  course_list[0x36] = 0x1a;
  course_list[0x37] = 0x1b;
  course_list[0x38] = 0x1c;
  course_list[0x39] = 0x1d;
  course_list[0x3a] = 0x1e;
  course_list[0x3b] = 0x1f;
  course_list[0x3c] = 0x20;
  course_list[0x3d] = 0x21;
  course_list[0x3e] = 0x22;
  course_list[0x3f] = 0x23;
  course_list[0x40] = 0x24;
  course_list[0x41] = 0x25;
  course_list[0x42] = 0x26;
  course_list[0x43] = 0x27;
  course_list[0x44] = 0x28;
  course_list[0x45] = 0x29;
  course_list[0x46] = 0x2a;
  course_list[0x47] = 0x2b;
  course_list[0x48] = 0x2c;
  course_list[0x49] = 0x2d;
  course_list[0x4a] = 0x2e;
  course_list[0x4b] = 0x2f;
  course_list[0x4c] = 0x30;
  course_list[0x4d] = 0x31;
  course_list[0x4e] = 0x32;
  course_list[0x4f] = 0x33;
  course_list[0x50] = 0x34;
  course_list[0x51] = 0x35;
  course_list[0x52] = 0x36;
  course_list[0x53] = 0x37;
  course_list[0x54] = 0x38;
  course_list[0x55] = 0x39;
  course_list[0x56] = 0x3a;
  course_list[0x57] = 0x3b;
  course_list[0x58] = 0x3c;
  course_list[0x59] = 0x3d;
  course_list[0x5a] = 0x3e;
  course_list[0x5b] = 0x3f;
  course_list[0x5c] = 0x40;
  course_list[0x5d] = 0x41;
  course_list[0x5e] = 0x42;
  course_list[0x5f] = 0x43;
  course_list[0x60] = 0x44;
  course_list[0x61] = 0x45;
  course_list[0x62] = 0x46;
  course_list[99] = 0x47;
  course_list[100] = 0x48;
  course_list[0x65] = 0x49;
  course_list[0x66] = 0x4a;
  course_list[0x67] = 0x4b;
  course_list[0x68] = 0x4c;
  course_list[0x69] = 0x4d;
  course_list[0x6a] = 0x4e;
  course_list[0x6b] = 0x4f;
  course_list[0x6c] = 0x50;
  course_list[0x6d] = 0x51;
  course_list[0x6e] = 0x52;
  course_list[0x6f] = 0x53;
  course_list[0x70] = 0x54;
  course_list[0x71] = 0x55;
  course_list[0x72] = 0x56;
  course_list[0x73] = 0x57;
  course_list[0x74] = 0x58;
  course_list[0x75] = 0x59;
  course_list[0x76] = COURSE_LIST_END;
  g_course_843e80 = 1;
  vehicles_any_active();
  instance_index = 0;
  course_id = COURSE_ARCTIC;
  /* Pass 1: instance and save each course in instancing order. */
  do {
    split_sector = course_get_split_sector_8();
    /* Unload the current course first when the split sector check asks for it. */
    if (split_sector != 0) {
      course_unload_current();
    }
    course_open_and_select(course_id);
    debug_printf("instancing course %d\n",instance_index);
    course_world_instance_for_viewport();
    debug_printf("saving course... %d\n",instance_index);
    nop_stub();
    debug_printf("course saved %d\n",instance_index);
    course_id = course_list[instance_index];
    instance_index = instance_index + 1;
  } while (course_id != ~COURSE_ARCTIC);
  course_open_and_select(COURSE_ARCTIC);
  course_id = COURSE_ARCTIC;
  /* Pass 2: for each course id 0..89, load vehicle data and prepare parts. */
  course_cursor = course_list + 0x1c;
  do {
    course_id_copy = course_id;
    vehicle_load_from_piz(course_id_copy);
    camera_world_attach();
    course_model_path_copy(course_id,course_id_copy);
    vehicle_prepare_parts_0();
    course_id = course_cursor[1];
    course_cursor = course_cursor + 1;
  } while (course_id != ~COURSE_ARCTIC);
  course_unload_current();
  g_course_843e80 = 2;
  return;
}


