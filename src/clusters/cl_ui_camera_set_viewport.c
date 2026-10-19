// cluster-entries: ui_camera_set_viewport ui_camera_create_main
/* Installed cluster ui_camera_set_viewport (tools/cluster_install.py from output/cluster_pilot2/ui_camera_set_viewport.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/*004d1ca0*/ extern struct RwCamera * RwCameraSetFarClipPlane(struct RwCamera *p0, RwReal p1);
/*004d1bd0*/ extern struct RwCamera * RwCameraSetNearClipPlane(struct RwCamera *p0, RwReal p1);
/*004d1e10*/ extern struct RwCamera * RwCameraSetViewWindow(struct RwCamera *p0, struct RwV2d *p1);
/*004d78f0*/ extern struct RwRaster * RwRasterSubRaster(struct RwRaster *p0, struct RwRaster *p1, struct RwRect *p2);
/* DECLS-END */
/* Cluster ui_camera_set_viewport: the UI camera (created by ui_camera_create_main) draws into the centre
   quarter-size rectangle of the viewport camera's frame buffer and z buffer. */
extern RwCamera *g_ui_69d698;
extern RwCamera *ui_camera_create(RwInt32 width, RwInt32 height, int flags);
extern short screen_width(void);
extern short screen_height(void);
extern RwCamera *get_viewport_camera(int index);

/* Sets the UI camera view window and its sub-rectangle (a quarter of the screen offset, half the screen size). */
void ui_camera_set_viewport(void)
{
    RwRect rect;
    RwV2d view_window;
    RwCamera *viewport;
    short sw;

    sw = screen_width();
    rect.x = sw / 4;
    sw = screen_height();
    rect.y = sw / 4;
    sw = screen_width();
    rect.w = sw / 2;
    sw = screen_height();
    rect.h = sw / 2;

    view_window.x = 0.8f;
    view_window.y = 0.8f;
    RwCameraSetViewWindow(g_ui_69d698, &view_window);

    viewport = get_viewport_camera(-1);
    RwRasterSubRaster(RwCamera_frameBuffer(g_ui_69d698), RwCamera_frameBuffer(viewport), &rect);
    viewport = get_viewport_camera(-1);
    RwRasterSubRaster(RwCamera_zBuffer(g_ui_69d698), RwCamera_zBuffer(viewport), &rect);
}

/* Creates the main UI camera: clip planes, then its viewport. Returns 1. */
int ui_camera_create_main(void)
{
    g_ui_69d698 = ui_camera_create(0, 0, 1);
    RwCameraSetNearClipPlane(g_ui_69d698, 0.5f);
    RwCameraSetFarClipPlane(g_ui_69d698, 180.0f);
    ui_camera_set_viewport();
    return 1;
}

