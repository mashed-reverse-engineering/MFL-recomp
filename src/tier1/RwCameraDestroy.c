/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
typedef void (__cdecl *FnFree2)(void *, void *);
/*007d1ff0*/ extern int g_rw_7d1ff0;
/*0062e534*/ extern char g_rw_62e534[];
extern void *__cdecl rwPluginRegistryDeInitObject(void *reg, void *obj);
/*004d18b0*/ extern void *__cdecl rwObjectHasFrameReleaseFrame(void *obj);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Destroys a camera: plugin destructors, releases its frame, returns it to the camera free list; returns TRUE. */
int RwCameraDestroy(void *cam)
{
  rwPluginRegistryDeInitObject(g_rw_62e534, cam);
  rwObjectHasFrameReleaseFrame(cam);
  (*(FnFree2 *)(RwEngineInstance + 0x11c))(FLD(RwEngineInstance, void *, g_rw_7d1ff0), cam);
  return 1;
}
