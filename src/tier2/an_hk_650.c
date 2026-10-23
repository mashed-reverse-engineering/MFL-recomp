#include "mfl_tier2.h"
typedef int CourseId;
/*0042b110*/ extern int current_course_id();
/*004b2a60*/ extern int sprintf();
/*00652cc0*/ extern undefined1 g_led_file_name;
#define LED_BUF_SIZE (64)
#define LED_PREFIX_DWORDS (5)
/* led_file_name */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "builds toastart/common/led/LE<n>.LED into the static buffer at 0x652CC0 (no loading here; LE =
   line exceptions, chunk 0x13269901)" */

/* Builds "toastart/common/led/LE<course_id>.LED" in the static buffer g_led_file_name and returns it.
   course_id is the current course (current_course_id()); the file is not opened here.
   Each call overwrites g_led_file_name. */
undefined1 * led_file_name(void)

{
  char ch;
  CourseId course_id;
  char *cursor;
  uint suffix_bytes;
  int i;
  uint dword_count;
  char *buf_cursor;
  undefined4 *zero_ptr;
  char file_name[LED_BUF_SIZE];
  char path_prefix[LED_BUF_SIZE];

  /* copy the directory prefix "toastart/common/led/" (5 dwords, then its NUL byte) into path_prefix */
  cursor = "toastart/common/led/";
  buf_cursor = path_prefix;
  for (i = LED_PREFIX_DWORDS; i != 0; i = i + -1) {
    *(undefined4 *)buf_cursor = *(undefined4 *)cursor;
    cursor = cursor + 4;
    buf_cursor = buf_cursor + 4;
  }
  *buf_cursor = *cursor;
  /* clear the rest of path_prefix past the prefix (inferred: scratch space, 40 bytes + 3) */
  zero_ptr = ((undefined4 *)(path_prefix + 21));
  for (i = 10; i != 0; i = i + -1) {
    *zero_ptr = 0;
    zero_ptr = zero_ptr + 1;
  }
  *(undefined2 *)zero_ptr = 0;
  *(undefined1 *)((char *)zero_ptr + 2) = 0;
  /* clear file_name (64 bytes in total, starting with its first byte) */
  (*(char *)file_name) = '\0';
  zero_ptr = ((undefined4 *)(file_name + 1));
  for (i = 0xf; i != 0; i = i + -1) {
    *zero_ptr = 0;
    zero_ptr = zero_ptr + 1;
  }
  *(undefined2 *)zero_ptr = 0;
  *(undefined1 *)((char *)zero_ptr + 2) = 0;
  /* format "LE<course_id>.LED" into file_name */
  course_id = current_course_id();
  sprintf(file_name,"LE%d.LED",course_id);
  /* copy path_prefix to the start of g_led_file_name, up to and including its NUL */
  i = 0;
  do {
    ch = path_prefix[i];
    (&g_led_file_name)[i] = ch;
    i = i + 1;
  } while (ch != '\0');
  /* measure file_name: suffix_bytes = length + NUL */
  cursor = file_name;
  do {
    ch = *cursor;
    cursor = cursor + 1;
  } while (ch != '\0');
  suffix_bytes = (char *)cursor - (char *)file_name;
  /* walk to the NUL of g_led_file_name; 0x652cbf is one byte before it, so the first step lands on it */
  cursor = (char *)0x652cbf;
  do {
    buf_cursor = cursor + 1;
    cursor = cursor + 1;
  } while (*buf_cursor != '\0');
  /* append file_name (with its NUL) at that position: 4 bytes at a time, then the tail bytes */
  buf_cursor = file_name;
  for (dword_count = suffix_bytes >> 2; dword_count != 0; dword_count = dword_count - 1) {
    *(undefined4 *)cursor = *(undefined4 *)buf_cursor;
    buf_cursor = buf_cursor + 4;
    cursor = cursor + 4;
  }
  for (suffix_bytes = suffix_bytes & 3; suffix_bytes != 0; suffix_bytes = suffix_bytes - 1) {
    *cursor = *buf_cursor;
    buf_cursor = buf_cursor + 1;
    cursor = cursor + 1;
  }
  return &g_led_file_name;
}
