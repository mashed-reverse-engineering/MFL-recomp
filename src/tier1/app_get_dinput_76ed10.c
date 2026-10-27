#include "mfl_tier2.h"
/*0076ed10*/ extern undefined4 g_dinput_76ed10;
/*00481850*/ extern undefined __cdecl atomic_plugin_copy(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5, undefined4 p6, undefined4 p7, undefined4 p8);
/*004a46d0*/ extern int dinput_release_object();
/*004a4720*/ extern int game_register_native_filesystems();
/*004a5190*/ extern int joypads_search();
/*004a5470*/ extern int __cdecl dinput_keyboard_create(undefined4 p0);
/*004a5c20*/ extern int object_frames_create();
/*004a6ae0*/ extern void _rxPipelineDestroy_004deff0(struct RxPipeline *p0);
/*004a6b30*/ extern int object_set_field_6c();
/*004a8420*/ extern int debug_printf();
/*004ab220*/ extern int DirectInput8Create();
/*004b4244*/ extern int __cdecl _fputs(char *p0, FILE *p1);
/*004b4358*/ extern int __cdecl _fclose(FILE *p0);
/*004b4ba7*/ extern int entry();
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
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
/*005e12e4*/ extern undefined g_dinput_release_5e12e4;
/*005e1f6c*/ extern undefined g_dinput_create_5e1f6c;
/*0064d894*/ extern undefined4 g_course_64d894;
/*0076e898*/ extern undefined4 g_d3dx_76e898;
/*0076ece8*/ extern undefined4 g_video_select_76ece8;
/*0076ed10*/ extern undefined4 g_dinput_76ed10;
/*0076ed18*/ extern undefined4 g_app_76ed18;
/*0076ed20*/ extern undefined4 g_dinput_76ed20;
/*0076ed4c*/ extern undefined g_controller_dialog_76ed4c;
/*0076efe8*/ extern undefined g_joypad_get_76efe8;
/*0076efec*/ extern undefined g_joypad_get_76efec;
/*0076f068*/ extern undefined g_joypad_get_76f068;
/*0076f06c*/ extern undefined g_joypad_get_76f06c;
/*0076f128*/ extern undefined g_joypad_get_76f128;
/*00770f60*/ extern float g_app_770f60;
/*00770f64*/ extern undefined4 g_app_770f64;
/*00770f68*/ extern undefined4 g_app_770f68;
/*00770f6c*/ extern undefined4 g_dinput_keyboard_770f6c;
/*00770f74*/ extern undefined4 g_log_770f74;
/*00770fb4*/ extern undefined4 g_app_770fb4;
/*00770fe8*/ extern undefined4 g_app_770fe8;
/*00771030*/ extern undefined4 g_app_771030;
/*00771034*/ extern undefined4 g_app_771034;
/*00771038*/ extern undefined4 g_app_771038;
/*00771048*/ extern undefined4 g_app_771048;
/*00771054*/ extern undefined4 g_app_771054;
/*00771058*/ extern undefined4 g_app_771058;
/*00771aa0*/ extern undefined4 g_data_771aa0;
/*00771aa4*/ extern undefined4 g_data_771aa4;
/*00771aa8*/ extern undefined4 g_data_771aa8;
/*00771aac*/ extern undefined4 g_data_771aac;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007e7740*/ extern struct ControllerCfg g_controller_cfg[4];
/* app_get_dinput_76ed10 */

/* "return g_dinput_76ed10;" */

undefined4 app_get_dinput_76ed10(void)
{
  return g_dinput_76ed10;
}
