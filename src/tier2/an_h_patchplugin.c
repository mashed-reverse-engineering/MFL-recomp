#include "mfl_tier2.h"
/* RenderWare patch plugin (rppatch): RpPatchPluginAttach (0x559c70).  Written from the disassembly of MFL.exe.
   A file of its own: it takes the addresses of the callbacks, which stay the original's in diffemu this way. */

#define rwID_PATCHPLUGIN 0x123

typedef void *(*PluginObjectFunc)();      /* constructor / destructor / copy / stream callbacks (argument lists differ) */
typedef RwBool (*PluginBoolFunc)();

typedef struct PatchGlobals {
  RwInt32 engine_offset;
  RwInt32 atomic_offset;
  RwInt32 geometry_offset;
} PatchGlobals;

/*007da860*/ extern PatchGlobals _rpPatchGlobals;

/*004d2f20*/ extern RwInt32 RwEngineRegisterPlugin(RwInt32 size, RwUInt32 plugin_id, PluginObjectFunc init, PluginObjectFunc term);
/*004f7e80*/ extern RwInt32 RpAtomicRegisterPlugin(RwInt32 size, RwUInt32 plugin_id, PluginObjectFunc constructor,
                                                   PluginObjectFunc destructor, PluginObjectFunc copy);
/*004f7ee0*/ extern RwInt32 RpAtomicRegisterPluginStream(RwUInt32 plugin_id, PluginObjectFunc read, PluginObjectFunc write,
                                                         PluginObjectFunc get_size);
/*004f7f10*/ extern RwInt32 RpAtomicSetStreamAlwaysCallBack(RwUInt32 plugin_id, PluginBoolFunc always);
/*004f7f30*/ extern RwInt32 RpAtomicSetStreamRightsCallBack(RwUInt32 plugin_id, PluginBoolFunc rights);
/*004f9110*/ extern RwInt32 RpGeometryRegisterPlugin(RwInt32 size, RwUInt32 plugin_id, PluginObjectFunc constructor,
                                                     PluginObjectFunc destructor, PluginObjectFunc copy);
/*004f9140*/ extern RwInt32 RpGeometryRegisterPluginStream(RwUInt32 plugin_id, PluginObjectFunc read, PluginObjectFunc write,
                                                           PluginObjectFunc get_size);
/*004f9170*/ extern RwInt32 RpGeometrySetStreamAlwaysCallBack(RwUInt32 plugin_id, PluginBoolFunc always);

/*00559d40*/ extern void *PatchEngineOpen();
/*00559e20*/ extern void *PatchEngineClose();
/*005598e0*/ extern void *rpPatchAtomicConstructor();
/*00559900*/ extern void *rpPatchAtomicDestructor();
/*00559940*/ extern void *rpPatchAtomicCopy();
/*00559980*/ extern RwBool rpPatchAtomicAlways();
/*005599c0*/ extern RwBool rpPatchAtomicRights();
/*0053eac0*/ extern void *rpPatchAtomicReadWrite();     /* atomic stream read and write: the exe folds both into one "return the stream" body */
/*00405e40*/ extern void *stub_return_zero();           /* atomic stream size: 0 bytes */
/*0055a610*/ extern void *rpPatchGeometryConstructor();
/*0055a630*/ extern void *rpPatchGeometryDestructor();
/*0055a660*/ extern void *rpPatchGeometryCopy();
/*0055a6c0*/ extern void *rpPatchGeometryRead();
/*0055a700*/ extern void *rpPatchGeometryWrite();
/*0055a6a0*/ extern void *rpPatchGeometryGetSize();
/*0055a720*/ extern RwBool rpPatchGeometryAlways();

/* Registers the engine, atomic and geometry extensions of the patch plugin.  Like the exe, it does not check the
   offsets the registrations return. */
/* TIER2: validated 2026-11-14 authority=diffemu */
RwBool RpPatchPluginAttach(void)
{
  _rpPatchGlobals.engine_offset = RwEngineRegisterPlugin(0x18, rwID_PATCHPLUGIN, PatchEngineOpen, PatchEngineClose);

  _rpPatchGlobals.atomic_offset = RpAtomicRegisterPlugin(0xc, rwID_PATCHPLUGIN, rpPatchAtomicConstructor,
                                                         rpPatchAtomicDestructor, rpPatchAtomicCopy);
  RpAtomicRegisterPluginStream(rwID_PATCHPLUGIN, rpPatchAtomicReadWrite, rpPatchAtomicReadWrite, stub_return_zero);
  RpAtomicSetStreamAlwaysCallBack(rwID_PATCHPLUGIN, rpPatchAtomicAlways);
  RpAtomicSetStreamRightsCallBack(rwID_PATCHPLUGIN, rpPatchAtomicRights);

  _rpPatchGlobals.geometry_offset = RpGeometryRegisterPlugin(8, rwID_PATCHPLUGIN, rpPatchGeometryConstructor,
                                                             rpPatchGeometryDestructor, rpPatchGeometryCopy);
  RpGeometryRegisterPluginStream(rwID_PATCHPLUGIN, rpPatchGeometryRead, rpPatchGeometryWrite, rpPatchGeometryGetSize);
  RpGeometrySetStreamAlwaysCallBack(rwID_PATCHPLUGIN, rpPatchGeometryAlways);
  return TRUE;
}
