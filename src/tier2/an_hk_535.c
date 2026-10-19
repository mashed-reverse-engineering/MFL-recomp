#include "mfl_tier2.h"
/*00459e50*/ extern int course_objects_reset_physics();
/*0048e6d0*/ extern int rwp_object_get_handle();
/*00491530*/ extern int __cdecl rwp_object_create_from_atomic(struct RpAtomic *p0, undefined4 p1, float p2, undefined4 p3, undefined4 p4, undefined4 p5, undefined4 p6);
/*004c3f60*/ extern RpAtomic * clump_first_atomic(RpClump * p0);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*0056b440*/ extern void FUN_0056b440(undefined4 p0, undefined4 p1, undefined4 p2);
/*0058c8a0*/ extern int rwp_owner_object_get(int p0);
/*008c3f60*/ extern undefined4 g_course_highway_8c3f60;
/*008c3f64*/ extern undefined4 g_course_8c3f64;
/*008c3f68*/ extern undefined4 g_course_highway_8c3f68;
/*008c3fe0*/ extern undefined g_course_highway_8c3fe0;
/*008c47cc*/ extern undefined4 g_course_8c47cc;

#define HIGHWAY_OBJECT_TABLE_BYTES (800)
#define GAME_STATE_HIGHWAY_CLUMP_OFFSET (0xc1d8) /* inferred; an_hk_516 uses 0xc1bc for the render clump */
#define BODY_GROUP_COUNT (3)
#define BODIES_PER_GROUP (10)
#define BODY_RECORD_DWORDS (7)
#define BODY_GROUP_RECORD_DWORDS (0x46) /* BODIES_PER_GROUP * BODY_RECORD_DWORDS */
#define SPAWN_TABLE_STRIDE (3)
#define BODY_ID_BASE_GROUP_0 (2000)
#define BODY_ID_BASE_GROUP_1 (0x834)
#define BODY_ID_BASE_GROUP_2 (0x898)
#define FLOAT_BITS_1_0 (0x3f800000)
#define FLOAT_BITS_10_0 (0x41200000)
#define FLOAT_BITS_25_0 (0x41c80000)

/* course_highway_physics_objects_create */

/* "creates 3 x 10 physics bodies of three sizes from the highway atomic and registers them" */

/*
 * Creates BODY_GROUP_COUNT groups of BODIES_PER_GROUP physics objects from the first atomic of the
 * highway clump (inferred). Takes no return value.
 * game_state: game state pointer on entry (highway clump read at GAME_STATE_HIGHWAY_CLUMP_OFFSET).
 *   It is then reused as the base object id of the current group, so do not read it after the loop starts.
 * Per body, g_course_8c47cc gets a BODY_RECORD_DWORDS record: [0] handle, [1] owner object, [3] object id;
 *   the 3 dwords before the record are the body parameter block {0, 10.0f, 0} (inferred), which is also
 *   stored per object id in g_course_highway_8c3fe0 (cleared on entry).
 * Finally the physics is reset with course_objects_reset_physics(0x7d0) (argument meaning unknown).
 */
void __cdecl course_highway_physics_objects_create(int game_state)

{
  undefined4 *body_params;
  RpAtomic *atomic;
  int object_id;
  undefined4 object_handle;
  int owner_object;
  int body_index;
  undefined4 *body_record;
  struct {
    int group_index;
    undefined1 pad_14[8];
    undefined4 pos_x;
    undefined4 pos_y;
    undefined4 pos_z;
  } frame;

  mem_zero(&g_course_highway_8c3fe0,HIGHWAY_OBJECT_TABLE_BYTES);
  atomic = (RpAtomic *)clump_first_atomic(*(undefined4 *)(game_state + GAME_STATE_HIGHWAY_CLUMP_OFFSET));
  frame.group_index = 0;
  /* one pass per group: pick the base object id for this group's bodies */
  do {
    if (frame.group_index == 0) {
      game_state = BODY_ID_BASE_GROUP_0;
    }
    else if (frame.group_index == 1) {
      game_state = BODY_ID_BASE_GROUP_1;
    }
    else if (frame.group_index == 2) {
      game_state = BODY_ID_BASE_GROUP_2;
    }
    body_index = 0;
    body_record = &g_course_8c47cc + frame.group_index * BODY_GROUP_RECORD_DWORDS;
    /* each body: spawn at this group's position, create it from the atomic, record its handle and
       owner, then register its parameter block under the new object id */
    do {
      frame.pos_x = (&g_course_highway_8c3f60)[frame.group_index * SPAWN_TABLE_STRIDE];
      frame.pos_y = (&g_course_8c3f64)[frame.group_index * SPAWN_TABLE_STRIDE];
      frame.pos_z = (&g_course_highway_8c3f68)[frame.group_index * SPAWN_TABLE_STRIDE];
      object_id = rwp_object_create_from_atomic
                        (atomic,&frame.pos_x,0.0,0,FLOAT_BITS_1_0,FLOAT_BITS_25_0,body_index + game_state);
      body_record[3] = object_id;
      object_handle = rwp_object_get_handle(object_id);
      *body_record = object_handle;
      owner_object = rwp_owner_object_get(object_handle);
      body_params = body_record + -3;
      body_record[1] = owner_object;
      *body_params = 0;
      body_record[-2] = FLOAT_BITS_10_0;
      body_record[-1] = 0;
      FUN_0056b440(*(undefined4 *)(owner_object + 0x24),owner_object,7);
      body_index = body_index + 1;
      body_record = body_record + BODY_RECORD_DWORDS;
      *(undefined4 **)((char *)&g_course_highway_8c3fe0 + object_id * 4) = body_params;
    } while (body_index < BODIES_PER_GROUP);
    frame.group_index = frame.group_index + 1;
  } while (frame.group_index < BODY_GROUP_COUNT);
  course_objects_reset_physics(0x7d0);
  return;
}


