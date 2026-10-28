#include "mfl_tier2.h"
/*00421a70*/ extern char *course_table_find_entry();
/*0042f620*/ extern int game_content_exists_prefixed();
/*0042f780*/ extern char *__cdecl clump_load_by_name(char *p0, undefined4 p1, undefined4 p2);
/*0042fa80*/ extern char *course_content_path_build();
/*004353c0*/ extern int menu_powerup_set_for_event();
/*00464aa0*/ extern int powerups_clear();
/*0048b5f0*/ extern undefined __cdecl lua_run_course_script(void *p0);
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
/* course powerup layout loader (was: 'powerups_gold.lua' course powerup layout) */

/* Loads the powerup layout of course_id: opens the course .piz, places powerups from powerups_gold.dff if present, else runs the powerups_gold.lua / powerups.lua script (inferred). */
void course_load_powerups_gold(undefined4 course_id)

{
  vecStrcpyFunc *strcpy_fn;
  RwChar *course_entry;
  RpClump *clump;
  int powerup_set;
  undefined4 *unused_ptr;
  char *script_name;
  char *script_names [4];
  char piz_rel_path[128] = "";
  char piz_path[128] = "";
  
  course_entry = (RwChar *)course_table_find_entry(course_id);
  script_names[1] = "powerups_gold.lua";
  script_names[2] = "powerups_gold.lua";
  script_names[3] = "powerups_gold.lua";
  rwstrcpy(piz_rel_path,"toastart/tracks/");
  rwstrcat(piz_rel_path,course_entry);
  rwstrcat(piz_rel_path,".piz");
  strcpy_fn = &rwstrcpy;
  course_entry = (RwChar *)course_content_path_build(piz_rel_path);
  (*strcpy_fn)(piz_path,course_entry);
  powerups_clear();
  powerup_set = menu_powerup_set_for_event();
  if (powerup_set == 0) {
    return;
  }
  script_name = script_names[powerup_set];
  game_open_piz(piz_path);
  clump = (RpClump *)clump_load_by_name("powerups_gold.dff", 0x0, 0x0);
  if (clump != (RpClump *)0x0) {
    course_place_powerups_from_clump_r((void *)clump);
    RpClumpDestroy(clump);
  }
  else {
    powerup_set = game_content_exists_prefixed(0,script_name,1);
    if (powerup_set == 0) {
      powerup_set = game_content_exists_prefixed(0,"powerups.lua",1);
      if (powerup_set == 0) goto LAB_0042aac5;
      script_name = "powerups.lua";
    }
    lua_run_course_script(script_name);
  }
LAB_0042aac5:
  game_close_piz();
  return;
}


