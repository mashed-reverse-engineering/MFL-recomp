#include "mfl_tier2.h"
/*004276b0*/ extern int __cdecl player_stat_get_crowns(undefined4 p0);
/*004276d0*/ extern int __cdecl player_stat_get_pants(undefined4 p0);
/*004276f0*/ extern int __cdecl player_stat_get_crowns_total(undefined4 p0);
/*00427710*/ extern int __cdecl player_stat_get_pants_total(undefined4 p0);
/*00427730*/ extern int player_stat_get_turbo_used_team();
/*004277b0*/ extern int player_stat_get_aggression_team();
/*00427830*/ extern int __cdecl player_stat_get_kills(undefined4 p0);
/*004278c0*/ extern int player_stat_get_wall_hits_team();
/*00427940*/ extern int __cdecl player_stat_get_unforced_errors(undefined4 p0);
/*004279d0*/ extern int player_stat_get_turbo_used_total_team();
/*00427a50*/ extern int player_stat_get_aggression_total_team();
/*00427ad0*/ extern int __cdecl player_stat_get_kills_total(undefined4 p0);
/*00427b60*/ extern int player_stat_get_wall_hits_total_team();
/*00427be0*/ extern int __cdecl player_stat_get_unforced_errors_total(undefined4 p0);
/*0060ad64*/ extern undefined g_tutorial_state_60ad64;
/*0060ad80*/ extern undefined g_tutorial_state_60ad80;
/*0060ad9c*/ extern undefined g_tutorial_state_60ad9c;
/*0060ae28*/ extern undefined g_tutorial_state_60ae28;
/*0060ae44*/ extern undefined g_tutorial_state_60ae44;
/*0060ae60*/ extern undefined g_tutorial_state_60ae60;
/*0060ae7c*/ extern undefined g_tutorial_state_60ae7c;
/*0060ae98*/ extern undefined g_tutorial_state_60ae98;
/*0060aeb4*/ extern undefined g_tutorial_state_60aeb4;
/*0060aed0*/ extern undefined g_tutorial_state_60aed0;
/*0060aeec*/ extern void *g_tutorial_pages_event;
/*0060af44*/ extern void *g_tutorial_pages_basic;
/*0069cbd4*/ extern undefined4 g_event_type;
/*0069cbe0*/ extern undefined4 g_tutorial_page_index;
/*0069d100*/ extern undefined4 g_tutorial_line_count;
/*0069d104*/ extern undefined4 g_tutorial_display_ticks;
/*0069d108*/ extern undefined4 g_tutorial_line_entries[0x1000];
/*0069d158*/ extern float g_tutorial_line_width;
/*007ef7b8*/ extern undefined4 game_mode;

#define TUTORIAL_LINE_CLEAR_COUNT (0x14) /* line entries zeroed on reset (inferred) */
#define BASIC_PAGE_COUNT (2) /* pages in the basic tutorial list, wrap point (inferred) */
#define EVENT_PAGE_COUNT (0xe) /* pages in the event tutorial list, wrap point (inferred) */
#define STAT_PLAYER_COUNT (4) /* player slots summed by the stat loop (inferred) */
/* tutorial_state_reset */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "resets the tutorial / frontend counters when a race event starts (per event type)" */

/*
 * Resets the tutorial state when a race event starts. Takes no parameters, returns nothing.
 * Clears the line counters and line entries, advances g_tutorial_page_index, then walks the
 * tutorial page list (basic list for event types 0/1/2, event list otherwise) and stops on
 * the first page that should be shown, wrapping the index back to 0 at page_count.
 * g_event_type picks the list (inferred).
 */
void tutorial_state_reset(void)

{
  int *state_ref;
  int event_type;
  int page_index;
  int *page;
  int slot;
  undefined4 *line_entry;
  int stat_sum;
  bool skip_page;
  int page_count;
  
  event_type = g_event_type;
  g_tutorial_page_index = g_tutorial_page_index + 1;
  skip_page = g_event_type == EVENT_TIME_TRIAL_0;
  g_tutorial_line_count = 0;
  g_tutorial_line_width = 0;
  g_tutorial_display_ticks = 0;
  /* zero the tutorial line entries */
  line_entry = g_tutorial_line_entries;
  for (page_index = TUTORIAL_LINE_CLEAR_COUNT; page_index != 0; page_index = page_index + -1) {
    *line_entry = 0;
    line_entry = line_entry + 1;
  }
  if (((skip_page) || (event_type == 1)) || (page_count = EVENT_PAGE_COUNT, event_type == 2)) {
    page_count = BASIC_PAGE_COUNT;
  }
  page_index = g_tutorial_page_index;
  stat_sum = page_count;
  if (page_count <= (int)g_tutorial_page_index) {
    g_tutorial_page_index = 0;
    page_index = g_tutorial_page_index;
  }
  /* main loop: pick the current page, skip pages that do not apply to game_mode, stop on a page to show */
  do {
    skip_page = false;
    if (((event_type == 0) || (event_type == 1)) || (event_type == 2)) {
      page = (int *)(&g_tutorial_pages_basic)[page_index];
    }
    else {
      page = (int *)(&g_tutorial_pages_event)[page_index];
    }
    if (game_mode == GAME_MODE_STANDARD) {
      if (page != (int *)&g_tutorial_state_60ad80) {
        state_ref = (int *)&g_tutorial_state_60ae44;
LAB_00431e5a:
        if (page != state_ref) goto LAB_00431e61;
      }
LAB_00431e5c:
      skip_page = true;
    }
    else if (game_mode == GAME_MODE_RACE) {
      if ((page == (int *)&g_tutorial_state_60ad80) || (page == (int *)&g_tutorial_state_60ae44)
         ) {
        skip_page = true;
      }
      if ((page == (int *)&g_tutorial_state_60ad64) || (page == (int *)&g_tutorial_state_60ae28)
         ) {
        skip_page = true;
      }
      if (page != (int *)&g_tutorial_state_60ad9c) {
        state_ref = (int *)&g_tutorial_state_60ae60;
        goto LAB_00431e5a;
      }
      goto LAB_00431e5c;
    }
LAB_00431e61:
    if (((((event_type == 6) || (event_type == 7)) || (event_type == 8)) || ((event_type == 0 || (event_type == 1)))) ||
       (event_type == 2)) {
LAB_00431ecc:
      if (skip_page) goto LAB_00431ed0;
    }
    else {
      if ((page == (int *)&g_tutorial_state_60ae98) || (page == (int *)&g_tutorial_state_60ae44)
         ) {
        skip_page = true;
      }
      if ((page == (int *)&g_tutorial_state_60ae60) || (page == (int *)&g_tutorial_state_60ae7c)
         ) {
        skip_page = true;
      }
      if ((page == (int *)&g_tutorial_state_60ae28) || (page == (int *)&g_tutorial_state_60aeb4)
         ) {
        skip_page = true;
      }
      if (page != (int *)&g_tutorial_state_60aed0) goto LAB_00431ecc;
      skip_page = true;
LAB_00431ed0:
      page_index = page_index + 1;
      g_tutorial_page_index = page_index;
    }
    if (page_index == page_count) {
      page_index = 0;
      g_tutorial_page_index = 0;
    }
    if (!skip_page) {
      if (((event_type == 0) || (event_type == 1)) || (event_type == 2)) {
        return;
      }
      slot = *page;
      /* scan the page's op list for a stat op (3 or 2) (inferred) */
      while (slot != -1) {
        if (slot == 3) goto LAB_00431f35;
        if (slot == 2) break;
        state_ref = page + 1;
        page = page + 1;
        slot = *state_ref;
      }
      if ((*page == 3) || (*page == 2)) {
LAB_00431f35:
        page = page + 1;
        /* sum the page's stat ops over the player slots (inferred) */
        stat_sum = 0;
        slot = 0;
        do {
          if (*page == 0) {
            event_type = player_stat_get_crowns(slot);
            stat_sum = stat_sum + event_type;
          }
          if (*page == 1) {
            event_type = player_stat_get_pants(slot);
            stat_sum = stat_sum + event_type;
          }
          if (*page == 4) {
            event_type = player_stat_get_kills(slot);
            stat_sum = stat_sum + event_type;
          }
          if (*page == 6) {
            event_type = player_stat_get_unforced_errors(slot);
            stat_sum = stat_sum + event_type;
          }
          if (*page == 2) {
            event_type = player_stat_get_turbo_used_team(slot);
            stat_sum = stat_sum + event_type;
          }
          if (*page == 3) {
            event_type = player_stat_get_aggression_team(slot);
            stat_sum = stat_sum + event_type;
          }
          if (*page == 5) {
            event_type = player_stat_get_wall_hits_team(slot);
            stat_sum = stat_sum + event_type;
          }
          if (*page == 7) {
            event_type = player_stat_get_crowns_total(slot);
            stat_sum = stat_sum + event_type;
          }
          if (*page == 8) {
            event_type = player_stat_get_pants_total(slot);
            stat_sum = stat_sum + event_type;
          }
          if (*page == 0xb) {
            event_type = player_stat_get_kills_total(slot);
            stat_sum = stat_sum + event_type;
          }
          if (*page == 0xd) {
            event_type = player_stat_get_unforced_errors_total(slot);
            stat_sum = stat_sum + event_type;
          }
          if (*page == 9) {
            event_type = player_stat_get_turbo_used_total_team(slot);
            stat_sum = stat_sum + event_type;
          }
          if (*page == 10) {
            event_type = player_stat_get_aggression_total_team(slot);
            stat_sum = stat_sum + event_type;
          }
          if (*page == 0xc) {
            event_type = player_stat_get_wall_hits_total_team(slot);
            stat_sum = stat_sum + event_type;
          }
          slot = slot + 1;
          event_type = g_event_type;
          page_index = g_tutorial_page_index;
        } while (slot < STAT_PLAYER_COUNT);
      }
      /* non-zero stat sum: the page applies, stop here (inferred) */
      if (stat_sum != 0) {
        return;
      }
      g_tutorial_page_index = page_index + 1;
      page_index = g_tutorial_page_index;
      if (g_tutorial_page_index == page_count) {
        g_tutorial_page_index = 0;
        page_index = 0;
      }
    }
  } while( true );
}


