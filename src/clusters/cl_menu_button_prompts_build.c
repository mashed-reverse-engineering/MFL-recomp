// cluster-entries: menu_button_prompts_build
/* Installed cluster menu_button_prompts_build (tools/cluster_install.py from output/cluster_pilot_uc3/menu_button_prompts_build.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0042fb80*/ extern int menu_page_for_game_mode();
/*00430b20*/ extern int frontend_stub_return_0_b();
/*0069cbd0*/ extern undefined4 g_menu_current;
/*008c80a0*/ extern struct MenuItemAnim g_menu_item_anims[30];
/* menu_button_prompts_build */

/* Appends ONE button-prompt entry (a 0x34 byte MenuItemAnim at g_menu_item_anims[*next_slot]) and advances *next_slot.  The entry is the prompt of kind `page`
   (ref_a = its text id) and, while it appears or disappears, shows the text of the neighbouring prompt kind found in the item list (ref_b).  `expected_id` is compared
   with frontend_stub_return_0_b() only for kind 1 (Continue: ref_a = -1 when equal, i.e. no text while idle).  Kinds without a case (default) append nothing. */

static int menu_item_list_find_index_current_r(int p0, int p1);
extern void menu_item_list_find_index_current(void);
static int menu_text_slot_set_r(int * p0, int p1, int p2, int p3, int p4);
extern void menu_text_slot_set(void);
/* Names inferred: direction = in_eax (0 = look up the neighbour kind in the current menu's list, 2 = look it up but build NOTHING (page is forced to 0), else = look it up in the next menu g_menu_current + 1), next_slot = running index into g_menu_item_anims, neighbour_kind = prompt kind (item list marker 0xFF08, 0 = none), page = prompt kind being built: 1 Continue (text 0x43), 2 Back (0x44), 4 (0x22a), 5 (0x13; the store of 0x49 before it is dead), 6 (0x5b, item id 0xFF11 instead of 0xFF10), 8 (text menu_page_for_game_mode()), 10 (0x138).  Same kind as the neighbour -> ref_b -1 and state 0x1000 (finished); no neighbour -> state 1 idle, counter 0; else state 0 (appearing), counter 0x1ff. */
void menu_button_prompts_build_n(int direction, int *next_slot, undefined4 page, int expected_id)

{
  int page_id;
  int neighbour_kind;
  
  neighbour_kind = 0;
  if (direction == 0) {
    if ((g_menu_current != 0) &&
       (neighbour_kind = menu_item_list_find_index_current_r(0xff080000,0), neighbour_kind == -1)) {
      neighbour_kind = 0;
    }
  }
  else if (direction == 2) {
    page = 0;
    neighbour_kind = menu_item_list_find_index_current_r(0xff080000,0);
  }
  else {
    g_menu_current = g_menu_current + 1;
    neighbour_kind = menu_item_list_find_index_current_r(0xff080000,0);
    if (neighbour_kind == -1) {
      neighbour_kind = 0;
    }
    g_menu_current = g_menu_current + -1;
  }
  switch(page) {
  case 1:
    menu_text_slot_set_r(next_slot,0xff100000,0x40,0x1ac,0);
    g_menu_item_anims[*next_slot].ref_a = 0x43;
    page_id = frontend_stub_return_0_b();
    if (expected_id == page_id) {
      g_menu_item_anims[*next_slot].ref_a = -1;
    }
    switch(neighbour_kind) {
    case 1:
switchD_00438c6c_caseD_6:
      g_menu_item_anims[*next_slot].ref_b = -1;
      g_menu_item_anims[*next_slot].state = 0x1000;
      g_menu_item_anims[*next_slot].anim_counter = 0x1ff;
      *next_slot = *next_slot + 1;
      return;
    case 2:
switchD_00438c6c_caseD_2:
      g_menu_item_anims[*next_slot].ref_b = 0x44;
      break;
    default:
switchD_00438c6c_caseD_3:
      g_menu_item_anims[*next_slot].ref_b = -1;
      g_menu_item_anims[*next_slot].state = 1;
      g_menu_item_anims[*next_slot].anim_counter = 0;
      *next_slot = *next_slot + 1;
      return;
    case 4:
switchD_00438c6c_caseD_4:
      g_menu_item_anims[*next_slot].ref_b = 0x22a;
      break;
    case 5:
switchD_00438c6c_caseD_5:
      g_menu_item_anims[*next_slot].ref_b = 0x49;
      g_menu_item_anims[*next_slot].ref_b = 0x13;
LAB_00438cb5:
      g_menu_item_anims[*next_slot].state = 0;
      g_menu_item_anims[*next_slot].anim_counter = 0x1ff;
      *next_slot = *next_slot + 1;
      return;
    case 6:
      g_menu_item_anims[*next_slot].ref_b = 0x5b;
      goto LAB_00438cb5;
    case 8:
switchD_00438c6c_caseD_8:
      neighbour_kind = menu_page_for_game_mode();
      g_menu_item_anims[*next_slot].ref_b = neighbour_kind;
      break;
    case 10:
switchD_00438c6c_caseD_a:
      g_menu_item_anims[*next_slot].ref_b = 0x138;
      goto LAB_00438cb5;
    }
LAB_00438c83:
    g_menu_item_anims[*next_slot].state = 0;
    g_menu_item_anims[*next_slot].anim_counter = 0x1ff;
    *next_slot = *next_slot + 1;
    return;
  case 2:
    menu_text_slot_set_r(next_slot,0xff100000,0x40,0x1ac,0);
    g_menu_item_anims[*next_slot].ref_a = 0x44;
    switch(neighbour_kind) {
    case 1:
      g_menu_item_anims[*next_slot].ref_b = 0x43;
      break;
    case 2:
switchD_00438d36_caseD_a:
      g_menu_item_anims[*next_slot].ref_b = -1;
      g_menu_item_anims[*next_slot].state = 0x1000;
      goto LAB_00438bfa;
    default:
      goto switchD_00438d36_caseD_3;
    case 4:
switchD_00438d36_caseD_4:
      g_menu_item_anims[*next_slot].ref_b = 0x22a;
      break;
    case 5:
switchD_00438d36_caseD_5:
      g_menu_item_anims[*next_slot].ref_b = 0x49;
      g_menu_item_anims[*next_slot].ref_b = 0x13;
      goto LAB_00438c83;
    case 6:
switchD_00438d36_caseD_6:
      g_menu_item_anims[*next_slot].ref_b = 0x5b;
      goto LAB_00438c83;
    case 8:
switchD_00438d36_caseD_8:
      neighbour_kind = menu_page_for_game_mode();
      g_menu_item_anims[*next_slot].ref_b = neighbour_kind;
      break;
    case 10:
switchD_00438ac0_caseD_a:
      g_menu_item_anims[*next_slot].ref_b = 0x138;
      goto LAB_00438c83;
    }
    break;
  default:
    goto switchD_0043894c_caseD_3;
  case 4:
    menu_text_slot_set_r(next_slot,0xff100000,0x40,0x1ac,0);
    g_menu_item_anims[*next_slot].ref_a = 0x22a;
    switch(neighbour_kind) {
    case 1:
switchD_00438b52_caseD_1:
      g_menu_item_anims[*next_slot].ref_b = 0x43;
      goto LAB_00438cb5;
    case 2:
switchD_00438b52_caseD_2:
      g_menu_item_anims[*next_slot].ref_b = 0x44;
      goto LAB_00438cb5;
    case 4:
switchD_00438b52_caseD_8:
      g_menu_item_anims[*next_slot].ref_b = -1;
      g_menu_item_anims[*next_slot].state = 0x1000;
      g_menu_item_anims[*next_slot].anim_counter = 0x1ff;
      *next_slot = *next_slot + 1;
      return;
    case 5:
      goto switchD_00438d36_caseD_5;
    case 6:
      goto switchD_00438d36_caseD_6;
    case 8:
      goto switchD_00438d36_caseD_8;
    case 10:
      goto switchD_00438ac0_caseD_a;
    }
switchD_00438d36_caseD_3:
    g_menu_item_anims[*next_slot].ref_b = -1;
    g_menu_item_anims[*next_slot].state = 1;
    g_menu_item_anims[*next_slot].anim_counter = 0;
    *next_slot = *next_slot + 1;
switchD_0043894c_caseD_3:
    return;
  case 5:
    menu_text_slot_set_r(next_slot,0xff100000,0x40,0x1ac,0);
    g_menu_item_anims[*next_slot].ref_a = 0x49;
    g_menu_item_anims[*next_slot].ref_a = 0x13;
    switch(neighbour_kind) {
    case 1:
      goto switchD_00438b52_caseD_1;
    case 2:
      goto switchD_00438b52_caseD_2;
    default:
switchD_00438b52_caseD_3:
      g_menu_item_anims[*next_slot].ref_b = -1;
      g_menu_item_anims[*next_slot].state = 1;
      g_menu_item_anims[*next_slot].anim_counter = 0;
      *next_slot = *next_slot + 1;
      return;
    case 4:
switchD_00438b52_caseD_4:
      g_menu_item_anims[*next_slot].ref_b = 0x22a;
      goto LAB_00438cb5;
    case 5:
      goto switchD_00438b52_caseD_8;
    case 6:
switchD_00438b52_caseD_6:
      g_menu_item_anims[*next_slot].ref_b = 0x5b;
      break;
    case 8:
      goto switchD_00438d36_caseD_8;
    case 10:
switchD_00438b52_caseD_a:
      g_menu_item_anims[*next_slot].ref_b = 0x138;
    }
    break;
  case 6:
    menu_text_slot_set_r(next_slot,0xff110000,0x40,0x1ac,0);
    g_menu_item_anims[*next_slot].ref_a = 0x5b;
    switch(neighbour_kind) {
    case 1:
switchD_00438d36_caseD_1:
      g_menu_item_anims[*next_slot].ref_b = 0x43;
      goto LAB_00438c83;
    case 2:
      goto switchD_00438c6c_caseD_2;
    default:
      goto switchD_00438c6c_caseD_3;
    case 4:
      goto switchD_00438c6c_caseD_4;
    case 5:
      goto switchD_00438c6c_caseD_5;
    case 6:
      goto switchD_00438c6c_caseD_6;
    case 8:
      goto switchD_00438c6c_caseD_8;
    case 10:
      goto switchD_00438c6c_caseD_a;
    }
  case 8:
    menu_text_slot_set_r(next_slot,0xff230000,0x40,0x1ac,0);
    page_id = menu_page_for_game_mode();
    g_menu_item_anims[*next_slot].ref_a = page_id;
    switch(neighbour_kind) {
    case 1:
      goto switchD_00438b52_caseD_1;
    case 2:
      goto switchD_00438b52_caseD_2;
    default:
      goto switchD_00438b52_caseD_3;
    case 4:
      goto switchD_00438b52_caseD_4;
    case 5:
      g_menu_item_anims[*next_slot].ref_b = 0x49;
      g_menu_item_anims[*next_slot].ref_b = 0x13;
      break;
    case 6:
      goto switchD_00438b52_caseD_6;
    case 8:
      goto switchD_00438b52_caseD_8;
    case 10:
      goto switchD_00438b52_caseD_a;
    }
    break;
  case 10:
    menu_text_slot_set_r(next_slot,0xff100000,0x40,0x1ac,0);
    g_menu_item_anims[*next_slot].ref_a = 0x138;
    switch(neighbour_kind) {
    case 1:
      goto switchD_00438d36_caseD_1;
    case 2:
      g_menu_item_anims[*next_slot].ref_b = 0x44;
      break;
    default:
      goto switchD_00438d36_caseD_3;
    case 4:
      goto switchD_00438d36_caseD_4;
    case 5:
      goto switchD_00438d36_caseD_5;
    case 6:
      goto switchD_00438d36_caseD_6;
    case 8:
      goto switchD_00438d36_caseD_8;
    case 10:
      goto switchD_00438d36_caseD_a;
    }
  }
  g_menu_item_anims[*next_slot].state = 0;
LAB_00438bfa:
  g_menu_item_anims[*next_slot].anim_counter = 0x1ff;
  *next_slot = *next_slot + 1;
  return;
}

/* menu_item_list_find_index_current_r: C -> original menu_item_list_find_index_current (registers ebx,edi) */
__declspec(naked) int menu_item_list_find_index_current_r(int p0, int p1)
{
    __asm {
        push ebx
        push edi
        mov ebx, dword ptr [esp + 12]
        mov edi, dword ptr [esp + 16]
        call menu_item_list_find_index_current
        pop edi
        pop ebx
        ret
    }
}

/* menu_text_slot_set_r: C -> original menu_text_slot_set (registers eax,edx,s0,s1,s2) */
__declspec(naked) int menu_text_slot_set_r(int * p0, int p1, int p2, int p3, int p4)
{
    __asm {
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        mov eax, dword ptr [esp + 16]
        mov edx, dword ptr [esp + 20]
        call menu_text_slot_set
        add esp, 12
        ret
    }
}

/* entry thunk at the original address of menu_button_prompts_build: register inputs (eax,esi,s0,s1) -> natural C menu_button_prompts_build_n; ret 0 */
__declspec(naked) void menu_button_prompts_build(void)
{
    __asm {
        push dword ptr [esp + 8]
        push dword ptr [esp + 8]
        push esi
        push eax
        call menu_button_prompts_build_n
        add esp, 16
        ret
    }
}
