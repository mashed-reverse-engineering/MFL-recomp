#include "mfl_tier2.h"
/*005b7ef0*/ extern int __cdecl FUN_005b7ef0(void *a1);
/*005be7d0*/ extern int __cdecl FUN_005be7d0(int a1, void *a2);
/*005be810*/ extern int __cdecl FUN_005be810(int a1, void *a2);
/*005beec0*/ extern int __cdecl FUN_005beec0(void *a1);
/*005bef40*/ extern int __cdecl FUN_005bef40(int a1, int a2, int a3, int a4, void *a5, int a6);
/*005dd5a2*/ extern undefined g_game_5dd5a2;
/*005f765c*/ extern undefined g_data_5f765c;
/*007dab98*/ extern undefined g_data_7dab98;
/*007dabbc*/ extern char *g_data_7dabbc;
/*007dabc0*/ extern char *g_data_7dabc0;
/*007dabc4*/ extern undefined g_data_7dabc4;
/*007dabf0*/ extern undefined4 g_data_7dabf0;
/*007dabf8*/ extern undefined4 g_data_7dabf8;
/*007dabfc*/ extern undefined4 g_data_7dabfc;
/* gap_5b8120 */

/* no context */

undefined4 gap_5b8120(void)

{
  int iVar1;
  
  g_data_7dabbc = (char *)(&g_data_7dabbc);
  g_data_7dabc0 = (char *)(&g_data_7dabbc);
  g_data_7dabf0 = 1;
  iVar1 = FUN_005bef40(0x24,8,4,0,&g_data_7dab98,0x40808);
  if (iVar1 != 0) {
    iVar1 = FUN_005b7ef0(&g_data_7dabc4);
    g_data_7dabf8 = iVar1;
    g_data_7dabfc = iVar1;
    if (iVar1 != 0) {
      FUN_005be810(iVar1,&g_data_5f765c);
      FUN_005be7d0(iVar1,&g_game_5dd5a2);
      return 1;
    }
    FUN_005beec0(&g_data_7dab98);
  }
  g_data_7dabf8 = 0;
  g_data_7dabfc = 0;
  g_data_7dabf0 = 0;
  return 0;
}


