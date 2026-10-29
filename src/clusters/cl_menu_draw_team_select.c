// cluster-entries: menu_draw_team_select
/* Installed cluster menu_draw_team_select (tools/cluster_install.py from output/cluster_pilot_pg5/menu_draw_team_select.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0042c440*/ extern int __cdecl ui_rt2d_text_draw(int text_id, float x, float y, unsigned int colour, float scale, int flags);
/*0042c540*/ extern void __cdecl text_draw_plain(char *text, float x, float y, unsigned int colour, float scale, int flags);
/*0042c8e0*/ extern double __cdecl text_width(int text_id, float scale);
/*00430eb0*/ extern void __cdecl ui_keyboard_icon_find(int controller, float x, float y, unsigned int colour, int flag);
/*00435040*/ extern void *__cdecl team_color_names(int team);
/*004832d0*/ extern void __cdecl draw_sprite_uv(void *texture, float x, float y, float width, float height, unsigned int colour, float u0, float u1, float v0, float v1, int a, int b);
/*007d2128*/ extern char *RwEngineInstance;
/*0069c9b4*/ extern unsigned char g_menu_69c9b4;          /* fade (alpha) of the menu */
/*0069cf98*/ extern int g_menu_bot_skill_a;
/*008c86bc*/ extern int g_hud_8c86bc;
/*007ef7d0*/ extern int g_menu_7ef7d0;
/*0069cb10*/ extern int g_ui_69cb10[12 * 3];             /* per controller: team column (0 left, 1 / 2 right columns), 3 ints each */
/*007f0204*/ extern int g_player_slot0_team[];           /* player slots, 4 ints each: [-2] controller, [0] team */

#define TEAM_SELECT_GROUPS 12
#define TEAM_SELECT_PLAYERS 4
#define TEAM_SELECT_SLOT_INTS 4
#define TEAM_SELECT_LAST_SHARED_GROUP 6          /* group index to digit: groups >= 6 count from 1 again when g_menu_7ef7d0 is set */
#define TEAM_COLOUR_KEYBOARD 5                   /* the team colour whose icon is flagged */
#define TEXT_TEAM_1 0xa2
#define TEXT_TEAM_2 0xa3
#define TEXT_PLAYER_NAME_REFERENCE 0x37          /* text whose width centres the player number */
#define TEAM_BOX_WIDTH 0x22                      /* x step (34) per player sprite along the Team 1 / Team 2 row at y 108 (TEAM_BOX_BASE_A / _B are the x of its first sprite) */
#define TEAM_BOX_BASE_A 0xfa
#define TEAM_BOX_BASE_B 0x1ae
#define TEAM_SPRITE_STEP 0xb4                    /* x step (180) between the undecided / Team 1 / Team 2 positions of a player's sprite in its framed row */
#define TEAM_SPRITE_SIZE 48.0f
#define TEAM_SPRITE_ROW_Y 108.0f

/* menu_draw_team_select: the team select screen: two header boxes ("Team 1" / "Team 2", texts 162 and 163) and, for each of the 12
   controller groups, one row per player of that group: framed rect, the team sprite, the team column sprite, the keyboard / pad icon and the
   player number; all faded with g_menu_69c9b4 */
static void ui_draw_framed_rect_r(unsigned char p0, float p1, float p2, float p3, float p4);
extern void ui_draw_framed_rect(void);
void menu_draw_team_select(void)
{
  unsigned int colour_team[6];            /* r g b + fade per team colour */
  unsigned int colour_black;              /* fade only */
  unsigned int colour_white;              /* white + fade */
  unsigned int colour_black_text;
  unsigned char fade = g_menu_69c9b4;
  int column_a;
  int column_b;
  int base_a;
  int base_b;
  int group;
  int *group_entry;
  int slot_index;
  int *slot;
  float rect_y;
  float sprite_x;
  float sprite_y;
  float row_x;
  float row_y;
  float icon_y;
  void *texture;
  int column;
  int half_width;
  char digit[4];

  colour_team[0] = 0x3d3a98 | ((unsigned int)fade << 24);
  colour_team[1] = 0xae894e | ((unsigned int)fade << 24);
  colour_team[2] = 0x567661 | ((unsigned int)fade << 24);
  colour_team[3] = 0x62c3db | ((unsigned int)fade << 24);
  colour_team[4] = 0xa7a7eb | ((unsigned int)fade << 24);
  colour_team[5] = (unsigned int)fade << 24;
  colour_black = (unsigned int)fade << 24;
  g_menu_bot_skill_a = 0;
  g_hud_8c86bc = 1;
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE, 0);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, 0);
  ui_draw_framed_rect_r(fade, 250.0f, 140.0f, 140.0f, 28.0f);
  ui_rt2d_text_draw(TEXT_TEAM_1, 320.0f, 154.0f, colour_black, 0.8f, 2);
  ui_draw_framed_rect_r(fade, 430.0f, 140.0f, 140.0f, 28.0f);
  ui_rt2d_text_draw(TEXT_TEAM_2, 500.0f, 154.0f, colour_black, 0.8f, 2);
  colour_white = 0xffffff | ((unsigned int)fade << 24);
  colour_black_text = (unsigned int)fade << 24;
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE, 0);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, 0);
  column_a = 0;
  column_b = 0;
  rect_y = 200.0f;                /* keeps counting down the rows over all groups */
  group_entry = g_ui_69cb10;
  for (group = 0; ; ) {
    base_b = column_b * TEAM_BOX_WIDTH + TEAM_BOX_BASE_B;
    base_a = column_a * TEAM_BOX_WIDTH + TEAM_BOX_BASE_A;
    for (slot_index = 0; slot_index < TEAM_SELECT_PLAYERS; slot_index++) {
      slot = g_player_slot0_team + slot_index * TEAM_SELECT_SLOT_INTS;
      if (slot[-2] == group) {
        ui_draw_framed_rect_r(fade, 70.0f, rect_y, 510.0f, 28.0f);
        texture = team_color_names(slot[0]);
        column = *group_entry;
        sprite_x = (float)(column * TEAM_SPRITE_STEP) + 82.0f;
        row_y = rect_y - 2.0f;
        sprite_y = row_y - 8.0f;
        draw_sprite_uv(texture, sprite_x, sprite_y, TEAM_SPRITE_SIZE, TEAM_SPRITE_SIZE, colour_white, 0.0f, 1.0f, 0.0f, 1.0f, 1, 1);
        if (column != 0) {
          if (column == 1) {
            column_a++;
            row_x = (float)base_a;
            base_a += TEAM_BOX_WIDTH;
          } else if (column == 2) {
            column_b++;
            row_x = (float)base_b;
            base_b += TEAM_BOX_WIDTH;
          }
          draw_sprite_uv(texture, row_x, TEAM_SPRITE_ROW_Y, TEAM_SPRITE_SIZE, TEAM_SPRITE_SIZE, colour_white, 0.0f, 1.0f, 0.0f, 1.0f, 1, 1);
        }
        icon_y = row_y + 15.0f;
        ui_keyboard_icon_find(slot[-2], sprite_x + 70.0f, icon_y, colour_team[slot[0]], slot[0] == TEAM_COLOUR_KEYBOARD);
        half_width = (int)text_width(TEXT_PLAYER_NAME_REFERENCE, 0.9f) / 2;
        digit[0] = (char)group;
        if (g_menu_7ef7d0 != 0 && group_entry >= g_ui_69cb10 + TEAM_SELECT_LAST_SHARED_GROUP * 3) {
          digit[0] = (char)(group - TEAM_SELECT_LAST_SHARED_GROUP);
        }
        digit[0] = digit[0] + '1';
        digit[1] = 0;
        text_draw_plain(digit, (float)half_width + 4.0f + sprite_x + 65.0f, icon_y, colour_black_text, 0.6f, 0);
        rect_y = rect_y + 40.0f;
      }
    }
    group_entry += 3;
    group++;
    if (group_entry >= g_ui_69cb10 + TEAM_SELECT_GROUPS * 3) {
      RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)1);
      RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)1);
      return;
    }
  }
}

/* ui_draw_framed_rect_r: C -> original ui_draw_framed_rect (registers eax,s0,s1,s2,s3) */
__declspec(naked) void ui_draw_framed_rect_r(unsigned char p0, float p1, float p2, float p3, float p4)
{
    __asm {
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        mov eax, dword ptr [esp + 20]
        call ui_draw_framed_rect
        add esp, 16
        ret
    }
}
