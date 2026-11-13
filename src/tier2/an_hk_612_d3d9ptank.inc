/* mflrw.dll d3d9ptank binding (librw-mfl/capi/d3d9ptank.cpp; LIB_RW group d3d9ptank, engine switch window 6 lane K2): rpPTankD3D9InsEnding, rpPTankD3D9InsSetup,
   rpPTankD3D9Create and rpPTankD3D9RenderCallBack work on the exe's engine, device, device state caches, the ptank d3d9 bss block (addressed from
   rpTankD3D9IndexBuffer 0x7da6c4, its first word: +8 dynamic vertex buffer, +0xc first vertex, +0x14 point sprite flag have no symbol), the card's maximum point
   size, the ptank plugin offset and these exe callees (table order = d3d9ptank.cpp enum E, 44 slots).  No Win32 import slot, no slot filled at run time, no 0 slot.
   Include after an_hk_612_noded3d9.inc (an_hk_612.c and the earlier .inc files declare g_d3d9_device, the five _rwD3D9Last*Used caches, RwFrameGetLTM,
   _rwD3D9AtomicDefaultLightingCallback, RwD3D9SetTransformWorld, RwD3D9Get / SetRenderState, RwD3D9SetTextureStageState, FUN_004e55d0 = RwD3D9SetTexture,
   RwD3D9SetSurfaceProperties, RwD3D9SetStreamSource, _rwD3D9RenderStateFlushCache, RwD3D9DynamicVertexBufferLock / Unlock, RpAtomicSetGeometry, RpGeometryDestroy,
   RwD3D9IndexBufferCreate).
   Trap rows (tools/delink/rw_trap.txt): the seven point sprite instance functions rpPTankD3D9Ins*_spr (slots 35-41: values rpPTankD3D9Create stores into the
   atomic's ptank data) and rpPTankGENInsPosCCCSNR (slot 32: a value it compares with).  No kept code may name a trapped function (trapcheck), so each of these slots
   is a stub that faults at a named address (see an_hk_612_d3d9patch.inc; lane ASM's T6 form replaces it by defining HK_TRAP_STUB before the include).  Slot 32 as a
   stub means the compare never matches, so an atomic whose generic position function is the trapped rpPTankGENInsPosCCCSNR keeps it instead of getting the trapped
   rpPTankD3D9InsPosCCCSNR_spr: both end in a trap when called, only the name in the trap message differs.
   Slots 42 / 43 (rpPTankD3D9InsSetup / InsEnding) are rows of this group: the original addresses, which the link turns into the DLL thunks. */
#ifndef MFL_DIFFEMU
#ifndef HK_TRAP_STUB
#define HK_TRAP_STUB(fn) void __cdecl fn(void) { *(volatile int *)0 = 0; }
#endif
HK_TRAP_STUB(mfl_trap_rpPTankGENInsPosCCCSNR_005470e0)
HK_TRAP_STUB(mfl_trap_rpPTankD3D9InsSetup_spr_005461e0)
HK_TRAP_STUB(mfl_trap_rpPTankD3D9InsPosNCPSNR_spr_00546ac0)
HK_TRAP_STUB(mfl_trap_rpPTankD3D9InsPosCCPSNR_spr_00546b30)
HK_TRAP_STUB(mfl_trap_rpPTankD3D9InsPosNCCSNR_spr_00546a10)
HK_TRAP_STUB(mfl_trap_rpPTankD3D9InsPosCCCSNR_spr_00546a70)
HK_TRAP_STUB(mfl_trap_rpPTankD3D9InsNormals_spr_00546bb0)
HK_TRAP_STUB(mfl_trap_rpPTankD3D9InsColor_spr_00546bf0)
/*007da6c4*/ extern int rpTankD3D9IndexBuffer;      /* first word of the ptank d3d9 bss block */
/*0063a220*/ extern float rpTankD3D9VideoCardMaxPointSize;
/*007da6bc*/ extern int g_ptank_plugin_offset;
extern void __cdecl RpGeometryCreateSpace(void), RpMaterialCreate(void), RpGeometryTriangleSetMaterial(void), RpMaterialDestroy(void), rpPTankGENInstanceSelectCB(void);
extern void __cdecl rpPTankGENInsPosNCPSNR(void), rpPTankGENInsPosCCPSNR(void), rpPTankGENInsPosNCCSNR(void), rpPTankGENInsNormals(void), rpPTankGENInsColorPP(void);
extern void __cdecl rpPTankD3D9InsSetup(void), rpPTankD3D9InsEnding(void);
typedef void (__cdecl *MflrwD3d9ptankBind)(char **engine, void *const *table);
static void hk_bind_d3d9ptank(int mflrw)
{
  static void *const d3d9ptankTable[44] = {
    &rpTankD3D9IndexBuffer, &rpTankD3D9VideoCardMaxPointSize, &g_ptank_plugin_offset, &g_d3d9_device, &_rwD3D9LastFVFUsed, &_rwD3D9LastVertexDeclarationUsed,
    &_rwD3D9LastVertexShaderUsed, &_rwD3D9LastPixelShaderUsed, &_rwD3D9LastIndexBufferUsed,
    /* 9.. */ (void *)RwD3D9DynamicVertexBufferUnlock, (void *)RwD3D9DynamicVertexBufferLock, (void *)RwFrameGetLTM, (void *)_rwD3D9AtomicDefaultLightingCallback,
    (void *)RwD3D9SetTransformWorld, (void *)RwD3D9GetRenderState, (void *)RwD3D9SetRenderState, (void *)RwD3D9SetTextureStageState, (void *)FUN_004e55d0,
    (void *)RwD3D9SetSurfaceProperties, (void *)RwD3D9SetStreamSource, (void *)_rwD3D9RenderStateFlushCache,
    /* 21.. */ (void *)RpGeometryCreateSpace, (void *)RpMaterialCreate, (void *)RpGeometryTriangleSetMaterial, (void *)RpAtomicSetGeometry, (void *)RpGeometryDestroy,
    (void *)RpMaterialDestroy, (void *)RwD3D9IndexBufferCreate, (void *)rpPTankGENInstanceSelectCB,
    /* 29.. */ (void *)rpPTankGENInsPosNCPSNR, (void *)rpPTankGENInsPosCCPSNR, (void *)rpPTankGENInsPosNCCSNR, (void *)mfl_trap_rpPTankGENInsPosCCCSNR_005470e0,
    (void *)rpPTankGENInsNormals, (void *)rpPTankGENInsColorPP,
    /* 35.. */ (void *)mfl_trap_rpPTankD3D9InsSetup_spr_005461e0, (void *)mfl_trap_rpPTankD3D9InsPosNCPSNR_spr_00546ac0,
    (void *)mfl_trap_rpPTankD3D9InsPosCCPSNR_spr_00546b30, (void *)mfl_trap_rpPTankD3D9InsPosNCCSNR_spr_00546a10,
    (void *)mfl_trap_rpPTankD3D9InsPosCCCSNR_spr_00546a70, (void *)mfl_trap_rpPTankD3D9InsNormals_spr_00546bb0, (void *)mfl_trap_rpPTankD3D9InsColor_spr_00546bf0,
    /* 42.. */ (void *)rpPTankD3D9InsSetup, (void *)rpPTankD3D9InsEnding};
  MflrwD3d9ptankBind b = mflrw ? (MflrwD3d9ptankBind)GetProcAddress(mflrw, "mflrw_d3d9ptank_bind") : 0;
  if (b != 0) b(&RwEngineInstance, d3d9ptankTable);      /* the DLL copies the table */
}
#endif
