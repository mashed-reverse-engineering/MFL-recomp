#include "mfl_tier2.h"
extern char *PTR_g_texdict_load_5de908_00609a14;
/*0042f6e0*/ extern int game_content_find();
/*004b2ed0*/ extern char * __cdecl _strchr(char *p0, int p1);
/*004c3de0*/ extern int FUN_004c3de0();
/*0069bf78*/ extern undefined g_texdict_load_69bf78;
/*0069c778*/ extern undefined g_texdict_load_69c778;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;

#define FILE_EXT_SEPARATOR (0x2e) /* '.' */
/* texdict_load_named_d */

/*
 * Loads a texture dictionary by file name (inferred: from the name and the
 * content lookup). Copies file_name into g_texdict_load_69bf78; if it has no
 * '.', appends the default extension from PTR_g_texdict_load_5de908_00609a14.
 * If game_content_find() finds the result in g_texdict_load_69c778, returns
 * FUN_004c3de0's result for that container, load_arg_2 and load_arg_3 passed
 * through unchanged (meanings unknown). Otherwise returns 0.
 * unused_4 and unused_5 are not read.
 */

undefined4 __cdecl texdict_load_named_d(char *file_name,undefined4 load_arg_2,undefined4 load_arg_3,char *unused_4,char *unused_5)

{
  char *dot_pos;
  int content_found;
  undefined4 load_result;
  
  rwstrcpy(&g_texdict_load_69bf78,file_name);
  dot_pos = _strchr(file_name,FILE_EXT_SEPARATOR);
  /* No extension in the name: append the default one. */
  if (dot_pos == (char *)0x0) {
    rwstrcat
              (&g_texdict_load_69bf78,PTR_g_texdict_load_5de908_00609a14);
  }
  /* Only load if the name is found in the content list (inferred). */
  content_found = game_content_find(&g_texdict_load_69c778,&g_texdict_load_69bf78);
  if (content_found != 0) {
    load_result = FUN_004c3de0(((int)&g_texdict_load_69c778), load_arg_2, load_arg_3);
    return load_result;
  }
  return 0;
}


