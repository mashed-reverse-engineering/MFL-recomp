// cluster-entries: video_play_loop
/* Installed cluster video_play_loop (tools/cluster_install.py from output/cluster_pilot_kc/video_play_loop.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall Sleep(int);
/*00475730*/ extern int get_viewport_camera();
/*004a20d0*/ extern int input_update();
/*004a3490*/ extern int __cdecl app_get_app_76e890_3490(undefined4 p0);
/*004a34b0*/ extern int app_get_two_size_pairs();
/*004a34f0*/ extern int app_get_app_76e8a4();
/*004a3500*/ extern int __cdecl video_draw_textured_quad(RwCamera *camera, undefined4 *rect_min, undefined4 *rect_max, int unused, undefined4 texture, float ratio_x, float ratio_y);      /* reads 7 stack words: the original leaves the two arguments of app_get_app_76e8a4 (ratio_x, ratio_y) on the stack and they become arguments 6 and 7 */
/*004a39a0*/ extern undefined __cdecl movie_stop(undefined4 p0);
/*004a39e0*/ extern int __cdecl movie_poll_and_set_surface(undefined4 p0, undefined4 p1);
/*004a3fe0*/ extern int video_texture_setup();
/*004a9380*/ extern int app_get_app_7e7704();
/*004d1b80*/ extern int RwCameraEndUpdate();
/*004d1b90*/ extern int RwCameraBeginUpdate();
/*004d1d40*/ extern struct RwCamera * RwCameraClear(struct RwCamera *p0, struct RwRGBA *p1, RwInt32 p2);
/*004d1d70*/ extern struct RwCamera * RwCameraShowRaster(struct RwCamera *p0, void *p1, RwUInt32 p2);
/*007efce0*/ extern struct PlayerInput g_player_inputs_previous[12];
/* video_play_loop */

/* "loops the movie playback until a button is pressed, clearing and drawing each frame" */

/* (inferred) plays the four intro movies (ids 1..4, movie_ids) one after the other: each frame polls the movie, clears the camera to teal (0,128,128) and draws the video texture as a quad (ratio_x/y = video size / texture size), a menu_select press (released edge) skips to the next movie; size_a / size_b are the two pairs from app_get_two_size_pairs */
void video_play_loop(void)

{
  bool button_pressed;
  RwCamera *camera;
  int iVar2;
  byte *input_cursor;
  void *device;
  int movie_index;
  undefined4 texture_arg;
  RwUInt32 flags;
  struct {
    RwRGBA clear_colour;
    float ratio_y;
    float ratio_x;
    float size_b_w;
    float size_b_h;
    float size_a_w;
    float size_a_h;
    undefined4 rect_max_x;
    undefined4 rect_max_y;
    undefined4 rect_min_x;
    undefined4 rect_min_y;
    undefined4 movie_ids [5];
  } frame;
  
  movie_index = 0;
  frame.clear_colour.red = '\0';
  frame.clear_colour.green = 0x80;
  frame.clear_colour.blue = 0x80;
  frame.clear_colour.alpha = 0xff;
  frame.movie_ids[0] = 1;
  frame.movie_ids[1] = 2;
  frame.movie_ids[2] = 3;
  frame.movie_ids[3] = 4;
  video_texture_setup(0,1,0);
  app_get_two_size_pairs(&frame.size_a_w,&frame.size_b_w);
  do {
    button_pressed = false;
    texture_arg = 0;
    camera = (RwCamera *)get_viewport_camera(0x0);
    input_update();
    iVar2 = movie_poll_and_set_surface(0,texture_arg);
    if (iVar2 != 0) {
      input_cursor = &g_player_inputs_previous[0].menu_select;
      do {
        if ((input_cursor[-0x4c0] != '\0') && (*input_cursor == 0)) {
          button_pressed = true;
          break;
        }
        input_cursor = input_cursor + 0x4c;
      } while ((int)input_cursor < (int)&g_player_inputs_previous + 0x13a);
      RwCameraClear(camera,&frame.clear_colour,1);
      iVar2 = RwCameraBeginUpdate(camera);
      if (iVar2 != 0) {
        frame.rect_min_x = 0;
        frame.ratio_x = frame.size_a_w / frame.size_b_w;
        frame.rect_min_y = 0;
        frame.rect_max_x = 0x3f800000;
        frame.rect_max_y = 0x3f800000;
        frame.ratio_y = frame.size_a_h / frame.size_b_h;
        texture_arg = app_get_app_76e8a4(*(int *)&frame.ratio_x,*(int *)&frame.ratio_y);
        video_draw_textured_quad(camera,&frame.rect_min_x,&frame.rect_max_x,0,texture_arg,frame.ratio_x,frame.ratio_y);
        RwCameraEndUpdate(camera);
      }
    }
    flags = 0;
    device = (void *)app_get_app_7e7704();
    RwCameraShowRaster(camera,device,flags);
    Sleep(5);
    iVar2 = app_get_app_76e890_3490(0);
    if ((iVar2 == 0) || (button_pressed)) {
      movie_stop(0);
      movie_index = movie_index + 1;
      if (3 < movie_index) {
        return;
      }
      video_texture_setup(0,frame.movie_ids[movie_index],0);
      app_get_two_size_pairs(&frame.size_a_w,&frame.size_b_w);
    }
  } while( true );
}

