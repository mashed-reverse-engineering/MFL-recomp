#include "mfl_tier2.h"
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004d1860*/ extern void _rwObjectHasFrameSetFrame(void *p0, struct RwFrame *p1);
/*004d1bd0*/ extern struct RwCamera * RwCameraSetNearClipPlane(struct RwCamera *p0, RwReal p1);
/*004d1ca0*/ extern struct RwCamera * RwCameraSetFarClipPlane(struct RwCamera *p0, RwReal p1);
/*004d1e10*/ extern struct RwCamera * RwCameraSetViewWindow(struct RwCamera *p0, struct RwV2d *p1);
/*004d2d80*/ extern struct RwCamera * RwCameraClone(struct RwCamera *p0);
/*006c2940*/ extern undefined4 g_aux_camera;
/*006c2944*/ extern undefined4 g_car_6c2944;
/*006c2948*/ extern undefined4 g_aux_camera_6c2948;
static RwCamera * aux_camera_create_r(RwInt32 reg_esi, RwInt32 reg_edi);
extern void aux_camera_create(void);

#define AUX_CAMERA_VIEW_WINDOW (0.8)
#define AUX_CAMERA_NEAR_CLIP (0.5)
#define AUX_CAMERA_FAR_CLIP (180.0)

__declspec(naked) RwCamera * aux_camera_create_r(RwInt32 reg_esi, RwInt32 reg_edi)
{
    __asm {
        push esi
        push edi
        mov esi, dword ptr [esp + 12]
        mov edi, dword ptr [esp + 16]
        call aux_camera_create
        pop edi
        pop esi
        ret
    }
}

/*
 * aux_camera_setup: creates the auxiliary camera, sets its view window
 * (0.8 x 0.8), near plane (0.5) and far plane (180.0), then clones it into
 * g_car_6c2944 and gives the clone a new frame (g_aux_camera_6c2948).
 * edi_arg / esi_arg are the two cdecl stack arguments (the original loads esi from the 2nd and edi from the 1st slot) forwarded to
 * aux_camera_create_r.
 */

/* "aux_camera_create + view window 0.8 x 0.8" */

void aux_camera_setup(undefined4 edi_arg,undefined4 esi_arg)

{
  RwV2d view_window;

  aux_camera_create_r(esi_arg,edi_arg);
  view_window.x = AUX_CAMERA_VIEW_WINDOW;
  view_window.y = AUX_CAMERA_VIEW_WINDOW;
  RwCameraSetViewWindow(g_aux_camera,&view_window);
  RwCameraSetNearClipPlane(g_aux_camera,AUX_CAMERA_NEAR_CLIP);
  RwCameraSetFarClipPlane(g_aux_camera,AUX_CAMERA_FAR_CLIP);
  g_car_6c2944 = RwCameraClone(g_aux_camera);
  g_aux_camera_6c2948 = RwFrameCreate();
  _rwObjectHasFrameSetFrame(g_car_6c2944,g_aux_camera_6c2948);
  return;
}


