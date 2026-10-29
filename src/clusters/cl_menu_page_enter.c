// cluster-entries: menu_page_enter
/* Installed cluster menu_page_enter (tools/cluster_install.py from output/cluster_pilot_x5/menu_page_enter.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/*00416c40*/ extern /* RET? */ undefined ai_bots_clear_stats();
/*007eebc0*/ extern struct GameState g_game_state;
/*008c80a0*/ extern struct MenuItemAnim g_menu_item_anims[30];
/*0069d2e0*/ extern struct MenuRecord g_menu_records[14];
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*00481fc0*/ extern /* RET? */ undefined gfx_set_89ddfc();
/*005deb38*/ extern float k_f_58;
/*00437b70*/ extern undefined __cdecl menu_page_enter_set_panel_state(undefined4 p0);
/*00438580*/ extern /* RET? */ undefined menu_page_update_entry_states();
/*004379c0*/ extern /* RET? */ undefined menu_panel_states_decay();
/*00432d60*/ extern /* RET? */ undefined menu_table_clear_b();
/*00426650*/ extern /* RET? */ undefined player_stats_clear_b();
/*0040cdb0*/ extern /* RET? */ undefined round_stats_reset();
/* DECLS-END */
extern int g_game_state_field608_0xae8;
extern int g_game_state_field612_0xaec;
extern int g_game_state_field616_0xaf0;
extern int g_game_state_field620_0xaf4;
extern int g_game_state_field633_0xb14;
extern int g_game_state_field637_0xb18;
extern int g_game_state_field641_0xb1c;
extern int g_game_state_field645_0xb20;
extern int g_game_state_field649_0xb24;
extern int g_game_state_field662_0xb44;
extern int g_game_state_field666_0xb48;
extern int g_game_state_field670_0xb4c;
extern int g_game_state_field674_0xb50;
extern unsigned int g_ai_8c8740[12];
extern int g_ai_69d220;
extern int g_ai_8c86b8;
extern int g_ai_8c86cc;
extern int g_career_row;
extern int g_menu_69ca1c;
extern int g_menu_69cc4c;
extern int g_menu_current;
extern int g_menu_event_course_index;
extern int g_menu_609a58[];
extern char *g_menu_page_item_lists[];
extern char g_menu_page_18_game_mode[];
extern char g_menu_page_24_game_mode[];
extern int nop_b(void);
extern int game_state_get_2812(void);

#define ITEM_ID_FRAME 0xff000000u     /* item list marker looked up for the frame slot (slot 0) */
#define ITEM_ID_ROW   0xff040000u     /* marker looked up (and stored as item_id) for every list row */
#define ITEM_ID_PROMPT 0xff080000u    /* marker of the button prompts */

/* menu_page_enter */

/* "(page, kind): kind 0 -> push the page `page` onto the menu stack, 1 -> go back one page, 2 -> re-enter the current page".  Takes a snapshot of the options of the
   page (so that Cancel can restore them), then builds the g_menu_item_anims entries: slot 0 is the frame, then one entry per row of the page that is left and of the
   page that is entered (their rows slide in / out), at y = menu_item_y_position(...) + 30 * row, and finally the button prompt entry (menu_button_prompts_build). */
static int menu_item_list_find_index_a_r(char * p0, unsigned int p1, int p2);
extern void menu_item_list_find_index_a(void);
static int menu_count_items_r(char * p0);
extern void menu_count_items(void);
static int menu_item_y_position_r(int p0, int p1);
extern void menu_item_y_position(void);
static int menu_button_prompts_build_r(int p0, int * p1, int p2, int p3);
extern void menu_button_prompts_build(void);
void menu_page_enter(int page, int kind)
{
  int cur;
  int i;
  int next_slot;      /* number of g_menu_item_anims entries filled so far (the running slot index) */
  int row_count;      /* rows that were matched in the item lists so far (stored as the entry_count of the page record) */
  int idx;            /* record index of the page below the one that is built (the record rec[idx + 1] is the page of the rows) */
  char *old_list;     /* item list that is left (kind 0: the current page) */
  char *new_list;     /* item list that is entered */
  short new_row;      /* index of the row of new_list that carries the ITEM_ID_ROW marker (loop counter-th match), -1 none */
  short old_row;
  int base_y;
  int new_count;
  int old_count;
  int row_y;
  int slide_y;
  int row;
  MenuItemAnim *anim;
  char *row_list;

  if (kind == 0) {
    menu_page_enter_set_panel_state(page);
  }
  else if (kind == 1) {
    if (g_menu_current - 2 >= 0) {
      menu_page_enter_set_panel_state(g_menu_records[g_menu_current - 1].page_id);
    }
  }
  else if (kind == 2) {
    menu_panel_states_decay();
  }
  gfx_set_89ddfc(0xff);
  if (page == 0x1a) {
    g_game_state_field616_0xaf0 = g_game_state_field608_0xae8;
    g_game_state_field620_0xaf4 = g_game_state_field612_0xaec;
  }
  else if (page == 0x13) {
    g_game_state_field633_0xb14 = g_game_state.option_sliders[0];
    g_game_state_field649_0xb24 = g_game_state.option_sliders[4];
    g_game_state_field645_0xb20 = g_game_state.option_sliders[3];
    g_game_state_field641_0xb1c = g_game_state.option_sliders[2];
    g_game_state_field637_0xb18 = g_game_state.option_sliders[1];
  }
  else if (page == 0x20) {
    g_ai_8c86b8 = nop_b();
  }
  else if (page == 0x23) {
    g_ai_8c86cc = game_state_get_2812();
  }
  else if (page == 0x1c) {
    for (i = 0; i < 12; i++) {
      g_ai_8c8740[i] = g_player_inputs[i].rumble_enabled;
    }
  }
  else {
    if (page == 0x1f || page == 0x21) {
      g_game_state_field662_0xb44 = g_game_state.option_flags[1];
      g_game_state_field666_0xb48 = g_game_state.option_flags[2];
      g_game_state_field670_0xb4c = g_game_state.option_flags[3];
      g_game_state_field674_0xb50 = g_game_state.option_flags[4];
    }
    if (page == 1) {
      ai_bots_clear_stats();
      player_stats_clear_b();
      round_stats_reset();
      g_menu_69cc4c = 1;
    }
  }
  menu_table_clear_b();
  cur = g_menu_current;
  row_count = 0;
  next_slot = 0;
  if (kind == 0) {
    g_menu_records[cur + 1].item_list = g_menu_page_item_lists[page];
    g_menu_records[cur + 1].page_id = page;
    g_menu_records[cur + 1].selected = 0;
    menu_page_update_entry_states(cur);
    idx = g_menu_current;
    new_list = g_menu_records[g_menu_current + 1].item_list;
    if (g_menu_current == 0) {
      old_list = 0;
    }
    else {
      old_list = g_menu_records[g_menu_current].item_list;
    }
  }
  else if (kind == 2) {
    idx = g_menu_current;
    old_list = g_menu_records[g_menu_current].item_list;
    new_list = 0;
  }
  else {
    cur = g_menu_current - 1;
    if (cur < 1) {
      g_menu_current = cur;
      return;
    }
    idx = cur;
    new_list = g_menu_records[cur].item_list;
    old_list = g_menu_records[g_menu_current].item_list;
    g_menu_current = cur;
  }
  new_row = menu_item_list_find_index_a_r(new_list, ITEM_ID_FRAME, 0);
  old_row = menu_item_list_find_index_a_r(old_list, ITEM_ID_FRAME, 0);
  if (new_row != -1 || old_row != -1) {
    anim = &g_menu_item_anims[0];
    ((unsigned char *)&anim->colour)[0] = 0xff;
    ((unsigned char *)&anim->colour)[1] = 0xff;
    ((unsigned char *)&anim->colour)[2] = 0xff;
    anim->ref_a = new_row;
    anim->ref_b = old_row;
    anim->item_id = ITEM_ID_FRAME;
    anim->x = 64.0f;
    anim->y = 48.0f;
    anim->scale = 0.6f;
    anim->param = 0;
    if (old_row == -1) {
      anim->state = 1;
      anim->anim_counter = 0;
      next_slot = 1;
    }
    else {
      anim->anim_counter = 0x1ff;
      anim->state = 2;
      if (new_row != -1) {
        anim->state = 0;
      }
      next_slot = 1;
    }
  }
  /* the y of the first row: the list is centred on 240; the count of the list that is shown at the end decides (kind 0: the new page, else the old one) */
  if (kind == 0) {
    new_count = menu_count_items_r(new_list);
    old_count = menu_count_items_r(old_list);
  }
  else {
    old_count = menu_count_items_r(old_list);
    new_count = menu_count_items_r(new_list);
  }
  if (g_menu_current == 0) {
    if (g_menu_records[1].item_list == g_menu_page_24_game_mode) {
      base_y = menu_item_y_position_r(new_count, 0x1c);
    }
    else {
      base_y = menu_item_y_position_r(new_count, 0x1e);
    }
  }
  else if (g_menu_records[idx + 1].item_list == g_menu_page_24_game_mode) {
    base_y = menu_item_y_position_r(old_count, 0x1c);
  }
  else {
    base_y = menu_item_y_position_r(old_count, 0x1e);
  }
  row = 0;
  slide_y = 0;
  row_y = 0;
  anim = &g_menu_item_anims[next_slot];
  for (;;) {
    new_row = menu_item_list_find_index_a_r(new_list, ITEM_ID_ROW, row);
    old_row = menu_item_list_find_index_a_r(old_list, ITEM_ID_ROW, row);
    if (new_row == -1) {
      if (old_row == -1) {
        if (kind == 0) {
          g_menu_records[idx + 1].entry_count = row_count;
          g_ai_69d220 = row_count;
        }
        else {
          g_ai_69d220 = g_menu_records[idx].entry_count;
        }
        new_row = menu_item_list_find_index_a_r(new_list, ITEM_ID_PROMPT, 0);
        menu_button_prompts_build_r(kind, &next_slot, new_row, page);
        if (kind == 0) {
          g_menu_current = g_menu_current + 1;
        }
        g_menu_69ca1c = kind;
        return;
      }
    }
    else {
      row_count = row_count + 1;
    }
    row_list = g_menu_records[idx + 1].item_list;
    *(char **)anim->_gap_30 = row_list;
    anim->item_id = ITEM_ID_ROW;
    anim->x = 64.0f;
    anim->colour = 0xffffffff;
    anim->scale = 0.8f;
    if ((row_list == g_menu_page_18_game_mode || row_list == g_menu_page_24_game_mode
         || (g_menu_records[idx].item_list == g_menu_page_18_game_mode && kind == 2)) && row_y != 0) {
      anim->scale = 0.58f;
    }
    anim->param = 0;
    anim->state = 0;
    anim->y = (float)((double)row_y + (double)base_y);
    if (*(char **)anim->_gap_30 == g_menu_page_24_game_mode) {
      g_menu_event_course_index = g_menu_609a58[g_career_row];
      anim->y = (float)(((double)slide_y + (double)base_y) - k_f_58);
    }
    if (*(char **)anim->_gap_30 == g_menu_page_18_game_mode) {
      g_menu_event_course_index = g_menu_609a58[g_career_row];
      anim->y = anim->y - k_f_58;
    }
    *(int *)anim->_gap_20 = row;
    anim->ref_a = new_row;
    anim->ref_b = old_row;
    anim->anim_counter = 0x1ff;
    if (g_menu_current == 0) {
      anim->state = 1;
      anim->anim_counter = 0;
    }
    anim++;
    next_slot = next_slot + 1;   /* the running slot index is the count of entries filled */
    row_y = row_y + 0x1e;
    row = row + 1;
    slide_y = slide_y + 0x1c;
  }
}

/* menu_item_list_find_index_a_r: C -> original menu_item_list_find_index_a (registers edx,ebx,edi) */
__declspec(naked) int menu_item_list_find_index_a_r(char * p0, unsigned int p1, int p2)
{
    __asm {
        push ebx
        push edi
        mov edx, dword ptr [esp + 12]
        mov ebx, dword ptr [esp + 16]
        mov edi, dword ptr [esp + 20]
        call menu_item_list_find_index_a
        pop edi
        pop ebx
        ret
    }
}

/* menu_count_items_r: C -> original menu_count_items (registers edx) */
__declspec(naked) int menu_count_items_r(char * p0)
{
    __asm {
        mov edx, dword ptr [esp + 4]
        call menu_count_items
        ret
    }
}

/* menu_item_y_position_r: C -> original menu_item_y_position (registers eax,ecx) */
__declspec(naked) int menu_item_y_position_r(int p0, int p1)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        mov ecx, dword ptr [esp + 8]
        call menu_item_y_position
        ret
    }
}

/* menu_button_prompts_build_r: C -> original menu_button_prompts_build (registers eax,esi,s0,s1) */
__declspec(naked) int menu_button_prompts_build_r(int p0, int * p1, int p2, int p3)
{
    __asm {
        push esi
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        mov eax, dword ptr [esp + 16]
        mov esi, dword ptr [esp + 20]
        call menu_button_prompts_build
        add esp, 8
        pop esi
        ret
    }
}
