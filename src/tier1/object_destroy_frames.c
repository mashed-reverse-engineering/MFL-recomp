#include "mfl_tier2.h"
/*004a6b30*/ extern int object_set_field_6c();
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;


/* object_destroy_frames */

/* "clears the atomic render state and destroys the frames" */
/* obj+0x184: passed to object_set_field_6c (meaning unknown); obj+0x188: frame count; obj[0..count-1]: RwFrame pointers */

void object_destroy_frames(char *obj)
{
  int frame_index;

  object_set_field_6c(*(undefined4 *)(obj + 0x184),0);
  frame_index = 0;
  if (0 < *(int *)(obj + 0x188)) {
    do {
      RwFrameDestroy(*(RwFrame **)(obj + frame_index * 4));
      frame_index = frame_index + 1;
    } while (frame_index < *(int *)(obj + 0x188));
  }
  /* last step: engine callback at RwEngineInstance+0x10c, called with obj (what it does: unknown) */
  RwFree(obj);
  return;
}
