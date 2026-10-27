// cluster-entries: triggers_update
/* Installed cluster triggers_update (tools/cluster_install.py from output/cluster_pilot_kb/triggers_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0062cdc0*/ extern undefined over;
/*007106e0*/ extern struct Trigger g_triggers[300];
/*0071aaf4*/ extern undefined4 g_trigger_count;
/* triggers_update */

/* "broadphase / narrow phase over all active triggers and callbacks (inferred)" */

void triggers_update(void)

{
  byte *seen_bits;
  short other_index;
  int cell;
  float *other;
  code *trigger_cb;
  float dx;
  float radius_sum;
  float dz;
  float dy;
  int byte_index;
  uint *cell_ptr;
  Trigger *trigger;
  uint trigger_i;
  int object_i;
  uint cells_left;
  
  trigger_i = 0;
  if (g_trigger_count != 0) {
    trigger = g_triggers;
    do {
      cells_left = trigger->cell_count;
      if (cells_left != 0) {
        cell_ptr = trigger->cells + cells_left;
        do {
          cell = cell_ptr[-1];
          cell_ptr = cell_ptr + -1;
          if (((*(int *)(cell + 0x1b0) == 1) && (*(int *)(cell + 0x1b4) == 0)) &&
             (object_i = 0, 0 < *(int *)(cell + 400))) {
            do {
              other = *(float **)(cell + object_i * 4);
              other_index = *(short *)(other + 8);
              if (((((int)trigger_i < (int)other_index) && (trigger->type != *(uint *)&other[4])) &&
                  ((trigger_cb = (code *)trigger->callback, trigger_cb != (code *)0x0 || (*(int *)&other[7] != 0))
                  )) && (((*(byte *)((int)trigger->cells + ((int)other_index >> 3) + 0x44) &
                          (byte)(1 << ((byte)other_index & 7))) == 0 &&
                         (dx = trigger->position_radius[0] - *other,
                         dy = trigger->position_radius[1] - other[1],
                         dz = trigger->position_radius[2] - other[2],
                         radius_sum = trigger->position_radius[3] + other[3],
                         dz * dz + dx * dx + dy * dy < radius_sum * radius_sum)))) {
                if (trigger_cb != (code *)0x0) {
                  (*trigger_cb)(trigger,other);
                }
                if (*(code **)&other[7] != (code *)0x0) {
                  (**(code **)&other[7])(other,trigger);
                }
                seen_bits = (byte *)((int)trigger->cells + ((int)*(short *)(other + 8) >> 3) + 0x44);
                *seen_bits = *seen_bits | '\x01' << ((byte)*(short *)(other + 8) & 7);
                byte_index = (int)(short)trigger->index >> 3;
                *(byte *)(byte_index + 0x68 + (int)other) =
                     *(byte *)(byte_index + 0x68 + (int)other) | '\x01' << ((byte)trigger->index & 7);
              }
              object_i = object_i + 1;
            } while (object_i < *(int *)(cell + 400));
          }
          cells_left = cells_left - 1;
        } while (cells_left != 0);
      }
      trigger_i = trigger_i + 1;
      trigger = trigger + 1;
    } while (trigger_i < g_trigger_count);
  }
  return;
}

