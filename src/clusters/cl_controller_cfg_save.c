// cluster-entries: controller_cfg_save
/* Installed cluster controller_cfg_save (tools/cluster_install.py from output/cluster_pilot11/controller_cfg_save.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004a5550*/ extern void log_printf(const char *fmt, ...);
/*004b4531*/ extern FILE * FUN_004b4531(char *name, void *mode);
/*004b4358*/ extern int __cdecl _fclose(FILE *p0);
/*004b4b12*/ extern size_t __cdecl _fwrite(void *p0, size_t p1, size_t p2, FILE *p3);
/*005e14cc*/ extern undefined g_controller_5e14cc;
/*007e7740*/ extern struct ControllerCfg g_controller_cfg[4];
/* controller_cfg_save */

/* WARNING: Unknown calling convention */
/* fwrite 0x200 bytes from g_controller_cfg[n] */

static char * controller_cfg_path_r(int p0);
extern void controller_cfg_path(void);
int controller_cfg_save_n(int in_eax)

{
  char *cfg_path;
  FILE *file;
  
  cfg_path = controller_cfg_path_r(in_eax);
  log_printf("Attempting to save controller config %s\n",cfg_path);
  file = FUN_004b4531(cfg_path,&g_controller_5e14cc);
  if (file != (FILE *)0x0) {
    _fwrite(g_controller_cfg + in_eax,1,0x200,file);
    _fclose(file);
    log_printf("\tSaved controller config %s\n",cfg_path);
    return 1;
  }
  log_printf("\tUnable to save controller config %s\n",cfg_path);
  return 0;
}

/* controller_cfg_path_r: C -> original controller_cfg_path (registers eax) */
__declspec(naked) char * controller_cfg_path_r(int p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call controller_cfg_path
        ret
    }
}

/* entry thunk at the original address of controller_cfg_save: register inputs (eax) -> natural C controller_cfg_save_n; ret 0 */
__declspec(naked) void controller_cfg_save(void)
{
    __asm {
        push eax
        call controller_cfg_save_n
        add esp, 4
        ret
    }
}
