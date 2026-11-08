/* fakedev.h: minimal fake IDirect3DDevice9 for the native KES checks (plan_engine_switch_2026-11-07_v2 section 2 step 1).
   119-slot vtable; every slot is a trap that prints "TRAP unexpected method <n>" and exits (code 5), except the recorders that are
   filled in.  Recorders log the method slot (FAKEDEV_TAG | slot) and their arguments through fakedev_log and return fakedev_hr.
   Lane D (d3drs) recorders: 49 SetMaterial, 57 SetRenderState, 65 SetTexture, 67 SetTextureStageState, 69 SetSamplerState,
   73 SetCurrentTexturePalette.  Other lanes add their slots to fakedev.c (or write their own into fakedev_vtbl after fakedev_init). */
#ifndef FAKEDEV_H
#define FAKEDEV_H
#include <stdint.h>
#define FAKEDEV_SLOTS 119
#define FAKEDEV_TAG 0xde500000u
extern void *fakedev_vtbl[FAKEDEV_SLOTS];
extern void (*fakedev_log)(uint32_t w);   /* the check's log sink; must be set before the first device call */
extern int32_t fakedev_hr;                /* HRESULT returned by the recorders (default 0 = D3D_OK) */
void *fakedev_init(void);                 /* fills the vtable, returns the device object (its first word is the vtable) */
/* lane D2 (d3dim2d): 4 GetAvailableTextureMem (returns fakedev_texmem), 26 / 27 Create{Vertex,Index}Buffer (fakedev_createhr != 0: fails, *pp = 0),
   81 / 82 Draw[Indexed]Primitive, buffer objects with Release / Lock (fakedev_lockhr < 0: fails, *pp untouched) / Unlock (logs a hash of the locked bytes) */
#define FAKEDEV_BUFS 4096
#define FAKEDEV_SCRATCH 0x400000
extern uint32_t fakedev_texmem;
extern int32_t fakedev_createhr, fakedev_lockhr;
void fakedev_reset_bufs(void);            /* empties the buffer pool (per seed) */
/* lane D3 (d3dlights): 51 SetLight (logs the index and the whole D3DLIGHT9), 53 LightEnable
   lane D4 (d3dxform): 44 SetTransform (logs the state and the whole matrix), 87 SetVertexDeclaration, 100 SetStreamSource */
#endif
