#include "mfl_tier2.h"
extern char PTR_LAB_005de97c[];
/*0069be78*/ extern undefined1 g_course_content_69be78;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/* course_content_path_build */

/* "builds the course content path in g_course_69be78" */

/* Builds the course content path in the global g_course_content_69be78 (inferred: the buffer named in the string comment above).
 * Copies the base path string PTR_LAB_005de97c into it (inferred: content directory prefix), appends course_name,
 * then converts every '/' in the result to '\' in place.
 * course_name: course file name or sub-path to append (inferred).
 * Returns: pointer to g_course_content_69be78. */
undefined1 * course_content_path_build(RwChar *course_name)

{
  char *next;
  char ch;
  char *cursor;

  cursor = &g_course_content_69be78;
  rwstrcpy(&g_course_content_69be78,(RwChar *)&PTR_LAB_005de97c)
  ;
  rwstrcat(&g_course_content_69be78,course_name);
  ch = g_course_content_69be78;
  /* Walk the string up to its NUL terminator, replacing each '/' with '\'. */
  while (ch != '\0') {
    if (*cursor == '/') {
      *cursor = '\\';
    }
    next = cursor + 1;
    cursor = cursor + 1;
    ch = *next;
  }
  return &g_course_content_69be78;
}


