#include "mfl_tier2.h"
/*0042f780*/ extern int __cdecl clump_load_by_name(char *p0, undefined4 p1, undefined4 p2);
/*004d5e10*/ extern struct RwTexDictionary * RwTexDictionarySetCurrent(struct RwTexDictionary *p0);
/*00658fd8*/ extern undefined g_team_658fd8[0x1000];
static int team_panel_load_parts_r(int reg_eax, void *reg_ebx);
extern void team_panel_load_parts(void);
extern char g_team_6592b0[];
#define TEAM_PANEL_RECORD_SIZE (0x16c) /* stride between team panel records (inferred) */
#define TEAM_PANEL_TEAM_INDEX_OFFSET (0x164) /* record offset where the team index is stored (inferred) */
__declspec(naked) int team_panel_load_parts_r(int reg_eax, void *reg_ebx)
{
    __asm {
        push ebx
        mov eax, dword ptr [esp + 8]
        mov ebx, dword ptr [esp + 12]
        call team_panel_load_parts
        pop ebx
        ret
    }
}

/* load_team_panels */

/* team0panel team1panel */

/* Loads the clumps "team0panel" and "team1panel" and fills in the team panel records
   starting at g_team_658fd8 (one record per team, TEAM_PANEL_RECORD_SIZE bytes each).
   For each team it loads the clump by name, passes the clump and its record to
   team_panel_load_parts, and stores the team index in the record.
   Takes no parameters and returns nothing. */
void load_team_panels(void)

{
  undefined4 clump;
  undefined *team_panel;
  int team_index;
  char *panel_names [2];
  
  panel_names[0] = "team0panel";
  panel_names[1] = "team1panel";
  /* Clear the current RW texture dictionary before loading (inferred). */
  RwTexDictionarySetCurrent((RwTexDictionary *)0x0);
  team_index = 0;
  team_panel = g_team_658fd8;
  /* One pass per team: load its clump, fill its record, then step to the next record.
     The loop ends at g_team_6592b0, the end of the two team records (inferred). */
  do {
    clump = clump_load_by_name(panel_names[team_index], 0x0, 0x0);
    team_panel_load_parts_r(clump,(void *)team_panel);
    *(int *)(team_panel + TEAM_PANEL_TEAM_INDEX_OFFSET) = team_index;
    team_panel = team_panel + TEAM_PANEL_RECORD_SIZE;
    team_index = team_index + 1;
  } while ((char *)team_panel < ((char *)&g_team_6592b0));
  return;
}


