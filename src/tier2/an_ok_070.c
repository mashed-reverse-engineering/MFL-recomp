#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall DispatchMessageA(int);
extern __declspec(dllimport) int __stdcall PeekMessageA(int, int, int, int, int);
extern __declspec(dllimport) int __stdcall ShowWindow(int, int);
extern __declspec(dllimport) int __stdcall TranslateMessage(int);
extern __declspec(dllimport) int __stdcall UpdateWindow(int);
extern __declspec(dllimport) int __stdcall WaitMessage(void);
/*00481850*/ extern undefined __cdecl atomic_plugin_copy(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5, undefined4 p6, undefined4 p7, undefined4 p8);
/*004a5c20*/ extern int object_frames_create();
/*004a6ae0*/ extern void _rxPipelineDestroy_004deff0(struct RxPipeline *p0);
/*004b2a60*/ extern int sprintf();
/*004b3bca*/ extern void __cdecl _free_004b45fd(void *p0);
/*004b4ba7*/ extern int entry();
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d4490*/ extern RwMatrix * RwMatrixInvert(RwMatrix *p0, RwMatrix *p1);
/*004d77e0*/ extern RwBool RwRasterDestroy(struct RwRaster *p0);
/*004d7950*/ extern struct RwRaster * RwRasterCreate(RwInt32 p0, RwInt32 p1, RwInt32 p2, RwInt32 p3);
/*004dbc40*/ extern void __cdecl RwD3D9DeletePixelShader(void *p0);
/*004def80*/ extern struct RxPipeline * RxPipelineCreate();
/*004e64f0*/ extern struct RxPipeline * RxLockedPipeUnlock(RxLockedPipe *p0);
/*004e6f40*/ extern RxLockedPipe * RxPipelineLock(struct RxPipeline *p0);
/*004e7100*/ extern RxLockedPipe * RxLockedPipeAddFragment(RxLockedPipe *p0, RwUInt32 *p1, struct RxNodeDefinition *p2);
/*004f68f0*/ extern struct RpAtomic * RpAtomicCreate();
/*004f69e0*/ extern struct RpAtomic * RpAtomicSetGeometry(struct RpAtomic *p0, struct RpGeometry *p1, RwUInt32 p2);
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*0062c5c8*/ extern undefined4 g_app_62c5c8;
/*0062c5cc*/ extern undefined4 g_app_62c5cc;
/*0064d894*/ extern undefined4 g_course_64d894;
/*0076e898*/ extern undefined4 g_d3dx_76e898;
/*0076ece8*/ extern char *g_video_select_76ece8;
/*00770fe8*/ extern undefined4 g_app_770fe8;
/*00771030*/ extern undefined4 g_app_771030;
/*00771034*/ extern undefined4 g_app_771034;
/*00771038*/ extern undefined4 g_app_771038;
/*00771048*/ extern undefined4 g_app_771048;
/*00771054*/ extern undefined4 g_app_771054;
/*00771058*/ extern undefined4 g_app_771058;
/*007711d4*/ extern undefined g_controller_cfg_7711d4;
/*007712e8*/ extern char *g_video_mode_counts;
/*007712ec*/ extern undefined4 g_video_7712ec;
/*007712f4*/ extern undefined4 g_app_7712f4;
/*007714f8*/ extern char *g_video_mode_lists;
/*007714fc*/ extern undefined4 g_video_subsystem_index;
/*00771500*/ extern int g_app_771500;
/*00771504*/ extern undefined4 g_app_771504;
/*00771908*/ extern undefined g_app_771908;
/*00771a10*/ extern float g_video_get_771a10;
/*00771a14*/ extern float g_video_get_771a14;
/*00771a18*/ extern undefined4 g_pump_windows_771a18;
/*00771a1c*/ extern undefined4 g_pump_windows_771a1c;
/*00771a24*/ extern undefined4 g_video_get_771a24;
/*00771a28*/ extern undefined4 g_video_get_771a28;
/*00771aa0*/ extern undefined4 g_data_771aa0;
/*00771aa4*/ extern undefined4 g_data_771aa4;
/*00771aa8*/ extern undefined4 g_data_771aa8;
/*00771aac*/ extern undefined4 g_data_771aac;
/*007e7700*/ extern undefined4 g_app_7e7700;
/*007e7704*/ extern undefined4 g_app_7e7704;
/*007e7720*/ extern undefined g_pump_windows_7e7720;
/*007e7724*/ extern undefined4 g_pump_windows_7e7724;
/*007e7740*/ extern struct ControllerCfg g_controller_cfg[4];
/* joypad_get_field_770fe8 */

/* "(index): g_app_770fe8[index]" */

extern undefined4 __cdecl joypad_get_field_770fe8(char *param_1);


/* d3d9_pipeline_destroy_default */

/* "_rxPipelineDestroy(g_app_771030)" */

extern undefined4 d3d9_pipeline_destroy_default(void);


/* object_frames_create_forward */

/* "calls object_frames_create" */

/* Forwards (base, count) unchanged to object_frames_create and returns nothing.
   base: first object/frame argument (inferred), count: number of frames (inferred) */

/* TIER2: validated 2026-10-18 authority=diffemu */
void object_frames_create_forward(undefined4 base,undefined4 count)
{
  object_frames_create(base,count);
  return;
}


/* object_frame_set_transform */

/* "(object, index, matrix): RwFrameTransform replace" */

extern void __cdecl object_frame_set_transform(char *param_1,char *param_2,RwMatrix *param_3);


/* object_frames_create_with_matrices */

/* "(base, count, matrices): object_frames_create and sets the transform of every frame" */

extern undefined4 object_frames_create_with_matrices(char *param_1,char *param_2,char *param_3);
#undef uVar1
#undef iVar2


/* object_atomic_create */

/* "(object, index): creates an atomic on frame index sharing the geometry of the object" */

extern RpAtomic * object_atomic_create(char *param_1,char *param_2);


/* video_raster_create */

/* "g_app_771038 = RwRasterCreate flag raster" */

extern undefined4 __cdecl video_raster_create(char *param_1);


/* video_raster_destroy */

/* "destroys g_app_771038" */

extern undefined4 video_raster_destroy(void);


/* d3d9_pixel_shader_destroy */

/* "RwD3D9DeletePixelShader(g_data_771aa0)" */

extern void d3d9_pixel_shader_destroy(void);


#define VIDEO_MODE_WIDTH_OFF (0xc4)
#define VIDEO_MODE_HEIGHT_OFF (0xcc)
#define LOW_16_MASK (0xffff)
#define VIDEO_DIM_MIN_EXCL (0x101)

/* video_set_flag_771058_by_mode */

/* "sets g_app_771058 when both mode dimensions exceed 256 and not g_course_64d894" */

/* Sets g_app_771058 to 1 only when g_course_64d894 is 0 and the selected video mode is wider and taller
   than 256 (width at +0xc4, height at +0xcc of g_video_select_76ece8; offsets inferred).
   Otherwise clears it to 0. */

/* TIER2: validated 2026-10-18 authority=diffemu */
void video_set_flag_771058_by_mode(void)
{
  /* any failed test clears the flag; the height test is the one that sets it to 1 first */
  if (((g_course_64d894 != 0) || ((*(uint *)(g_video_select_76ece8 + VIDEO_MODE_WIDTH_OFF) & LOW_16_MASK) < VIDEO_DIM_MIN_EXCL)) ||
     (g_app_771058 = 1, (*(uint *)(g_video_select_76ece8 + VIDEO_MODE_HEIGHT_OFF) & LOW_16_MASK) < VIDEO_DIM_MIN_EXCL)) {
    g_app_771058 = 0;
  }
  return;
}


/* d3d9_pipelines_destroy */

/* "destroys g_app_771054 and g_app_771048" */

extern void d3d9_pipelines_destroy(void);


/* app_get_app_771058 */

/* "return g_app_771058;" */

extern undefined4 app_get_app_771058(void);


/* d3d9_set_shader_flag */

/* "g_app_771058 = param" */

extern void d3d9_set_shader_flag(char *param_1);


/* rx_pipeline_create_c */

/* "RxPipelineCreate + lock + nodes (variant c)" */

extern RxPipeline * __cdecl rx_pipeline_create_c(RxNodeDefinition *nodeDef0);


/* camera_view_matrix_from_frame */

/* "inverts the frame LTM and writes the 4x4 view matrix (x/y/z axis negated)" */

extern void camera_view_matrix_from_frame(float *param_1,char *param_2);


/* matrix_transpose_row_copy */

/* "copies the 4 elements of a matrix column into a row vector" */

extern void matrix_transpose_row_copy(undefined4 *param_1,undefined4 *param_2);


/* camera_view_params_pack */

/* "packs the +0x70/+0x74 fields of a camera into an 8 dword structure (zeroing the rest)" */

extern void __cdecl camera_view_params_pack (undefined4 *param_1,char *param_2,char *param_3,char *param_4,char *param_5, char *param_6);


/* controller_cfg_path */

/* formats "contcfg%d.bin" */

extern undefined * controller_cfg_path(void);


/* controller_present_004a70c0 */

/* duplicate name: was controller_present (first address 004a5b20) */

extern bool __cdecl controller_present_004a70c0(int controller_index);


/* video_dialog_free */

/* "frees the per subsystem mode tables" */

extern void video_dialog_free(void);


/* app_get_app_62c5c8 */

/* "return g_app_62c5c8;" */

extern undefined4 app_get_app_62c5c8(void);


/* app_get_app_62c5cc */

/* "return g_app_62c5cc;" */

extern undefined4 app_get_app_62c5cc(void);


/* app_get_app_771504 */

/* "return g_app_771504;" */

extern undefined4 app_get_app_771504(void);


/* app_get_app_7712f4 */

/* "return g_app_7712f4;" */

extern undefined4 app_get_app_7712f4(void);


#define VIDEO_MODE_MIN_WIDTH (0x280)

/* video_mode_find_first_640x480_plus */

/* "searches the mode list for the first entry >= 640x480 with the flag set" */

/* Returns the index of the first video mode in the current subsystem's list whose
   width (entry[0]) is at least VIDEO_MODE_MIN_WIDTH (640) and whose flag (entry[3])
   is non-zero. Entries are 6 dwords (24 bytes) each. Returns 0 if nothing matches,
   which is also the result when the first entry matches (inferred: callers treat 0
   as "none"). The 0x1e0 (480) test on the width is redundant with the 640 test. */

/* TIER2: unvalidated 2026-10-17 authority=diffemu */
int video_mode_find_first_640x480_plus(void)
{
  int mode_count;
  int *mode_entry;
  int mode_index;

  mode_count = *(int *)(g_video_mode_counts + g_video_subsystem_index * 4);
  mode_index = 0;
  if (0 < mode_count) {
    mode_entry = *(int **)(g_video_mode_lists + g_video_subsystem_index * 4);
    mode_index = 0;
    /* scan entries until one has the flag set and width >= 640 */
    while (((mode_entry[3] == 0 || (*mode_entry < VIDEO_MODE_MIN_WIDTH)) || (*mode_entry < 0x1e0))) {
      mode_index = mode_index + 1;
      mode_entry = mode_entry + 6;
      if (mode_count <= mode_index) {
        return 0;
      }
    }
  }
  return mode_index;
}


/* pump_windows_messages */

/* PeekMessage loop; returns true on WM_QUIT (0x12) */

extern bool pump_windows_messages(void);


/* window_show */

/* "ShowWindow + UpdateWindow(g_app_7e7704)" */

extern void window_show(char *param_1);


/* app_get_app_7e7704 */

/* "return g_app_7e7704;" */

extern undefined4 app_get_app_7e7704(void);


/* app_get_app_7e7700 */

/* "return g_app_7e7700;" */

extern undefined4 app_get_app_7e7700(void);


/* app_get_71908 */

/* "returns &g_app_771908 (command line / path buffer)" */

extern undefined * app_get_71908(void);


/* video_get_window_size_a */

/* "(out w, out h) from g_app_771a10 / 771a14" */

extern void video_get_window_size_a(undefined4 *param_1);


/* video_get_window_size_b */

/* "(out w, out h) from g_app_771a24 / 771a28" */

extern void video_get_window_size_b(undefined4 *param_1,undefined4 *param_2);



