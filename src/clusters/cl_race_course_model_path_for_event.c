// cluster-entries: race_course_model_path_for_event
/* Installed cluster race_course_model_path_for_event (tools/cluster_install.py from output/cluster_pilot_uc3/race_course_model_path_for_event.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00434bb0*/ extern int get_event_type(void);
/*004379a0*/ extern int menu_event_course_index_get(void);
/*0050b5e0*/ extern void nop_stub(void);
/*004231b0*/ extern int vehicle_model_path(int model_base);
/*0042fa80*/ extern int course_content_path_build(int path);
/*004a43d0*/ extern void game_open_piz(int path);
/*00423320*/ extern void vehicle_load(int slot, int model);
/*0041d950*/ extern void load_ghost_model(int *course_ids, unsigned char *colours);
/*00425450*/ extern void load_debris_model(int *models);
/*004a4440*/ extern void game_close_piz(void);

extern PlayerSlot g_player_slots[4];
typedef struct EventEntry { int field_0; int course; } EventEntry;

/* Loads the models of an event from the piz archive: the model base is (course number / 6) * 6 of the event `index` of `events` (menu selected entry for
   the event types 6-8 and 0xb-0xd, 12 for type 0xe), the four players' models are team + base (player number + base for event type 0x*12), the ghost model
   gets the course ids and two colours. The archive stays open only during the loading. */
void race_course_model_path_for_event(EventEntry **events, int index)
{
  unsigned char colours[8] = { 0xc8, 0xc8, 0xc8, 0x80, 0xff, 0xff, 0x00, 0x80 };   /* two RGBA colours */
  int course_ids[2] = { 0, 0 };
  int models[4] = { 0, 0, 0, 0 };
  int type;
  int base;
  int i;

  base = events[index + 1]->course / 6;
  type = get_event_type();
  if (type == EVENT_QUICK_RACE_11 || get_event_type() == EVENT_QUICK_RACE_12 || get_event_type() == EVENT_QUICK_RACE_13) {
    base = events[menu_event_course_index_get() + 1]->course / 6;
  }
  if (get_event_type() == EVENT_MULTIPLAYER_6 || get_event_type() == EVENT_MULTIPLAYER_7 || get_event_type() == EVENT_MULTIPLAYER_8) {
    base = events[menu_event_course_index_get() + 1]->course / 6;
  }
  if (get_event_type() == EVENT_TURBO_TOURNAMENT) {
    base = 12;
  }
  base = base * 6;
  nop_stub();
  game_open_piz(course_content_path_build(vehicle_model_path(base)));
  for (i = 0; i < 4; i++) {
    int model = g_player_slots[i].team + base;
    if (get_event_type() == EVENT_QUICK_RACE) {
      model = i + base;
    }
    models[i] = model;
    vehicle_load(i, model);
  }
  course_ids[0] = base;
  course_ids[1] = base;
  load_ghost_model(course_ids, colours);
  load_debris_model(models);
  game_close_piz();
}

