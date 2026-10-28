#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall DispatchMessageA(int);
extern __declspec(dllimport) int __stdcall PeekMessageA(int, int, int, int, int);
extern __declspec(dllimport) int __stdcall TranslateMessage(int);
extern __declspec(dllimport) int __stdcall WaitMessage(void);
extern char s_videocfg_bin_005e1520[];
/*00481790*/ extern undefined __cdecl atomic_plugin_set_radius_squared(undefined4 p0, undefined4 p1);
/*00481850*/ extern undefined __cdecl atomic_plugin_copy(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5, undefined4 p6, undefined4 p7, undefined4 p8);
/*004a5530*/ extern undefined __cdecl log_print(char *p0);
/*004a5550*/ extern int log_printf();
/*004a6ae0*/ extern void _rxPipelineDestroy_004deff0(struct RxPipeline *p0);
/*004a6b30*/ extern int object_set_field_6c();
/*004a8670*/ extern undefined video_mode_name_format_004a8670(undefined4 p0, undefined4 p1);
/*004aa9e0*/ extern int FUN_004aa9e0(undefined4 p0);
/*004b2a60*/ extern int sprintf();
/*004b3bca*/ extern void __cdecl _free_004b45fd(void *p0);
/*004b4358*/ extern int __cdecl _fclose(FILE *p0);
/*004b4531*/ extern int crt_fopen_body(undefined4 p0, undefined4 p1);
/*004b49bf*/ extern size_t __cdecl _fread(void *p0, size_t p1, size_t p2, FILE *p3);
/*004b4b12*/ extern size_t __cdecl _fwrite(void *p0, size_t p1, size_t p2, FILE *p3);
/*004b4ba7*/ extern int entry();
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d16e0*/ extern struct RwFrame * RwFrameSetIdentity(struct RwFrame *p0);
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
/*005003b0*/ extern int FUN_005003b0(void);
/*005009c0*/ extern int FUN_005009c0(void);
/*005e0450*/ extern undefined g_piz_5e0450;
/*005e14cc*/ extern undefined g_controller_5e14cc;
/*0062c5c8*/ extern undefined4 g_app_62c5c8;
/*0062c5cc*/ extern undefined4 g_app_62c5cc;
/*0064d894*/ extern undefined4 g_course_64d894;
/*0076e898*/ extern undefined4 g_d3dx_76e898;
/*0076ece8*/ extern undefined4 g_video_select_76ece8;
/*00771030*/ extern undefined4 g_app_771030;
/*00771034*/ extern undefined4 g_app_771034;
/*00771038*/ extern undefined4 g_app_771038;
/*0077103c*/ extern undefined4 g_d3d9_shaders_77103c;
/*00771048*/ extern undefined4 g_app_771048;
/*00771054*/ extern undefined4 g_app_771054;
/*00771058*/ extern undefined4 g_app_771058;
/*007711d4*/ extern undefined g_controller_cfg_7711d4;
/*007712d8*/ extern undefined4 g_video_cfg_7712d8;
/*007712dc*/ extern undefined4 g_video_cfg_7712dc;
/*007712e0*/ extern undefined4 g_video_cfg_7712e0;
/*007712e4*/ extern undefined g_video_cfg_7712e4;
/*007712e8*/ extern undefined4 g_video_mode_counts;
/*007712ec*/ extern undefined4 g_video_7712ec;
/*007712f0*/ extern undefined4 g_video_7712f0;
/*007712f4*/ extern undefined4 g_app_7712f4;
/*007712f8*/ extern struct VideoCfg g_video_cfg;
/*007714f8*/ extern undefined4 g_video_mode_lists;
/*007714fc*/ extern undefined4 g_video_subsystem_index;
/*00771500*/ extern int g_app_771500;
/*00771504*/ extern undefined4 g_app_771504;
/*00771a18*/ extern undefined4 g_pump_windows_771a18;
/*00771a1c*/ extern undefined4 g_pump_windows_771a1c;
/*00771aa0*/ extern undefined4 g_data_771aa0;
/*00771aa4*/ extern undefined4 g_data_771aa4;
/*00771aa8*/ extern undefined4 g_data_771aa8;
/*00771aac*/ extern undefined4 g_data_771aac;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007e7720*/ extern undefined g_pump_windows_7e7720;
/*007e7724*/ extern undefined4 g_pump_windows_7e7724;
/*007e7740*/ extern struct ControllerCfg g_controller_cfg[4];

/* offsets inside the "videocfg.bin" block (inferred: second word, third word, the byte after them) */
#define VIDEO_CFG_OFS_WORD1 (4)
#define VIDEO_CFG_OFS_WORD2 (8)
#define VIDEO_CFG_OFS_BYTE (0xc)

/* video_cfg_path */

/* Copies fields of the "videocfg.bin" block into the g_video_cfg_7712d8..7712e4 globals
   (inferred: words at offsets 0, 4, 8 and a byte at offset 12). Returns &g_video_cfg_7712d8. */

/* TIER2: validated 2026-10-18 authority=diffemu */
undefined * video_cfg_path(void)
{
  g_video_cfg_7712d8 = (*(uint *)((char *)&s_videocfg_bin_005e1520 + 0));
  g_video_cfg_7712e4 = s_videocfg_bin_005e1520[VIDEO_CFG_OFS_BYTE];
  g_video_cfg_7712dc = (*(uint *)((char *)&s_videocfg_bin_005e1520 + VIDEO_CFG_OFS_WORD1));
  g_video_cfg_7712e0 = (*(uint *)((char *)&s_videocfg_bin_005e1520 + VIDEO_CFG_OFS_WORD2));
  return &g_video_cfg_7712d8;
}
