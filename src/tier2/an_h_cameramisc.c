#include "mfl_tier2.h"
/* RenderWare camera helpers: the default camera end-update callback (0x4d2d10) and the clump destructor's per-camera
   callback (0x4f6e80).  Written from the disassembly of MFL.exe. */

typedef struct ListLink ListLink;        /* doubly linked list link (next, prev) */
struct ListLink {
  ListLink *next;
  ListLink *prev;
};

typedef struct ClumpCameraExt {           /* camera plugin data of the clump: owning clump + link in its camera list */
  void *clump;
  ListLink clump_link;
} ClumpCameraExt;

typedef RwBool (*RwStandardFunc)(void *out, void *in, RwInt32 n);
#define RW_STD_CAMERA_END_UPDATE RW_GLOBAL(0x70, RwStandardFunc)      /* stdFunc[rwSTANDARDCAMERAENDUPDATE] */
#define RW_CURRENT_CAMERA RW_GLOBAL(0x00, void *)                       /* curCamera */

/*007d2128*/ extern char *RwEngineInstance;
/*007d535c*/ extern RwInt32 _rpClumpCameraExtOffset;
/*004d1e80*/ extern RwBool RwCameraDestroy(void *camera);

#define CLUMP_CAMERA_EXT(camera) ((ClumpCameraExt *)((char *)(camera) + _rpClumpCameraExtOffset))

/* TIER2: unvalidated 2026-11-14 authority=diffemu */
void *CameraEndUpdate(void *camera)
{
  if (RW_STD_CAMERA_END_UPDATE(NULL, camera, 0)) {
    RW_CURRENT_CAMERA = NULL;
    return camera;
  }
  return NULL;
}

/* RpClumpDestroy's camera callback: unlinks the camera from its clump and destroys it. */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
void *DestroyClumpCamera(void *camera, void *data)
{
  ClumpCameraExt *ext = CLUMP_CAMERA_EXT(camera);

  ext->clump_link.prev->next = ext->clump_link.next;
  ext->clump_link.next->prev = ext->clump_link.prev;
  ext->clump_link.prev = NULL;
  ext->clump_link.next = NULL;
  ext->clump = NULL;
  RwCameraDestroy(camera);
  return camera;
}
