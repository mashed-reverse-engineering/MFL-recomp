#include "mfl_tier2.h"
/*0042b450*/ extern undefined __cdecl course_open_and_select(undefined4 p0);
/*0048a610*/ extern int spring_grid_init();

#ifndef NULL
#define NULL 0
#endif

#define SETUP_ENTRIES_OFFSET 4 /* Offset to course entries table (unknown header at offset 0) */
#define SETUP_ENTRY_SIZE     4 /* Size of each course entry pointer in bytes */


/* course_load_from_setup */

/* "(setup, index): course_open_and_run_scripts(*entry) and spring_grid_init" */

/* Load and initialize the course at the given index from setup data. */
void course_load_from_setup(char *setup, int course_index)
{
  int *entry;
  
  /* Retrieve course entry pointer from setup table (unknown header structure at offset 0) */
  entry = *(int **)(setup + SETUP_ENTRIES_OFFSET + course_index * SETUP_ENTRY_SIZE);
  /* If course entry exists, open and select the course, then initialize the spring grid */
  if (entry != NULL) {
    course_open_and_select(*entry);
    spring_grid_init();
    return;
  }
  return;
}

