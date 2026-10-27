#include "mfl_tier2.h"
/*0042b110*/ extern int current_course_id();

#define COURSE_ID_EXCEPTION_A 0x24   /* course that uses the alternate colours */
#define COURSE_ID_EXCEPTION_B 0xf    /* course that uses the alternate colours */


/* fx_style_init_j: fills the fx style record with colours and parameters; the alternate set is used for two courses. */

/* "colour bb c5 c8 aa unless course 0x24 or 0xF" */

void fx_style_init_j(undefined4 *style)
{
  int course_id;

  course_id = current_course_id();
  if (course_id != COURSE_ID_EXCEPTION_A) {
    course_id = current_course_id();
    if (course_id != COURSE_ID_EXCEPTION_B) {
      /* default style: float 1.0, colour bytes bb c5 c8 aa, floats 0.4 0.4 2.0, int 2 */
      *style = 0x3f800000;                       /* 1.0f */
      *(undefined1 *)(style + 1) = 0xbb;
      *(undefined1 *)((char *)style + 5) = 0xc5;
      *(undefined1 *)((char *)style + 6) = 200;
      *(undefined1 *)((char *)style + 7) = 0xaa;
      style[10] = 0x3ecccccd;                    /* 0.4f */
      style[0xb] = 0x3ecccccd;                   /* 0.4f */
      style[9] = 0x40000000;                     /* 2.0f */
      style[0xc] = 2;
      return;
    }
  }
  /* alternate style (courses 0x24 and 0xF): colour bytes a5 a5 9b 8c, floats 0.75 1.5 0.4 0.4, int 2, flags |= 0x40, int 15 */
  *(undefined1 *)(style + 1) = 0xa5;
  *(undefined1 *)((char *)style + 5) = 0xa5;
  style[10] = 0x3ecccccd;                        /* 0.4f */
  style[0xb] = 0x3ecccccd;                       /* 0.4f */
  *style = 0x3f400000;                           /* 0.75f */
  *(undefined1 *)((char *)style + 6) = 0x9b;
  *(undefined1 *)((char *)style + 7) = 0x8c;
  style[9] = 0x3fc00000;                         /* 1.5f */
  style[0xe] = 0xf;
  style[0xd] = style[0xd] | 0x40;                /* flag bit, meaning unknown */
  style[0xc] = 2;
  return;
}
