#include "mfl_tier2.h"
/*0042a420*/ extern int *course_token_list_base(void);
/* line_exceptions_find_index_a */
/* "searches the course token list (course_fn_42a420) for the id after unused_2, -10/-1/0x100 are
   terminators/repeat markers" */
/* TIER2: unvalidated 2026-10-21 authority=diffemu */
int line_exceptions_find_index_a(int id, int start)
{
  int *list;
  int i;
  int v;
  int w;
  int x;

  list = course_token_list_base();
  for (i = start; list[i] != -10;) {
    v = list[i];
    if (v == -1) {
      return 0;
    }
    w = list[i + 1];
    if (w == 0x100) {
      if (id == v) {
        if (i != start) {
          return id - 1;
        }
        x = list[i];
        if (x != -1) {
          do {
            if (x == -10) {
              return -10;
            }
            x = list[i + 1];
            i++;
          } while (x != -1);
        }
        return list[i - 1];
      }
      if (id == list[i + 2]) {
        return list[i];
      }
      i += 2;
    }
    else {
      if (v < id && id <= w) {
        return id - 1;
      }
      if (id == v) {
        if (i != start) {
          return list[i - 2];
        }
        x = list[i];
        if (x != -1) {
          do {
            if (x == -10) {
              return -10;
            }
            x = list[i + 1];
            i++;
          } while (x != -1);
        }
        return list[i - 1];
      }
      i++;
    }
  }
  return id;
}
