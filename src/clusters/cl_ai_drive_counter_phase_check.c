// cluster-entries: ai_drive_counter_phase_check
/* Installed cluster ai_drive_counter_phase_check (tools/cluster_install.py from output/cluster_pilot_uc3/ai_drive_counter_phase_check.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0041a370*/ extern float ai_get_counter(int index);
/*0047b520*/ extern void car_current_matrix(float **matrix, int car);
/*005dd330*/ extern float float_1_0;
/*005dd364*/ extern float k_f_n1;
/*005de25c*/ extern float k_f_0_005;
/*005de270*/ extern float k_f_n0_1;

/* Phase (counter `counter` minus counter 0, wrapped into -1..1) of an AI counter: when it is ahead by more than 0.005 or behind by more than 0.1 the car's x/z position is written to out[0..1] and 1 is returned, else 0. unused1/unused2 are never read. */
int ai_drive_counter_phase_check(int unused1, int unused2, float *out, int counter)
{
  float *matrix;
  float phase;

  phase = ai_get_counter(counter) - ai_get_counter(0);
  while (phase > float_1_0) {
    phase = phase - float_1_0;
  }
  while (phase < k_f_n1) {
    phase = phase + float_1_0;
  }
  if (!(phase > k_f_0_005) && !(phase < k_f_n0_1)) {
    return 0;
  }
  car_current_matrix(&matrix, counter);
  out[0] = matrix[12];
  out[1] = matrix[14];
  return 1;
}

