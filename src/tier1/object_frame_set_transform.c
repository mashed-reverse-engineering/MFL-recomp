#include "mfl_tier2.h"
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);


#define FRAME_PTR_SIZE 4


/* object_frame_set_transform */

/* "(object, index, matrix): RwFrameTransform replace" */
/* Sets the transform matrix of the specified frame in an object, replacing its current transform. */
void __cdecl object_frame_set_transform(char *object,int frame_index,RwMatrix *matrix)
{
  RwFrameTransform(*(RwFrame **)(object + frame_index * FRAME_PTR_SIZE),matrix,rwCOMBINEREPLACE);
  return;
}
