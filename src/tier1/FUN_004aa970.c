#include "mfl_tier2.h"
/*004dbc40*/ extern void __cdecl RwD3D9DeletePixelShader(void *p0);
/*00771aa0*/ extern undefined4 g_data_771aa0;
/*00771aa4*/ extern undefined4 g_data_771aa4;
/*00771aa8*/ extern undefined4 g_data_771aa8;
/*00771aac*/ extern undefined4 g_data_771aac;
/* Releases the four D3D9 pixel shaders held in g_data_771aa0..g_data_771aac (inferred: shader handles cached by the renderer). Each non-NULL slot is deleted and reset to NULL, so calling ... */
/* FUN_004aa970
 * Releases the four D3D9 pixel shaders held in g_data_771aa0..g_data_771aac (inferred: shader
 * handles cached by the renderer). Each non-NULL slot is deleted and reset to NULL, so calling
 * this twice is safe. Takes no parameters and returns nothing.
 */
void FUN_004aa970(void)
{
  if (g_data_771aa0 != (void *)0x0) {
    RwD3D9DeletePixelShader(g_data_771aa0);
    g_data_771aa0 = (void *)0x0;
  }
  if (g_data_771aa4 != (void *)0x0) {
    RwD3D9DeletePixelShader(g_data_771aa4);
    g_data_771aa4 = (void *)0x0;
  }
  if (g_data_771aa8 != (void *)0x0) {
    RwD3D9DeletePixelShader(g_data_771aa8);
    g_data_771aa8 = (void *)0x0;
  }
  if (g_data_771aac != (void *)0x0) {
    RwD3D9DeletePixelShader(g_data_771aac);
    g_data_771aac = (void *)0x0;
  }
  return;
}
