#include "mfl_tier2.h"
/*00421a70*/ extern int course_table_find_entry();
/*0042f620*/ extern int game_content_exists_prefixed();
/*0042f780*/ extern int __cdecl clump_load_by_name(char *p0, undefined4 p1, undefined4 p2);
/*0042fa80*/ extern int course_content_path_build();
/*004353c0*/ extern int menu_powerup_set_for_event();
/*00464aa0*/ extern int powerups_clear();
/*0048b5f0*/ extern undefined __cdecl lua_run_course_script(undefined4 p0);
/*004a43d0*/ extern undefined __cdecl game_open_piz(char *p0);
/*004a4440*/ extern int game_close_piz();
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*007d2128*/ extern char *RwEngineInstance;
static int course_place_powerups_from_clump_r(void *reg_ecx);
extern void course_place_powerups_from_clump(void);
__declspec(naked) int course_place_powerups_from_clump_r(void *reg_ecx)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        call course_place_powerups_from_clump
        ret
    }
}

/* course_load_powerups_gold */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "'powerups_gold.lua' course powerup layout" */

void course_load_powerups_gold(undefined4 param_1)

{
  vecStrcpyFunc *pRVar1;
  RwChar *course_entry;
  RpClump *clump;
  int iVar2;
  undefined4 *puVar3;
  char *pcVar4;
  char *apcStack_120 [4];
  char local_110[128] = "";
  char local_90[128] = "";
  
  course_entry = (RwChar *)course_table_find_entry(param_1);
  apcStack_120[1] = "powerups_gold.lua";
  apcStack_120[2] = "powerups_gold.lua";
  apcStack_120[3] = "powerups_gold.lua";
  rwstrcpy(local_110,"toastart/tracks/");
  rwstrcat(local_110,course_entry);
  rwstrcat(local_110,".piz");
  pRVar1 = &rwstrcpy;
  course_entry = (RwChar *)course_content_path_build(local_110);
  (*pRVar1)(local_90,course_entry);
  powerups_clear();
  iVar2 = menu_powerup_set_for_event();
  if (iVar2 == 0) {
    return;
  }
  pcVar4 = apcStack_120[iVar2];
  game_open_piz(local_90);
  clump = (RpClump *)clump_load_by_name("powerups_gold.dff", 0x0, 0x0);
  if (clump != (RpClump *)0x0) {
    course_place_powerups_from_clump_r((void *)clump);
    RpClumpDestroy(clump);
  }
  else {
    iVar2 = game_content_exists_prefixed(0,pcVar4,1);
    if (iVar2 == 0) {
      iVar2 = game_content_exists_prefixed(0,"powerups.lua",1);
      if (iVar2 == 0) goto LAB_0042aac5;
      pcVar4 = "powerups.lua";
    }
    lua_run_course_script((undefined4)pcVar4);
  }
LAB_0042aac5:
  game_close_piz();
  return;
}


