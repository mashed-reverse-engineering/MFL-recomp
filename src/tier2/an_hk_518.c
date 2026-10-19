#include "mfl_tier2.h"
extern char UNK_005df51c[];
/*00475730*/ extern int get_viewport_camera();
/*004a1860*/ extern int game_set_option_flags();
/*004a5d10*/ extern int object_frames_create_with_matrices();
/*004c3f60*/ extern RpAtomic * clump_first_atomic(RpClump * p0);
/*004d1ca0*/ extern struct RwCamera * RwCameraSetFarClipPlane(struct RwCamera *p0, RwReal p1);
/*006a2780*/ extern undefined4 g_course_sands_6a2780;
/*008bf088*/ extern undefined4 g_course_sands_8bf088;
#define FAR_CLIP_DISTANCE (280.0)
#define FOG_PLANE_180_BITS (0x43340000)
/*
 * course_sands_cb0 (inferred: course callback for the Sands track, cb0 = first callback slot)
 * game_state: course/game state block, fields read at offsets 0xc000..0xc9fc.
 * Stores a result from func_0x004849e0 in g_course_sands_8bf088, does one-time camera and
 * option setup when two state floats are equal (inferred: first-frame/init check), then
 * creates the object frames for the first atomic of the clump at 0xc124 into
 * g_course_sands_6a2780. Returns nothing.
 */

void course_sands_cb0(char *game_state)

{
  RwCamera *viewport_camera;
  int fog_camera;
  undefined4 first_atomic;
  RwReal farClip;

  g_course_sands_8bf088 = func_0x004849e0(&UNK_005df51c,*(undefined4 *)(game_state + 0xc9d0));
  /* init block (inferred): set far clip and fog plane on the viewport camera, then set option flags */
  if (*(float *)(game_state + 0xc9f8) == *(float *)(game_state + 0xc9fc)) {
    farClip = FAR_CLIP_DISTANCE;
    viewport_camera = (RwCamera *)get_viewport_camera(0);
    RwCameraSetFarClipPlane(viewport_camera,farClip);
    fog_camera = get_viewport_camera(0);
    *(undefined4 *)(fog_camera + 0x88) = FOG_PLANE_180_BITS;
    get_viewport_camera(0);
    game_set_option_flags(0x2f,0x44,0x47);
  }
  func_0x0045b670(game_state);
  /* build object frames from the first atomic of the clump (inferred: sands track model) */
  first_atomic = clump_first_atomic(*(undefined4 *)(game_state + 0xc124));
  g_course_sands_6a2780 =
       object_frames_create_with_matrices(first_atomic,*(undefined4 *)(game_state + 0xc000),game_state);
  return;
}


