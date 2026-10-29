#include "mfl_tier2.h"

enum FxBufferIndex {
  FX_BUFFER_VERTEX_1_INDEX = 1,
  FX_BUFFER_VERTEX_2_INDEX = 2,
  FX_BUFFER_COUNT_INDEX = 3,
  FX_BUFFER_SYSTEM_INDEX = 10
};
/*0049bd90*/ extern int fx_system_destroy();
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*0049bd90*/ extern int fx_system_destroy();
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/* fx_buffers_free */

/* Frees the three vertex buffers of an FX system and destroys the FX system. */
void fx_buffers_free(undefined4 *fx_buffers)
{
  /* Reset active vertex buffer count */
  fx_buffers[FX_BUFFER_COUNT_INDEX] = 0;
  /* Free each of the three vertex buffers */
  RwFree(*fx_buffers);
  RwFree(fx_buffers[FX_BUFFER_VERTEX_1_INDEX]);
  RwFree(fx_buffers[FX_BUFFER_VERTEX_2_INDEX]);
  /* Destroy associated FX system and clear pointer */
  fx_system_destroy(fx_buffers[FX_BUFFER_SYSTEM_INDEX]);
  fx_buffers[FX_BUFFER_SYSTEM_INDEX] = 0;
  /* Free the buffers structure itself */
  RwFree(fx_buffers);
  return;
}
