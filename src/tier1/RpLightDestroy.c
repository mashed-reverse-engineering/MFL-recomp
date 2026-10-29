/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
typedef void (__cdecl *FnFree2)(void *, void *);
/*007d52d0*/ extern int g_rp_7d52d0;
/*0062ebcc*/ extern char g_rp_62ebcc[];
extern void *__cdecl rwPluginRegistryDeInitObject(void *reg, void *obj);
/*004d18b0*/ extern void *__cdecl rwObjectHasFrameReleaseFrame(void *obj);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Destroys a light: plugin destructors, releases its frame, returns it to the light free list; returns TRUE. */
int RpLightDestroy(void *l)
{
  rwPluginRegistryDeInitObject(g_rp_62ebcc, l);
  rwObjectHasFrameReleaseFrame(l);
  (*(FnFree2 *)(RwEngineInstance + 0x11c))(FLD(RwEngineInstance, void *, g_rp_7d52d0), l);
  return 1;
}
