// cluster-entries: course_object_render_6a1f28 course_object_render_6a1f28_b
/* Installed cluster course_object_render_6a1f28 (tools/cluster_install.py from output/cluster_pilot_zy/course_object_render_6a1f28.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*006a1f28*/ extern int g_course_6a1f28;
/*004a5d70*/ extern void object_render(int object);

/* Renders the course object g_course_6a1f28.  The body at 0x4558d0 overwrites its own stack argument with g_course_6a1f28 and tail-jumps to object_render (callers push an argument that is ignored). */
void course_object_render_6a1f28_b(int ignored)
{
  object_render(g_course_6a1f28);
}

/* The 5 byte entry at 0x454430 is a plain `jmp` to the body above. */
void course_object_render_6a1f28(int ignored)
{
  course_object_render_6a1f28_b(ignored);
}

