#include "mfl_tier2.h"
extern char *PTR_g_game_5de410_00609a08;
/*0042f6e0*/ extern int game_content_find();
/*004b2ed0*/ extern char * __cdecl _strchr(char *p0, int p1);
/*004c3d20*/ extern int FUN_004c3d20();
/*004c3d80*/ extern int texdict_load_file(char * p0, int p1, int p2);
/*0069c178*/ extern undefined g_texdict_load_69c178;
/*0069c378*/ extern undefined g_texdict_load_69c378;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;

#define EXTENSION_SEPARATOR (0x2e) /* '.' */

/* texdict_load_named_b */

/*
 * Loads a texture dictionary by file name (called by 11 callers).
 * Copies file_name into a scratch buffer; if it has no '.' extension, appends
 * the default extension (PTR_g_game_5de410_00609a08, inferred). Then asks
 * game_content_find for the full name, and:
 *   find result 1: loads it with texdict_load_file(path, extra_arg_2, extra_arg_3)
 *   other non-zero: hands it to FUN_004c3d20 (inferred: alternate loader)
 *   0: not found, returns 0.
 * extra_arg_2 / extra_arg_3 are only passed through (inferred).
 * Returns the loader's result, or 0 if the content was not found.
 * (was: "copies the name, strips the extension, finds the content (11 callers)")
 */

undefined4 texdict_load_named_b(char *file_name, undefined4 extra_arg_2, undefined4 extra_arg_3)

{
  char *extension_dot;
  int find_result;
  undefined4 load_result;

  rwstrcpy(&g_texdict_load_69c178,file_name);
  /* no '.' in the name: append the default extension */
  extension_dot = _strchr(file_name,EXTENSION_SEPARATOR);
  if (extension_dot == (char *)0x0) {
    rwstrcat(&g_texdict_load_69c178,PTR_g_game_5de410_00609a08);
  }
  find_result = game_content_find(&g_texdict_load_69c378,&g_texdict_load_69c178);
  /* found as the expected kind: load the dictionary */
  if (find_result == 1) {
    load_result = texdict_load_file((char *)(((char *)&g_texdict_load_69c378)), extra_arg_2, extra_arg_3);
    return load_result;
  }
  /* other non-zero result: alternate path (inferred) */
  if (find_result != 0) {
    load_result = FUN_004c3d20(((char *)&g_texdict_load_69c378), extra_arg_2, extra_arg_3);
    return load_result;
  }
  return 0;
}


