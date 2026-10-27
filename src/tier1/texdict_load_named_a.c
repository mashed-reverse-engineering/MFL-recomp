#include "mfl_tier2.h"
extern char *PTR_g_texdict_load_5de920_00609a04;
/*0042f6e0*/ extern int game_content_find();
/*004b2ed0*/ extern char * __cdecl _strchr(char *p0, int p1);
/*004c3c60*/ extern int FUN_004c3c60();
/*0069bd78*/ extern undefined g_texdict_load_69bd78;
/*0069c578*/ extern undefined g_texdict_load_69c578;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
#define FILE_EXTENSION_SEPARATOR (0x2e) /* '.' */
/* texdict_load_named_a */
/* "content find + RwStream texdict read" */
/*
 * Loads the texture dictionary called texdict_name (inferred: name of a texdict file).
 * Copies the name into the shared buffer g_texdict_load_69c578. If the name has no '.'
 * extension, the default extension string is appended. Then game_content_find() checks
 * that the content exists; if it does, FUN_004c3c60() is called with unused2/unused3
 * (inferred: the actual texdict read) and its result is returned. Returns 0 if not found.
 */
undefined4 texdict_load_named_a(char *texdict_name, undefined4 unused2, undefined4 unused3)
{
  char *extension_dot;
  int content_found;
  undefined4 load_result;

  rwstrcpy(&g_texdict_load_69c578,texdict_name);
  /* no '.' in the name: append the default extension */
  extension_dot = _strchr(texdict_name,FILE_EXTENSION_SEPARATOR);
  if (extension_dot == (char *)0x0) {
    rwstrcat
              (&g_texdict_load_69c578,PTR_g_texdict_load_5de920_00609a04);
  }
  /* the content must exist before the dictionary is read */
  content_found = game_content_find(&g_texdict_load_69bd78,&g_texdict_load_69c578);
  if (content_found != 0) {
    load_result = FUN_004c3c60(((char *)&g_texdict_load_69bd78), unused2, unused3);
    return load_result;
  }
  return 0;
}
