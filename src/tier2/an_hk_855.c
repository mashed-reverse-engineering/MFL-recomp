#include "mfl_tier2.h"
extern char LAB_004069c0[];
/*00475730*/ extern int get_viewport_camera();
/*004d5540*/ extern RwReal RwV2dNormalize(struct RwV2d *p0, struct RwV2d *p1);
/*004f5dd0*/ extern struct RpWorld * RpWorldForAllMaterials(struct RpWorld *p0, RpMaterialCallBack p1, void *p2);
/*005dd410*/ extern float k_f_n2;
/*005dd634*/ extern float k_f_2;
/*005ddacc*/ extern float k_f_360;
/*005ddad0*/ extern float k_f_180;
/*005ddad4*/ extern float k_f_90;
/*005ddc28*/ extern float k_f_0;
/*005fb15c*/ extern float g_skydome_follow_5fb15c;
/*005fb160*/ extern float g_skydome_follow_5fb160;
/*005fb164*/ extern undefined4 g_skydome_follow_5fb164;
/*008d98b4*/ extern undefined4 g_skydome_follow_8d98b4;
/*008d98b8*/ extern float g_skydome_follow_8d98b8;
/* Offsets (inferred from use): camera frame direction components (x, y, z), and the RpWorld pointer inside the context object */
#define CAMERA_DIR_X_OFFSET (0x30)
#define CAMERA_DIR_Y_OFFSET (0x34)
#define CAMERA_DIR_Z_OFFSET (0x38)
#define CONTEXT_WORLD_OFFSET (0xc9d4)
/* skydome_follow_camera */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "inferred: uses the camera position projected on XZ (normalised)" */
/*
 * Per-frame update of the skydome scroll (inferred). Parameter context: game object whose
 * +0xc9d4 field is the RpWorld (inferred). Takes no return value; writes two globals:
 *   g_skydome_follow_8d98b4 = camera_angle, a diamond pseudo-angle in -2..2 of the camera XZ direction,
 *   g_skydome_follow_8d98b8 = skydome yaw relative to the camera heading, degrees in 0..360.
 */

void skydome_follow_camera(int context)

{
  float camera_heading;
  int camera_frame;
  struct {
    float camera_angle;
    RwV2d xz;
  } scratch;

  camera_frame = get_viewport_camera(0);
  camera_frame = *(int *)(camera_frame + 4);
  scratch.xz.x = *(float *)(camera_frame + CAMERA_DIR_X_OFFSET);
  scratch.xz.y = *(float *)(camera_frame + CAMERA_DIR_Z_OFFSET);
  RwV2dNormalize(&scratch.xz,&scratch.xz);
  /* Diamond pseudo-angle of the camera direction: x in the front half, else 2 - x or -2 - x (inferred) */
  scratch.camera_angle = scratch.xz.x;
  if (scratch.xz.y < k_f_0) {
    scratch.camera_angle = k_f_n2;
    if (k_f_0 < scratch.xz.x) {
      scratch.camera_angle = k_f_2;
    }
    scratch.camera_angle = scratch.camera_angle - scratch.xz.x;
  }
  scratch.xz.x = g_skydome_follow_5fb15c;
  *(undefined4 *)&scratch.xz.y = g_skydome_follow_5fb164;
  /* Second normalise: its result is overwritten below, the call is kept. g_5fb164 is copied as raw bits */
  RwV2dNormalize(&scratch.xz,&scratch.xz);
  *(float *)&g_skydome_follow_8d98b4 = scratch.camera_angle;
  scratch.xz.x = scratch.camera_angle;
  scratch.xz.y = 0.0;
  /* Pass (camera_angle, 0) to the callback for every material in the world (texture offset, inferred) */
  RpWorldForAllMaterials(*(RpWorld **)(context + CONTEXT_WORLD_OFFSET),(RpMaterialCallBack)&LAB_004069c0,&scratch.xz)
  ;
  /* Camera heading in degrees, 0..360 (inferred): y * 90, or 180 - y * 90 when x < 0, plus 360 when x >= 0 and y < 0 */
  camera_heading = *(float *)(camera_frame + CAMERA_DIR_Y_OFFSET) * k_f_90;
  if (*(float *)(camera_frame + CAMERA_DIR_X_OFFSET) < k_f_0) {
    camera_heading = k_f_180 - camera_heading;
  }
  else {
    if (*(float *)(camera_frame + CAMERA_DIR_Y_OFFSET) < k_f_0) {
      camera_heading = camera_heading + k_f_360;
    }
  }
  /* Skydome heading from the same mapping, applied to the skydome direction globals 5fb15c / 5fb160 */
  g_skydome_follow_8d98b8 = g_skydome_follow_5fb160 * k_f_90;
  if (g_skydome_follow_5fb15c < k_f_0) {
    g_skydome_follow_8d98b8 = k_f_180 - g_skydome_follow_8d98b8;
  }
  else {
    if (g_skydome_follow_5fb160 < k_f_0) {
      g_skydome_follow_8d98b8 = g_skydome_follow_8d98b8 + k_f_360;
    }
  }
  /* Skydome yaw relative to the camera, wrapped into 0..360 */
  g_skydome_follow_8d98b8 = g_skydome_follow_8d98b8 - camera_heading;
  if (g_skydome_follow_8d98b8 < k_f_0) {
    g_skydome_follow_8d98b8 = g_skydome_follow_8d98b8 + k_f_360;
  }
  if (k_f_360 < g_skydome_follow_8d98b8) {
    g_skydome_follow_8d98b8 = g_skydome_follow_8d98b8 - k_f_360;
  }
  return;
}


