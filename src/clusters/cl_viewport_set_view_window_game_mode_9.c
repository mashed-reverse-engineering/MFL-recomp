// cluster-entries: viewport_set_view_window_game_mode_9
/* Installed cluster viewport_set_view_window_game_mode_9 (tools/cluster_install.py from output/cluster_pilot_zx/viewport_set_view_window_game_mode_9.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*004045d0*/ extern int training_text_box_b();
/*004046a0*/ extern int training_text_box_c();
/*00404720*/ extern int training_text_box_a();
/*00404840*/ extern int training_text_box_d();
/*00404b30*/ extern int hud_bomb_timer_text();
/*0040fdc0*/ extern int race_get_phase();
/*004d1e10*/ extern struct RwCamera * RwCameraSetViewWindow(struct RwCamera *p0, struct RwV2d *p1);
/*005ddc28*/ extern float k_f_0;
/*0064d8d8*/ extern float g_course_64d8d8[0x1000];
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007ef7b8*/ extern undefined4 game_mode;
extern char g_crown_64d950[];

#define VIEW_WINDOW_SIZE (0.8)
#define HUD_ENTRY_DWORDS (5)
#define HUD_ENTRY_THRESHOLD_IDX (4)


/* viewport_set_view_window_game_mode_9 */

/*
 * Draws the race/training HUD overlays for game modes 5, 8, 9 and 10 (inferred: the original comment named only mode 9).
 * Each mode saves the main camera view window, sets it to VIEW_WINDOW_SIZE, turns z test and z write off,
 * runs the draw for the current race phase, turns z test and z write back on and restores the saved window.
 * Takes no parameters and returns nothing.
 */

static int hud_message_slide_r(int p0, int p1);
extern void hud_message_slide(void);
void viewport_set_view_window_game_mode_9(void)

{
  int race_phase_2;
  RwCamera *camera;
  undefined4 *entry;
  int race_phase;
  RwV2d new_view_window;
  RwV2d saved_view_window;
  
  race_phase = 0;
  /* game mode 9: draw the training text box for phases 3/4/5, else the bomb timer text in phase 6 (below) */
  if (game_mode == 9) {
    camera = RwCameraGetCurrentCamera();
    RwCameraEndUpdate(camera);
    saved_view_window.x = (RwCamera_viewWindow(camera)).x;
    saved_view_window.y = (RwCamera_viewWindow(camera)).y;
    new_view_window.x = VIEW_WINDOW_SIZE;
    new_view_window.y = VIEW_WINDOW_SIZE;
    RwCameraSetViewWindow(camera,&new_view_window);
    race_phase = RwCameraBeginUpdate(camera);
    if (race_phase == 0) {
      return;
    }
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
    race_phase = race_get_phase();
    if (((race_phase == 5) || (race_phase = race_get_phase(), race_phase == 4)) ||
       (race_phase = race_get_phase(), race_phase == 3)) {
      training_text_box_c();
      goto LAB_00404d2e;
    }
  }
  else {
    /* modes 5 and 10 are handled inside this block; mode 8 skips it and takes the separate block after it */
    if (game_mode != 8) {
      if (game_mode != 5) {
        if (game_mode == 10) {
          camera = RwCameraGetCurrentCamera();
          RwCameraEndUpdate(camera);
          saved_view_window.x = (RwCamera_viewWindow(camera)).x;
          saved_view_window.y = (RwCamera_viewWindow(camera)).y;
          new_view_window.x = VIEW_WINDOW_SIZE;
          new_view_window.y = VIEW_WINDOW_SIZE;
          RwCameraSetViewWindow(camera,&new_view_window);
          race_phase_2 = RwCameraBeginUpdate(camera);
          if (race_phase_2 != 0) {
            RwRenderStateSet
                      (rwRENDERSTATEZTESTENABLE,(void *)0x0);
            RwRenderStateSet
                      (rwRENDERSTATEZWRITEENABLE,(void *)0x0);
            race_phase_2 = race_get_phase();
            if (((race_phase_2 == 5) || (race_phase_2 = race_get_phase(), race_phase_2 == 4)) ||
               (race_phase_2 = race_get_phase(), race_phase_2 == 3)) {
              training_text_box_b();
            }
            else {
              race_phase_2 = race_get_phase();
              if (race_phase_2 == 6) {
                /* phase 6: for each HUD message entry (HUD_ENTRY_DWORDS dwords) whose float at HUD_ENTRY_THRESHOLD_IDX is above k_f_0, slide it in */
                entry = g_course_64d8d8;
                do {
                  if (k_f_0 < *(float *)&entry[HUD_ENTRY_THRESHOLD_IDX]) {
                    hud_message_slide_r(entry,race_phase);
                  }
                  entry = entry + HUD_ENTRY_DWORDS;
                  race_phase = race_phase + 1;
                } while ((LONG_PTR)entry < ((LONG_PTR)&g_crown_64d950));
              }
            }
            RwRenderStateSet
                      (rwRENDERSTATEZTESTENABLE,(void *)0x1);
            RwRenderStateSet
                      (rwRENDERSTATEZWRITEENABLE,(void *)0x1);
            RwCameraEndUpdate(camera);
            RwCameraSetViewWindow(camera,&saved_view_window);
            RwCameraBeginUpdate(camera);
          }
        }
        return;
      }
      camera = RwCameraGetCurrentCamera();
      RwCameraEndUpdate(camera);
      saved_view_window.x = (RwCamera_viewWindow(camera)).x;
      saved_view_window.y = (RwCamera_viewWindow(camera)).y;
      new_view_window.x = VIEW_WINDOW_SIZE;
      new_view_window.y = VIEW_WINDOW_SIZE;
      RwCameraSetViewWindow(camera,&new_view_window);
      race_phase = RwCameraBeginUpdate(camera);
      if (race_phase == 0) {
        return;
      }
      RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
      RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
      race_phase = race_get_phase();
      if (((race_phase == 5) || (race_phase = race_get_phase(), race_phase == 4)) ||
         (race_phase = race_get_phase(), race_phase == 3)) {
        training_text_box_d();
      }
      goto LAB_00404d2e;
    }
    camera = RwCameraGetCurrentCamera();
    RwCameraEndUpdate(camera);
    saved_view_window.x = (RwCamera_viewWindow(camera)).x;
    saved_view_window.y = (RwCamera_viewWindow(camera)).y;
    new_view_window.x = VIEW_WINDOW_SIZE;
    new_view_window.y = VIEW_WINDOW_SIZE;
    RwCameraSetViewWindow(camera,&new_view_window);
    race_phase = RwCameraBeginUpdate(camera);
    if (race_phase == 0) {
      return;
    }
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
    race_phase = race_get_phase();
    if (((race_phase == 5) || (race_phase = race_get_phase(), race_phase == 4)) ||
       (race_phase = race_get_phase(), race_phase == 3)) {
      training_text_box_a();
      goto LAB_00404d2e;
    }
  }
  race_phase = race_get_phase();
  if (race_phase == 6) {
    hud_bomb_timer_text();
  }
  /* restore: z test and z write back on, then put the saved view window back */
LAB_00404d2e:
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
  RwCameraEndUpdate(camera);
  RwCameraSetViewWindow(camera,&saved_view_window);
  RwCameraBeginUpdate(camera);
  return;
}

/* hud_message_slide_r: C -> original hud_message_slide (registers eax,s0) */
__declspec(naked) int hud_message_slide_r(int p0, int p1)
{
    __asm {
        push dword ptr [esp + 8]
        mov eax, dword ptr [esp + 8]
        call hud_message_slide
        add esp, 4
        ret
    }
}
