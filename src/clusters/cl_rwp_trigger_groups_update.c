// cluster-entries: rwp_trigger_groups_update
/* Installed cluster rwp_trigger_groups_update (tools/cluster_install.py from output/cluster_pilot_kd/rwp_trigger_groups_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00408e80*/ extern int player_record_get_8d8ca8();
/*00408ea0*/ extern int player_record_get_8d8ca0();
/*0040fbf0*/ extern int race_get_focus_player();
/*00480480*/ extern int course_moving_platforms_update();
/*004807c0*/ extern int rwp_trigger_group_advance_and_fire();
/*0048e3d0*/ extern int rwp_object_get_user_data();
/*0048e450*/ extern int rwp_object_get_type();
/*0048e6d0*/ extern int rwp_object_get_handle();
/*0058c8a0*/ extern int rwp_owner_object_get(int p0);
/*005dd61c*/ extern float k_f_0_001;
/*006c2ee0*/ extern undefined4 g_rwp_trigger_group_count;
/*006c2ee4*/ extern undefined4 g_car_6c2ee4[0x1000];
/* rwp_trigger_groups_update */

/* "per group tests contacts of the focus player (route record) against each trigger entry and
   fires" */

/* Per frame, for each trigger group: update its moving platforms, then test each trigger entry (matching focus-player record/progress, or objects of the wanted type whose user-data flag / owner speed matches) and set the trigger flag; fires rwp_trigger_group_advance_and_fire when the entry asks for it (inferred). */
void rwp_trigger_groups_update(undefined4 platform_update_arg, char *param_2, char *param_3, char *param_4, char *param_5, char *param_6, char *param_7, char *param_8, char *param_9, char *param_10, char *param_11, char *param_12, char *param_13)

{
  float vec_y;
  float vec_x;
  float vec_z;
  undefined4 object;
  int *owner_ref;
  int focus_record_or_i;
  uint user_data;
  int rwp_handle;
  float *vec;
  undefined4 *group;
  int *trigger;
  struct {
    int *type_ptr;
    undefined4 *object_ptr;
    undefined4 focus_player;
    int object_i;
    int group_handle;
    int trigger_i;
    int group_i;
    int focus_progress;
    int object_type;
    int focus_record;
    undefined1 focus_unused [4];
  } frame;
  
  frame.group_i = 0;
  race_get_focus_player(&frame.focus_player,frame.focus_unused);
  focus_record_or_i = player_record_get_8d8ca0(frame.focus_player);
  frame.focus_record = focus_record_or_i;
  frame.focus_progress = player_record_get_8d8ca8(frame.focus_player);
  if (0 < (int)g_rwp_trigger_group_count) {
    group = g_car_6c2ee4;
    do {
      rwp_handle = group[group[0x20]];
      frame.trigger_i = 0;
      frame.group_handle = rwp_handle;
      course_moving_platforms_update(platform_update_arg,rwp_handle);
      if (0 < *(int *)(rwp_handle + 0x2428)) {
        trigger = (int *)(rwp_handle + 0x5a0);
        do {
          frame.object_i = 0;
          if ((((-1 < trigger[-0x116]) && (*trigger == 0)) &&
              ((trigger[-0x114] == -1 || (frame.focus_progress - trigger[-0x115] == trigger[-0x114])))) &&
             (focus_record_or_i == trigger[-0x116])) {
            group[0x22] = 1;
            *trigger = 1;
            if (trigger[-0x109] != 0) {
LAB_00480e37:
              rwp_trigger_group_advance_and_fire();
              return;
            }
          }
          if (0 < *(int *)(rwp_handle + 0x144)) {
            frame.object_ptr = (undefined4 *)(rwp_handle + 0xc4);
            do {
              object = *frame.object_ptr;
              frame.object_type = rwp_object_get_type(object);
              focus_record_or_i = 0;
              if (0 < trigger[-0x10b]) {
                frame.type_ptr = trigger + -0x113;
                do {
                  if (*trigger != 0) break;
                  if (frame.object_type == *frame.type_ptr) {
                    user_data = rwp_object_get_user_data(object);
                    rwp_handle = trigger[-0x10a];
                    if (rwp_handle == 0) {
                      user_data = user_data & 4;
LAB_00480d8f:
                      if (user_data == 0) goto LAB_00480daf;
                    }
                    else {
                      if (rwp_handle == 1) {
                        user_data = user_data & 2;
                        goto LAB_00480d8f;
                      }
                      if ((((rwp_handle != 2) ||
                           (rwp_handle = rwp_object_get_handle(object), rwp_handle == 0)) ||
                          (rwp_handle = rwp_owner_object_get(rwp_handle), rwp_handle == 0)) ||
                         ((owner_ref = *(int **)(*(char **)(rwp_handle + 0x10) + 4), owner_ref == (int *)0x0
                          || (rwp_handle = *(int *)(*(char **)(*owner_ref + 0x10) + 8),
                             vec_y = *(float *)(owner_ref[1] * 0x20 + 4 + rwp_handle),
                             vec = (float *)(owner_ref[1] * 0x20 + rwp_handle), vec_x = *vec,
                             vec_z = vec[2],
                             !(vec_z * vec_z + vec_y * vec_y + vec_x * vec_x > k_f_0_001)))))
                      goto LAB_00480daf;
                    }
                    group[0x22] = 1;
                    *trigger = 1;
                    if (trigger[-0x109] != 0) goto LAB_00480e37;
                  }
LAB_00480daf:
                  focus_record_or_i = focus_record_or_i + 1;
                  frame.type_ptr = frame.type_ptr + 1;
                } while (focus_record_or_i < trigger[-0x10b]);
              }
              frame.object_ptr = frame.object_ptr + 1;
              frame.object_i = frame.object_i + 1;
              focus_record_or_i = frame.focus_record;
              rwp_handle = frame.group_handle;
            } while (frame.object_i < *(int *)(frame.group_handle + 0x144));
          }
          frame.trigger_i = frame.trigger_i + 1;
          trigger = trigger + 0x117;
        } while (frame.trigger_i < *(int *)(rwp_handle + 0x2428));
      }
      frame.group_i = frame.group_i + 1;
      group = group + 0x23;
    } while (frame.group_i < (int)g_rwp_trigger_group_count);
  }
  return;
}

