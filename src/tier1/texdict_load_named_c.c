#include "mfl_tier2.h"
extern char *PTR_g_texdict_load_5de910_00609a10;
/*0042f6e0*/ extern int game_content_find();
/*004b2ed0*/ extern char * __cdecl _strchr(char *p0, int p1);
/*004c3cc0*/ extern int FUN_004c3cc0();
/*0069c078*/ extern undefined g_texdict_load_69c078;
/*0069c278*/ extern undefined g_texdict_load_69c278;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
#define EXTENSION_DOT (0x2e) /* '.' */
/* texdict_load_named_c */
/* Loads the texture dictionary named dict_name. The name is copied into g_texdict_load_69c078;
   if it has no '.' the default extension (PTR_g_texdict_load_5de910_00609a10) is appended.
   game_content_find looks the name up in g_texdict_load_69c278; if found, the content is passed
   to FUN_004c3cc0 with load_arg_2 and load_arg_3 (FUN_004c3cc0 is inferred to be the loader) and
   its result is returned. Returns 0 if the content was not found.
   Parameters: dict_name = dictionary file name; load_arg_2, load_arg_3 = (inferred) forwarded unchanged. */
undefined4 __cdecl texdict_load_named_c(char *dict_name,undefined4 load_arg_2,undefined4 load_arg_3)
{
  char *dot_pos;
  int found;
  undefined4 loader_result;

  rwstrcpy(&g_texdict_load_69c078,dict_name);
  dot_pos = _strchr(dict_name,EXTENSION_DOT);
  /* no extension in the name: append the default one */
  if (dot_pos == (char *)0x0) {
    rwstrcat
              (&g_texdict_load_69c078,PTR_g_texdict_load_5de910_00609a10);
  }
  found = game_content_find(&g_texdict_load_69c278,&g_texdict_load_69c078);
  /* content found: hand it to the loader (inferred) */
  if (found != 0) {
    loader_result = FUN_004c3cc0(((char *)&g_texdict_load_69c278), load_arg_2, load_arg_3);
    return loader_result;
  }
  return 0;
}
