#include "mfl_tier2.h"
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004d1860*/ extern void _rwObjectHasFrameSetFrame(void *p0, struct RwFrame *p1);
/*004f46d0*/ extern struct RpWorld * RpWorldRemoveClump(struct RpWorld *p0, struct RpClump *p1);
/*004f4980*/ extern struct RpWorld * RpWorldRemoveLight(struct RpWorld *p0, struct RpLight *p1);
/*004f4eb0*/ extern RwBool RpLightDestroy(struct RpLight *p0);
/*004f5820*/ extern RwBool RpWorldDestroy(struct RpWorld *p0);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*0064cbbc*/ extern undefined4 g_cup_64cbbc;
/*0064cbc0*/ extern undefined4 g_cup_64cbc0;
/*0064cbc8*/ extern struct RpLight *g_load_64cbc8;
/*0064cbcc*/ extern struct RpLight *g_cup_64cbcc;
/*0064cbd4*/ extern RwFrame *g_cup_64cbd4;
/* cup_scene_destroy_lights */

/* "removes and destroys the scene lights and their frames" */
/* Tears down the cup scene: takes both scene lights out of the world, destroys each light's
   parent frame and the light itself, then removes the clump from the world, destroys the
   world and the clump, and finally destroys the cached frame g_cup_64cbd4 and clears it.
   Takes no parameters and returns nothing. */

void cup_scene_destroy_lights(void)

{
  RwFrame *light_frame;

  /* Remove both lights from the world before they are freed. */
  RpWorldRemoveLight(g_cup_64cbbc,g_cup_64cbcc);
  RpWorldRemoveLight(g_cup_64cbbc,g_load_64cbc8);
  /* Light g_cup_64cbcc: destroy its parent frame if it has one, detach it from its frame, then free it. */
  light_frame = RpLight_object_object_parent(g_cup_64cbcc);
  if (light_frame != (RwFrame *)0x0) {
    RwFrameDestroy(light_frame);
  }
  _rwObjectHasFrameSetFrame(g_cup_64cbcc,(RwFrame *)0x0);
  RpLightDestroy(g_cup_64cbcc);
  /* Light g_load_64cbc8: same teardown as above. */
  light_frame = RpLight_object_object_parent(g_load_64cbc8);
  if (light_frame != (RwFrame *)0x0) {
    RwFrameDestroy(light_frame);
  }
  _rwObjectHasFrameSetFrame(g_load_64cbc8,(RwFrame *)0x0);
  RpLightDestroy(g_load_64cbc8);
  /* Remove the clump from the world, then destroy the world and the clump g_cup_64cbc0. */
  RpWorldRemoveClump(g_cup_64cbbc,g_cup_64cbc0);
  RpWorldDestroy(g_cup_64cbbc);
  RpClumpDestroy(g_cup_64cbc0);
  /* Destroy the cached frame if it is set, then clear the pointer. */
  if (g_cup_64cbd4 != (RwFrame *)0x0) {
    RwFrameDestroy(g_cup_64cbd4);
  }
  g_cup_64cbd4 = (RwFrame *)0x0;
  return;
}


