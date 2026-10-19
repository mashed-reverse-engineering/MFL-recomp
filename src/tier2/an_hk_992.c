#include "mfl_tier2.h"
/*0068b8f8*/ extern char g_race_phase_arg[];
/*0042d9a0*/ extern int store_u32();
/*0042d9b0*/ extern int rw_stream_chunk_copy();
/*00495e10*/ extern int world_sector_list_clear();
/*004c4180*/ extern int FUN_004c4180(undefined4 p0, undefined4 p1);
/*004c6210*/ extern int __cdecl FUN_004c6210(int a1, int a2);
/*005543d0*/ extern int rw_first_field(int * p0);
/*00702a98*/ extern int (__cdecl *g_physics_702a98)();
/*00702ea0*/ extern int g_particle_system_702ea0;
/*00702ea8*/ extern undefined4 g_particle_system_702ea8;
/*007032a8*/ extern undefined4 g_physics_7032a8;
/*007036bc*/ extern undefined4 g_particle_system_7036bc;
/*007036c0*/ extern undefined4 g_physics_7036c0;
/*007036c4*/ extern undefined4 g_particles_7036c4;
/*007036d0*/ extern undefined4 g_particle_system_7036d0;
/*007036d4*/ extern undefined4 g_particle_system_7036d4;
/*007036d8*/ extern int g_particle_system_7036d8;
/*007036dc*/ extern int g_particle_system_7036dc;
static void world_render_with_sector_callback_r(int reg_eax, int stk0);
extern void world_render_with_sector_callback(void);
static void world_render_count_sectors_r(int reg_esi, int stk0);
extern void world_render_count_sectors(void);
/* adapters for the register convention callees: eax / esi = particle system, stack argument = param_2 */
__declspec(naked) void world_render_with_sector_callback_r(int reg_eax, int stk0)
{
    __asm {
        push dword ptr [esp + 8]
        mov eax, dword ptr [esp + 8]
        call world_render_with_sector_callback
        add esp, 4
        ret
    }
}
__declspec(naked) void world_render_count_sectors_r(int reg_esi, int stk0)
{
    __asm {
        push esi
        push dword ptr [esp + 12]
        mov esi, dword ptr [esp + 12]
        call world_render_count_sectors
        add esp, 4
        pop esi
        ret
    }
}

/* particle_system_set_current */

/* "marks a system current (+0xC = 1) and stores it as g_particles_7036bc" */

void particle_system_set_current(int param_1,undefined4 param_2)

{
  undefined4 uVar1;
  int iVar2;
  bool bVar3;
  undefined4 *puVar4;
  
  bVar3 = param_1 != g_particle_system_7036bc;
  *(undefined4 *)(param_1 + 0xc) = 1;
  if (bVar3) {
    g_particle_system_7036bc = param_1;
    g_particle_system_702ea0 = FUN_004c4180(param_1,(undefined4)&g_particle_system_702ea8);
  }
  world_sector_list_clear();
  g_physics_7032a8 = 0;
  uVar1 = rw_first_field((int)g_race_phase_arg);
  switch(uVar1) {
  case 3:
    puVar4 = &g_particle_system_7036d4;
    goto LAB_004961bf;
  default:
    goto switchD_0049613d_caseD_4;
  case 0x14:
    g_particle_system_7036d0 = 0;
    break;
  case 0x15:
    g_particle_system_7036d0 = 1;
    break;
  case 0x16:
    g_particle_system_7036d0 = 2;
    break;
  case 0x17:
    if (g_particle_system_7036d8 == 0) {
      g_particle_system_7036d8 = g_particle_system_702ea0 + -1;
    }
    else {
      g_particle_system_7036d8 = g_particle_system_7036d8 + -1;
    }
    break;
  case 0x18:
    g_particle_system_7036d8 = (g_particle_system_7036d8 + 1) % g_particle_system_702ea0;
    break;
  case 0x19:
    puVar4 = &g_physics_7036c0;
    goto LAB_004961bf;
  case 0x1a:
    puVar4 = &g_particles_7036c4;
LAB_004961bf:
    rw_stream_chunk_copy((int)g_race_phase_arg,puVar4);
    store_u32((int)g_race_phase_arg,0);
    goto switchD_0049613d_caseD_4;
  case 0x1b:
    if (g_particle_system_7036dc == 0) {
      g_particle_system_7036dc = g_particle_system_702ea0 + -1;
    }
    else {
      g_particle_system_7036dc = g_particle_system_7036dc + -1;
    }
    break;
  case 0x1c:
    g_particle_system_7036dc = (g_particle_system_7036dc + 1) % g_particle_system_702ea0;
    break;
  case 0x1d:
    g_particle_system_7036d8 = g_particle_system_7036dc;
    break;
  case 0x1e:
    g_particle_system_7036dc = g_particle_system_7036d8;
  }
  store_u32((int)g_race_phase_arg,0);
switchD_0049613d_caseD_4:
  if (g_particle_system_7036d4 == 0) {
    world_render_with_sector_callback_r(param_1,param_2);
  }
  else {
    world_render_count_sectors_r(param_1,param_2);
  }
  if (g_particles_7036c4 != 0) {
    (*g_physics_702a98)((&g_particle_system_702ea8)[g_particle_system_7036dc]);
  }
  if (g_particle_system_7036d0 == 1) {
    iVar2 = 0;
    if (0 < (int)g_particle_system_702ea0) {
      do {
        FUN_004c6210((&g_particle_system_702ea8)[iVar2] + 0x60,0);
        iVar2 = iVar2 + 1;
      } while (iVar2 < (int)g_particle_system_702ea0);
    }
  }
  else if (g_particle_system_7036d0 == 2) {
    FUN_004c6210(((&g_particle_system_702ea8)[g_particle_system_7036d8] + 0x60), 0x0);
    return;
  }
  return;
}


