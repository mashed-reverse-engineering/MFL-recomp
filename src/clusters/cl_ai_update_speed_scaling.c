// cluster-entries: ai_update_speed_scaling
/* Installed cluster ai_update_speed_scaling (tools/cluster_install.py from output/cluster_pilot_uc3/ai_update_speed_scaling.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/*008c9ae0*/ extern struct AiBotRecord g_ai_bots[4];
/*005ddc28*/ extern float k_f_0;
/*005dd338*/ extern float k_f_0_01;
/*005ddb00*/ extern float k_f_0_025;
/*005dd378*/ extern float k_f_0_1;
/*005dd9c4*/ extern float k_f_0_15;
/*005dd9c8*/ extern float k_f_0_16666667;
/*005dd628*/ extern float k_f_0_25;
/*005ddc1c*/ extern float k_f_0_33333334;
/*005dda24*/ extern float k_f_0_75;
/*005dd418*/ extern float k_f_12;
/*005dd634*/ extern float k_f_2;
/*005ddea0*/ extern float k_f_20;
/*005de268*/ extern float k_f_22;
/*005dd32c*/ extern float k_f_3;
/*005dd33c*/ extern float k_f_4;
/*005de264*/ extern float k_f_42;
/*005dd814*/ extern float k_f_6;
/*005dd7e8*/ extern float k_f_60;
/*005de260*/ extern float k_f_62;
/*005ddb50*/ extern float k_f_8;
/*005dd364*/ extern float k_f_n1;

extern float k_f_0_05, k_f_0_5, k_f_0_9, k_f_0_95, k_f_1_1, k_f_1_2, k_f_1_25, k_f_1_5, k_f_2_5, k_f_3_3333333en4;
/* DECLS-END */
extern float car_get_6292ec(void);
extern int car_set_field_154(int slot, float v);
extern float player_progress_b(int slot);
extern float ai_get_counter(int slot);
extern float player_distance_get(int slot);
extern float random_range(float lo, float hi);
extern int player_record_get_8d8ca8(int slot);
extern int weapon_slot_6a5d24_get(int slot);
extern int race_setup_get_slot_type(int slot);
extern int training_lesson_get(void);
extern int ui_get_car_8c8788(void);
extern int car_get_mode(int slot, int *a, int *b);
extern int car_is_active(int slot);
extern int camera_get_car_8c747c(void);
extern int get_event_type(void);
extern int current_course_id(void);
extern int race_get_phase(void);
extern int game_mode, g_ai_catchup_class, g_car_8c998c, g_car_8c9994, g_tick_count, g_race_clock_b;
extern float g_ai_player_counters[4], g_ai_leader_slots, g_car_8c9eb4, g_car_8c9eb8, g_car_8c9ebc, g_ai_skill, g_car_8c9990;
extern int g_ai_update_603c70[], g_ai_update_603d50[];
extern float float_1_0, float_10_0;

/* Per frame speed factor of the cars (car_set_field_154: car base speed times a factor): the leader slots by progress, the rubber banding of the bots in modes 4 / 9
   (counter of the player against the skill), the doubled speed of cars with an opponent weapon, the tutorial lessons of event type 0x11, and the catch-up class
   (changed over time and by random draws from a table indexed by skill and race time slot). */
void ai_update_speed_scaling(void)
{
  int i;
  int cat;
  int skill;
  int mode;
  int weapons;
  int lesson;
  int *bot;
  int a;
  int b;
  int idx;
  int thr;
  int flag;
  float f;
  float factor;
  float c;
  float cls;
  float limit;
  float d;
  double sum;
  double t;
  double diff;

  if (camera_get_car_8c747c() != 0) {
    g_ai_catchup_class = 2;
  }
  if (get_event_type() == EVENT_TRAINING) {
    g_ai_catchup_class = 2;
  }
  for (i = 0; i < 4; i++) {
    car_set_field_154(i, car_get_6292ec());
    f = (float)player_record_get_8d8ca8(i);
    sum = player_progress_b(i) * k_f_0_01 + f;
    g_ai_player_counters[i] = (float)sum;
    mode = game_mode;
    if ((mode == 4 || mode == 9 || mode == 7 || mode == 8) && sum >= k_f_3) {
      f = (float)i;
      if (g_ai_leader_slots != f && g_car_8c9eb4 != f && g_car_8c9eb8 != f && g_car_8c9ebc != f) {
        if (g_ai_leader_slots == k_f_n1) {
          g_ai_leader_slots = f;
        }
        else if (g_car_8c9eb4 == k_f_n1) {
          g_car_8c9eb4 = f;
        }
        else if (g_car_8c9eb8 == k_f_n1) {
          g_car_8c9eb8 = f;
        }
        else if (g_car_8c9ebc == k_f_n1) {
          g_car_8c9ebc = f;
        }
      }
    }
  }
  if (mode == 9) {
    factor = 1.0f;
    c = ai_get_counter(1);
    if (c < k_f_1_5) {
      factor = 0.95f;
    }
    if (c < k_f_0_75) {
      factor = 0.9f;
    }
    limit = 1.15f;
    d = (float_10_0 - g_ai_skill) * k_f_0_05;
    if (current_course_id() == 5) {
      d = d * k_f_2_5;
      limit = 1.45f;
    }
    if (k_f_1_5 - d < c) {
      factor = limit;
    }
    if (c < k_f_0_025) {
      factor = 0.25f;
    }
    car_set_field_154(1, car_get_6292ec() * factor);
    mode = game_mode;
  }
  if (mode == 4) {
    for (i = 1; i < 4; i++) {
      factor = 1.0f;
      c = ai_get_counter(i);
      if (c > k_f_0_75) {
        factor = (float_10_0 - g_ai_skill) * k_f_0_1 * k_f_0_16666667 + float_1_0;
      }
      if (c > k_f_2) {
        factor = (float_10_0 - g_ai_skill) * k_f_0_1 * k_f_0_33333334 + factor;
      }
      if (c < k_f_0_5) {
        factor = factor - k_f_0_15;
      }
      if (c < k_f_0_05) {
        factor = factor - k_f_0_25;
      }
      car_set_field_154(i, car_get_6292ec() * factor);
    }
  }
  weapons = 0;
  for (i = 0; i < 4; i++) {
    if (weapon_slot_6a5d24_get(i) != 0) {
      weapons = 1;
    }
  }
  if (weapons != 0) {
    for (i = 0; i < 4; i++) {
      if (race_setup_get_slot_type(i) == 2) {
        car_set_field_154(i, (double)car_get_6292ec() * 2);
      }
    }
  }
  if (get_event_type() == EVENT_TRAINING) {
    if (training_lesson_get() == 0) {
      car_set_field_154(1, (double)car_get_6292ec() * 2);
    }
    if (training_lesson_get() == 2) {
      d = player_distance_get(1);
      if (d <= k_f_4) {
        car_set_field_154(1, car_get_6292ec() * k_f_1_1);
      }
      if (d <= k_f_2) {
        car_set_field_154(1, car_get_6292ec() * k_f_1_25);
      }
      if (d == k_f_0) {
        car_set_field_154(1, car_get_6292ec() * k_f_1_5);
      }
      if (d > k_f_0) {
        car_set_field_154(1, car_get_6292ec() * k_f_0_95);
      }
    }
    if (training_lesson_get() == 3) {
      lesson = ui_get_car_8c8788();
      if (lesson == 1) {
        if (player_distance_get(1) <= k_f_6) {
          car_set_field_154(1, car_get_6292ec() * k_f_1_2);
        }
      }
      else if (lesson == 2) {
        if (player_distance_get(0) > k_f_6) {
          car_set_field_154(1, car_get_6292ec() * k_f_1_2);
        }
      }
      else if (lesson == 3) {
        if (player_distance_get(1) <= k_f_8) {
          car_set_field_154(1, car_get_6292ec() * k_f_1_2);
        }
      }
      else if (lesson == 4) {
        if (player_distance_get(1) >= k_f_0) {
          car_set_field_154(1, car_get_6292ec() * k_f_0_9);
        }
        if (player_distance_get(0) >= k_f_0) {
          car_set_field_154(1, car_get_6292ec() * k_f_1_1);
        }
      }
    }
    if (training_lesson_get() == 5) {
      car_get_mode(0, &a, &b);
      b = car_is_active(0);
      for (i = 1; i < 4; i++) {
        if (a == 0 && b != 0) {
          car_set_field_154(i, car_get_6292ec() * k_f_0_5);
          car_set_field_154(0, car_get_6292ec() * k_f_1_25);
        }
        else {
          car_set_field_154(i, car_get_6292ec() * k_f_1_5);
        }
      }
    }
  }
  if (game_mode == GAME_MODE_RACE) {
    g_ai_catchup_class = 2;
    return;
  }
  if (game_mode == GAME_MODE_CHASE_FUGITIVE) {
    g_ai_catchup_class = 2;
    return;
  }
  if (game_mode == GAME_MODE_SURVIVAL) {
    g_ai_catchup_class = 0;
    return;
  }
  if (race_get_phase() != RACE_PHASE_RACING) {
    return;
  }
  if (g_ai_catchup_class == 2) {
    g_car_8c998c = g_car_8c998c + g_tick_count;
    if (g_car_8c998c > 180000) {
      g_ai_catchup_class = 1;
      g_car_8c998c = 0;
    }
  }
  skill = (int)g_ai_skill;
  flag = 0;
  t = (double)g_race_clock_b * k_f_3_3333333en4;
  diff = t - g_car_8c9990;
  if (diff > float_1_0 && t < k_f_2) {
    cat = 0;
    flag = 1;
  }
  if (diff > float_10_0) {
    if (t < k_f_12) {
      cat = 1;
      flag = 1;
    }
    if (t > k_f_20 && t < k_f_22) {
      cat = 2;
      flag = 1;
    }
    if (diff > k_f_20) {
      if (t < k_f_42) {
        cat = 3;
        flag = 1;
      }
      if (diff > k_f_20 && t > k_f_60 && t < k_f_62) {
        cat = 4;
        flag = 1;
      }
    }
  }
  if (flag != 0) {
    g_car_8c9990 = (float)t;
    g_car_8c9994 = cat;
    bot = &g_ai_bots[0].catchup_cycles;
    do {
      bot[1] = 0;
      *bot = 0;
      bot = bot + 0x1e;
    } while ((int)bot < 0x8c9cc4);
  }
  if (g_ai_catchup_class != 0) {
    return;
  }
  if (flag == 0) {
    return;
  }
  idx = (cat + skill * 5) * 4;
  thr = *(int *)((char *)g_ai_update_603c70 + idx);
  if (random_range(0.0f, 100.0f) <= (float)thr) {
    g_ai_catchup_class = 1;
    for (i = 0; i < 4; i++) {
      g_ai_bots[i].base_value = 0;
    }
    return;
  }
  thr = *(int *)((char *)g_ai_update_603d50 + idx);
  if (random_range(0.0f, 100.0f) <= (float)thr) {
    g_ai_catchup_class = 2;
  }
}

