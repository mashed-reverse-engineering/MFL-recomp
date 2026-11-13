/* mflrw.dll d3d9patch binding (librw-mfl/capi/d3d9patch.cpp; LIB_RW group d3d9patch, engine switch window 6 lane K2): _rpD3D9PatchInstanceAtomic and
   rwD3D9PatchAtomicAllInOneNodeNode work on the exe's engine, patch plugin offset, 32 bit index flag, resources module offset and these exe callees (table order =
   d3d9patch.cpp enum E, 27 slots).  No Win32 import slot, no slot filled at run time, no 0 slot.
   Include after an_hk_612_noded3d9.inc (it and the earlier .inc files / an_hk_612.c declare resourcesModule, RwResourcesFreeResEntry, RwD3D9CreateVertexDeclaration,
   RwD3D9DestroyVertexBuffer, RwResourcesAllocateResEntry, _rwD3D9ResourceEntryInstanceDataDestroy, RwD3D9IndexBufferCreate, _rwD3D9AtomicDefaultLightingCallback,
   RwFrameGetLTM, RwD3D9SetTransformWorld, RwD3D9GetRenderState, FUN_004dbca0 = RwD3D9GetCaps, RwD3D9SetRenderState).
   Trap rows (tools/delink/rw_trap.txt): the four tri patch functions (slots 0-3) and rpPatchReInstanceAtomic (slot 16).  No kept code may name a trapped function
   (trapcheck), so each of these slots is a stub that faults at a named address: the release exe's filter (recomp/release/mfl_trap.c) then writes
   "trap 0xc0000005 at <addr> in _mfl_trap_<function>_<va>+<off>" to mfl_trap.log and ends with exit code 3, as an int3 of the trap fill does.  Lane ASM's common stub
   form (T6) replaces this one by defining HK_TRAP_STUB before the include.
   Slot 15 (_rpD3D9PatchInstanceAtomic) is a row of this group: the original address, which the link turns into the DLL thunk.  Slots 4-7 are group bezpat rows. */
#ifndef MFL_DIFFEMU
#ifndef HK_TRAP_STUB
#define HK_TRAP_STUB(fn) void __cdecl fn(void) { *(volatile int *)0 = 0; }
#endif
HK_TRAP_STUB(mfl_trap_rpPatchRefineTriVector_0055c050)
HK_TRAP_STUB(mfl_trap_rpPatchRefineTriColor_0055c3d0)
HK_TRAP_STUB(mfl_trap_rpPatchRefineTriTexCoords_0055ca30)
HK_TRAP_STUB(mfl_trap_rpPatchGenerateTriIndex_0055d5d0)
HK_TRAP_STUB(mfl_trap_rpPatchReInstanceAtomic_0055d760)
/*007da864*/ extern int g_rp_7da864;      /* patch plugin offset in the atomic */
/*007da890*/ extern int g_rp_7da890;      /* 32 bit index buffers usable */
extern void __cdecl _rpPatchRefineQuadVector(void), _rpPatchRefineQuadColor(void), _rpPatchRefineQuadTexCoords(void), _rpPatchGenerateQuadIndex(void);
extern void __cdecl RwD3D9CreateVertexBuffer(void), _rpD3D9PatchInstanceAtomic(void), rwD3D9IndexBuffer32bitsCreate(void);
typedef void (__cdecl *MflrwD3d9patchBind)(char **engine, void *const *table);
static void hk_bind_d3d9patch(int mflrw)
{
  static void *const d3d9patchTable[27] = {
    (void *)mfl_trap_rpPatchRefineTriVector_0055c050, (void *)mfl_trap_rpPatchRefineTriColor_0055c3d0, (void *)mfl_trap_rpPatchRefineTriTexCoords_0055ca30,
    (void *)mfl_trap_rpPatchGenerateTriIndex_0055d5d0,
    /* 4.. */ (void *)_rpPatchRefineQuadVector, (void *)_rpPatchRefineQuadColor, (void *)_rpPatchRefineQuadTexCoords, (void *)_rpPatchGenerateQuadIndex,
    /* 8.. */ &g_rp_7da864, &g_rp_7da890, &resourcesModule,
    /* 11.. */ (void *)RwResourcesFreeResEntry, (void *)RwD3D9CreateVertexDeclaration, (void *)RwD3D9CreateVertexBuffer, (void *)RwD3D9DestroyVertexBuffer,
    (void *)_rpD3D9PatchInstanceAtomic, (void *)mfl_trap_rpPatchReInstanceAtomic_0055d760, (void *)RwResourcesAllocateResEntry,
    (void *)_rwD3D9ResourceEntryInstanceDataDestroy, (void *)rwD3D9IndexBuffer32bitsCreate, (void *)RwD3D9IndexBufferCreate,
    /* 21.. */ (void *)_rwD3D9AtomicDefaultLightingCallback, (void *)RwFrameGetLTM, (void *)RwD3D9SetTransformWorld, (void *)RwD3D9GetRenderState, (void *)FUN_004dbca0,
    (void *)RwD3D9SetRenderState};
  MflrwD3d9patchBind b = mflrw ? (MflrwD3d9patchBind)GetProcAddress(mflrw, "mflrw_d3d9patch_bind") : 0;
  if (b != 0) b(&RwEngineInstance, d3d9patchTable);      /* the DLL copies the table */
}
#endif
