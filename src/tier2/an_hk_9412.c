#include "mfl_tier2.h"
/*0042f780*/ extern int __cdecl clump_load_by_name(char *p0, undefined4 p1, undefined4 p2);
/*004d5e10*/ extern struct RwTexDictionary * RwTexDictionarySetCurrent(struct RwTexDictionary *p0);
/*006597b8*/ extern undefined g_endpoint_6597b8[0x1000];
static int team_endpoint_panel_load_parts_r(int reg_eax, void *reg_ebx);
extern void team_endpoint_panel_load_parts(void);
extern char g_endpoint_659a78[];
__declspec(naked) int team_endpoint_panel_load_parts_r(int reg_eax, void *reg_ebx)
{
    __asm {
        push ebx
        mov eax, dword ptr [esp + 8]
        mov ebx, dword ptr [esp + 12]
        call team_endpoint_panel_load_parts
        pop ebx
        ret
    }
}

/* load_team_endpoint_panel */

/* teameppanel.dff */

void load_team_endpoint_panel(void)

{
  undefined4 reg_eax;
  undefined *reg_ebx;
  int iVar1;
  
  RwTexDictionarySetCurrent((RwTexDictionary *)0x0);
  iVar1 = 0;
  reg_ebx = g_endpoint_6597b8;
  do {
    reg_eax = clump_load_by_name("teameppanel.dff", 0x0, 0x0);
    team_endpoint_panel_load_parts_r(reg_eax,(void *)reg_ebx);
    *(int *)(reg_ebx + 0x158) = iVar1;
    reg_ebx = reg_ebx + 0x160;
    iVar1 = iVar1 + 1;
  } while ((char *)reg_ebx < ((char *)&g_endpoint_659a78));
  return;
}


