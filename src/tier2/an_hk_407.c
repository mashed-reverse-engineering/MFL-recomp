#include "mfl_tier2.h"
extern char LAB_004818c0[];
extern char LAB_004819a0[];
/*00481810*/ extern int atomic_plugin_construct();
/*00481850*/ extern undefined __cdecl atomic_plugin_copy(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5, undefined4 p6, undefined4 p7, undefined4 p8);
/*004818d0*/ extern undefined __cdecl atomic_plugin_stream_read(struct RwStream *p0, undefined4 p1, int p2);
/*00481940*/ extern undefined __cdecl atomic_plugin_stream_write(struct RwStream *p0, undefined4 p1, int p2);
/*004d2f20*/ extern RwInt32 RwEngineRegisterPlugin(RwInt32 p0, RwUInt32 p1, RwPluginObjectConstructor p2, RwPluginObjectDestructor p3);
/*004f7e80*/ extern RwInt32 RpAtomicRegisterPlugin(RwInt32 p0, RwUInt32 p1, RwPluginObjectConstructor p2, RwPluginObjectDestructor p3, RwPluginObjectCopy p4);
/*004f7ee0*/ extern RwInt32 RpAtomicRegisterPluginStream(RwUInt32 p0, RwPluginDataChunkReadCallBack p1, RwPluginDataChunkWriteCallBack p2, RwPluginDataChunkGetSizeCallBack p3);
/*004f7f10*/ extern void FUN_004f7f10(undefined4 p0, undefined4 p1);
/*0053eac0*/ extern int FUN_0053eac0();
/*0089de0c*/ extern undefined4 g_gfx_89de0c;

#define ATOMIC_PLUGIN_ID (0x26990004)
#define ATOMIC_PLUGIN_DATA_SIZE (0x2c)

/* atomic_plugin_register */

/* "RwEngineRegisterPlugin / RpAtomicRegisterPlugin 0x26990004" */

/* Registers the atomic plugin with the RenderWare engine: the engine plugin, the atomic
   extension (construct/destruct/copy) and its stream chunk read/write/size handlers.
   No parameters. Returns 1 on success, 0 if any registration failed. (inferred) */
undefined4 atomic_plugin_register(void)

{
  RwInt32 register_result;

  register_result = RwEngineRegisterPlugin(ATOMIC_PLUGIN_DATA_SIZE,ATOMIC_PLUGIN_ID,FUN_0053eac0,FUN_0053eac0);
  /* Engine plugin registered (negative result = failure): register the atomic extension */
  if (register_result < 0) {
    return 0;
  }
  g_gfx_89de0c = RpAtomicRegisterPlugin
                           (ATOMIC_PLUGIN_DATA_SIZE,ATOMIC_PLUGIN_ID,atomic_plugin_construct,FUN_0053eac0,
                            atomic_plugin_copy);
  /* Extension registered (g_gfx_89de0c holds its plugin id): register the stream chunk handlers */
  if ((int)g_gfx_89de0c < 0) {
    return 0;
  }
  register_result = RpAtomicRegisterPluginStream
                    (ATOMIC_PLUGIN_ID,atomic_plugin_stream_read,atomic_plugin_stream_write,
                     (RwPluginDataChunkGetSizeCallBack)&LAB_004818c0);
  /* Stream plugin must return the same id as the extension; then call FUN_004f7f10 (inferred: post-register hook) */
  if (g_gfx_89de0c != register_result) {
    return 0;
  }
  FUN_004f7f10(ATOMIC_PLUGIN_ID,&LAB_004819a0);
  return 1;
}


