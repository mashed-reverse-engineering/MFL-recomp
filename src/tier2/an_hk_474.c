#include "mfl_tier2.h"
/*0045f5a0*/ extern int explosion_queue();
/*004866b0*/ extern int fx_emitter_spawn_a_default();
/*0048e650*/ extern int rwp_object_set_in_world();
/*00492d10*/ extern int rwp_object_set_active();
/*0049c9a0*/ extern int fx_object_break_into_particles();
/*0058c8a0*/ extern char *rwp_owner_object_get(void *p0);
/*005ddaf0*/ extern float k_f_1_0en5;
/*005ddb60*/ extern float k_f_n8;
/*005de25c*/ extern float k_f_0_005;
/*0062ac74*/ extern float g_car_62ac74;
/*006fa190*/ extern undefined g_rw_6fa190;
/*006fa4b0*/ extern undefined4 g_rw_6fa4b0;
/*006fab78*/ extern undefined4 g_rw_6fab78;
/*006fcdd8*/ extern undefined4 g_rw_6fcdd8;
#define FLAG_MOVED_TOO_FAR (8) /* (inferred) set when the squared speed exceeds k_f_1_0en5 */
#define FLAG_EXPIRED (2) /* (inferred) set when the object is removed from the world */
#define EXPLOSION_ID_THRESHOLD (149999) /* ids above this (>= 150000) get the explosion */
#define BODY_ENTRY_STRIDE (0x20)
/* rwp_object_debris_expire_check */

/* "if the body has moved too far/fallen below -8 removes it, spawns burst and explosion for ids >=
   150000" */
/* slot: index into the per-object global arrays (g_rw_6fab78 = object handle, g_rw_6fcdd8 = flags, g_rw_6fa4b0 = object id, g_rw_6fa190 = per-slot limit on dist_sq).
   unused_arg: never read. Returns nothing. */

void __cdecl rwp_object_debris_expire_check(int slot)

{
  char *object_handle;
  int *pos_ref;
  float dist_sq;
  char *owner;
  float *pos_entry;
  struct {
    float local_1c;
    float local_18;
    float local_14;
    undefined4 local_10;
    undefined4 local_c;
    undefined4 local_8;
    undefined4 local_4;
  } burst_pos;

  /* look up the object's owner, then the body position entry the owner points at */
  object_handle = (&g_rw_6fab78)[slot];
  owner = rwp_owner_object_get(object_handle);
  if ((owner != 0) && (pos_ref = *(int **)(*(char **)(owner + 0x10) + 4), pos_ref != (int *)0x0)) {
    pos_entry = (float *)(*(char **)(*(char **)(*pos_ref + 0x10) + 8) + pos_ref[1] * BODY_ENTRY_STRIDE);
    burst_pos.local_1c = *pos_entry;
    burst_pos.local_18 = pos_entry[1];
    burst_pos.local_14 = pos_entry[2];
    /* squared distance from the origin; the body has moved too far if it exceeds the limit */
    dist_sq = burst_pos.local_14 * burst_pos.local_14 + burst_pos.local_1c * burst_pos.local_1c + burst_pos.local_18 * burst_pos.local_18;
    /* too far from the origin: flag it */
    if (k_f_1_0en5 < dist_sq) {
      (&g_rw_6fcdd8)[slot] = (&g_rw_6fcdd8)[slot] | FLAG_MOVED_TOO_FAR;
    }
    /* expired: dist_sq above this slot's limit (g_rw_6fa190), or y below -8 */
    if ((((float *)&g_rw_6fa190)[slot] < dist_sq) || (burst_pos.local_18 < k_f_n8)) {
      /* scale the position down for the burst particles */
      burst_pos.local_1c = burst_pos.local_1c * k_f_0_005;
      burst_pos.local_18 = burst_pos.local_18 * k_f_0_005;
      burst_pos.local_14 = burst_pos.local_14 * k_f_0_005;
      fx_object_break_into_particles(object_handle,&burst_pos.local_1c);
      rwp_object_set_in_world(slot,0);
      rwp_object_set_active(slot,0);
      (&g_rw_6fcdd8)[slot] = (&g_rw_6fcdd8)[slot] | FLAG_EXPIRED;
      /* high-id objects also get a burst effect and an explosion at the object's position (inferred) */
      if (EXPLOSION_ID_THRESHOLD < (int)(&g_rw_6fa4b0)[slot]) {
        /* object_handle is reused here as the object's data pointer (+4), not a handle */
        object_handle = *(char **)(object_handle + 4);
        fx_emitter_spawn_a_default((undefined4 *)(object_handle + 0x40),&g_car_62ac74,0x43b40000);
        burst_pos.local_10 = *(undefined4 *)(object_handle + 0x40);
        burst_pos.local_c = *(undefined4 *)(object_handle + 0x44);
        burst_pos.local_8 = *(undefined4 *)(object_handle + 0x48);
        burst_pos.local_4 = 0x3f800000;
        explosion_queue(&burst_pos.local_10,0x41f00000,0x43c80000,0x42c80000,1);
      }
    }
  }
  return;
}


