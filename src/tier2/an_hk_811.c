#include "mfl_tier2.h"
/*00484c40*/ extern undefined __cdecl spline_tween_b(RwMatrix *p1, int p2, float p3);
/*004c48b0*/ extern void FUN_004c48b0(void *p0, undefined4 p1, undefined4 p2);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
#define OWNER_FRAME_OFFSET (4)
#define PATH_LENGTH_OFFSET (0xc)
/* frame_follow_path_step */

/*
 * Advances one path-following object by time_step and moves its frame along the path.
 *   owner_or_time: on entry, a non-null owner whose word at +OWNER_FRAME_OFFSET is the RwFrame to move
 *                  (inferred: the null test is a float compare, so the pointer bits are passed in here);
 *                  overwritten with the new path time, so it is not a time before the wrap.
 *   path:          path object (int); the float at +PATH_LENGTH_OFFSET is the path length (inferred).
 *   path_time:     in/out path time; receives the new time.
 *   time_step:     amount added to *path_time this step.
 *   tween_mode:    0 -> spline_tween_b(matrix, path, time); nonzero -> FUN_004c48b0 with the time bits (inferred: alternate tween).
 * Time wraps by a single subtraction of the path length, so a step longer than the length is not fully wrapped (inferred).
 * The tween result is applied to the frame with RwFrameTransform(..., rwCOMBINEREPLACE).
 * (was: "advances time with wrap, interpolates the path (spline_tween_b) and sets the frame matrix")
 */

void frame_follow_path_step(float owner_or_time,int path,float *path_time,float time_step,int tween_mode)

{
  RwFrame *frame;
  RwMatrix tween_matrix;

  if ((owner_or_time != 0.0) && (path != 0)) {
    /* frame is the RwFrame stored at +OWNER_FRAME_OFFSET of the owner (inferred) */
    frame = *(RwFrame **)(*(int *)&owner_or_time + OWNER_FRAME_OFFSET);
    /* advance time; wrap once if it passes the path length */
    owner_or_time = time_step + *path_time;
    if (*(float *)(path + PATH_LENGTH_OFFSET) < owner_or_time) {
      owner_or_time = owner_or_time - *(float *)(path + PATH_LENGTH_OFFSET);
    }
    /* build the matrix for this time: spline by default, alternate tween when tween_mode != 0 */
    if (tween_mode == 0) {
      spline_tween_b(&tween_matrix,path,owner_or_time);
    }
    else {
      FUN_004c48b0(&tween_matrix,path,*(int *)&owner_or_time);
    }
    *path_time = owner_or_time;
    RwFrameTransform(frame,&tween_matrix,rwCOMBINEREPLACE);
  }
  return;
}


