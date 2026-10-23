#include "mfl_tier2.h"
/*0042a3f0*/ extern void * course_get_rpworld();
/*00452b90*/ extern undefined __cdecl viewport_camera_sync_to_main(undefined4 p0, void *p1, void *p2);
/*00452bc0*/ extern int course_arctic_camera_relative_update();
/*00475730*/ extern int get_viewport_camera();
/*00495c30*/ extern int viewport_get_camera();
/*0049b700*/ extern int fx_ptank_render_b();
/*004d1d40*/ extern struct RwCamera * RwCameraClear(struct RwCamera *p0, struct RwRGBA *p1, RwInt32 p2);
/*004d1e10*/ extern struct RwCamera * RwCameraSetViewWindow(struct RwCamera *p0, struct RwV2d *p1);
/*004f4440*/ extern struct RpWorld * RpWorldAddCamera(struct RpWorld *p0, struct RwCamera *p1);
/*004f4470*/ extern struct RpWorld * RpWorldRemoveCamera(struct RpWorld *p0, struct RwCamera *p1);
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;

/* offsets into course_state of the clumps this function renders (inferred from use) */
#define COURSE_BACKDROP_CLUMP_OFFSET (0xc0f0)
#define COURSE_CULLED_CLUMP_OFFSET (0xc130)

/*
 * Renders the Arctic course's background pass (inferred).
 * Points viewport 0's camera at the course view window, clears it to a dark
 * slate colour (inferred from the 'P' 'X' '`' values), adds it to the course
 * world, updates it relative to the course, draws the backdrop and culled
 * clumps, then removes the camera from the world again.
 * course_state: pointer to the course state block (inferred); the two clump
 * pointers are read from it at COURSE_*_CLUMP_OFFSET.
 */
void course_arctic_cb4(char *course_state)

{
  int camera;
  RwV2d *view_window;
  RwCamera *cam;
  RpWorld *world;
  undefined4 viewport_camera;
  RwRGBA *clear_colour_ptr;
  RwInt32 clear_mode;
  RwRGBA clear_colour;

  /* clear colour: red 'P' (80), green 'X' (88), blue '`' (96), opaque */
  clear_colour.red = 'P';
  clear_colour.green = 'X';
  clear_colour.blue = '`';
  clear_colour.alpha = 0xff;
  /* view window is RwCamera.viewWindow (+0x68) of the camera returned here;
     the same local is reused for the BeginUpdate result below */
  camera = get_viewport_camera(0);
  view_window = (RwV2d *)(camera + 0x68);
  cam = (RwCamera *)viewport_get_camera(0);
  RwCameraSetViewWindow(cam,view_window);
  viewport_camera_sync_to_main(0,cam,view_window);
  /* clear the colour buffer with the colour above */
  clear_mode = rwCAMERACLEARIMAGE;
  clear_colour_ptr = &clear_colour;
  cam = (RwCamera *)viewport_get_camera(0);
  RwCameraClear(cam,clear_colour_ptr,clear_mode);
  /* add the camera to the course world and update it */
  cam = (RwCamera *)viewport_get_camera(0);
  world = (RpWorld *)course_get_rpworld();
  RpWorldAddCamera(world,cam);
  course_arctic_camera_relative_update(course_state);
  /* draw only between BeginUpdate and EndUpdate, and only if BeginUpdate succeeded */
  viewport_camera = viewport_get_camera(0);
  camera = RwCameraBeginUpdate(viewport_camera);
  if (camera != 0) {
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,(void *)0x0);
    RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)rwBLENDSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,(void *)rwBLENDINVSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
    /* backdrop clump, drawn without depth test or depth write (inferred) */
    if (*(RpClump **)(course_state + COURSE_BACKDROP_CLUMP_OFFSET) != (RpClump *)0x0) {
      RpClumpRender(*(RpClump **)(course_state + COURSE_BACKDROP_CLUMP_OFFSET));
    }
    fx_ptank_render_b();
    /* culled clump: back faces culled while it is drawn, then culling is turned off again */
    RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)rwCULLMODECULLBACK);
    RpClumpRender(*(RpClump **)(course_state + COURSE_CULLED_CLUMP_OFFSET));
    RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)rwCULLMODECULLNONE);
    RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)rwBLENDSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,(void *)rwBLENDINVSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,(void *)0x0);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
    viewport_camera = viewport_get_camera(0);
    RwCameraEndUpdate(viewport_camera);
  }
  cam = (RwCamera *)viewport_get_camera(0);
  world = (RpWorld *)course_get_rpworld();
  RpWorldRemoveCamera(world,cam);
  return;
}
