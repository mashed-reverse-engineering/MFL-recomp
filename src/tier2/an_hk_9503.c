#include "mfl_tier2.h"
/*004b3404*/ extern int toupper();
/*00554370*/ extern int __cdecl FUN_00554370(int p0);
/*005543a0*/ extern int __cdecl FUN_005543a0(int p0, int p1);
/*005543d0*/ extern char * __cdecl rw_first_field(int p0);
/* compile: /GS */

/* course_materials_fixup_cb_4065c0 */

/*
 * Callback of course_materials_fixup: walks the user data array entries of a RenderWare object and
 * sets *found to 1 when one of the entry names, upper-cased, is "JACFX.CFXTYPE".
 * Returns the object, so that a "for all" iteration continues.
 */
/* TIER2: unvalidated 2026-10-22 authority=static */
int course_materials_fixup_cb_4065c0(int object, int *found)
{
  char upper_name [64];
  int count;
  int entry;
  int i;
  char c;
  char *name;
  int length;
  int remaining;
  int is_cfxtype;
  char *a;
  char *b;

  count = FUN_00554370(object);
  if ((count != 0) && (entry = 0, 0 < count)) {
    do {
      name = rw_first_field(FUN_005543a0(object,entry));
      length = 0;
      do {
        c = name[length];
        length = length + 1;
      } while (c != '\0');
      length = length - 1; /* strlen */
      i = 0;
      if (0 < length) {
        do {
          upper_name[i] = (char)toupper(name[i]);
          i = i + 1;
        } while (i < length);
      }
      upper_name[i] = '\0';
      remaining = 0xe;
      is_cfxtype = 1;
      a = upper_name;
      b = "JACFX.CFXTYPE";
      do {
        if (remaining == 0) break;
        remaining = remaining + -1;
        is_cfxtype = *a == *b;
        a = a + 1;
        b = b + 1;
      } while (is_cfxtype);
      if (is_cfxtype) {
        *found = 1;
      }
      entry = entry + 1;
    } while (entry < count);
  }
  return object;
}
