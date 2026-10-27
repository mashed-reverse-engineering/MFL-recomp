#include "mfl_tier2.h"
/*00475730*/ extern int get_viewport_camera();
/*004b2a60*/ extern int sprintf();
/*004c3bf0*/ extern char *FUN_004c3bf0(void *p0, undefined4 p1, undefined4 p2);
/*004c4050*/ extern char *FUN_004c4050(undefined4 p0);
/*004c5440*/ extern int __cdecl FUN_004c5440(int a1, int a2, int a3);
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004d16e0*/ extern struct RwFrame * RwFrameSetIdentity(struct RwFrame *p0);
/*004d1860*/ extern void _rwObjectHasFrameSetFrame(void *p0, struct RwFrame *p1);
/*004d1990*/ extern struct RwBBox * RwBBoxInitialize(struct RwBBox *p0, struct RwV3d *p1);
/*004d19d0*/ extern struct RwBBox * RwBBoxAddPoint(struct RwBBox *p0, struct RwV3d *p1);
/*004f4570*/ extern struct RpWorld * RpWorldAddClump(struct RpWorld *p0, struct RpClump *p1);
/*004f4930*/ extern struct RpWorld * RpWorldAddLight(struct RpWorld *p0, struct RpLight *p1);
/*004f4a00*/ extern struct RpLight * RpLightSetRadius(struct RpLight *p0, RwReal p1);
/*004f4a20*/ extern struct RpLight * RpLightSetColor(struct RpLight *p0, struct RwRGBAReal *p1);
/*004f4cb0*/ extern struct RpLight * RpLightSetConeAngle(struct RpLight *p0, RwReal p1);
/*004f4ef0*/ extern struct RpLight * RpLightCreate(RwInt32 p0);
/*004f5970*/ extern struct RpWorld * RpWorldCreate(struct RwBBox *p0);
/*005501d0*/ extern struct RpMaterial * RpMatFXMaterialSetEffects(struct RpMaterial *p0, enum RpMatFXMaterialFlags p1);
/*0064cbbc*/ extern undefined4 g_cup_64cbbc;
/*0064cbc0*/ extern undefined4 g_cup_64cbc0;
/*0064cbc4*/ extern undefined4 g_load_64cbc4;
/*0064cbc8*/ extern struct RpLight *g_load_64cbc8;
/*0064cbcc*/ extern undefined4 g_cup_64cbcc;
/*0064cbd0*/ extern float g_cup_64cbd0;
/*0064cbd4*/ extern undefined4 g_cup_64cbd4;
/*0064cbd8*/ extern undefined4 g_cup_64cbd8;
/*0064d890*/ extern undefined4 g_load_64d890;
/* RpLightType (RenderWare): rpLIGHTAMBIENT=2, rpLIGHTSPOTSOFT=130 */
#define RP_LIGHT_TYPE_AMBIENT (2)
#define RP_LIGHT_TYPE_SPOTSOFT (0x82)
/* RpLightFlag (RenderWare): rpLIGHTLIGHTATOMICS=1 */
#define RP_LIGHT_FLAG_LIGHTATOMICS (1)
/* pi/4 radians */
#define SPOT_LIGHT_CONE_ANGLE (0.7853982)
#define SPOT_LIGHT_RADIUS (100.0)
/* world bounds are +/-100 on each axis */
#define BOUNDS_HALF_EXTENT (100.0)
/* load_cup_model */
/* compile: /GS */
/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* loads cup.dff */
/* Builds the cup scene: loads cup.dff into a clump, puts it in a new world
 * (bounds +/-100 on each axis), and adds an ambient light (0.75 grey) and a
 * soft spot light (white, radius 100, cone pi/4, on its own frame) to that world.
 * Stores the world, clump, light and frame pointers in the g_cup_* / g_load_*
 * globals. Also creates two identity frames stored in g_cup_64cbd4 and
 * g_cup_64cbd8 (purpose not identified; not used here).
 * Takes no parameters, returns nothing. */
void load_cup_model(void)
{
  RpMaterial *cup_material;
  RwFrame *light_frame;
  RwV3d bounds_min;
  RwV3d bounds_max;
  RwRGBAReal ambient_color;
  RwRGBAReal spot_color;
  RwBBox world_bounds;
  undefined1 dff_path [256];

  spot_color.red = 1.0;
  spot_color.green = 1.0;
  spot_color.blue = 1.0;
  spot_color.alpha = 1.0;
  ambient_color.red = 0.75;
  ambient_color.green = 0.75;
  ambient_color.blue = 0.75;
  ambient_color.alpha = 1.0;
  bounds_min.x = -BOUNDS_HALF_EXTENT;
  bounds_min.y = -BOUNDS_HALF_EXTENT;
  bounds_min.z = -BOUNDS_HALF_EXTENT;
  bounds_max.x = BOUNDS_HALF_EXTENT;
  bounds_max.y = BOUNDS_HALF_EXTENT;
  bounds_max.z = BOUNDS_HALF_EXTENT;
  g_load_64cbc4 = get_viewport_camera(0);
  /* world bounding box from bounds_min to bounds_max */
  RwBBoxInitialize(&world_bounds,&bounds_min);
  RwBBoxAddPoint(&world_bounds,&bounds_max);
  g_cup_64cbbc = RpWorldCreate(&world_bounds);
  sprintf(dff_path,"cup.dff");
  g_cup_64cbc0 = (RpClump *)FUN_004c3bf0(dff_path,0,0);
  /* FUN_004c5440(clump, 0x40, 1): arguments not identified */
  FUN_004c5440(g_cup_64cbc0,0x40,1);
  /* when g_load_64d890 is set, clear the MatFX effects on the clump's material (inferred) */
  if (g_load_64d890 != 0) {
    cup_material = (RpMaterial *)FUN_004c4050(g_cup_64cbc0);
    RpMatFXMaterialSetEffects(cup_material,rpMATFXEFFECTNULL);
  }
  g_cup_64cbd0 = 0;
  /* ambient light, then the spot light on its own frame */
  g_cup_64cbcc = RpLightCreate(RP_LIGHT_TYPE_AMBIENT);
  RpLightSetColor(g_cup_64cbcc,&ambient_color);
  g_load_64cbc8 = RpLightCreate(RP_LIGHT_TYPE_SPOTSOFT);
  light_frame = RwFrameCreate();
  _rwObjectHasFrameSetFrame(g_load_64cbc8,light_frame);
  RpLightSetConeAngle(g_load_64cbc8,SPOT_LIGHT_CONE_ANGLE);
  RpLight_object_object_flags(g_load_64cbc8) = RP_LIGHT_FLAG_LIGHTATOMICS;
  RpLightSetColor(g_load_64cbc8,&spot_color);
  RpLightSetRadius(g_load_64cbc8,SPOT_LIGHT_RADIUS);
  RpWorldAddLight(g_cup_64cbbc,g_load_64cbc8);
  RpWorldAddLight(g_cup_64cbbc,g_cup_64cbcc);
  RpWorldAddClump(g_cup_64cbbc,g_cup_64cbc0);
  g_cup_64cbd4 = RwFrameCreate();
  RwFrameSetIdentity(g_cup_64cbd4);
  g_cup_64cbd8 = RwFrameCreate();
  RwFrameSetIdentity(g_cup_64cbd8);
  return;
}
