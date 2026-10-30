// cluster-entries: course_sandsnow_object_create course_sandsnow_object_render
/* Installed cluster course_sandsnow_object_create (tools/cluster_install.py from output/cluster_pilot_te5/course_sandsnow_object.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004a5d10*/ extern void *__cdecl object_frames_create_with_matrices(void *atomic, void *source, void *matrices);
/*004a5d70*/ extern void __cdecl object_render(void *object);
/*004c3f60*/ extern void *__cdecl clump_first_atomic(void *clump);
/*006a2638*/ extern void *g_course_sandsnow_6a2638;

#define COURSE_CLUMP_OFFSET 0xc110       /* clump table of the course block, one pointer per object index */
#define COURSE_SOURCE_OFFSET 0xc000      /* per index: source pointer for the frames */
#define COURSE_MATRICES_STRIDE 0xc00     /* per index: block of matrices at the start of the course block */

/* course_sandsnow_object_create: builds the render object from the first atomic of clump `clump_index` of the course block, with the source and the matrices of slot `index`
   (kept in g_course_sandsnow_6a2638); the caller passes clump 5, slot 0 */
void course_sandsnow_object_create(char *course, int clump_index, int index)
{
  void *atomic;

  atomic = clump_first_atomic(*(void **)(course + COURSE_CLUMP_OFFSET + clump_index * 4));
  g_course_sandsnow_6a2638 = object_frames_create_with_matrices(atomic, *(void **)(course + COURSE_SOURCE_OFFSET + index * 4), course + index * COURSE_MATRICES_STRIDE);
}

/* course_sandsnow_object_render: draws the object that course_sandsnow_object_create built */
void course_sandsnow_object_render(void)
{
  object_render(g_course_sandsnow_6a2638);
}

