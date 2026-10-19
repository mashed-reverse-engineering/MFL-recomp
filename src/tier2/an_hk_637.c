#include "mfl_tier2.h"
typedef int CourseId;
/*0042b110*/ extern int current_course_id();
/*00485300*/ extern int __cdecl file_read_into_buffer(void *p0, void *p1, RwUInt32 p2);      /* 3 stack args (the other pushes were sprintf's arguments, hoisted before the call) */
/*004b2a60*/ extern int sprintf();
/*008c7e80*/ extern undefined4 g_gsd_load_8c7e80;
/*008c7e84*/ extern undefined4 g_gsd_load_8c7e84;
/*008c7e8c*/ extern undefined4 g_gsd_load_8c7e8c;
/*008c7e90*/ extern undefined4 g_gsd_load_8c7e90;
/*008c7e94*/ extern undefined4 g_gsd_load_8c7e94;
/*008c7e98*/ extern undefined4 g_gsd_load_8c7e98;
/*008c7e9c*/ extern undefined4 g_gsd_load_8c7e9c;
/*008c7ea4*/ extern undefined4 g_gsd_load_8c7ea4;
/*008c7ea8*/ extern undefined4 g_gsd_load_8c7ea8;
/*008c7eac*/ extern undefined4 g_gsd_load_8c7eac;
/*005df2bc*/ extern char s_Boid_d_gsd_005df2bc[];

/* Bytes read from the .gsd file into the globals at 0x8c7e80 (inferred: spans 8c7e80..8c7eac) */
#define GSD_HEADER_SIZE (0x30)

/* gsd_load */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "Boid%d.gsd loader (the GSD format in MashedFormats)" */

/* Loads the GSD header for the current course (Boid<course_id>.gsd) into the globals at 0x8c7e80.
   Takes no parameters; returns nothing. If the file cannot be read, the globals get default values
   (values inferred). */
void gsd_load(void)

{
  CourseId course_id;
  int read_result;
  undefined1 gsd_path [256];

  course_id = current_course_id();
  sprintf(gsd_path,(char *)s_Boid_d_gsd_005df2bc,course_id);
  read_result = file_read_into_buffer(gsd_path,&g_gsd_load_8c7e80,GSD_HEADER_SIZE);
  /* Return value 0 = read failed (inferred from the branch): fall back to the built-in defaults */
  if (read_result == 0) {
    g_gsd_load_8c7e84 = 0x80;
    g_gsd_load_8c7e90 = 0x80;
    g_gsd_load_8c7e94 = 0x80;
    g_gsd_load_8c7e98 = 0x80;
    g_gsd_load_8c7e80 = 400;
    g_gsd_load_8c7e8c = 100;
    g_gsd_load_8c7e9c = 0;
    g_gsd_load_8c7ea4 = 1;
    g_gsd_load_8c7ea8 = 0;
    g_gsd_load_8c7eac = 0;
  }
  return;
}


