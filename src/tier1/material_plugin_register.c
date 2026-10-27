#include "mfl_tier2.h"
extern char LAB_00481b30[];
extern char LAB_00481ac0[];
extern char LAB_00481b90[];
/*00481b70*/ extern int material_plugin_construct();
/*00481ba0*/ extern undefined __cdecl material_plugin_stream_read(struct RwStream *p0, undefined4 p1, int p2);
/*00481bf0*/ extern undefined __cdecl material_plugin_stream_write(struct RwStream *p0, undefined4 p1, int p2);
/*00481c30*/ extern int material_plugin_copy();
/*00481c90*/ extern int material_plugin_clear_12();
/*004d2f20*/ extern RwInt32 RwEngineRegisterPlugin(RwInt32 p0, RwUInt32 p1, RwPluginObjectConstructor p2, RwPluginObjectDestructor p3);
/*004f8260*/ extern RwInt32 RpMaterialRegisterPlugin(RwInt32 p0, RwUInt32 p1, RwPluginObjectConstructor p2, RwPluginObjectDestructor p3, RwPluginObjectCopy p4);
/*004f8290*/ extern RwInt32 RpMaterialRegisterPluginStream(RwUInt32 p0, RwPluginDataChunkReadCallBack p1, RwPluginDataChunkWriteCallBack p2, RwPluginDataChunkGetSizeCallBack p3);
/*004f82c0*/ extern void FUN_004f82c0(undefined4 p0, void *p1);
/*0053eac0*/ extern int FUN_0053eac0();
/*0089de04*/ extern undefined4 g_app_89de04;
#define MATERIAL_PLUGIN_ID (0x26990003)
#define MATERIAL_PLUGIN_SIZE (0x30)
/* material_plugin_register */
/* "RpMaterialRegisterPlugin 0x26990003 (0x30 bytes per material)" */
/* Registers the material plugin with the RenderWare engine: the engine plugin, the
   material plugin (MATERIAL_PLUGIN_SIZE bytes of extra data per material, with
   constructor/destructor/copy callbacks) and its stream read/write/size handlers.
   Returns 1 if everything registered, 0 otherwise. (inferred) */
undefined4 material_plugin_register(void)
{
  RwInt32 plugin_offset;

  /* Engine plugin registration; a negative result means failure. */
  plugin_offset = RwEngineRegisterPlugin
                    (0,MATERIAL_PLUGIN_ID,(RwPluginObjectConstructor)&LAB_00481b30,
                     (RwPluginObjectDestructor)&LAB_00481ac0);
  if (plugin_offset < 0) {
    return 0;
  }
  g_app_89de04 = RpMaterialRegisterPlugin
                           (MATERIAL_PLUGIN_SIZE,MATERIAL_PLUGIN_ID,material_plugin_construct,FUN_0053eac0,
                            material_plugin_copy);
  /* Stream chunk handlers for the material plugin. */
  plugin_offset = RpMaterialRegisterPluginStream
                    (MATERIAL_PLUGIN_ID,material_plugin_stream_read,material_plugin_stream_write,
                     (RwPluginDataChunkGetSizeCallBack)&LAB_00481b90);
  /* Both registrations must return the same plugin offset. (inferred) */
  if (g_app_89de04 != plugin_offset) {
    return 0;
  }
  /* Install the clear callback for the plugin. (inferred) */
  FUN_004f82c0(MATERIAL_PLUGIN_ID,material_plugin_clear_12);
  return 1;
}
