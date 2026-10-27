// cluster-entries: rwp_trigger_group_advance_and_fire
/* Installed cluster rwp_trigger_group_advance_and_fire (tools/cluster_install.py from output/cluster_pilot_kb/rwp_trigger_group_advance_and_fire.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00408e80*/ extern int player_record_get_8d8ca8();
/*0040fbf0*/ extern int race_get_focus_player();
/*0042a3b0*/ extern int course_get_world_object();
/*0048e410*/ extern int rwp_object_find_index_by_id();
/*0048e650*/ extern int rwp_object_set_in_world();
/*00492d10*/ extern int rwp_object_set_active();
/*006c2ee0*/ extern undefined4 g_rwp_trigger_group_count;
/*006c2f64*/ extern undefined4 g_car_6c2f64[0x1000];
/*006c33c8*/ extern undefined4 g_gfx_6c33c8;
/* rwp_trigger_group_advance_and_fire */

/* "steps the active trigger group and fires its callback (table 0x6c33c8) when matching objects
   touch" */

/* Each group (0x23 ints: [0] current step, [1] step count, [2] pending flag; the step -> object-set pointers sit before it) advances one step when pending, fires the
   callback of the step it leaves (table at g_gfx_6c33c8, 0x84 byte stride, ids from 2000), then for every step's object set puts the objects into / out of the world
   (only the current step's objects, or ones shared with it, stay) and resets the set's per-entry records. */
void rwp_trigger_group_advance_and_fire(void)

{
  int step_or_set;
  undefined4 arg;
  int j;
  uint *rec4;
  int *cur_obj;
  bool in_current;
  uint is_current;
  int *obj_slot;
  undefined4 *rec9;
  int *group;
  struct {
    int obj_index;
    uint focus_value;
    int step_index;
    int group_index;
    undefined4 focus_player;
    undefined4 uStack_c;
    uint is_current_saved;
    undefined1 local_4 [4];
  } frame;
  
  frame.group_index = 0;
  race_get_focus_player(&frame.focus_player,frame.local_4);
  frame.focus_value = player_record_get_8d8ca8(frame.focus_player);
  if ((int)frame.focus_value < 0) {
    frame.focus_value = 0;
  }
  if (0 < (int)g_rwp_trigger_group_count) {
    group = g_car_6c2f64;
    do {
      if (group[2] != 0) {
        step_or_set = group[*group + -0x10];
        if (-1 < step_or_set + -2000) {
          arg = course_get_world_object();
          if ((code *)(&g_gfx_6c33c8)[(step_or_set + -2000) * 0x21] != (code *)0x0) {
            (*(code *)(&g_gfx_6c33c8)[(step_or_set + -2000) * 0x21])(arg);
          }
        }
        step_or_set = *group;
        *group = step_or_set + 1;
        if (group[1] <= step_or_set + 1) {
          *group = 0;
        }
      }
      group[2] = 0;
      frame.step_index = 0;
      if (0 < group[1]) {
        do {
          step_or_set = group[frame.step_index + -0x20];
          is_current = (uint)(group[*group + -0x20] == step_or_set);
          frame.obj_index = 0;
          frame.is_current_saved = is_current;
          if (0 < *(int *)(step_or_set + 0x144)) {
            obj_slot = (int *)(step_or_set + 0xc4);
            do {
              is_current = frame.is_current_saved;
              in_current = false;
              j = 0;
              frame.uStack_c = 0;
              if (0 < *(int *)(group[*group + -0x20] + 0x144)) {
                cur_obj = (int *)(group[*group + -0x20] + 0xc4);
                do {
                  if (*obj_slot == *cur_obj) {
                    in_current = true;
                    goto LAB_004808e2;
                  }
                  j = j + 1;
                  cur_obj = cur_obj + 1;
                } while (j < *(int *)(group[*group + -0x20] + 0x144));
                in_current = false;
              }
LAB_004808e2:
              if ((frame.is_current_saved != 0) || (in_current)) {
                rwp_object_set_in_world(*obj_slot,1);
                j = *obj_slot;
                arg = 1;
              }
              else {
                rwp_object_set_in_world(*obj_slot,0);
                j = *obj_slot;
                arg = 0;
              }
              rwp_object_set_active(j,arg);
              frame.obj_index = frame.obj_index + 1;
              obj_slot = obj_slot + 1;
            } while (frame.obj_index < *(int *)(step_or_set + 0x144));
          }
          j = 0;
          if (0 < *(int *)(step_or_set + 0x2428)) {
            rec4 = (uint *)(step_or_set + 0x14c);
            do {
              rec4[0x115] = 0;
              *rec4 = -(uint)(is_current != 0) & frame.focus_value;
              j = j + 1;
              rec4 = rec4 + 0x117;
            } while (j < *(int *)(step_or_set + 0x2428));
          }
          j = 0;
          if (0 < *(int *)(step_or_set + 0x250c)) {
            rec9 = (undefined4 *)(step_or_set + 0x2440);
            do {
              arg = rwp_object_find_index_by_id(rec9[-4]);
              rec9[-5] = arg;
              *rec9 = 1;
              rec9[1] = 0;
              j = j + 1;
              rec9 = rec9 + 7;
            } while (j < *(int *)(step_or_set + 0x250c));
          }
          frame.step_index = frame.step_index + 1;
        } while (frame.step_index < group[1]);
      }
      frame.group_index = frame.group_index + 1;
      group = group + 0x23;
    } while (frame.group_index < (int)g_rwp_trigger_group_count);
  }
  return;
}

