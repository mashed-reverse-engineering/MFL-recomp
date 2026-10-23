#include "mfl_tier2.h"
/*0042a420*/ extern int *course_token_list_base(void);
/* line_exceptions_find_value */
/* "same token walk, returns the value following the id" */
/* TIER2: unvalidated 2026-10-21 authority=diffemu */
int line_exceptions_find_value(int id, int start)
{
  int *list;
  int i;
  int v;
  int w;
  int first;
  int x;

  list = course_token_list_base();
  first = list[start];
  if (first == -10) {
    return id;
  }
  for (i = start; list[i] != -10;) {
    v = list[i];
    if (v == -1) {
      return 0;
    }
    w = list[i + 1];
    if (w == 0x100) {
      if (id == v) {
        return list[i + 2];
      }
      if (id == list[i + 2]) {
        return id + 1;
      }
      i += 2;
    }
    else {
      if (id >= v && id < w) {
        return id + 1;
      }
      if (id == w) {
        x = list[i + 2];
        if (x == -10) {
          return id;
        }
        if (x == -1) {
          return first;
        }
        return list[i + 3];
      }
      i++;
    }
  }
  return id;
}
