#include "mfl_tier2.h"
extern char s_english__005dfa40[];
extern char s_french__005dfa38[];
extern char s_italian__005dfa2c[];
extern char s_german__005dfa24[];
extern char s_spanish__005dfa18[];
/*004b2a60*/ extern int sprintf();
/*005dd5a2*/ extern undefined g_game_5dd5a2;
/*005dfa10*/ extern undefined4 g_sound_5dfa10;
/*005dfa14*/ extern undefined1 g_sound_5dfa14;
/*00618478*/ extern undefined1 g_sound_language_618478;
/*006add78*/ extern undefined1 g_sound_6add78;
/*006adcf4*/ extern char g_sound_frame_6adcf4[];
#define LANG_ENGLISH (0)
#define LANG_FRENCH (1)
#define LANG_GERMAN (2)
#define LANG_SPANISH (3)
#define LANG_ITALIAN (4)
#define SLOT_STRIDE (0x80)
#define SLOT_PATH_OFFSET (0x83)
#define SLOT_TABLE_LAST (0x2ff)
/* sound_language_path */

/* "pcdics\english french italian" */

/*
 * Builds the sound path for each of the six slots (SLOT_STRIDE apart, up to SLOT_TABLE_LAST).
 * Per slot: formats the base string from g_game_5dd5a2 into g_sound_6add78 (inferred), then appends
 * the "toastaudio\pc\audio\pcdics\" directory and the name of `language` to the slot's text in
 * g_sound_frame_6adcf4, then the tail copied from g_sound_language_618478, then the g_sound_5dfa10
 * word and the g_sound_5dfa14 byte.
 * language: LANG_* id; unknown ids use the English name. Returns after the last slot.
 */
void sound_language_path(undefined4 language)

{
  undefined1 suffix_byte;
  undefined4 french_word1;
  undefined4 english_word1;
  char english_byte8;
  char name_byte;
  char *str_end;
  int words_left;
  undefined4 name_word_a;
  uint remaining;
  undefined4 name_word_b;
  int slot_offset;
  char *copy_src;
  undefined4 *dst_cursor;
  char *str_pos;
  char *frame_dst;
  undefined4 *frame_end;

  slot_offset = 0;
  /* One pass per slot; slot_offset is the byte offset of the slot. */
  do {
    /* Format the slot's base string (inferred: g_game_5dd5a2 is the format). */
    sprintf((char *)&g_sound_6add78 + slot_offset,&g_game_5dd5a2);
    dst_cursor = (undefined4 *)(slot_offset + ((LONG_PTR)&g_sound_frame_6adcf4 + SLOT_PATH_OFFSET));
    /* Walk to the end of the slot's existing text. */
    do {
      str_pos = (char *)((char *)dst_cursor + 1);
      dst_cursor = (undefined4 *)((char *)dst_cursor + 1);
    } while (*str_pos != '\0');
    /* Copy the shared directory prefix into the slot, 8 words. The loop header also prefetches the
       name words: name_word_a = German, name_word_b = Italian, name_byte = Italian's last byte
       (and english_word1, french_word1, english_byte8). Spanish and the default overwrite them. */
    str_pos = "toastaudio\\pc\\audio\\pcdics\\";
    for (words_left = 7; english_byte8 = s_english__005dfa40[8], english_word1 = (*(uint *)((char *)&s_english__005dfa40 + 4)),
        french_word1 = (*(uint *)((char *)&s_french__005dfa38 + 4)), name_byte = s_italian__005dfa2c[8],
        name_word_b = (*(uint *)((char *)&s_italian__005dfa2c + 4)), name_word_a = (*(uint *)((char *)&s_german__005dfa24 + 4)), words_left != 0;
        words_left = words_left + -1) {
      *dst_cursor = *(undefined4 *)str_pos;
      str_pos = str_pos + 4;
      dst_cursor = dst_cursor + 1;
    }
    /* Append the chosen language name to the slot's text. Spanish has its own case; unknown ids take
       the default (English). */
    switch(language) {
    case LANG_ENGLISH:
      str_pos = (char *)(slot_offset + ((LONG_PTR)&g_sound_frame_6adcf4 + SLOT_PATH_OFFSET));
      do {
        str_end = str_pos;
        str_pos = str_end + 1;
      } while (str_end[1] != '\0');
      *(undefined4 *)(str_end + 1) = (*(uint *)((char *)&s_english__005dfa40 + 0));
      *(undefined4 *)(str_end + 5) = english_word1;
      str_end[9] = english_byte8;
      break;
    case LANG_FRENCH:
      str_pos = (char *)(slot_offset + ((LONG_PTR)&g_sound_frame_6adcf4 + SLOT_PATH_OFFSET));
      do {
        str_end = str_pos;
        str_pos = str_end + 1;
      } while (str_end[1] != '\0');
      *(undefined4 *)(str_end + 1) = (*(uint *)((char *)&s_french__005dfa38 + 0));
      *(undefined4 *)(str_end + 5) = french_word1;
      break;
    case LANG_GERMAN:
      str_pos = (char *)(slot_offset + ((LONG_PTR)&g_sound_frame_6adcf4 + SLOT_PATH_OFFSET));
      do {
        str_end = str_pos;
        str_pos = str_end + 1;
      } while (str_end[1] != '\0');
      *(undefined4 *)(str_end + 1) = (*(uint *)((char *)&s_german__005dfa24 + 0));
      *(undefined4 *)(str_end + 5) = name_word_a;
      break;
    case LANG_SPANISH:
      str_pos = (char *)(slot_offset + ((LONG_PTR)&g_sound_frame_6adcf4 + SLOT_PATH_OFFSET));
      do {
        str_end = str_pos + 1;
        str_pos = str_pos + 1;
        name_word_a = (*(uint *)((char *)&s_spanish__005dfa18 + 0));
        name_word_b = (*(uint *)((char *)&s_spanish__005dfa18 + 4));
        name_byte = s_spanish__005dfa18[8];
      } while (*str_end != '\0');
      goto LAB_0046ee4a;
    case LANG_ITALIAN:
      str_pos = (char *)(slot_offset + ((LONG_PTR)&g_sound_frame_6adcf4 + SLOT_PATH_OFFSET));
      do {
        str_end = str_pos;
        str_pos = str_end + 1;
      } while (str_end[1] != '\0');
      *(undefined4 *)(str_end + 1) = (*(uint *)((char *)&s_italian__005dfa2c + 0));
      *(undefined4 *)(str_end + 5) = name_word_b;
      str_end[9] = name_byte;
      break;
    default:
      str_pos = (char *)(slot_offset + ((LONG_PTR)&g_sound_frame_6adcf4 + SLOT_PATH_OFFSET));
      do {
        str_end = str_pos + 1;
        str_pos = str_pos + 1;
        name_word_a = (*(uint *)((char *)&s_english__005dfa40 + 0));
        name_word_b = (*(uint *)((char *)&s_english__005dfa40 + 4));
        name_byte = s_english__005dfa40[8];
      } while (*str_end != '\0');
LAB_0046ee4a:
      *(undefined4 *)str_pos = name_word_a;
      *(undefined4 *)(str_pos + 4) = name_word_b;
      str_pos[8] = name_byte;
    }
    /* Measure this slot's tail string in g_sound_language_618478 (name_byte is reused as the scan char). */
    str_pos = &g_sound_language_618478 + slot_offset;
    str_end = str_pos;
    do {
      name_byte = *str_end;
      str_end = str_end + 1;
    } while (name_byte != '\0');
    /* Find the end of the slot's text in the frame buffer. */
    frame_dst = (char *)(slot_offset + ((LONG_PTR)&g_sound_frame_6adcf4 + SLOT_PATH_OFFSET));
    do {
      copy_src = frame_dst + 1;
      frame_dst = frame_dst + 1;
    } while (*copy_src != '\0');
    /* Copy the tail into the slot: whole words first, then the remaining bytes. */
    copy_src = str_pos;
    for (remaining = (uint)((str_end - str_pos)) >> 2; remaining != 0; remaining = remaining - 1) {
      *(undefined4 *)frame_dst = *(undefined4 *)copy_src;
      copy_src = copy_src + 4;
      frame_dst = frame_dst + 4;
    }
    for (remaining = (str_end - str_pos) & 3; suffix_byte = g_sound_5dfa14,
        dst_cursor = (undefined4 *)(slot_offset + ((LONG_PTR)&g_sound_frame_6adcf4 + SLOT_PATH_OFFSET)), remaining != 0; remaining = remaining - 1) {
      *frame_dst = *copy_src;
      copy_src = copy_src + 1;
      frame_dst = frame_dst + 1;
    }
    /* Find the new end of the slot's text, then append the g_sound_5dfa10 word and the suffix byte. */
    do {
      frame_end = dst_cursor;
      dst_cursor = (undefined4 *)((char *)frame_end + 1);
    } while (*(char *)((char *)frame_end + 1) != '\0');
    slot_offset = slot_offset + SLOT_STRIDE;
    *(undefined4 *)((char *)frame_end + 1) = g_sound_5dfa10;
    *(undefined1 *)((char *)frame_end + 5) = suffix_byte;
    if (SLOT_TABLE_LAST < slot_offset) {
      return;
    }
  } while( true );
}



