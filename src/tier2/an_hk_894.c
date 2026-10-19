#include "mfl_tier2.h"
/*004867f0*/ extern int particles_set_position();
/*00486820*/ extern int particles_set_size();
/*00486840*/ extern int particles_set_color();
/*00486880*/ extern int gfx_set_gfx_6c50c8();
/*00486890*/ extern int gfx_set_params_6c5188();
/*00486b50*/ extern int particles_add();
/*005dd32c*/ extern float k_f_3;
/*005dd330*/ extern float float_1_0;
/*005dd33c*/ extern float k_f_4;
/*005dd340*/ extern float k_f_0_5;
/*005dd41c*/ extern float float_5_0;

/* ribbon header: int-indexed slots */
#define RIBBON_FLAGS_IDX (0x14)
#define RIBBON_SAMPLE_STEP_IDX (0x17)
#define RIBBON_PHASE_IDX (0x18)
#define RIBBON_PHASE_RATE_IDX (0x19)
#define RIBBON_POINT_COUNT_IDX (2)
#define RIBBON_STRIP_COUNT_IDX (3)
/* RIBBON_FLAGS_IDX bits: which per-point fields get emitted */
#define RIBBON_FLAG_SIZE (2)
#define RIBBON_FLAG_COLOR (4)
#define RIBBON_FLAG_GFX (0x10)
#define RIBBON_FLAG_PARAMS (0x20)
/* ribbon point: byte offsets inside one point record */
#define RIBBON_POINT_STRIDE (0x30)
#define RIBBON_POINT_POS_OFF (4)
#define RIBBON_POINT_GFX_OFF (0x18)
#define RIBBON_POINT_FLAG_OFF (0x1c)
#define RIBBON_POINT_PARAMS_OFF (0x20)
/* fx_ribbon_flush_spline */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/*
 * Emits particles along a ribbon: a chain of points, each 0x30 bytes, starting at ribbon[0].
 * Consecutive unflagged points form a run; each segment of a run is a Catmull-Rom curve
 * through four neighbouring points, sampled from the phase up to 1.0 in steps of sample_step.
 * At every sample it sets the particle position, plus size, colour, gfx id and params when
 * the matching RIBBON_FLAG_* bit is set in ribbon[RIBBON_FLAGS_IDX], then calls particles_add.
 * ribbon: the ribbon header (see RIBBON_*_IDX). Advances the phase in ribbon[RIBBON_PHASE_IDX]
 * by ribbon[RIBBON_PHASE_RATE_IDX] and resets it to 0 once it passes sample_step (inferred).
 * (was: "smooths the ribbon points with Catmull-Rom and writes the segments into the vertex batch";
 * the callees are particles_* and gfx_*, not a vertex batch)
 */

void fx_ribbon_flush_spline(int *ribbon)

{
  float10 t_sq;
  float10 sample_step;
  float10 phase_start;
  int points_base;
  float10 t_cu;
  int point_off;
  undefined4 *point;
  int *flag_ptr;
  float *ctrl_next2;
  int point_index;
  int run_len;
  float *ctrl_prev;
  float *ctrl_next;
  int run_start;
  float *ctrl_cur;
  struct {
    float t;
    int run_pos;
    int run_end;
    int resume_index;
  } walk;
  struct {
    int strip_index;
    undefined1 pad_24[4];
    undefined4 color;
    float size_x;
    float size_y;
    float pos_x;
    float pos_y;
    float pos_z;
  } emit;

  sample_step = *(float *)&ribbon[RIBBON_SAMPLE_STEP_IDX];
  /* t_sq holds the old phase here; it is reused as t squared in the sample loop */
  t_sq = *(float *)&ribbon[RIBBON_PHASE_IDX];
  phase_start = *(float *)&ribbon[RIBBON_PHASE_IDX];
  *(float *)&ribbon[RIBBON_PHASE_IDX] = (*(float *)&ribbon[RIBBON_PHASE_RATE_IDX] + t_sq);
  run_start = 0;
  emit.strip_index = 0;
  walk.resume_index = 0;
  if (sample_step < *(float *)&ribbon[RIBBON_PHASE_RATE_IDX] + t_sq) {
    ribbon[RIBBON_PHASE_IDX] = 0;
  }
  /* one pass per strip (inferred), each walking the whole point chain */
  if (0 < ribbon[RIBBON_STRIP_COUNT_IDX]) {
    do {
      /* points_base is the point count here, the search index below, then the points base */
      points_base = ribbon[RIBBON_POINT_COUNT_IDX];
      walk.run_end = points_base + -1;
      run_len = points_base + -2;
      walk.run_pos = 0;
      if ((points_base < 2) || (0 < run_len)) {
        if (0 < run_len) goto LAB_0049ba40;
      }
      else {
        run_len = 1;
LAB_0049ba40:
        do {
          points_base = walk.run_end;
          /* find where this run ends: the first flagged point at or after run_start, else the last point */
          if (run_start < ribbon[RIBBON_POINT_COUNT_IDX]) {
            flag_ptr = (int *)(run_start * RIBBON_POINT_STRIDE + RIBBON_POINT_FLAG_OFF + *ribbon);
            point_off = run_start;
            do {
              points_base = point_off;
              if (*flag_ptr != 0) break;
              point_off = point_off + 1;
              flag_ptr = flag_ptr + 0xc;
              points_base = walk.run_end;
            } while (point_off < ribbon[RIBBON_POINT_COUNT_IDX]);
          }
          walk.run_end = points_base;
          points_base = *ribbon;
          point_index = walk.run_pos + run_start;
          point_off = point_index * RIBBON_POINT_STRIDE;
          /* unflagged point: emit its spline segment; flagged point: the run ends here */
          if (*(int *)(point_off + RIBBON_POINT_FLAG_OFF + points_base) == 0) {
            run_len = walk.run_end - run_start;
            /* Catmull-Rom control points: ctrl_prev, ctrl_cur, ctrl_next, ctrl_next2 (4 consecutive
               point positions; the first and last segment of a run repeat an end point) */
            ctrl_prev = (float *)(point_off + -0x2c + points_base);
            ctrl_next2 = (float *)(point_off + 100 + points_base);
            ctrl_cur = (float *)(point_off + RIBBON_POINT_POS_OFF + points_base);
            if (walk.run_pos == 0) {
              ctrl_prev = (float *)(walk.resume_index * RIBBON_POINT_STRIDE + RIBBON_POINT_POS_OFF + points_base);
              ctrl_cur = ctrl_prev;
            }
            ctrl_next = (float *)(point_off + 0x34 + points_base);
            if (walk.run_pos == run_len + -1) {
              ctrl_next2 = (float *)(walk.run_end * RIBBON_POINT_STRIDE + RIBBON_POINT_POS_OFF + *ribbon);
              ctrl_next = ctrl_next2;
            }
            if (point_index == walk.run_end + -1) {
              walk.resume_index = walk.run_end + 1;
            }
            walk.t = phase_start;
            /* sample t from phase_start up to 1.0 in steps of sample_step */
            if (phase_start < float_1_0) {
              do {
                t_sq = walk.t * walk.t;
                t_cu = walk.t * t_sq;
                /* point[0..2] is colour, point[4..5] size, point[0x10..0x11] the next point's size
                   (dword indices); the size lerps from this point to the next by t */
                emit.pos_x = (*ctrl_cur + *ctrl_cur +
                           (*ctrl_next - *ctrl_prev) * walk.t +
                           (((*ctrl_cur * k_f_3 - *ctrl_prev) - *ctrl_next * k_f_3) + *ctrl_next2) * t_cu +
                           ((*ctrl_next * k_f_4 + ((*ctrl_prev + *ctrl_prev) - *ctrl_cur * float_5_0)) -
                           *ctrl_next2) * t_sq) * k_f_0_5;
                emit.pos_y = (ctrl_cur[1] + ctrl_cur[1] +
                           (ctrl_next[1] - ctrl_prev[1]) * walk.t +
                           (((ctrl_cur[1] * k_f_3 - ctrl_prev[1]) - ctrl_next[1] * k_f_3) + ctrl_next2[1]) *
                           t_cu + ((ctrl_next[1] * k_f_4 +
                                    ((ctrl_prev[1] + ctrl_prev[1]) - ctrl_cur[1] * float_5_0)) -
                                   ctrl_next2[1]) * t_sq) * k_f_0_5;
                point = (undefined4 *)(point_off + *ribbon);
                emit.pos_z = (ctrl_cur[2] + ctrl_cur[2] +
                          (ctrl_next[2] - ctrl_prev[2]) * walk.t +
                          (((ctrl_cur[2] * k_f_3 - ctrl_prev[2]) - ctrl_next[2] * k_f_3) + ctrl_next2[2]) *
                          t_cu + ((ctrl_next[2] * k_f_4 +
                                   ((ctrl_prev[2] + ctrl_prev[2]) - ctrl_cur[2] * float_5_0)) -
                                  ctrl_next2[2]) * t_sq) * k_f_0_5;
                emit.color = *point;
                emit.size_x = *(float *)&point[4] - (*(float *)&point[4] - *(float *)&point[0x10]) * walk.t;
                emit.size_y = *(float *)&point[5] - (*(float *)&point[5] - *(float *)&point[0x11]) * walk.t;
                particles_set_position(&emit.pos_x);
                if ((*(byte *)(ribbon + RIBBON_FLAGS_IDX) & RIBBON_FLAG_SIZE) != 0) {
                  particles_set_size(&emit.size_x);
                }
                if ((*(byte *)(ribbon + RIBBON_FLAGS_IDX) & RIBBON_FLAG_PARAMS) != 0) {
                  gfx_set_params_6c5188(point_off + RIBBON_POINT_PARAMS_OFF + *ribbon);
                }
                if ((*(byte *)(ribbon + RIBBON_FLAGS_IDX) & RIBBON_FLAG_GFX) != 0) {
                  gfx_set_gfx_6c50c8(*(undefined4 *)(point_off + RIBBON_POINT_GFX_OFF + *ribbon));
                }
                if ((*(byte *)(ribbon + RIBBON_FLAGS_IDX) & RIBBON_FLAG_COLOR) != 0) {
                  particles_set_color(&emit.color);
                }
                /* ribbon + 4: particle emitter state passed to particles_add (inferred) */
                particles_add(ribbon + 4);
                walk.t = sample_step + walk.t;
              } while (walk.t < float_1_0);
            }
            walk.run_pos = walk.run_pos + 1;
          }
          else {
            walk.resume_index = point_index + 1;
            walk.run_pos = 0;
          }
          run_start = walk.resume_index;
        } while (walk.run_pos < run_len);
      }
      emit.strip_index = emit.strip_index + 1;
    } while (emit.strip_index < ribbon[RIBBON_STRIP_COUNT_IDX]);
  }
  return;
}


