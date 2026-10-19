#include "mfl_tier2.h"
/*007106e0*/ extern struct Trigger g_triggers[300];
/*0071aaf4*/ extern undefined4 g_trigger_count;
/* struct Trigger is 0x8c bytes = 35 floats: step between two consecutive triggers */
#define TRIGGER_STRIDE_FLOATS (0x23)
/* triggers_sort_axis */

/* Overlap sweep over all pairs of triggers g_triggers[0 .. g_trigger_count-1] (a = lower index, b = higher index).
   Takes no parameters, returns nothing. No sorting happens here, despite the name
   (was: "sort of the triggers along an axis for the sweep (inferred)").
   A pair is tested only if the two triggers have different type. Overlap is a sphere test:
   squared centre distance below (radius_a + radius_b)^2. On overlap, a's callback is called as (a, b)
   and b's callback as (b, a), each only if that trigger has one set. */

void triggers_sort_axis(void)

{
  code *a_callback;
  float dx;
  uint trigger_count;
  float *a_y;
  float *b_y;
  bool outer_again;
  struct {
    uint b_index;
    uint b_start;
    float *b_start_y;
  } frame;

  if (g_trigger_count != 0) {
    /* a_y / b_y are float pointers at the y field of the trigger: a_y[-1] is x, a_y[1] is z,
       a_y[2] is the radius, a_y[3] is type, a_y[6] is callback (same for b_y) */
    a_y = g_triggers[0].position_radius + 1;
    frame.b_start = 1;
    frame.b_start_y = g_triggers[1].position_radius + 1;
    trigger_count = g_trigger_count;
    /* outer loop: a is trigger b_start-1, b runs over b_start .. count-1 */
    do {
      b_y = frame.b_start_y;
      frame.b_index = frame.b_start;
      if (frame.b_start < trigger_count) {
        /* inner loop: test a against each b from b_start to the last trigger */
        do {
          /* only pairs of different type, and only if a or b has a callback */
          if ((a_y[3] != b_y[3]) &&
             ((a_callback = *(code **)&a_y[6], a_callback != (code *)0x0 || (b_y[6] != 0.0)))) {
            dx = ((Trigger *)(a_y + -1))->position_radius[0] -
                    ((Trigger *)(b_y + -1))->position_radius[0];
            /* sphere overlap: squared centre distance < (r_a + r_b)^2 */
            if ((a_y[1] - b_y[1]) * (a_y[1] - b_y[1]) +
                dx * dx + (*a_y - *b_y) * (*a_y - *b_y) <
                (a_y[2] + b_y[2]) * (a_y[2] + b_y[2])) {
              /* fire a's callback as (a, b) */
              if (a_callback != (code *)0x0) {
                (*a_callback)((Trigger *)(a_y + -1),(Trigger *)(b_y + -1));
              }
              /* fire b's callback as (b, a) */
              if (*(code **)&b_y[6] != (code *)0x0) {
                (**(code **)&b_y[6])((Trigger *)(b_y + -1),(Trigger *)(a_y + -1));
              }
            }
          }
          frame.b_index = frame.b_index + 1;
          trigger_count = g_trigger_count;
          b_y = b_y + TRIGGER_STRIDE_FLOATS;
        } while (frame.b_index < g_trigger_count);
      }
      a_y = a_y + TRIGGER_STRIDE_FLOATS;
      frame.b_start_y = frame.b_start_y + TRIGGER_STRIDE_FLOATS;
      outer_again = frame.b_start < trigger_count;
      frame.b_start = frame.b_start + 1;
    } while (outer_again);
  }
  return;
}


