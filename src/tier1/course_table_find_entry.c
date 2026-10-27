#include "mfl_tier2.h"
/*00603fc8*/ extern struct CourseEntry g_course_table[28];
/*006047a8*/ extern int g_course_table_count;


/* course_table_find_entry */

/* Look up a course entry in g_course_table by course ID, returning NULL if not found. */

CourseEntry * course_table_find_entry(uint course_id)
{
  CourseEntry *entry;
  int index;
  
  index = 0;
  if (0 < g_course_table_count) {
    entry = g_course_table;
    do {
      if (entry->course_id == course_id) {
        return entry;
      }
      index = index + 1;
      entry = entry + 1;
    } while (index < g_course_table_count);
  }
  return (CourseEntry *)0x0;
}
