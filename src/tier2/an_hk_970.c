#include "mfl_tier2.h"
/*0045f330*/ extern undefined __cdecl explosion_trigger_callback(struct RwV3d *p0, float *p1);
/*00496850*/ extern int trigger_sphere_register(float *param_1);
/*006a4c28*/ extern struct ExplosionEntry g_explosion_queue[100];
/* explosion_queue_update */

/* "per frame: for each g_explosion_queue entry with frames_left > 0: decrement and
   trigger_sphere_register({x,y,z,radius}, type 0x19, owner = the entry, callback
   explosion_trigger_callback)" */

/* sphere trigger descriptor passed by address to trigger_sphere_register (8 words) */
typedef struct {
  float x;
  float y;
  float z;
  float radius;
  int type;
  ExplosionEntry *owner;
  int unused;
  void *callback;
} SphereTrigger;

void explosion_queue_update(void)

{
  int *frames_left;
  SphereTrigger sphere;
  
  frames_left = &g_explosion_queue[0].frames_left;
  do {
    if (0 < *frames_left) {
      *frames_left = *frames_left + -1;
      sphere.x = *(float *)&frames_left[1];
      sphere.y = *(float *)&frames_left[2];
      sphere.z = *(float *)&frames_left[3];
      sphere.radius = *(float *)&frames_left[4];
      sphere.owner = (ExplosionEntry *)(frames_left + -4);
      sphere.unused = 0;
      sphere.type = 0x19;
      sphere.callback = explosion_trigger_callback;
      trigger_sphere_register(&sphere.x);
    }
    frames_left = frames_left + 9;
  } while ((int)frames_left < (int)&g_explosion_queue + 0xe20);
  return;
}
