// cluster-entries: aux_camera_create aux_camera_setup
/* Installed cluster aux_camera_create (tools/cluster_install.py from output/cluster_pilot2/aux_camera_create.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/*004d2d80*/ extern struct RwCamera * RwCameraClone(struct RwCamera *p0);
/*004d1ec0*/ extern struct RwCamera * RwCameraCreate();
/*004d1ca0*/ extern struct RwCamera * RwCameraSetFarClipPlane(struct RwCamera *p0, RwReal p1);
/*004d1bd0*/ extern struct RwCamera * RwCameraSetNearClipPlane(struct RwCamera *p0, RwReal p1);
/*004d1e10*/ extern struct RwCamera * RwCameraSetViewWindow(struct RwCamera *p0, struct RwV2d *p1);
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004d7950*/ extern struct RwRaster * RwRasterCreate(RwInt32 p0, RwInt32 p1, RwInt32 p2, RwInt32 p3);
/*004d1860*/ extern void _rwObjectHasFrameSetFrame(void *p0, struct RwFrame *p1);
/* DECLS-END */
/* Cluster aux_camera_create: the auxiliary (second) render camera with its own frame, frame buffer and z buffer. */
extern RwCamera *g_aux_camera;
extern RwCamera *g_car_6c2944;
extern RwFrame *g_aux_camera_6c2948;
extern void aux_frame_destroy(RwCamera *camera);

/* Creates the auxiliary camera with a width x height frame buffer and z buffer. Returns NULL on failure. */
RwCamera *aux_camera_create_n(RwInt32 width, RwInt32 height)
{
    RwFrame *frame;

    g_aux_camera = RwCameraCreate();
    if (g_aux_camera != NULL) {
        frame = RwFrameCreate();
        _rwObjectHasFrameSetFrame(g_aux_camera, frame);
        RwCamera_frameBuffer(g_aux_camera) = RwRasterCreate(width, height, 0, 2);
        RwCamera_zBuffer(g_aux_camera) = RwRasterCreate(width, height, 0, 1);
        if (RwCamera_object_object_parent(g_aux_camera) != NULL &&
            RwCamera_frameBuffer(g_aux_camera) != NULL && RwCamera_zBuffer(g_aux_camera) != NULL) {
            return g_aux_camera;
        }
    }
    aux_frame_destroy(g_aux_camera);
    return NULL;
}

/* Creates the auxiliary camera at the given size, sets its view window and clip planes, and clones it for the car view. */
void aux_camera_setup(RwInt32 width, RwInt32 height)
{
    RwV2d view_window;

    aux_camera_create_n(width, height);
    view_window.x = 0.8f;
    view_window.y = 0.8f;
    RwCameraSetViewWindow(g_aux_camera, &view_window);
    RwCameraSetNearClipPlane(g_aux_camera, 0.5f);
    RwCameraSetFarClipPlane(g_aux_camera, 180.0f);
    g_car_6c2944 = RwCameraClone(g_aux_camera);
    g_aux_camera_6c2948 = RwFrameCreate();
    _rwObjectHasFrameSetFrame(g_car_6c2944, g_aux_camera_6c2948);
}

/* entry thunk at the original address of aux_camera_create: register inputs (edi,esi) -> natural C aux_camera_create_n; ret 0 */
__declspec(naked) void aux_camera_create(void)
{
    __asm {
        push esi
        push edi
        call aux_camera_create_n
        add esp, 8
        ret
    }
}
