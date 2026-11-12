/* c_D3DXMatrixTranspose (0x004fc629): D3DX plain C matrix transpose, slot 37 of the D3DX dispatch table (no SSE2 variant). */
#include "mfl_tier2.h"

typedef struct D3DXMATRIX {
  float _11, _12, _13, _14;
  float _21, _22, _23, _24;
  float _31, _32, _33, _34;
  float _41, _42, _43, _44;
} D3DXMATRIX;

/* out = transpose(m); out may be m (the off-diagonal pairs are swapped, the diagonal is copied only when out != m). */
/* TIER2: unvalidated 2026-11-10 authority=static */
D3DXMATRIX * __stdcall c_D3DXMatrixTranspose(D3DXMATRIX *out, const D3DXMATRIX *m)
{
  float f;

  f = m->_12; out->_12 = m->_21; out->_21 = f;
  f = m->_13; out->_13 = m->_31; out->_31 = f;
  f = m->_14; out->_14 = m->_41; out->_41 = f;
  f = m->_23; out->_23 = m->_32; out->_32 = f;
  f = m->_24; out->_24 = m->_42; out->_42 = f;
  f = m->_34; out->_34 = m->_43; out->_43 = f;
  if (out != m) {
    out->_11 = m->_11;
    out->_22 = m->_22;
    out->_33 = m->_33;
    out->_44 = m->_44;
  }
  return out;
}
