#include "mfl_tier2.h"
extern undefined1 g_ui_2d_8c87bc[];
/*00405c80*/ extern undefined __cdecl clump_for_all_atomics_with_camera_matrix(struct RpClump *p0);
/*00405cd0*/ extern undefined __cdecl clump_clear_atomic_plugin_flag_1(struct RpClump *p0);
/*004069f0*/ extern int skydome_follow_camera();
/*00408900*/ extern int fx_objects_destroy();
/*00409eb0*/ extern int player_rank_key();
/*0040c620*/ extern int course_editor_read_vec_chunk();
/*00413880*/ extern int current_course_id_with_mirror();
/*004219b0*/ extern undefined __cdecl course_call_cb1(undefined4 p0, undefined4 p1);
/*004219e0*/ extern undefined __cdecl course_call_cb4(undefined4 p0);
/*004219f0*/ extern undefined __cdecl course_call_cb5(undefined4 p0);
/*00421a00*/ extern int course_call_cb6();
/*00421a10*/ extern undefined __cdecl course_call_cb7(undefined4 p0);
/*00421a20*/ extern int course_call_cb8();
/*00421a30*/ extern int course_call_cb9();
/*00421a40*/ extern int course_call_cb10();
/*00421a50*/ extern int course_call_cb11();
/*00421aa0*/ extern int course_select();
/*00421ad0*/ extern int course_has_cb1();
/*00421b00*/ extern int course_has_cb4();
/*00421b10*/ extern int course_has_cb5();
/*00421b20*/ extern int course_has_cb6();
/*00421b30*/ extern int course_has_cb7();
/*00421b40*/ extern int course_has_cb8();
/*00421b50*/ extern int course_has_cb9();
/*00421b60*/ extern int course_has_cb10();
/*00421b70*/ extern int course_has_cb11();
/*00429c20*/ extern int vehicle_slot_set();
/*00429cd0*/ extern undefined __stdcall load_copter_model(char *p0);
/*0042a110*/ extern int course_destroy_extra_clumps();
/*00434a20*/ extern int get_second_camera();
/*0046a010*/ extern int sound_stop_all_voices_and_streams();
/*00473a50*/ extern int course_unload_atomics_6ae24c();
/*00474880*/ extern int course_objects_destroy_atomics();
/*00475510*/ extern int course_grid_buffers_free();
/*00475730*/ extern int get_viewport_camera();
/*004757a0*/ extern int get_viewport_camera_matrix();
/*0047a830*/ extern int car_is_active();
/*00480bc0*/ extern undefined __cdecl rwp_trigger_groups_update(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5, undefined4 p6, undefined4 p7, undefined4 p8, undefined4 p9, undefined4 p10, undefined4 p11, undefined4 p12);
/*00484910*/ extern undefined __cdecl clump_for_all_materials_uv_anim(struct RpClump *p0, undefined4 p1);
/*00485030*/ extern int frame_follow_path_step();
/*00485240*/ extern RwBool RwTexDictionaryDestroy_004d5ac0(struct RwTexDictionary *p0);
/*00488da0*/ extern undefined __cdecl course_destroy_world(struct CourseWorld *p0, struct CourseCounts *p1);
/*0048d730*/ extern undefined __stdcall course_collision_objects_cull(struct RwCamera *p0);
/*00490ef0*/ extern int rwp_world_flush();
/*00490f30*/ extern int rwp_world_destroy();
/*00492e00*/ extern int rwp_objects_reset_all();
/*004a07c0*/ extern int rain_drops_update_and_respawn();
/*004a0850*/ extern int fx_buffer_free();
/*004a1860*/ extern int game_set_option_flags();
/*004a1880*/ extern int game_get_62ad2c();
/*004a43d0*/ extern undefined __cdecl game_open_piz(char *p0);
/*004b2b48*/ extern int _ftol();
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d1bd0*/ extern struct RwCamera * RwCameraSetNearClipPlane(struct RwCamera *p0, RwReal p1);
/*004d1ca0*/ extern struct RwCamera * RwCameraSetFarClipPlane(struct RwCamera *p0, RwReal p1);
/*004d1cd0*/ extern enum RwFrustumTestResult RwCameraFrustumTestSphere(struct RwCamera *p0, struct RwSphere *p1);
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d56a0*/ extern struct RwV3d * RwV3dTransformVector(struct RwV3d *p0, struct RwV3d *p1, RwMatrix *p2);
/*004f6240*/ extern struct RwSphere * RpAtomicGetWorldBoundingSphere(struct RpAtomic *p0);
/*00569270*/ extern int FUN_00569270();
/*005dd330*/ extern float float_1_0;
/*005dd364*/ extern float k_f_n1;
/*005dd5a0*/ extern undefined2 g_course_5dd5a0;
/*005dd5a2*/ extern undefined g_game_5dd5a2;
/*005dda1c*/ extern float k_f_3_3333333en4;
/*005dda20*/ extern float k_f_4_2949673e9;
/*005de648*/ extern undefined1 g_course_list_5de648;
/*005de6a8*/ extern float k_f_n0_5;
/*006095bc*/ extern undefined4 g_course_race_6095bc;
/*006095c0*/ extern undefined4 g_course_update_6095c0;
/*006095c4*/ extern undefined1 g_course_update_6095c4;
/*00664414*/ extern undefined4 g_load_664414;
/*00664418*/ extern struct CourseCounts g_course_counts_data;
/*00665138*/ extern struct CourseFiles g_course_files_data;
/*00668c18*/ extern struct CourseWorld g_course_world;
/*00675668*/ extern struct CourseLap g_course_lap_data;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*008c87c0*/ extern undefined4 g_course_piz_path;
extern undefined1 s_ToastArt_005de63c[];
extern undefined1 s_tracks_005de634[];
/*008c8840*/ extern float g_course_8c8840;
/*008c8844*/ extern undefined4 g_course_8c8844;
/*008c8848*/ extern undefined4 g_course_8c8848;
/*008c884c*/ extern undefined4 g_course_fog_8c884c;
/*008c8850*/ extern undefined1 g_course_8c8850;
/*008c8854*/ extern undefined1 g_course_8c8854;
/*008c8858*/ extern undefined1 g_course_8c8858;
/*008c8860*/ extern undefined4 g_course_8c8860;
/*008c8864*/ extern undefined4 g_course_8c8864;
/*008c88a4*/ extern undefined4 g_game_8c88a4;
/*008c88b0*/ extern undefined4 g_course_8c88b0;
/*008c88f4*/ extern undefined4 g_game_8c88f4;
/*008c8900*/ extern undefined4 g_course_active_8c8900;
/*008c8944*/ extern undefined4 g_game_8c8944;
/*008c8950*/ extern undefined4 g_course_active_8c8950;
/*008c8994*/ extern undefined4 g_game_8c8994;
/*008c89a0*/ extern undefined4 g_course_active_8c89a0;
/*008c89e4*/ extern undefined4 g_game_8c89e4;
/*008c89f0*/ extern undefined4 g_course_active_8c89f0;
/*008c8a34*/ extern undefined4 g_game_8c8a34;
/*008c8a40*/ extern undefined4 g_course_active_8c8a40;
/*008c8a84*/ extern undefined4 g_game_8c8a84;
/*008c8a90*/ extern undefined4 g_course_active_8c8a90;
/*008c8ad4*/ extern undefined4 g_game_8c8ad4;
#define TOAST_ART_STR_LEN (8)
#define TRACKS_STR_LEN (6)
/* offset of the path being built inside g_ui_2d_8c87bc (inferred: scan buffer start, from the header comment) */
#define SCAN_BUF_OFS (3)
/* course_list_scan */

/* "scans ToastArt/tracks" */

/* TIER2: unvalidated 2026-10-11 authority=diffemu */
/*
 * Builds a course-list path in g_course_piz_path and returns it (inferred: the
 * path of the course list file). The scan buffer is g_ui_2d_8c87bc + 3; each
 * piece below is appended to it with a word copy, then a byte copy.
 *   use_backslash: nonzero selects "\\" as the separator, zero selects "/" (inferred).
 *   dir_name:      directory piece, always appended after a separator (inferred).
 *   sub_name:      sub-folder piece, appended after a separator when non-empty (inferred).
 *   ext_name:      suffix piece, appended with no separator when non-empty (inferred).
 *   unused_5..7:   never read.
 */
/* TIER2: validated 2026-10-18 authority=diffemu */
undefined4 * __cdecl course_list_scan(int use_backslash,char *dir_name,char *sub_name,char *ext_name,char *unused_5, char *unused_6,char *unused_7)
{
  char ch;
  undefined2 tracks_tail;
  undefined4 toast_tail;
  int list_off;
  char *cursor;
  uint len;
  uint words;
  char *piece;
  char *src;
  undefined2 *tail_ptr;
  char *dst;
  undefined1 *tail_byte;

  *(undefined1 *)&g_course_piz_path = 0;
  /* separator: "/" when use_backslash is zero, else "\\" and copy g_course_list_5de648 into the output path */
  if (use_backslash == 0) {
    piece = "/";
  }
  else {
    piece = "\\";
    list_off = 0;
    do {
      cursor = &g_course_list_5de648 + list_off;
      *(char *)((char *)&g_course_piz_path + list_off) = *cursor;
      list_off = list_off + 1;
    } while (*cursor != '\0');
    /* walk to the end of the scan buffer, then store the 2-byte value g_course_5dd5a0 (inferred: backslash separator) */
    tail_ptr = (undefined2 *)((char *)&g_ui_2d_8c87bc + SCAN_BUF_OFS);
    do {
      cursor = (char *)((char *)tail_ptr + 1);
      tail_ptr = (undefined2 *)((char *)tail_ptr + 1);
    } while (*cursor != '\0');
    *tail_ptr = g_course_5dd5a0;
  }
  /* append "ToastArt" (with its NUL) after the separator, then the byte at offset 9 */
  ch = s_ToastArt_005de63c[TOAST_ART_STR_LEN];
  toast_tail = (*(undefined4 *)((char *)&s_ToastArt_005de63c + 4));
  cursor = (char *)((char *)&g_ui_2d_8c87bc + SCAN_BUF_OFS);
  do {
    dst = cursor;
    cursor = dst + 1;
  } while (dst[1] != '\0');
  *(undefined4 *)(dst + 1) = (*(undefined4 *)&s_ToastArt_005de63c);
  *(undefined4 *)(dst + 5) = toast_tail;
  dst[9] = ch;
  cursor = piece;
  do {
    ch = *cursor;
    cursor = cursor + 1;
  } while (ch != '\0');
  dst = (char *)((char *)&g_ui_2d_8c87bc + SCAN_BUF_OFS);
  do {
    src = dst + 1;
    dst = dst + 1;
  } while (*src != '\0');
  src = piece;
  /* copy the separator: whole words first, then the leftover bytes */
  for (len = (uint)((cursor - piece)) >> 2; len != 0; len = len - 1) {
    *(undefined4 *)dst = *(undefined4 *)src;
    src = src + 4;
    dst = dst + 4;
  }
  for (len = (cursor - piece) & 3; len != 0; len = len - 1) {
    *dst = *src;
    src = src + 1;
    dst = dst + 1;
  }
  /* append the one-byte value g_game_5dd5a2 (inferred: a separator or terminator char) */
  tail_byte = (undefined1 *)((char *)&g_ui_2d_8c87bc + SCAN_BUF_OFS);
  do {
    cursor = tail_byte + 1;
    tail_byte = tail_byte + 1;
  } while (*cursor != '\0');
  *tail_byte = g_game_5dd5a2;
  cursor = piece;
  do {
    ch = *cursor;
    cursor = cursor + 1;
  } while (ch != '\0');
  dst = (char *)((char *)&g_ui_2d_8c87bc + SCAN_BUF_OFS);
  do {
    src = dst + 1;
    dst = dst + 1;
  } while (*src != '\0');
  src = piece;
  for (len = (uint)((cursor - piece)) >> 2; len != 0; len = len - 1) {
    *(undefined4 *)dst = *(undefined4 *)src;
    src = src + 4;
    dst = dst + 4;
  }
  for (len = (cursor - piece) & 3; ch = s_tracks_005de634[TRACKS_STR_LEN],
      tracks_tail = (*(ushort *)((char *)&s_tracks_005de634 + 4)), len != 0; len = len - 1) {
    *dst = *src;
    src = src + 1;
    dst = dst + 1;
  }
  /* append "tracks" (TRACKS_STR_LEN bytes plus NUL) after the separator */
  cursor = (char *)((char *)&g_ui_2d_8c87bc + SCAN_BUF_OFS);
  do {
    dst = cursor;
    cursor = dst + 1;
  } while (dst[1] != '\0');
  *(undefined4 *)(dst + 1) = (*(undefined4 *)&s_tracks_005de634);
  *(undefined2 *)(dst + 5) = tracks_tail;
  dst[7] = ch;
  cursor = piece;
  do {
    ch = *cursor;
    cursor = cursor + 1;
  } while (ch != '\0');
  dst = (char *)((char *)&g_ui_2d_8c87bc + SCAN_BUF_OFS);
  do {
    src = dst + 1;
    dst = dst + 1;
  } while (*src != '\0');
  src = piece;
  for (len = (uint)((cursor - piece)) >> 2; len != 0; len = len - 1) {
    *(undefined4 *)dst = *(undefined4 *)src;
    src = src + 4;
    dst = dst + 4;
  }
  /* append the separator, then dir_name */
  for (len = (cursor - piece) & 3; cursor = dir_name, len != 0; len = len - 1) {
    *dst = *src;
    src = src + 1;
    dst = dst + 1;
  }
  do {
    ch = *cursor;
    cursor = cursor + 1;
  } while (ch != '\0');
  len = (cursor - dir_name);
  cursor = (char *)((char *)&g_ui_2d_8c87bc + SCAN_BUF_OFS);
  do {
    dst = cursor + 1;
    cursor = cursor + 1;
  } while (*dst != '\0');
  for (words = len >> 2; words != 0; words = words - 1) {
    *(undefined4 *)cursor = *(undefined4 *)dir_name;
    dir_name = dir_name + 4;
    cursor = cursor + 4;
  }
  for (len = len & 3; len != 0; len = len - 1) {
    *cursor = *dir_name;
    dir_name = dir_name + 1;
    cursor = cursor + 1;
  }
  /* sub_name: separator + sub_name, only when sub_name is non-empty */
  cursor = sub_name;
  dst = piece;
  if (*sub_name != '\0') {
    do {
      ch = *dst;
      dst = dst + 1;
    } while (ch != '\0');
    len = (dst - piece);
    dst = (char *)((char *)&g_ui_2d_8c87bc + SCAN_BUF_OFS);
    do {
      src = dst + 1;
      dst = dst + 1;
    } while (*src != '\0');
    for (words = len >> 2; words != 0; words = words - 1) {
      *(undefined4 *)dst = *(undefined4 *)piece;
      piece = piece + 4;
      dst = dst + 4;
    }
    for (len = len & 3; len != 0; len = len - 1) {
      *dst = *piece;
      piece = piece + 1;
      dst = dst + 1;
    }
  }
  do {
    ch = *cursor;
    cursor = cursor + 1;
  } while (ch != '\0');
  len = (cursor - sub_name);
  piece = (char *)((char *)&g_ui_2d_8c87bc + SCAN_BUF_OFS);
  do {
    src = piece + 1;
    piece = piece + 1;
  } while (*src != '\0');
  for (words = len >> 2; words != 0; words = words - 1) {
    *(undefined4 *)piece = *(undefined4 *)sub_name;
    sub_name = sub_name + 4;
    piece = piece + 4;
  }
  for (len = len & 3; len != 0; len = len - 1) {
    *piece = *sub_name;
    sub_name = sub_name + 1;
    piece = piece + 1;
  }
  /* ext_name: appended with no separator, only when ext_name is non-empty */
  piece = ext_name;
  if (*ext_name != '\0') {
    do {
      ch = *piece;
      piece = piece + 1;
    } while (ch != '\0');
    len = (piece - ext_name);
    piece = (char *)((char *)&g_ui_2d_8c87bc + SCAN_BUF_OFS);
    do {
      src = piece + 1;
      piece = piece + 1;
    } while (*src != '\0');
    for (words = len >> 2; words != 0; words = words - 1) {
      *(undefined4 *)piece = *(undefined4 *)ext_name;
      ext_name = ext_name + 4;
      piece = piece + 4;
    }
    for (len = len & 3; len != 0; len = len - 1) {
      *piece = *ext_name;
      ext_name = ext_name + 1;
      piece = piece + 1;
    }
  }
  return &g_course_piz_path;
}
