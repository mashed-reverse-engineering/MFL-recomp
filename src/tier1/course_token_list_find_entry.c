#include "mfl_tier2.h"
/*0042a420*/ extern int *course_token_list_base(void);
/* course_token_list_find_entry */
/* "walks the course token list for the entry containing the id and returns its index" */
int course_token_list_find_entry(int id)
{
  int *list;
  int i;
  int ret;
  int v;
  int w;

  list = course_token_list_base();
  i = 0;
  ret = 0;
  for (;;) {
    v = list[i];
    if (v == -1 || v == -10) {
      w = list[i + 1];
      i++;
      if (w == -2) {
        return ret;
      }
      ret = i;
    }
    else {
      w = list[i + 1];
      if (w == 0x100) {
        if (id == v) {
          return ret;
        }
        if (id == list[i + 2]) {
          return ret;
        }
        i += 2;
      }
      else {
        if (id >= v && id <= w) {
          return ret;
        }
        i++;
      }
    }
  }
}
