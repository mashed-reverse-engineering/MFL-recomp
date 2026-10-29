// cluster-entries: menu_items_animate_update
/* Installed cluster menu_items_animate_update (tools/cluster_install.py from output/cluster_pilot_kc/menu_items_animate_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*005dd330*/ extern float float_1_0;
/*005dd634*/ extern float k_f_2;
/*005ddad0*/ extern float k_f_180;
/*005ddb58*/ extern float k_f_1000;
/*005de244*/ extern float k_f_500;
/*005deb30*/ extern float k_f_n800;
/*005deb34*/ extern float k_f_300;
/*005deb38*/ extern float k_f_58;
/*005deb3c*/ extern float k_f_n2000;
/*0060a8c0*/ extern undefined g_menu_page_18_game_mode;
/*0060a990*/ extern undefined g_menu_page_24_game_mode;
/*0069caec*/ extern undefined4 g_menu_start_race_requested;
/*0069d1e0*/ extern int g_frontend_state;
/*0069d220*/ extern undefined4 g_ai_69d220;
/*007ef7ec*/ extern float g_frame_seconds;
/*008c80a0*/ extern struct MenuItemAnim g_menu_item_anims[30];
/* menu_items_animate_update */

/* Fields of MenuItemAnim the shared header does not name yet (offsets in bytes). */
#define ITEM_FADE(item) (*(float *)((char *)(item) + 0x8))      /* slide / fade angle of the plain items, 0 .. 180 */
#define ITEM_ROW(item) (*(int *)((char *)(item) + 0x20))        /* row of the item in its page list */
#define ITEM_PAGE(item) (*(char **)((char *)(item) + 0x30))     /* page the item belongs to (address of the page's game mode word) */

#define MENU_ITEM_STATE_APPEARING 0
#define MENU_ITEM_STATE_IDLE 1
#define MENU_ITEM_STATE_DISAPPEARING 2
#define MENU_ITEM_STATE_FINISHED 0x1000
#define MENU_ANIM_DONE 0x1ff
#define MENU_ANIM_LIMIT 400
#define MENU_ITEM_ID_SLIDE_FAST 0xff040000   /* slides in / out about twice as fast */

static int menu_item_y_position_r(uint p0, int p1);
extern void menu_item_y_position(void);
/* True for the item ids that animate through the counter (the others use the fade angle). */
static int item_uses_counter(int id)
{
  return id == 0xff000000 || id == 0xff100000 || id == 0xff110000 || id == 0xff120000 || id == 0xff130000 || id == 0xff230000;
}

/* Steps the animation of all 30 menu items by the frame time: items in state 0 / 2 count the
   anim counter down to 0 (then 0 -> state 1 idle, 2 -> state 0x1000), items in state 1 / 0x1000 count it up to the limit 400 and set 0x1ff (the state names are the header's; which direction is on screen is not confirmed, inferred); the plain items advance a fade angle.
   Item 0xff040000 also recomputes its y position from its page and row after it has appeared.
   Returns 0 while an item is still in the middle of an animation after the race start was requested, else 1. */
undefined4 menu_items_animate_update(void)

{
  MenuItemAnim *item;
  float speed;
  int state;
  int step;
  int result;
  int row;
  int y;
  char *page;

  speed = float_1_0;
  if (3 < g_frontend_state) {
    speed = k_f_2;
  }
  result = 1;
  for (item = g_menu_item_anims; item < g_menu_item_anims + 30; item++) {
    if (((item->item_id != 0) && (item->state != MENU_ITEM_STATE_FINISHED)) && (g_menu_start_race_requested != 0)) {
      result = 0;
    }
    if (item->item_id == MENU_ITEM_ID_SLIDE_FAST) {
      state = item->state;
      if ((state != MENU_ITEM_STATE_APPEARING) && (state != MENU_ITEM_STATE_DISAPPEARING)) {
        step = (int)(g_frame_seconds * speed * k_f_1000);
        item->anim_counter = item->anim_counter + step;
        if (MENU_ANIM_LIMIT - 1 < item->anim_counter) {
          item->anim_counter = MENU_ANIM_DONE;
          item->state = MENU_ITEM_STATE_FINISHED;
        }
      }
      else {
        step = (int)(g_frame_seconds * speed * k_f_n2000);
        item->anim_counter = item->anim_counter + step;
        if (item->anim_counter < 1) {
          if (state == MENU_ITEM_STATE_DISAPPEARING) {
            item->anim_counter = MENU_ANIM_DONE;
            item->state = MENU_ITEM_STATE_FINISHED;
          }
          else {
            row = ITEM_ROW(item);
            item->anim_counter = 0;
            item->state = MENU_ITEM_STATE_IDLE;
            y = menu_item_y_position_r(g_ai_69d220,0x1e);
            page = ITEM_PAGE(item);
            item->y = (float)y + (float)(row * 0x1e);
            if (page == &g_menu_page_24_game_mode) {
              y = menu_item_y_position_r(g_ai_69d220,0x1c);
              item->y = ((float)y + (float)(row * 0x1c)) - k_f_58;
            }
            if (page == &g_menu_page_18_game_mode) {
              item->y = item->y - k_f_58;
            }
          }
        }
      }
    }
    else if (item_uses_counter(item->item_id)) {
      state = item->state;
      if ((state != MENU_ITEM_STATE_APPEARING) && (state != MENU_ITEM_STATE_DISAPPEARING)) {
        step = (int)(g_frame_seconds * speed * k_f_500);
        item->anim_counter = item->anim_counter + step;
        if (MENU_ANIM_LIMIT - 1 < item->anim_counter) {
          item->anim_counter = MENU_ANIM_DONE;
          item->state = MENU_ITEM_STATE_FINISHED;
        }
      }
      else {
        step = (int)(g_frame_seconds * speed * k_f_n800);
        item->anim_counter = item->anim_counter + step;
        if (item->anim_counter < 1) {
          if (state == MENU_ITEM_STATE_DISAPPEARING) {
            item->anim_counter = MENU_ANIM_DONE;
            item->state = MENU_ITEM_STATE_FINISHED;
          }
          else {
            item->anim_counter = 0;
            item->state = MENU_ITEM_STATE_IDLE;
          }
        }
      }
    }
    else {
      ITEM_FADE(item) = g_frame_seconds * k_f_300 + ITEM_FADE(item);
      if (k_f_180 <= ITEM_FADE(item)) {
        *(int *)&ITEM_FADE(item) = 0x43340000; /* 180.0f */
        item->state = MENU_ITEM_STATE_FINISHED;
      }
    }
  }
  return result;
}

/* menu_item_y_position_r: C -> original menu_item_y_position (registers eax,ecx) */
__declspec(naked) int menu_item_y_position_r(uint p0, int p1)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        mov ecx, dword ptr [esp + 8]
        call menu_item_y_position
        ret
    }
}
