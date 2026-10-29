#include "mfl_tier2.h"
/*004a8420*/ extern int debug_printf();
/*004b2a60*/ extern int sprintf();
/*004b2ed0*/ extern char * __cdecl _strchr(char *p0, int p1);
/*005d2cfc*/ extern char * __cdecl __strupr(char *p0);
/*005dd57c*/ extern undefined2 g_game_5dd57c;
/*005dd580*/ extern undefined g_game_path_5dd580;
/*005dd5a2*/ extern undefined g_game_5dd5a2;
/*005dd618*/ extern undefined g_game_5dd618;
#define DIR_NAME_MAX (8)
#define BASE_NAME_MAX (8)
#define MAX_DIR_LEVELS (6)
#define DIR_SLOT_SIZE (9)
#define OUTPUT_PATH_MAX (0x31)
#define PATH_SEP_BACKSLASH (0x5c)
#define PATH_SEP_SLASH (0x2f)
#define EXT_DOT (0x2e)
#define DRIVE_COLON (0x3a)
#define SCRATCH_PATH_OFS (20)
#define SCRATCH_BASE_OFS (8)
/* game_path_to_8_3_upper */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* drops drive letter; dirs and names max 8 letters; total max 0x31 */

/*
 * Converts a game file path into an upper-case 8.3-style DOS path (inferred from the
 * limits below).
 *   in_path:  input path, '\' or '/' separators. A drive prefix ("C:") is replaced by
 *             g_game_5dd5a2 (inferred).
 *   out_path: receives the converted path. A leading '\' is stripped (inferred).
 * Directory parts after the first are upper-cased, and "PS2" / "XBOX" / "PLATFORM" are replaced by g_game_path_5dd580.  The separator is "\\" when the
 * input contains a ':' (the first slot is then g_game_5dd5a2), else "/".
 * Each directory name is at most DIR_NAME_MAX letters, at most MAX_DIR_LEVELS levels.
 * The file base name is at most BASE_NAME_MAX letters, the extension follows '.'.
 * Returns out_path, or NULL if a name is too long or the total exceeds OUTPUT_PATH_MAX.
 */
char * game_path_to_8_3_upper(char *out_path,char *in_path)

{
  char ch;
  char *cursor;
  char *pos;
  int dir_idx;
  int n_left;
  uint count;
  uint word_count;
  char *src;
  char *dst;
  undefined4 *fill_ptr;
  bool matched;
  int dir_count;
  char *separator;
  char scratch_path[72];
  char dir_slots[184];

  /* clear the scratch buffer's tail, then copy in_path to the working copy (SCRATCH_PATH_OFS) */
  (*(char *)(scratch_path + SCRATCH_PATH_OFS)) = '\0';
  fill_ptr = (undefined4 *)((scratch_path + 21));
  for (dir_idx = 0xc; dir_idx != 0; dir_idx = dir_idx + -1) {
    *fill_ptr = 0;
    fill_ptr = fill_ptr + 1;
  }
  *(undefined1 *)fill_ptr = 0;
  dir_count = 0;
  cursor = in_path;
  do {
    ch = *cursor;
    cursor[(int)(scratch_path + 21) + (-1 - (int)in_path)] = ch;
    cursor = cursor + 1;
  } while (ch != '\0');
  /* split the working copy at each '\' or '/': every directory part goes into its own
     DIR_SLOT_SIZE slot, and fails if it is longer than DIR_NAME_MAX letters */
  dir_idx = 0;
  cursor = dir_slots;
  do {
    pos = _strchr((scratch_path + SCRATCH_PATH_OFS),PATH_SEP_BACKSLASH);
    if ((pos != (char *)0x0) || (pos = _strchr((scratch_path + SCRATCH_PATH_OFS),PATH_SEP_SLASH), pos != (char *)0x0)) {
      *pos = '\0';
      sprintf(cursor,&g_game_5dd618,(scratch_path + SCRATCH_PATH_OFS));
      pos = pos + 1;
      n_left = -1 - (int)pos;
      do {
        ch = *pos;
        pos[(int)(scratch_path + 21) + n_left] = ch;
        pos = pos + 1;
      } while (ch != '\0');
      dir_count = dir_count + 1;
      pos = cursor;
      do {
        ch = *pos;
        pos = pos + 1;
      } while (ch != '\0');
      /* directory name too long: report it and fail */
      if (DIR_NAME_MAX < (uint)((char *)pos - (char *)(cursor + 1))) {
        debug_printf("Directory names must have no more than %d letters\n",DIR_NAME_MAX);
        cursor = dir_slots + dir_idx * DIR_SLOT_SIZE;
        do {
          ch = *cursor;
          cursor = cursor + 1;
        } while (ch != '\0');
        debug_printf("[%s] has %d\n",dir_slots + dir_idx * DIR_SLOT_SIZE,
                     (char *)cursor - (char *)(dir_slots + dir_idx * DIR_SLOT_SIZE + 1));
        return (char *)0x0;
      }
    }
    dir_idx = dir_idx + 1;
    cursor = cursor + DIR_SLOT_SIZE;
  } while (dir_idx < MAX_DIR_LEVELS);
  /* what is left is the file name: cut at '.', base name goes to SCRATCH_BASE_OFS */
  pos = _strchr((scratch_path + SCRATCH_PATH_OFS),EXT_DOT);
  *pos = '\0';
  sprintf((scratch_path + SCRATCH_BASE_OFS),&g_game_5dd618,(scratch_path + SCRATCH_PATH_OFS));
  cursor = (scratch_path + SCRATCH_BASE_OFS);
  do {
    ch = *cursor;
    cursor = cursor + 1;
  } while (ch != '\0');
  /* base name must fit in BASE_NAME_MAX letters (the `< 9`) */
  if ((uint)((char *)cursor - (int)((scratch_path + SCRATCH_BASE_OFS) + 1)) < 9) {
    sprintf(scratch_path,&g_game_5dd618,pos + 1);
    /* a ':' in the input (drive path) means the first slot holds g_game_5dd5a2 and the
       separator is '\'; otherwise '/' (inferred) */
    cursor = _strchr(in_path,DRIVE_COLON);
    if (cursor == (char *)0x0) {
      separator = "/";
    }
    else {
      sprintf(dir_slots,&g_game_5dd618,&g_game_5dd5a2);
      separator = "\\";
    }
    /* skip slot 0 (drive/root), and for every later directory, upper-case it and map the
       platform folders PS2 / XBOX / PLATFORM to g_game_path_5dd580 (inferred) */
    if (1 < dir_count) {
      cursor = (dir_slots + DIR_SLOT_SIZE);
      dir_idx = dir_count + -1;
      do {
        __strupr(cursor);
        n_left = 4;
        matched = true;
        pos = cursor;
        dst = "PS2";
        do {
          if (n_left == 0) break;
          n_left = n_left + -1;
          matched = *pos == *dst;
          pos = pos + 1;
          dst = dst + 1;
        } while (matched);
        if (matched) {
LAB_0040364f:
          sprintf(cursor,&g_game_5dd618,&g_game_path_5dd580);
        }
        else {
          n_left = 5;
          matched = true;
          pos = cursor;
          dst = "XBOX";
          do {
            if (n_left == 0) break;
            n_left = n_left + -1;
            matched = *pos == *dst;
            pos = pos + 1;
            dst = dst + 1;
          } while (matched);
          if (matched) goto LAB_0040364f;
          n_left = 9;
          matched = true;
          pos = cursor;
          dst = "PLATFORM";
          do {
            if (n_left == 0) break;
            n_left = n_left + -1;
            matched = *pos == *dst;
            pos = pos + 1;
            dst = dst + 1;
          } while (matched);
          if (matched) goto LAB_0040364f;
        }
        cursor = cursor + DIR_SLOT_SIZE;
        dir_idx = dir_idx + -1;
      } while (dir_idx != 0);
    }
    /* out_path = root prefix (g_game_5dd5a2), then append each directory slot followed by
       the separator; the copies are done word-wise, then the leftover bytes */
    sprintf(out_path,&g_game_5dd5a2);
    if (0 < dir_count) {
      cursor = dir_slots;
      pos = cursor;
      do {
        do {
          ch = *cursor;
          cursor = cursor + 1;
        } while (ch != '\0');
        dst = out_path + -1;
        do {
          src = dst + 1;
          dst = dst + 1;
        } while (*src != '\0');
        src = pos;
        for (count = (uint)((cursor - pos)) >> 2; count != 0; count = count - 1) {
          *(undefined4 *)dst = *(undefined4 *)src;
          src = src + 4;
          dst = dst + 4;
        }
        for (count = (cursor - pos) & 3; cursor = separator, count != 0;
            count = count - 1) {
          *dst = *src;
          src = src + 1;
          dst = dst + 1;
        }
        do {
          ch = *cursor;
          cursor = cursor + 1;
        } while (ch != '\0');
        count = (cursor - separator);
        dst = out_path + -1;
        do {
          cursor = dst + 1;
          dst = dst + 1;
        } while (*cursor != '\0');
        src = separator;
        for (word_count = count >> 2; word_count != 0; word_count = word_count - 1) {
          *(undefined4 *)dst = *(undefined4 *)src;
          src = src + 4;
          dst = dst + 4;
        }
        cursor = pos + DIR_SLOT_SIZE;
        dir_count = dir_count + -1;
        for (count = count & 3; count != 0; count = count - 1) {
          *dst = *src;
          src = src + 1;
          dst = dst + 1;
        }
        pos = cursor;
      } while (dir_count != 0);
    }
    /* append the upper-cased base name, then the '.' (g_game_5dd57c, 2 bytes), then the
       upper-cased extension */
    __strupr((scratch_path + SCRATCH_BASE_OFS));
    __strupr(scratch_path);
    cursor = (scratch_path + SCRATCH_BASE_OFS);
    do {
      ch = *cursor;
      cursor = cursor + 1;
    } while (ch != '\0');
    count = (int)cursor - (int)(scratch_path + SCRATCH_BASE_OFS);
    cursor = out_path + -1;
    do {
      pos = cursor + 1;
      cursor = cursor + 1;
    } while (*pos != '\0');
    pos = (scratch_path + SCRATCH_BASE_OFS);
    for (word_count = count >> 2; word_count != 0; word_count = word_count - 1) {
      *(undefined4 *)cursor = *(undefined4 *)pos;
      pos = pos + 4;
      cursor = cursor + 4;
    }
    for (count = count & 3; count != 0; count = count - 1) {
      *cursor = *pos;
      pos = pos + 1;
      cursor = cursor + 1;
    }
    cursor = out_path + -1;
    do {
      pos = cursor + 1;
      cursor = cursor + 1;
    } while (*pos != '\0');
    *(undefined2 *)cursor = g_game_5dd57c;
    cursor = scratch_path;
    do {
      ch = *cursor;
      cursor = cursor + 1;
    } while (ch != '\0');
    count = (char *)cursor - (char *)scratch_path;
    cursor = out_path + -1;
    do {
      pos = cursor + 1;
      cursor = cursor + 1;
    } while (*pos != '\0');
    pos = scratch_path;
    for (word_count = count >> 2; word_count != 0; word_count = word_count - 1) {
      *(undefined4 *)cursor = *(undefined4 *)pos;
      pos = pos + 4;
      cursor = cursor + 4;
    }
    for (count = count & 3; count != 0; count = count - 1) {
      *cursor = *pos;
      pos = pos + 1;
      cursor = cursor + 1;
    }
    /* out_path starts with '\': shift the whole string one byte left (via dir_slots as a
       temporary copy) */
    if (*out_path == '\\') {
      (*(char *)(dir_slots + 56)) = '\0';
      fill_ptr = ((undefined4 *)(dir_slots + 57));
      for (dir_idx = 0x1f; dir_idx != 0; dir_idx = dir_idx + -1) {
        *fill_ptr = 0;
        fill_ptr = fill_ptr + 1;
      }
      *(undefined2 *)fill_ptr = 0;
      *(undefined1 *)((char *)fill_ptr + 2) = 0;
      cursor = out_path + 1;
      dir_idx = -(int)cursor;
      do {
        ch = *cursor;
        cursor[(int)((dir_slots + 56) + dir_idx)] = ch;
        cursor = cursor + 1;
      } while (ch != '\0');
      cursor = (dir_slots + 56);
      do {
        ch = *cursor;
        cursor[(char *)out_path - (char *)(dir_slots + 56)] = ch;
        cursor = cursor + 1;
      } while (ch != '\0');
    }
    /* the finished path must fit in OUTPUT_PATH_MAX letters */
    cursor = out_path;
    do {
      ch = *cursor;
      cursor = cursor + 1;
    } while (ch != '\0');
    if (OUTPUT_PATH_MAX < (uint)((char *)cursor - (char *)(out_path + 1))) {
      debug_printf("FileName is too long (%dchars max)\n",OUTPUT_PATH_MAX);
      out_path = (char *)0x0;
    }
  }
  else {
    debug_printf("File names must have no more than %d letters\n",BASE_NAME_MAX);
    out_path = (char *)0x0;
  }
  return out_path;
}


