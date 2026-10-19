#include "mfl_tier2.h"
/*00481790*/ extern undefined __cdecl atomic_plugin_set_radius_squared(undefined4 p0, undefined4 p1);
/*004a6b30*/ extern int object_set_field_6c();
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004d16e0*/ extern struct RwFrame * RwFrameSetIdentity(struct RwFrame *p0);
/*00771030*/ extern undefined4 g_app_771030;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
#define OBJECT_SIZE (0x19c)
#define MAX_FRAMES (0x30)
#define OBJECT_OWNER_OFFSET (0x184)
#define OBJECT_FRAME_COUNT_OFFSET (0x188)
#define OBJECT_RADIUS_SQUARED_BITS (0x447a0000) /* float 1000.0f */
/* object_frames_create */

/* "(count): allocates 0x19C byte object with up to 0x30 frames" */
/* Purpose (inferred): allocates an OBJECT_SIZE byte object for `owner` and fills its first frame_count
   slots with new identity RwFrames. Records owner and frame_count in the object, sets the radius-squared
   of `owner` to 1000.0f, and returns the object. Returns NULL when frame_count > MAX_FRAMES. */

void * object_frames_create(undefined4 owner,int frame_count)

{
  undefined4 app_value;
  void *object;
  RwFrame *frame;
  int frame_index;
  undefined4 radius_squared;

  radius_squared = OBJECT_RADIUS_SQUARED_BITS;
  /* Reject counts above MAX_FRAMES. */
  if (frame_count < (MAX_FRAMES + 1)) {
    frame_index = 0;
    object = RwMalloc(OBJECT_SIZE,0);
    app_value = g_app_771030;
    *(int *)((char *)object + OBJECT_FRAME_COUNT_OFFSET) = frame_count;
    *(undefined4 *)((char *)object + OBJECT_OWNER_OFFSET) = owner;
    *(undefined4 *)((char *)object + 0x180) = 0;
    object_set_field_6c(owner,app_value);
    /* Create frame_count frames: each new frame is reset to identity and stored in slot frame_index (4 bytes per slot, from offset 0). */
    if (0 < frame_count) {
      do {
        frame = RwFrameCreate();
        *(RwFrame **)((char *)object + frame_index * 4) = frame;
        RwFrameSetIdentity(frame);
        frame_index = frame_index + 1;
      } while (frame_index < frame_count);
    }
    atomic_plugin_set_radius_squared(owner,&radius_squared);
    /* Trailing state words (inferred): clear 0x190 and 0x194, set 0x198 to 2. Meanings not known. */
    *(undefined4 *)((char *)object + 400) = 0;
    *(undefined4 *)((char *)object + 0x194) = 0;
    *(undefined4 *)((char *)object + 0x198) = 2;
    return object;
  }
  return (void *)0x0;
}


