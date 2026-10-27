#include "mfl_tier2.h"
extern char LAB_004a2b30[];
extern char LAB_004a2b40[];
/*00405e40*/ extern int stub_return_zero();
/*004a2a30*/ extern int rw_register_image_formats();
/*004a2a70*/ extern int rw_plugins_attach();
/*004a4320*/ extern int filesystem_register_native();
/*004a4330*/ extern int video_select_and_set_mode();
/*004a43c0*/ extern int video_get_device_pointer();
/*004a5530*/ extern undefined __cdecl log_print(char *p0);
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0, ...);
/*004d3060*/ extern struct RwVideoMode * RwEngineGetVideoModeInfo(struct RwVideoMode *p0, RwInt32 p1);
/*004d3090*/ extern RwInt32 __cdecl RwEngineGetCurrentVideoMode(void);
/*004d3140*/ extern RwBool __cdecl RwEngineStart(void);
/*004d31d0*/ extern RwBool __cdecl RwEngineClose(void);
/*004d3240*/ extern RwBool RwEngineOpen(struct RwEngineOpenParams *p0);
/*004d3400*/ extern RwBool RwEngineTerm();
/*004d3440*/ extern RwBool __cdecl RwEngineInit(struct RwMemoryFunctions *p0, RwUInt32 p1, RwUInt32 p2);
/*004d5e10*/ extern struct RwTexDictionary * RwTexDictionarySetCurrent(struct RwTexDictionary *p0);
/*004da040*/ extern void RwD3D9EngineSetRefreshRate(RwUInt32 p0);
/*004da0e0*/ extern void FUN_004da0e0(undefined4 p0);
/*004dbda0*/ extern void FUN_004dbda0(void *p0, void *p1, undefined4 p2, void *p3, undefined4 p4, undefined4 p5, undefined4 p6, void *p7, undefined4 p8, undefined4 p9, undefined4 p10, void *p11, undefined4 p12);
/*004dbdb0*/ extern int FUN_004dbdb0(void);
/*004dbdc0*/ extern void FUN_004dbdc0();
/*004dbdd0*/ extern int FUN_004dbdd0(void);
/*0050b5e0*/ extern int nop_stub();
/*005609c0*/ extern void FUN_005609c0(void);
/*00568a90*/ extern int RtCharsetOpen();
/*0076e864*/ extern undefined4 g_rw_engine_76e864;
/*0076e868*/ extern undefined4 g_rw_engine_76e868;

#define DEFAULT_RES_ARENA_SIZE (0x200000)
#define VIDEO_MEMORY_BYTES (0x400000)
#define BYTES_PER_PIXEL (10)
#define REFRESH_RATE_HZ (60)

/* rw_engine_init */

/* "'Calling RwEngineInit', RtFSManager, plugins" */

/*
 * Brings up the RenderWare engine: core init, file system, plugins, display open,
 * video mode set, engine start. Then logs the active video mode and the free video memory.
 * unused_arg is never read (the caller's argument is ignored).
 * Returns 1 when the engine started, 0 on any failure. Failure paths after a successful
 * RwEngineInit shut the engine down again before returning.
 */
undefined4 __cdecl rw_engine_init(char *unused_arg)

{
  RwMemoryFunctions *memFuncs;
  RwUInt32 initFlags;
  RwBool ok;
  int status;
  RwInt32 mode_raw_or_width;
  int frame_buffer_bytes;
  int free_video_bytes;
  int current_mode;
  undefined1 *lab_2b30_ptr;
  undefined4 zero_arg;
  undefined4 video_mem_bytes;
  char *mode_format;
  RwInt32 mode_height;
  RwVideoMode *mode_info_ptr;
  RwUInt32 resArenaSize;
  char *msg;
  RwEngineOpenParams open_params;
  undefined1 stub_buf [16];
  RwVideoMode mode_info;

  /* Step 1: core RenderWare init with the default resource arena. */
  stub_return_zero(stub_buf);
  memFuncs = (RwMemoryFunctions *)stub_return_zero(stub_buf);
  log_print("Calling RwEngineInit\n");
  resArenaSize = DEFAULT_RES_ARENA_SIZE;
  initFlags = stub_return_zero();
  ok = RwEngineInit(memFuncs,initFlags,resArenaSize);
  if (ok != 0) {
    /* Step 2: file system manager, native file system, then plugins. */
    msg = "Calling RtFSManagerOpen\n";
    log_print("Calling RtFSManagerOpen\n");
    status = RtFSManagerOpen(0xffffffff,msg);
    if (status != 0) {
      log_print("Calling HardwareInstallFileSystem\n");
      filesystem_register_native();
      video_get_device_pointer(&open_params);
      log_print("Calling RenderwareAttachPlugins\n");
      rw_plugins_attach();
      /* Step 3: open the engine on the display device; on failure shut down and return 0. */
      log_print("Calling RwEngineOpen\n");
      ok = RwEngineOpen(&open_params);
      if (ok == 0) {
        FUN_005609c0();
        RwEngineTerm();
        return 0;
      }
      /* Step 4: pick and set the video mode; on failure close the engine and return 0. */
      log_print("Calling HardwareSetVideoMode\n");
      status = video_select_and_set_mode();
      if (status != 0) {
        /* Step 5: 60 Hz refresh rate, then start the engine. */
        FUN_004da0e0(1);
        RwD3D9EngineSetRefreshRate(REFRESH_RATE_HZ);
        log_print("Calling RwEngineStart\n");
        ok = RwEngineStart();
        if (ok != 0) {
          nop_stub();
          nop_stub();
          /* Current mode index is the low 16 bits of the returned word. */
          mode_raw_or_width = RwEngineGetCurrentVideoMode();
          current_mode = (int)(short)mode_raw_or_width;
          mode_info_ptr = &mode_info;
          /* status is reused here: its last value is passed on to FUN_004dbda0 below. */
          status = current_mode;
          RwEngineGetVideoModeInfo(mode_info_ptr,current_mode);
          mode_format = "\tUsing video mode %u : %ux%u\n";
          mode_raw_or_width = mode_info.width;
          mode_height = mode_info.height;
          FID_conflict__wprintf("\tUsing video mode %u : %ux%u\n");
          /* Free video memory = total minus width*height*10 bytes (per the log message).
             frame_buffer_bytes = width*height*12 (inferred). */
          free_video_bytes = mode_info.width * mode_info.height * -BYTES_PER_PIXEL + VIDEO_MEMORY_BYTES;
          frame_buffer_bytes = mode_info.width * mode_info.height * 0xc;
          video_mem_bytes = VIDEO_MEMORY_BYTES;
          msg =
          "At 10 bytes per pixel (16bit Z + 2*32bit FB) there is %d - %d = %d bytes of video memory free\n"
          ;
          FID_conflict__wprintf
                    (
                    "At 10 bytes per pixel (16bit Z + 2*32bit FB) there is %d - %d = %d bytes of video memory free\n"
                    );
          /* Post-start setup: image formats, charset, no current texture dictionary, then the
             hand-off of the mode info and memory figures to FUN_004dbda0 (inferred). */
          rw_register_image_formats();
          RtCharsetOpen();
          zero_arg = 0;
          RwTexDictionarySetCurrent((RwTexDictionary *)0x0);
          g_rw_engine_76e864 = FUN_004dbdd0();
          lab_2b30_ptr = &LAB_004a2b30;
          FUN_004dbdc0(0x4a2b30);
          g_rw_engine_76e868 = FUN_004dbdb0();
          FUN_004dbda0(&LAB_004a2b40,lab_2b30_ptr,zero_arg,msg,video_mem_bytes,frame_buffer_bytes,free_video_bytes,mode_format,current_mode,mode_raw_or_width,
                       mode_height,mode_info_ptr,status);
          return 1;
        }
      }
      FUN_005609c0();
      RwEngineClose();
      RwEngineTerm();
    }
  }
  return 0;
}


