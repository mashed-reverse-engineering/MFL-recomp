#include "mfl_tier2.h"
extern char *PTR_g_game_5de408_00609a00;
/*0042f6e0*/ extern int game_content_find();
/*004b2ed0*/ extern char * __cdecl _strchr(char *p0, int p1);
/*004c3bf0*/ extern int FUN_004c3bf0(undefined4 p0, undefined4 p1, undefined4 p2);
/*0069bb78*/ extern undefined g_clump_load_69bb78;
/*0069c878*/ extern undefined g_clump_load_69c878;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/* clump_load_by_name */

/* "adds .dff when no extension, finds the file with game_content_find and loads the clump" */

undefined4 clump_load_by_name(char *_Str, undefined4 unused2, undefined4 unused3)

{
  char *pcVar1;
  int iVar2;
  undefined4 uVar3;
  
  rwstrcpy(&g_clump_load_69c878,_Str);
  pcVar1 = _strchr(_Str,0x2e);
  if (pcVar1 == (char *)0x0) {
    rwstrcat(&g_clump_load_69c878,PTR_g_game_5de408_00609a00);
  }
  iVar2 = game_content_find(&g_clump_load_69bb78,&g_clump_load_69c878);
  if (iVar2 != 0) {
    uVar3 = FUN_004c3bf0(((int)&g_clump_load_69bb78), unused2, unused3);
    return uVar3;
  }
  return 0;
}
