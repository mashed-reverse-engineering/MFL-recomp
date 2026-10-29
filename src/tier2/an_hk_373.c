#include "mfl_tier2.h"
/*0041bd90*/ extern int hudpanel_render_all_slots();
/*0041bf10*/ extern int marker_particles_render();
/*00475730*/ extern void * get_viewport_camera();
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d1da0*/ extern struct RwCamera * RwCameraSetProjection(struct RwCamera *p0, enum RwCameraProjection p1);
/*004d1e10*/ extern struct RwCamera * RwCameraSetViewWindow(struct RwCamera *p0, struct RwV2d *p1);
/*00658450*/ extern undefined g_hudpanel_658450;
/*00658538*/ extern undefined g_hudpanel_658538;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
static int hudpanel_marker_calc_pixel_ray_r(int reg_eax);
extern void hudpanel_marker_calc_pixel_ray(void);
__declspec(naked) int hudpanel_marker_calc_pixel_ray_r(int reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call hudpanel_marker_calc_pixel_ray
        ret
    }
}

static int hudpanel_render_parts_if_active_r(int * reg_eax);
extern void hudpanel_render_parts_if_active(void);
extern char g_hudpanel_6584f8[];
extern char g_load_6586e8[];

#define MARKER_SLOT_STRIDE (0x6c)
#define MARKER_SLOT_ACTIVE_OFFSET (0x34) /* marker state: nonzero = slot in use, 2 = marker that needs a camera pixel ray (hudpanel_marker_calc_pixel_ray) */
#define HUD_PART_STRIDE (0x54)
#define MATRIX_FLOAT_COUNT (16) /* 4x4 RwMatrix as 16 floats */
#define MATRIX_FLAGS_IDENTITY_ORTHONORMAL (0x20003) /* rwMATRIXINTERNALIDENTITY | rwMATRIXTYPEORTHONORMAL */
#define HUD_VIEW_WINDOW_X (0.6)
#define HUD_VIEW_WINDOW_Y (0.45)

__declspec(naked) int hudpanel_render_parts_if_active_r(int * reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call hudpanel_render_parts_if_active
        ret
    }
}

/* hudpanel_markers_render_all */

/* "renders all marker slots with z write off plus the particle layer with the camera frame setup"
    */

/*
 * Draws the HUD marker overlay for the viewport camera, only when the menu enables markers for the current event.
 * Pass 1 (z write off): hudpanel_render_all_slots draws the slots; then each slot with a non-zero state gets its camera pixel ray computed (state 2 only; nothing is drawn by that loop), then the marker particles are drawn.
 * Pass 2 (z test and write off): the HUD parts drawn with the camera switched to a parallel projection,
 * an identity frame transform and a fixed view window (inferred: a 2D overlay space).
 * Restores the camera frame transform, view window and perspective projection at the end.
 * Takes no parameters and returns nothing.
 */
void hudpanel_markers_render_all(void)

{
  RwFrame *camera_frame;
  int iVar1;
  RwCamera *camera;
  undefined *entry;
  RwMatrix *src;
  RwMatrix *dst;
  RwV2d saved_view_window;
  RwV2d hud_view_window;
  RwMatrix identity_matrix;
  RwMatrix saved_frame_matrix;

  hud_view_window.x = HUD_VIEW_WINDOW_X;
  hud_view_window.y = HUD_VIEW_WINDOW_Y;
  iVar1 = menu_markers_enabled_for_event();
  if (iVar1 != 0) {
    hudpanel_render_all_slots();
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
    /* Walk the marker slot array; for each slot in use compute its camera pixel ray (the callee acts on state 2 only, it draws nothing). */
    entry = &g_hudpanel_658538;
    do {
      if (*(int *)(entry + MARKER_SLOT_ACTIVE_OFFSET) != 0) {
        hudpanel_marker_calc_pixel_ray_r(entry);
      }
      entry = entry + MARKER_SLOT_STRIDE;
    } while ((LONG_PTR)entry < ((LONG_PTR)&g_load_6586e8));
    marker_particles_render();
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
    camera = (RwCamera *)get_viewport_camera(0);
    RwCameraEndUpdate(camera);
    camera_frame = RwCamera_object_object_parent(camera);
    /* Save the camera frame's modelling matrix (16 floats) so it can be restored after pass 2. */
    src = &RwFrame_modelling(camera_frame);
    dst = &saved_frame_matrix;
    for (iVar1 = MATRIX_FLOAT_COUNT; iVar1 != 0; iVar1 = iVar1 + -1) {
      (dst->right).x = (src->right).x;
      src = (RwMatrix *)&(src->right).y;
      dst = (RwMatrix *)&(dst->right).y;
    }
    saved_view_window.x = (RwCamera_viewWindow(camera)).x;
    saved_view_window.y = (RwCamera_viewWindow(camera)).y;
    /* Build an identity transform for the camera frame (flags, then right/up/at/pos). */
    identity_matrix.flags = identity_matrix.flags | MATRIX_FLAGS_IDENTITY_ORTHONORMAL;
    identity_matrix.at.z = 1.0;
    identity_matrix.up.y = 1.0;
    identity_matrix.right.x = 1.0;
    identity_matrix.up.x = 0.0;
    identity_matrix.right.z = 0.0;
    identity_matrix.right.y = 0.0;
    identity_matrix.at.y = 0.0;
    identity_matrix.at.x = 0.0;
    identity_matrix.up.z = 0.0;
    identity_matrix.pos.z = 0.0;
    identity_matrix.pos.y = 0.0;
    identity_matrix.pos.x = 0.0;
    RwFrameTransform(camera_frame,&identity_matrix,rwCOMBINEREPLACE);
    RwCameraSetViewWindow(camera,&hud_view_window);
    RwCameraSetProjection(camera,rwPARALLEL);
    RwCameraBeginUpdate(camera);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
    /* Walk the HUD part array; each part is passed to hudpanel_render_parts_if_active, which does the active check. */
    entry = &g_hudpanel_658450;
    do {
      hudpanel_render_parts_if_active_r(entry);
      entry = entry + HUD_PART_STRIDE;
    } while ((LONG_PTR)entry < ((LONG_PTR)&g_hudpanel_6584f8));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
    RwCameraEndUpdate(camera);
    RwCameraSetProjection(camera,rwPERSPECTIVE);
    RwCameraSetViewWindow(camera,&saved_view_window);
    RwFrameTransform(camera_frame,&saved_frame_matrix,rwCOMBINEREPLACE);
    RwCameraBeginUpdate(camera);
  }
  return;
}


