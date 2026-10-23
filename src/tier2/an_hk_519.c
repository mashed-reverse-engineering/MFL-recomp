#include "mfl_tier2.h"
/*0040fdb0*/ extern int get_player_count();
/*0042a3f0*/ extern void * course_get_rpworld();
/*00452b90*/ extern undefined __cdecl viewport_camera_sync_to_main(undefined4 p0, void *p1, void *p2);
/*00475730*/ extern int get_viewport_camera();
/*00495c30*/ extern int viewport_get_camera();
/*004d1d40*/ extern struct RwCamera * RwCameraClear(struct RwCamera *p0, struct RwRGBA *p1, RwInt32 p2);
/*004d1e10*/ extern struct RwCamera * RwCameraSetViewWindow(struct RwCamera *p0, struct RwV2d *p1);
/*004f4440*/ extern struct RpWorld * RpWorldAddCamera(struct RpWorld *p0, struct RwCamera *p1);
/*004f4470*/ extern struct RpWorld * RpWorldRemoveCamera(struct RpWorld *p0, struct RwCamera *p1);
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*008d8b30*/ extern undefined4 player_count;

#define WHITE_CHANNEL (0xff)
#define CAMERA_CLEAR_IMAGE_AND_Z (3)
#define CAMERA_CLEAR_Z_ONLY (2)
#define CAMERA_VIEW_WINDOW_OFFSET (0x68)
#define FOG_RED_OFFSET (0xc9f4)
#define FOG_GREEN_OFFSET (0xc9f5)
#define FOG_BLUE_OFFSET (0xc9f6)
#define BACKGROUND_CLUMP_OFFSET (0xc0f0) /* inferred */
#define FOREGROUND_CLUMP_OFFSET (0xc114) /* inferred */

/*
 * Draws one frame of the superg course's render pass (inferred from the calls).
 * Points the viewport camera at the course view window, clears it to white,
 * adds it to the course world, renders the background clump with fog and
 * Z-write off, then the foreground clump and the player pass with Z-write on,
 * and finally removes the camera from the world and clears Z again.
 * param_1: course render state (inferred), holds the fog colour bytes and clump pointers.
 */
void course_superg_cb4(char *param_1)

{
  int camera;
  RwV2d *view_window;
  RwCamera *viewport_camera;
  RpWorld *course_world;
  undefined4 scratch; /* reused: viewport camera, begin/end update arg, player count */
  RwRGBA *clear_color;
  RwInt32 clear_flags;
  RwRGBA white_color;

  white_color.red = WHITE_CHANNEL;
  white_color.green = WHITE_CHANNEL;
  white_color.blue = WHITE_CHANNEL;
  white_color.alpha = WHITE_CHANNEL;
  camera = get_viewport_camera(0);
  view_window = (RwV2d *)(camera + CAMERA_VIEW_WINDOW_OFFSET);
  viewport_camera = (RwCamera *)viewport_get_camera(0);
  RwCameraSetViewWindow(viewport_camera,view_window);
  viewport_camera_sync_to_main(0,viewport_camera,view_window);
  clear_color = &white_color;
  clear_flags = CAMERA_CLEAR_IMAGE_AND_Z;
  viewport_camera = (RwCamera *)viewport_get_camera(0);
  RwCameraClear(viewport_camera,clear_color,clear_flags);
  viewport_camera = (RwCamera *)viewport_get_camera(0);
  course_world = (RpWorld *)course_get_rpworld();
  RpWorldAddCamera(course_world,viewport_camera);
  scratch = viewport_get_camera(0);
  camera = RwCameraBeginUpdate(scratch);
  /* only render when the camera update started */
  if (camera != 0) {
    /* fog on, colour packed from the three fog bytes in the render state */
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,(void *)0x1);
    RwRenderStateSet
              (rwRENDERSTATEFOGCOLOR,
               (void *)(((*(byte *)(param_1 + FOG_RED_OFFSET) | 0x4000) << 8 |
                        (uint)*(byte *)(param_1 + FOG_GREEN_OFFSET)) << 8 | (uint)*(byte *)(param_1 + FOG_BLUE_OFFSET))
              );
    /* background: drawn without depth writes */
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
    if (*(RpClump **)(param_1 + BACKGROUND_CLUMP_OFFSET) != (RpClump *)0x0) {
      RpClumpRender(*(RpClump **)(param_1 + BACKGROUND_CLUMP_OFFSET));
    }
    /* foreground: depth writes back on, then the per-player pass (func_0x0045c1f0, inferred) */
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
    scratch = get_player_count();
    func_0x0045c1f0(scratch);
    if (*(RpClump **)(param_1 + FOREGROUND_CLUMP_OFFSET) != (RpClump *)0x0) {
      RpClumpRender(*(RpClump **)(param_1 + FOREGROUND_CLUMP_OFFSET));
    }
    /* fog off, end the camera update */
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,(void *)0x0);
    scratch = viewport_get_camera(0);
    RwCameraEndUpdate(scratch);
  }
  /* detach the camera from the course world and clear Z only */
  clear_color = &white_color;
  viewport_camera = (RwCamera *)viewport_get_camera(0);
  course_world = (RpWorld *)course_get_rpworld();
  RpWorldRemoveCamera(course_world,viewport_camera);
  clear_flags = CAMERA_CLEAR_Z_ONLY;
  viewport_camera = (RwCamera *)get_viewport_camera(0);
  RwCameraClear(viewport_camera,clear_color,clear_flags);
  return;
}


