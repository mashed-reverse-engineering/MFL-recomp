#include "mfl_tier2.h"
/*0069cbd0*/ extern undefined4 g_menu_current;
/*0069d2e0*/ extern struct MenuRecord g_menu_records[14];
/* menu_current_item_offset */
/* "computes the item list offset of the selected entry of the current menu" */
int menu_current_item_offset(void)
{
  int *list;
  int i;
  int n;

  list = (int *)g_menu_records[g_menu_current].item_list;
  i = 0;
  if (g_menu_records[g_menu_current].selected != -1) {
    n = g_menu_records[g_menu_current].selected + 1;
    do {
      while (list[i] != 0xff040000) {
        i++;
      }
      i++;
      n--;
    } while (n != 0);
  }
  while (list[i] != 0xff050000 && list[i] != 0xff140000 && list[i] != 0xff060000) {
    i++;
  }
  if (list[i] == 0xff060000) {
    return -1;
  }
  return list[i + 1];
}
