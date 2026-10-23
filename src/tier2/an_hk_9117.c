#include "mfl_tier2.h"
/*00406950*/ extern undefined __cdecl world_for_all_materials_cb_406910(struct RpWorld *p0, void *p1);
/*0040d0d0*/ extern char *__cdecl find_texture_dict_b(RwChar *p0);
/*00449700*/ extern undefined __cdecl course_sky_atomic_create(struct RwTexture *p0, int p1, undefined4 p2);
/*00495b60*/ extern undefined __cdecl viewport_raster_create(uint p0, RwInt32 p1, RwInt32 p2, uint p3);
/*00495c10*/ extern char *viewport_get_field_702254();
/*00495c30*/ extern int viewport_get_camera();
/*004d1bd0*/ extern struct RwCamera * RwCameraSetNearClipPlane(struct RwCamera *p0, RwReal p1);
/*004d1ca0*/ extern struct RwCamera * RwCameraSetFarClipPlane(struct RwCamera *p0, RwReal p1);
/* course_superg_cb0 */

/* WARNING: Removing unreachable block (ram,0x00495c39) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int course_superg_cb0(char *param_1)

{
  RwTexture *texture_dict;
  void *pData;
  RwCamera *pRVar1;

  texture_dict = (RwTexture *)find_texture_dict_b("smoke");
  course_sky_atomic_create(texture_dict,0x3f800000,0xc0000000);
  viewport_raster_create(0,0x100,0x100,0x600);
  pData = (void *)viewport_get_field_702254(0);
  world_for_all_materials_cb_406910(*(RpWorld **)(param_1 + 0xc9d4),pData);
  pRVar1 = (RwCamera *)viewport_get_camera(0);
  RwCameraSetNearClipPlane(pRVar1,0.5f);
  pRVar1 = (RwCamera *)viewport_get_camera(0);
  RwCameraSetFarClipPlane(pRVar1,*(float *)(param_1 + 0xc9fc));
  *(float *)(viewport_get_camera(0) + 0x88) = *(float *)(param_1 + 0xc9f8);
  /* the original ends in a tail jump to viewport_get_camera(0) */
  return viewport_get_camera(0);
}
