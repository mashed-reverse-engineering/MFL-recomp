/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
typedef void (__cdecl *FnFree2)(void *, void *);
typedef int (__cdecl *FnI3)(void *, void *, int);
/*007d21d8*/ extern int g_rw_7d21d8;
/*0062e718*/ extern char g_rw_62e718[];
extern void *__cdecl rwPluginRegistryDeInitObject(void *reg, void *obj);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Destroys a raster: plugin destructors, device destroy callback, returns it to the raster free list; returns TRUE. */
int RwRasterDestroy(void *r)
{
  rwPluginRegistryDeInitObject(g_rw_62e718, r);
  (*(FnI3 *)(RwEngineInstance + 0x5c))(0, r, 0);
  (*(FnFree2 *)(RwEngineInstance + 0x11c))(FLD(RwEngineInstance, void *, g_rw_7d21d8 + 0x60), r);
  return 1;
}
