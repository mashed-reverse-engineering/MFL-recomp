#include "mfl_tier2.h"
/*004231b0*/ extern int vehicle_model_path();
/*004b2ed0*/ extern char * __cdecl _strchr(char *p0, int p1);
/*00560d00*/ extern void FUN_00560d00(void);
#define PATH_BUF_SIZE (32)
#define EXT_SEPARATOR (0x2e) /* '.' */
/* course_model_path_copy */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "copies the vehicle_model_path(id) string into a local buffer" */

/* Copies the model path for course_id (from vehicle_model_path) into a local buffer,
   then cuts it at the first '.' so the file extension is dropped.
   Parameters: course_id = course id passed to vehicle_model_path;
   unused_param_2 = never read or written by the decompiled body (inferred: check disassembly).
   Returns nothing: the stripped path lives only in the local buffer. */
void __cdecl course_model_path_copy(undefined4 course_id,char *unused_param_2)

{
  char ch;
  char *src;
  char *dst;
  char path_buf [PATH_BUF_SIZE];

  dst = path_buf;
  /* Call with no visible effect on the result (inferred: setup or lock, name unknown). */
  FUN_00560d00();
  src = (char *)vehicle_model_path(course_id);
  /* Copy the string byte by byte, including the terminating NUL. */
  do {
    ch = *src;
    src = src + 1;
    *dst = ch;
    dst = dst + 1;
  } while (ch != '\0');
  /* Find the extension separator and terminate the string there. */
  src = _strchr(path_buf,EXT_SEPARATOR);
  *src = '\0';
  return;
}


