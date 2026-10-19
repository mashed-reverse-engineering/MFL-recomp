#include "mfl_tier2.h"
/*004a6980*/ extern int app_get_app_771058();
/*004a6990*/ extern int d3d9_set_shader_flag();
/*004aa970*/ extern void FUN_004aa970(void);
/*004dbc10*/ extern RwBool __cdecl RwD3D9CreateVertexShader(RwUInt32 *p0, void **p1);
/*004dbc70*/ extern RwBool RwD3D9CreatePixelShader(RwUInt32 *p0, void **p1);
/*005e1838*/ extern undefined g_d3d9_shaders_5e1838;
/*005e1920*/ extern undefined g_d3d9_shaders_5e1920;
/*005e1980*/ extern undefined g_d3d9_shaders_5e1980;
/*005e19f0*/ extern undefined g_d3d9_shaders_5e19f0;
/*00771aa0*/ extern undefined4 g_data_771aa0;
/*00771aa4*/ extern undefined4 g_data_771aa4;
/*00771aa8*/ extern undefined4 g_data_771aa8;
/*00771aac*/ extern undefined4 g_data_771aac;
/* d3d9_shaders_create */

/* "RwD3D9CreateVertexShader / PixelShader" */

/*
 * Creates the D3D9 vertex shader and the three pixel shaders from their
 * bytecode blobs (inferred: the blobs are the g_d3d9_shaders_* globals).
 * Does nothing unless app_get_app_771058() returns 1 (inferred: app is ready).
 * On any creation failure it calls FUN_004aa970() and then
 * d3d9_set_shader_flag(0) (inferred: marks shaders as unavailable).
 * Takes no parameters and returns nothing.
 */
void d3d9_shaders_create(void)

{
  int app_state;
  RwBool create_result;

  /* bail out unless the app is in the state that allows shader creation */
  app_state = app_get_app_771058();
  if (app_state != 1) {
    return;
  }
  /* vertex shader first, then the pixel shaders; each step runs only if the previous one succeeded (result >= 0) */
  create_result = RwD3D9CreateVertexShader((RwUInt32 *)&g_d3d9_shaders_5e1838,(void **)&g_data_771aa0);
  if (((-1 < create_result) &&
      (create_result = RwD3D9CreatePixelShader((RwUInt32 *)&g_d3d9_shaders_5e1920,(void **)&g_data_771aa4),
      -1 < create_result)) &&
     (create_result = RwD3D9CreatePixelShader((RwUInt32 *)&g_d3d9_shaders_5e1980,(void **)&g_data_771aa8),
     -1 < create_result)) {
    create_result = RwD3D9CreatePixelShader((RwUInt32 *)&g_d3d9_shaders_5e19f0,(void **)&g_data_771aac);
    if (-1 < create_result) {
      return;
    }
    FUN_004aa970();
    d3d9_set_shader_flag(0);
    return;
  }
  FUN_004aa970();
  d3d9_set_shader_flag(0);
  return;
}


