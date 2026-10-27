#include "mfl_tier2.h"
/*00405e40*/ extern int stub_return_zero();
/*00486ef0*/ extern int particles_render();
/*00759648*/ extern undefined4 g_fx_dust_759648;
/*00759658*/ extern undefined g_fx_dust_759658;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;


/* fx_dust_render */

/* "culling mode 1 and z write off, renders the dust particles" */

void fx_dust_render(void)
{
  void *local_4;
  
  if (g_fx_dust_759648 != 0) {
    RwRenderStateGet(rwRENDERSTATECULLMODE,&local_4);
    RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)0x1);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
    stub_return_zero(0x3e99999a);
    particles_render(&g_fx_dust_759658);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
    RwRenderStateSet(rwRENDERSTATECULLMODE,local_4);
    stub_return_zero(0);
  }
  return;
}
