#include "mfl_tier2.h"
typedef int CourseId;
/*0042b110*/ extern int current_course_id();
/*004b2a60*/ extern int sprintf();
/*0066431c*/ extern undefined4 g_ai_editor_66431c;
/*00664320*/ extern undefined1 g_ai_file_name;
/* 32-byte name buffers: 1 byte + NAME_BUF_ZERO_DWORDS dwords + 2 bytes + 1 byte, all cleared to "" */
#define NAME_BUF_ZERO_DWORDS (7)
/* offset into the string at g_ai_editor_66431c where the AI name is appended (inferred) */
#define EDITOR_NAME_OFFSET (3)
/* ai_file_name */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/*
 * Returns &g_ai_file_name, which this function leaves as "" (inferred: the copy of blank_name
 * always stores a NUL first). Also appends "AI<course id>.AI" for the current course to the
 * string at g_ai_editor_66431c, starting at EDITOR_NAME_OFFSET (inferred, check disassembly:
 * the first copied byte lands on the last char of that string).
 * (was: "builds AI<course id>.AI into the static buffer at 0x664320 and returns it (no loading here)")
 */

undefined1 * ai_file_name(void)

{
  char ch;
  CourseId course_id;
  char *cursor;
  uint name_len;
  int i;
  uint dword_count;
  char *src;
  undefined4 *zero_cursor;
  char ai_name[32];
  char blank_name[32];

  /* clear blank_name to "" */
  (*(char *)blank_name) = '\0';
  zero_cursor = ((undefined4 *)(blank_name + 1));
  for (i = NAME_BUF_ZERO_DWORDS; i != 0; i = i + -1) {
    *zero_cursor = 0;
    zero_cursor = zero_cursor + 1;
  }
  *(undefined2 *)zero_cursor = 0;
  *(undefined1 *)((char *)zero_cursor + 2) = 0;
  /* clear ai_name to "" */
  (*(char *)ai_name) = '\0';
  zero_cursor = ((undefined4 *)(ai_name + 1));
  for (i = NAME_BUF_ZERO_DWORDS; i != 0; i = i + -1) {
    *zero_cursor = 0;
    zero_cursor = zero_cursor + 1;
  }
  *(undefined2 *)zero_cursor = 0;
  *(undefined1 *)((char *)zero_cursor + 2) = 0;
  /* ai_name = "AI<course id>.AI" */
  course_id = current_course_id();
  sprintf(ai_name,"AI%d.AI",course_id);
  /* copy blank_name (empty) into g_ai_file_name, up to and including its NUL */
  i = 0;
  do {
    ch = (blank_name)[i];
    (&g_ai_file_name)[i] = ch;
    i = i + 1;
  } while (ch != '\0');
  /* name_len = strlen(ai_name) */
  cursor = ai_name;
  do {
    ch = *cursor;
    cursor = cursor + 1;
  } while (ch != '\0');
  name_len = (char *)cursor - (char *)ai_name;
  /* cursor = last char of g_ai_editor_66431c, scanning from EDITOR_NAME_OFFSET (inferred) */
  cursor = (char *)((char *)&g_ai_editor_66431c + EDITOR_NAME_OFFSET);
  do {
    src = cursor + 1;
    cursor = cursor + 1;
  } while (*src != '\0');
  /* copy name_len bytes of ai_name to cursor: whole dwords first, then the remaining name_len & 3 bytes */
  src = ai_name;
  for (dword_count = name_len >> 2; dword_count != 0; dword_count = dword_count - 1) {
    *(undefined4 *)cursor = *(undefined4 *)src;
    src = src + 4;
    cursor = cursor + 4;
  }
  for (name_len = name_len & 3; name_len != 0; name_len = name_len - 1) {
    *cursor = *src;
    src = src + 1;
    cursor = cursor + 1;
  }
  return &g_ai_file_name;
}


