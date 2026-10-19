#include "mfl_tier2.h"
/*0040c9f0*/ extern undefined __cdecl players_get_wins(undefined4 p0);
/*0040cad0*/ extern int players_get_points();
/*0040cc40*/ extern int player_get_stat_8b90();
/*0040cc50*/ extern int player_get_wins();
/*0040fdb0*/ extern int get_player_count();
/*004136d0*/ extern undefined __cdecl race_start_setup(undefined4 p0);
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
/*00427c70*/ extern int player_stat_get_place_team();
/*00427d30*/ extern int player_stat_get_team_l();
/*00427de0*/ extern undefined __cdecl player_stat_get_by_kind(undefined4 p0, undefined4 p1, undefined4 p2);
/*0042c8e0*/ extern double text_width();
/*0042e960*/ extern int intro_banner_set_active();
/*0042e990*/ extern int time_is_better_than_best();
/*00431b60*/ extern int ui_format_number_999();
/*00431c20*/ extern int tutorial_text_format_ratio();
/*00431d70*/ extern int tutorial_state_reset();
/*00432060*/ extern int team_names_string_build();
/*005dd32c*/ extern float k_f_3;
/*005dd330*/ extern float float_1_0;
/*005dd33c*/ extern float k_f_4;
/*005dd7e8*/ extern float k_f_60;
/*005ddb48*/ extern float k_f_16;
/*005ddb50*/ extern float k_f_8;
/*005ddc28*/ extern float k_f_0;
/*005de3e4*/ extern float k_f_40;
/*0060ad5c*/ extern undefined4 g_menu_60ad5c;
/*0060ad60*/ extern undefined4 g_menu_60ad60;
/*0060aeec*/ extern void *g_tutorial_pages_event;
/*0060af44*/ extern void *g_tutorial_pages_basic;
/*0069c998*/ extern undefined4 g_menu_panel_state_69c998;
/*0069c99c*/ extern int g_frontend_69c99c;
/*0069cbd4*/ extern undefined4 g_event_type;
/*0069cbe0*/ extern undefined4 g_tutorial_page_index;
/*0069cc40*/ extern undefined4 g_team_mode;
/*0069cc48*/ extern undefined4 g_menu_69cc48;
/*0069d014*/ extern undefined4 g_tutorial_ranked_players[0x1000];
/*0069d0e8*/ extern float g_tutorial_69d0e8;
/*0069d0ec*/ extern float g_tutorial_69d0ec;
/*0069d0f0*/ extern float g_tutorial_69d0f0;
/*0069d0f4*/ extern float g_tutorial_69d0f4;
/*0069d100*/ extern undefined4 g_tutorial_line_count;
/*0069d104*/ extern undefined4 g_tutorial_display_ticks;
/*0069d108*/ extern undefined4 g_tutorial_line_entries;
/*0069d10c*/ extern undefined4 g_tutorial_69d10c;
/*0069d158*/ extern float g_tutorial_line_width;
/*0069d15c*/ extern float g_tutorial_69d15c;
/*0069d160*/ extern undefined4 g_tutorial_reset_requested;
/*0069d21c*/ extern undefined4 g_menu_multiplayer_kind;
/*0069d2a8*/ extern undefined4 g_tutorial_69d2a8;
/*0069d2c8*/ extern undefined4 g_tutorial_69d2c8;
/*0069d67c*/ extern float g_tutorial_69d67c;
/*007ef7e8*/ extern undefined4 g_frame_ticks;
/*007ef7ec*/ extern float g_frame_seconds;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/* tutorial_text_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "runs game_tick for the frame time, then builds the instruction text lines for event types 0..2
   using text_width (inferred)" */

void tutorial_text_update(void)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  int *piVar6;
  int iVar7;
  float10 fVar8;
  undefined4 uVar9;
  struct {
    int *piStack_10;
    int iStack_c;
    undefined4 uStack_8;
    int local_4;
  } frame;
  
  if (0 < (int)g_frame_ticks) {
    iVar4 = (g_frame_ticks - 1U) / 0x32 + 1;
    do {
      race_start_setup(0x32);
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
  }
  if (g_menu_panel_state_69c998 == 1) {
    if (((g_event_type == 0) || (g_event_type == 1)) || (g_event_type == 2)) {
      intro_banner_set_active(0);
    }
    if (g_tutorial_reset_requested != 0) {
      tutorial_state_reset();
      g_tutorial_reset_requested = 0;
    }
    if (g_tutorial_line_count == 0) {
      g_tutorial_display_ticks = 0x8e8;
      if (((g_event_type == 0) || (g_event_type == 1)) || (g_event_type == 2)) {
        piVar5 = (int *)(&g_tutorial_pages_basic)[g_tutorial_page_index];
      }
      else {
        piVar5 = (int *)(&g_tutorial_pages_event)[g_tutorial_page_index];
      }
      iVar4 = *piVar5;
      iVar7 = frame.local_4;
      if (iVar4 != -1) {
LAB_004322cd:
        frame.piStack_10 = piVar5;
        switch(iVar4) {
        case 1:
          frame.piStack_10 = piVar5 + 1;
          if (((g_event_type == 0) || (g_event_type == 1)) || (g_event_type == 2)) {
LAB_00432313:
            frame.piStack_10 = piVar5 + 1;
            (&g_tutorial_line_entries)[g_tutorial_line_count] = *frame.piStack_10;
            g_tutorial_line_count = g_tutorial_line_count + 1;
            fVar8 = (float10)text_width(*frame.piStack_10,0x3f333333);
            g_tutorial_line_width =
                 (float)(fVar8 + (float10)g_tutorial_line_width + (float10)k_f_8);
          }
          break;
        case 2:
          frame.piStack_10 = piVar5 + 1;
          player_stat_get_by_kind(g_tutorial_ranked_players,piVar5[1],0);
          frame.local_4 = get_player_count();
          if (g_team_mode == 0) {
            if (frame.local_4 < 1) break;
          }
          else {
            frame.local_4 = 2;
          }
          iVar4 = 0x84;
          piVar6 = g_tutorial_ranked_players;
          do {
            if (g_team_mode == 0) {
              iVar3 = g_player_slots[*piVar6].team + 0x7c;
              (&g_tutorial_line_entries)[g_tutorial_line_count] = iVar3;
              fVar8 = (float10)text_width(iVar3,0x3f333333);
            }
            else {
              iVar3 = g_player_slots[*piVar6].state;
              team_names_string_build(iVar3);
              iVar3 = iVar3 + 0xa7;
              (&g_tutorial_line_entries)[g_tutorial_line_count] = iVar3;
              fVar8 = (float10)text_width(iVar3,0x3f333333);
            }
            g_tutorial_line_width =
                 (float)(fVar8 + (float10)g_tutorial_line_width + (float10)k_f_8);
            if (*frame.piStack_10 == 0) {
              iVar7 = player_stat_get_crowns(*piVar6);
            }
            if (*frame.piStack_10 == 1) {
              iVar7 = player_stat_get_pants(*piVar6);
            }
            if (*frame.piStack_10 == 4) {
              iVar7 = player_stat_get_kills(*piVar6);
            }
            if (*frame.piStack_10 == 6) {
              iVar7 = player_stat_get_unforced_errors(*piVar6);
            }
            if (*frame.piStack_10 == 7) {
              iVar7 = player_stat_get_crowns_total(*piVar6);
            }
            if (*frame.piStack_10 == 8) {
              iVar7 = player_stat_get_pants_total(*piVar6);
            }
            if (*frame.piStack_10 == 0xb) {
              iVar7 = player_stat_get_kills_total(*piVar6);
            }
            if (*frame.piStack_10 == 0xd) {
              iVar7 = player_stat_get_unforced_errors_total(*piVar6);
            }
            (&g_tutorial_69d10c)[g_tutorial_line_count] = iVar4;
            ui_format_number_999(iVar7,iVar4);
            fVar8 = (float10)text_width(iVar4,0x3f333333);
            g_tutorial_line_count = g_tutorial_line_count + 2;
            piVar6 = piVar6 + 1;
            iVar4 = iVar4 + 1;
            frame.local_4 = frame.local_4 + -1;
            g_tutorial_line_width =
                 (float)(fVar8 + (float10)g_tutorial_line_width + (float10)k_f_8);
          } while (frame.local_4 != 0);
          goto LAB_00432894;
        case 3:
          iVar4 = piVar5[1];
          frame.piStack_10 = piVar5 + 1;
          if ((iVar4 == 5) || (iVar4 == 0xc)) {
            uVar9 = 1;
          }
          else {
            uVar9 = 0;
          }
          player_stat_get_by_kind(g_tutorial_ranked_players,iVar4,uVar9);
          frame.local_4 = get_player_count();
          if (g_team_mode == 0) {
            if (frame.local_4 < 1) break;
          }
          else {
            frame.local_4 = 2;
          }
          iVar3 = 0x91;
          piVar6 = g_tutorial_ranked_players;
          iVar4 = frame.iStack_c;
          do {
            if (g_team_mode == 0) {
              iVar2 = g_player_slots[*piVar6].team + 0x7c;
              (&g_tutorial_line_entries)[g_tutorial_line_count] = iVar2;
              fVar8 = (float10)text_width(iVar2,0x3f333333);
            }
            else {
              iVar4 = g_player_slots[*piVar6].state;
              team_names_string_build(iVar4);
              iVar4 = iVar4 + 0xa7;
              (&g_tutorial_line_entries)[g_tutorial_line_count] = iVar4;
              fVar8 = (float10)text_width(iVar4,0x3f333333);
              iVar4 = frame.iStack_c;
            }
            g_tutorial_line_width =
                 (float)(fVar8 + (float10)g_tutorial_line_width + (float10)k_f_8);
            if (*frame.piStack_10 == 2) {
              iVar4 = player_stat_get_place_team(*piVar6);
              iVar4 = iVar4 * 2000;
              iVar7 = player_stat_get_turbo_used_team(*piVar6);
              frame.uStack_8 = 0;
              frame.iStack_c = iVar4;
            }
            if (*frame.piStack_10 == 3) {
              iVar4 = player_stat_get_aggression_team(g_tutorial_ranked_players[0]);
              iVar7 = player_stat_get_aggression_team(*piVar6);
              frame.uStack_8 = 0;
              frame.iStack_c = iVar4;
            }
            if (*frame.piStack_10 == 5) {
              iVar4 = player_stat_get_wall_hits_team(g_tutorial_ranked_players[0]);
              iVar7 = player_stat_get_wall_hits_team(*piVar6);
              frame.uStack_8 = 1;
              frame.iStack_c = iVar4;
            }
            if (*frame.piStack_10 == 9) {
              iVar4 = player_stat_get_team_l(*piVar6);
              iVar4 = iVar4 * 2000;
              iVar7 = player_stat_get_turbo_used_total_team(*piVar6);
              frame.uStack_8 = 0;
              frame.iStack_c = iVar4;
            }
            if (*frame.piStack_10 == 10) {
              iVar4 = player_stat_get_aggression_total_team(g_tutorial_ranked_players[0]);
              iVar7 = player_stat_get_aggression_total_team(*piVar6);
              frame.uStack_8 = 0;
              frame.iStack_c = iVar4;
            }
            if (*frame.piStack_10 == 0xc) {
              iVar4 = player_stat_get_wall_hits_total_team(g_tutorial_ranked_players[0]);
              iVar7 = player_stat_get_wall_hits_total_team(*piVar6);
              frame.uStack_8 = 1;
              frame.iStack_c = iVar4;
            }
            (&g_tutorial_69d10c)[g_tutorial_line_count] = iVar3;
            tutorial_text_format_ratio(iVar4,iVar7,iVar3,frame.uStack_8);
            fVar8 = (float10)text_width(iVar3,0x3f333333);
            g_tutorial_line_count = g_tutorial_line_count + 2;
            piVar6 = piVar6 + 1;
            iVar3 = iVar3 + 1;
            frame.local_4 = frame.local_4 + -1;
            g_tutorial_line_width =
                 (float)(fVar8 + (float10)g_tutorial_line_width + (float10)k_f_8);
          } while (frame.local_4 != 0);
LAB_00432894:
          frame.piStack_10 = piVar5 + 1;
          break;
        case 4:
          goto switchD_004322dc_caseD_4;
        case 5:
          iVar4 = time_is_better_than_best();
          if (iVar4 != 0) goto LAB_00432313;
          g_tutorial_page_index = g_tutorial_page_index + 1;
          goto switchD_004322dc_caseD_4;
        case 6:
          if (piVar5[1] == 6) {
            iVar7 = player_stat_get_unforced_errors(0);
          }
          (&g_tutorial_line_entries)[g_tutorial_line_count] = 0x84;
          ui_format_number_999(iVar7,0x84);
          fVar8 = (float10)text_width(0x84,0x3f333333);
          g_tutorial_line_width = (float)(fVar8 + (float10)g_tutorial_line_width);
          (&g_tutorial_69d10c)[g_tutorial_line_count] = 0xc6;
          fVar8 = (float10)text_width(0xc6,0x3f333333);
          goto LAB_00432624;
        case 7:
          if (piVar5[1] == 2) {
            iVar7 = player_stat_get_turbo_used_team(0);
            (&g_tutorial_line_entries)[g_tutorial_line_count] = 0x91;
            tutorial_text_format_ratio(2000,iVar7,0x91,0);
            fVar8 = (float10)text_width(0x91,0x3f333333);
            g_tutorial_line_width = (float)(fVar8 + (float10)g_tutorial_line_width);
          }
          if (piVar5[1] == 5) {
            iVar7 = player_stat_get_wall_hits_team(0);
            (&g_tutorial_line_entries)[g_tutorial_line_count] = 0x92;
            tutorial_text_format_ratio(0,iVar7,0x92,1);
            fVar8 = (float10)text_width(0x92,0x3f333333);
            g_tutorial_line_width = (float)(fVar8 + (float10)g_tutorial_line_width);
          }
          (&g_tutorial_69d10c)[g_tutorial_line_count] = 0xc6;
          fVar8 = (float10)text_width(0xc6,0x3f333333);
LAB_00432624:
          frame.piStack_10 = piVar5 + 1;
          g_tutorial_line_count = g_tutorial_line_count + 2;
          g_tutorial_line_width =
               (float)(fVar8 + (float10)g_tutorial_line_width + (float10)k_f_16);
        }
        goto switchD_004322dc_default;
      }
    }
    else {
      g_tutorial_display_ticks = g_tutorial_display_ticks + -0xc;
      if ((float)(int)g_tutorial_display_ticks + g_tutorial_line_width < k_f_60) {
        tutorial_state_reset();
      }
    }
LAB_004328e0:
    uVar1 = g_menu_69cc48;
    if ((((g_event_type == 0) || (g_event_type == 1)) || (g_event_type == 2)) &&
       ((int)g_menu_69cc48 < 3)) {
      g_tutorial_69d2c8 = 0;
      g_menu_69cc48 = 3;
      g_tutorial_69d67c = 0.0;
      g_tutorial_69d15c = 0.0;
    }
    else if (5 < (int)g_menu_69cc48) goto LAB_00432ab5;
    switch(g_menu_69cc48) {
    case 0:
      g_tutorial_69d67c = (float)g_frontend_69c99c;
      g_tutorial_69d2c8 = 0x15e;
      players_get_points(g_tutorial_ranked_players);
      g_tutorial_69d15c = g_frame_seconds * k_f_3 + g_tutorial_69d15c;
      if (float_1_0 < g_tutorial_69d15c) {
        g_tutorial_69d15c = 1.0;
        g_menu_69cc48 = 1;
      }
      g_tutorial_69d2a8 = 0;
      break;
    case 1:
      g_tutorial_69d67c = (float)g_frontend_69c99c;
      players_get_points(g_tutorial_ranked_players);
      break;
    case 2:
      players_get_points(g_tutorial_ranked_players);
      g_tutorial_69d67c = 0.0;
      g_tutorial_69d15c = 0.0;
      if ((int)g_tutorial_69d2c8 < 0x14) {
        g_tutorial_69d2c8 = 0;
      }
      else {
        g_tutorial_69d2c8 = g_tutorial_69d2c8 + -0x14;
        if (g_tutorial_69d2c8 != 0) break;
      }
      g_menu_69cc48 = 3;
      g_tutorial_69d0e8 = g_tutorial_69d0e8 + k_f_40;
      g_tutorial_69d0ec = g_tutorial_69d0ec + k_f_40;
      g_tutorial_69d0f0 = g_tutorial_69d0f0 + k_f_40;
      g_tutorial_69d0f4 = g_tutorial_69d0f4 + k_f_40;
      break;
    case 3:
      players_get_wins(g_tutorial_ranked_players);
      g_tutorial_69d15c = 1.0;
      g_tutorial_69d67c = 255.0;
      g_menu_69cc48 = 4;
      break;
    case 4:
    case 5:
      players_get_wins(g_tutorial_ranked_players);
    }
  }
  else {
    g_tutorial_69d15c = g_tutorial_69d15c - g_frame_seconds * k_f_4;
    if (g_tutorial_69d15c < k_f_0) {
      g_tutorial_69d15c = 0.0;
    }
  }
LAB_00432ab5:
  iVar4 = g_tutorial_69d2a8;
  if (g_menu_multiplayer_kind == 1) {
    iVar7 = 0;
    do {
      iVar3 = player_get_wins(iVar7);
      iVar7 = iVar7 + 1;
      iVar4 = iVar7;
      if ((int)g_menu_60ad5c <= iVar3) break;
      iVar4 = g_tutorial_69d2a8;
    } while (iVar7 < 4);
  }
  g_tutorial_69d2a8 = iVar4;
  iVar4 = g_tutorial_69d2a8;
  if (g_menu_multiplayer_kind == 2) {
    iVar4 = 0;
    while( true ) {
      iVar7 = player_get_stat_8b90(iVar4);
      iVar4 = iVar4 + 1;
      if ((int)g_menu_60ad60 <= iVar7) break;
      if (3 < iVar4) {
        return;
      }
    }
  }
  g_tutorial_69d2a8 = iVar4;
  return;
switchD_004322dc_caseD_4:
  frame.piStack_10 = piVar5 + 1;
switchD_004322dc_default:
  iVar4 = frame.piStack_10[1];
  piVar5 = frame.piStack_10 + 1;
  if (iVar4 == -1) goto LAB_004328e0;
  goto LAB_004322cd;
}


