#include "mfl_tier2.h"
/*004a5c20*/ extern int object_frames_create();


/* object_frames_create_with_matrices */

/* "(base, count, matrices): object_frames_create and sets the transform of every frame" */

undefined4 object_frames_create_with_matrices(undefined4 base,int count,int matrix)

{
  undefined4 frames;
  int frame_index;
  
  frames = object_frames_create(base,count);
  frame_index = 0;
  if (0 < count) {
    do {
      object_frame_set_transform(frames,frame_index,matrix);
      matrix = matrix + 0x40;
      frame_index = frame_index + 1;
    } while (frame_index < count);
  }
  return frames;
}
