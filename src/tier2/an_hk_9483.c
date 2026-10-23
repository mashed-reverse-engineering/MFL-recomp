#include "mfl_tier2.h"
/*00406cf0*/ extern int fx_objects_all_exploded();
/*0040f700*/ extern float intro_banner_get_x(void);
/*0040fdc0*/ extern int race_get_phase();
/*0040ff60*/ extern int race_get_phase_timer();
/*0041a380*/ extern int ai_ftol_noop_wrapper();
/*00421210*/ extern int game_get_game_659d00();
/*0042c440*/ extern int __cdecl ui_rt2d_text_draw(int id, float x, float y, int colour, float scale, int flags);
/*0042e6c0*/ extern int race_lose_banner_draw();
/*0042e780*/ extern int intro_banner_draw_team_text();
/*0042ed70*/ extern int race_winner_banner_draw();
/*00434bb0*/ extern int get_event_type();
/*0044c540*/ extern int camera_get_car_8c747c();
/*00475730*/ extern char *get_viewport_camera();
/*004824a0*/ extern int draw_rect();
/*004d1d40*/ extern struct RwCamera * RwCameraClear(struct RwCamera *p0, struct RwRGBA *p1, RwInt32 p2);
/*004d1e10*/ extern struct RwCamera * RwCameraSetViewWindow(struct RwCamera *p0, struct RwV2d *p1);
/*005dd32c*/ extern float k_f_3;
/*005dd7f4*/ extern float k_f_80;
/*005ddb20*/ extern float k_f_0_9;
/*005ddc28*/ extern float k_f_0;
/*005ddf74*/ extern float k_f_64;
/*005de290*/ extern float float_50_0;
/*005de8fc*/ extern float k_f_68;
/*005de900*/ extern float k_f_52;
/*005de904*/ extern float k_f_480;
/*007d2128*/ extern char *RwEngineInstance;
/*007ef7b0*/ extern undefined4 g_car_7ef7b0;
/*007ef7b8*/ extern undefined4 game_mode;
/*007ef7fc*/ extern undefined4 g_race_7ef7fc;
/*008c8790*/ extern undefined4 g_intro_banner_text_id;
/*008c8794*/ extern float g_intro_banner_slide_step;
/*008c8798*/ extern undefined4 g_intro_banner_offset;
/*0041a370*/ extern float ai_get_counter();
/* race_status_messages_draw */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "'Car is wrecked', 'Time Trial Abandoned', 'Fugitive has lapped you!' (texts 598, 599, 534 ...)"
    */

void race_status_messages_draw(void)

{
  float fbits1;
  float fbits2;
  float fbits3;
  float fbits4;
  undefined4 uVar1;
  float fVar2;
  RwCamera *camera;
  int iVar3;
  RwCamera *pRVar4;
  undefined4 uVar5;
  double fVar6;
  RwRGBA *pRVar7;
  RwInt32 RVar8;
  struct {
    undefined4 local_28;
    RwRGBA local_24;
    undefined4 local_20;
    undefined4 uStack_1c;
    float local_18;
    float fStack_14;
  } frame;
  RwV2d RStack_10;
  RwV2d RStack_8;
  
  frame.local_24.red = '\0';
  frame.local_24.green = '\0';
  frame.local_24.blue = '\0';
  frame.local_24.alpha = '\0';
  frame.local_20 = 0xff000000;
  frame.local_18 = -2.5554925e+38;
  frame.local_28 = 0xffffffff;
  camera = (RwCamera *)get_viewport_camera(0);
  iVar3 = get_event_type();
  fVar2 = frame.local_18;
  uVar1 = frame.local_20;
  if ((((iVar3 == 0) || (iVar3 = get_event_type(), iVar3 == 1)) ||
      (iVar3 = get_event_type(), iVar3 == 2)) &&
     ((iVar3 = race_get_phase(), iVar3 == 7 && (iVar3 = game_get_game_659d00(), iVar3 == 0)))) {
    RwCameraEndUpdate(camera);
    RStack_8.x = RwCamera_viewWindow(camera).x;
    RStack_8.y = RwCamera_viewWindow(camera).y;
    RStack_10.x = 0.800000012f;
    RStack_10.y = 0.800000012f;
    RwCameraSetViewWindow(camera,&RStack_10);
    RVar8 = 2;
    pRVar7 = &frame.local_24;
    pRVar4 = (RwCamera *)get_viewport_camera(0);
    RwCameraClear(pRVar4,pRVar7,RVar8);
    RwCameraBeginUpdate(camera);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
    ui_rt2d_text_draw(0x256, 323.0f, 98.0f, uVar1, 1.0f, 2);
    ui_rt2d_text_draw(0x256, 320.0f, 95.0f, *(int *)&fVar2, 1.0f, 2);
    ui_rt2d_text_draw(599, 323.0f, 128.0f, uVar1, 1.0f, 2);
    ui_rt2d_text_draw(599, 320.0f, 125.0f, *(int *)&fVar2, 1.0f, 2);
    RwCameraEndUpdate(camera);
    RwCameraSetViewWindow(camera,&RStack_8);
    RwCameraBeginUpdate(camera);
  }
  iVar3 = camera_get_car_8c747c();
  if (iVar3 != 1) {
    if (g_intro_banner_text_id != 0) {
      RwCameraEndUpdate(camera);
      RStack_8.x = RwCamera_viewWindow(camera).x;
      RStack_8.y = RwCamera_viewWindow(camera).y;
      RStack_10.x = 0.800000012f;
      RStack_10.y = 0.800000012f;
      RwCameraSetViewWindow(camera,&RStack_10);
      RVar8 = 2;
      pRVar7 = &frame.local_24;
      pRVar4 = (RwCamera *)get_viewport_camera(0);
      RwCameraClear(pRVar4,pRVar7,RVar8);
      RwCameraBeginUpdate(camera);
      frame.local_28 = 0xff000000;
      frame.uStack_1c = 0x42a00000;
      frame.local_20 = 0x44200000;
      frame.local_18 = 0.0;
      frame.fStack_14 = g_intro_banner_slide_step;
      draw_rect(0,*(int *)&g_intro_banner_slide_step,0x44200000,0x42a00000,0xff000000);
      frame.fStack_14 = (k_f_480 - g_intro_banner_slide_step) - k_f_64;
      draw_rect(0,*(int *)&frame.fStack_14,0x44200000,frame.uStack_1c,frame.local_28);
      frame.local_28 = 0xffffffff;
      frame.uStack_1c = 0x3f800000;
      draw_rect(0,*(int *)&frame.fStack_14,0x44200000,0x3f800000,0xffffffff);
      frame.fStack_14 = g_intro_banner_slide_step + k_f_80;
      draw_rect(0,*(int *)&frame.fStack_14,0x44200000,frame.uStack_1c,0xffffffff);
      RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
      iVar3 = race_get_phase_timer();
      if (199 < iVar3) {
        ui_rt2d_text_draw(0x2e, 64.0f, (k_f_480 - g_intro_banner_slide_step) - k_f_52, 0xffffffff, 0.600000024f, 0);
      }
      ui_rt2d_text_draw(0x42, 64.0f, g_intro_banner_slide_step + float_50_0, 0xffffffff, 0.800000012f, 0);
      ui_rt2d_text_draw(g_intro_banner_text_id, 64.0f, g_intro_banner_slide_step + k_f_68, 0xffffffff, 0.600000024f, 0);
      if (g_intro_banner_slide_step == k_f_0) {
        if (((g_race_7ef7fc != 0) && (((byte)g_intro_banner_offset & 0x3f) < 0x34)) &&
           (iVar3 = get_event_type(), iVar3 != 0)) {
          ui_rt2d_text_draw(0xd4, 320.0f, 400.0f, 0xffffffff, 1.0f, 2);
        }
      }
      else {
        g_intro_banner_offset = 0;
      }
      RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
      RwCameraEndUpdate(camera);
      RwCameraSetViewWindow(camera,&RStack_8);
      RwCameraBeginUpdate(camera);
    }
    return;
  }
  iVar3 = race_get_phase();
  if (iVar3 != 0xb) {
    return;
  }
  iVar3 = get_event_type();
  if ((((iVar3 == EVENT_CUP_3) || (iVar3 = get_event_type(), iVar3 == 4)) ||
      (iVar3 = get_event_type(), iVar3 == 5)) && (g_car_7ef7b0 != 0)) {
    uVar5 = 1;
  }
  else {
    uVar5 = 0;
  }
  RwCameraEndUpdate(camera);
  RStack_8.x = RwCamera_viewWindow(camera).x;
  RStack_8.y = RwCamera_viewWindow(camera).y;
  RStack_10.x = 0.800000012f;
  RStack_10.y = 0.800000012f;
  RwCameraSetViewWindow(camera,&RStack_10);
  RVar8 = 2;
  pRVar7 = &frame.local_24;
  pRVar4 = (RwCamera *)get_viewport_camera(0);
  RwCameraClear(pRVar4,pRVar7,RVar8);
  RwCameraBeginUpdate(camera);
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
  switch(game_mode) {
  case 5:
    iVar3 = fx_objects_all_exploded();
    if (iVar3 == 0) {
LAB_0042f22a:
      race_lose_banner_draw();
      goto LAB_0042f393;
    }
    race_winner_banner_draw(uVar5);
    uVar5 = 1;
    break;
  default:
    race_winner_banner_draw(uVar5);
    break;
  case 8:
    iVar3 = ai_ftol_noop_wrapper(0);
    if (iVar3 == -1) goto LAB_0042f22a;
    goto LAB_0042f370;
  case 9:
    if (g_car_7ef7b0 == 0) {
      race_lose_banner_draw();
      fVar6 = (double)ai_get_counter(1);
      frame.local_18 = (float)fVar6;
      fVar6 = (double)ai_get_counter(0);
      if ((double)k_f_0_9 <= (double)frame.local_18 - fVar6) {
        ui_rt2d_text_draw(0x216, intro_banner_get_x() + k_f_3, 98.0f, uVar1, 1.0f, 2);
        ui_rt2d_text_draw(0x216, intro_banner_get_x(), 95.0f, *(int *)&fVar2, 1.0f, 2);
      }
    }
    else {
      race_winner_banner_draw(uVar5);
      intro_banner_draw_team_text(&frame.local_28,1);
      ui_rt2d_text_draw(0x215, 320.0f, 98.0f, uVar1, 1.0f, 2);
      ui_rt2d_text_draw(0x215, 320.0f, 95.0f, frame.local_28, 1.0f, 2);
    }
    goto LAB_0042f393;
  case 10:
    if (g_car_7ef7b0 == 0) {
      race_lose_banner_draw();
      ui_rt2d_text_draw(0x231, intro_banner_get_x() + k_f_3, 98.0f, uVar1, 1.0f, 2);
      ui_rt2d_text_draw(0x231, intro_banner_get_x(), 95.0f, *(int *)&fVar2, 1.0f, 2);
      goto LAB_0042f393;
    }
LAB_0042f370:
    race_winner_banner_draw(uVar5);
    uVar5 = 1;
  }
  intro_banner_draw_team_text(&frame.local_28,uVar5);
LAB_0042f393:
  RwCameraEndUpdate(camera);
  RwCameraSetViewWindow(camera,&RStack_8);
  RwCameraBeginUpdate(camera);
  return;
}


