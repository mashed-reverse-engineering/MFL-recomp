/* D3DX public math entry points that dispatch through the slot table 0x62ec88 (see recomp/d3dx_dispatch.c), and their init stubs.
   Each public entry is `jmp [slot]` in the original; each slot's static start value is an init stub `push 1; call D3DXCpuOptimizations;
   jmp [slot]`, so the first D3DX call of the process fills the table and then runs the real math.  Only the five slots with a live reader are
   here (3, 4, 6, 10, 37); the slot words name the init stubs, live callers name the public entries (thunk_FUN_* in symbols.txt). */
#include "mfl_tier2.h"

typedef void (*D3DXMathFn)(void);
typedef void *(__stdcall *D3DXMath3)(void *out, const void *a, const void *b);
typedef void *(__stdcall *D3DXMath2)(void *out, const void *a);

#define d3dx_slots PTR_FUN_0062ec88      /* the live dispatch table (Ghidra name) */
/*0062ec88*/ extern D3DXMathFn d3dx_slots[0x47];
/*0050501a*/ extern int __stdcall D3DXCpuOptimizations(int enable);

#define D3DX_SLOT_MATRIX_MULTIPLY 3
#define D3DX_SLOT_MATRIX_MULTIPLY_TRANSPOSE 4
#define D3DX_SLOT_VEC3_TRANSFORM_NORMAL 6
#define D3DX_SLOT_VEC3_TRANSFORM_COORD 10
#define D3DX_SLOT_MATRIX_TRANSPOSE 37

/* D3DXMatrixMultiply (0x004fc3da): out = a * b (4x4). */
/* TIER2: unvalidated 2026-11-13 authority=diffemu */
void *__stdcall d3dx_MatrixMultiply(void *out, const void *a, const void *b)
{
  return ((D3DXMath3)d3dx_slots[D3DX_SLOT_MATRIX_MULTIPLY])(out, a, b);
}

/* TIER2: unvalidated 2026-11-13 authority=diffemu */
void *__stdcall d3dx_init_MatrixMultiply(void *out, const void *a, const void *b)
{
  D3DXCpuOptimizations(1);
  return ((D3DXMath3)d3dx_slots[D3DX_SLOT_MATRIX_MULTIPLY])(out, a, b);
}

/* D3DXMatrixMultiplyTranspose (0x004fc6a3): out = transpose(a * b). */
/* TIER2: unvalidated 2026-11-13 authority=diffemu */
void *__stdcall d3dx_MatrixMultiplyTranspose(void *out, const void *a, const void *b)
{
  return ((D3DXMath3)d3dx_slots[D3DX_SLOT_MATRIX_MULTIPLY_TRANSPOSE])(out, a, b);
}

/* TIER2: unvalidated 2026-11-13 authority=diffemu */
void *__stdcall d3dx_init_MatrixMultiplyTranspose(void *out, const void *a, const void *b)
{
  D3DXCpuOptimizations(1);
  return ((D3DXMath3)d3dx_slots[D3DX_SLOT_MATRIX_MULTIPLY_TRANSPOSE])(out, a, b);
}

/* D3DXVec3TransformNormal (0x004fbc73): out = v * upper 3x3 of m. */
/* TIER2: unvalidated 2026-11-13 authority=diffemu */
void *__stdcall d3dx_Vec3TransformNormal(void *out, const void *v, const void *m)
{
  return ((D3DXMath3)d3dx_slots[D3DX_SLOT_VEC3_TRANSFORM_NORMAL])(out, v, m);
}

/* TIER2: unvalidated 2026-11-13 authority=diffemu */
void *__stdcall d3dx_init_Vec3TransformNormal(void *out, const void *v, const void *m)
{
  D3DXCpuOptimizations(1);
  return ((D3DXMath3)d3dx_slots[D3DX_SLOT_VEC3_TRANSFORM_NORMAL])(out, v, m);
}

/* D3DXVec3TransformCoord (0x004fbc49): out = (v, 1) * m, divided by w. */
/* TIER2: unvalidated 2026-11-13 authority=diffemu */
void *__stdcall d3dx_Vec3TransformCoord(void *out, const void *v, const void *m)
{
  return ((D3DXMath3)d3dx_slots[D3DX_SLOT_VEC3_TRANSFORM_COORD])(out, v, m);
}

/* TIER2: unvalidated 2026-11-13 authority=diffemu */
void *__stdcall d3dx_init_Vec3TransformCoord(void *out, const void *v, const void *m)
{
  D3DXCpuOptimizations(1);
  return ((D3DXMath3)d3dx_slots[D3DX_SLOT_VEC3_TRANSFORM_COORD])(out, v, m);
}

/* D3DXMatrixTranspose (0x004fc623): out = transpose(m). */
/* TIER2: unvalidated 2026-11-13 authority=diffemu */
void *__stdcall d3dx_MatrixTranspose(void *out, const void *m)
{
  return ((D3DXMath2)d3dx_slots[D3DX_SLOT_MATRIX_TRANSPOSE])(out, m);
}

/* TIER2: unvalidated 2026-11-13 authority=diffemu */
void *__stdcall d3dx_init_MatrixTranspose(void *out, const void *m)
{
  D3DXCpuOptimizations(1);
  return ((D3DXMath2)d3dx_slots[D3DX_SLOT_MATRIX_TRANSPOSE])(out, m);
}
