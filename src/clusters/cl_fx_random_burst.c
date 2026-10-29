// cluster-entries: fx_random_burst
/* Installed cluster fx_random_burst (tools/cluster_install.py from output/cluster_pilot_hb1/fx_random_burst.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00481fd0*/ extern double random_range(float lo, float hi);
/*00482010*/ extern int random_int_range();
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*005dd340*/ extern float k_f_0_5;
/*005e64a4*/ extern float k_f_0;
/*007ef7f4*/ extern float g_tick_seconds;
/* fx_random_burst */

/* "counts a delay down, then random spawn positions (random_range) and fx_spawn" */

static int fx_spawn_r(RwMatrix * p0, float * p1);
extern void fx_spawn(void);
static undefined4 line_of_sight_between_r(int p0, float * p1, float p2, float p3);
extern void line_of_sight_between(void);
void fx_random_burst_n(int in_esi, undefined4 in_ecx)

{
  int random_int;
  RwMatrix *reg_eax_00;
  float10 fVar1;
  
  if (!(*(float *)(in_esi + 0xa0) <= k_f_0)) {
    *(float *)(in_esi + 0xa0) = *(float *)(in_esi + 0xa0) - g_tick_seconds;
  }
  else {
    random_int = line_of_sight_between_r(in_esi,in_ecx,25.0,30.0);
    if (random_int != 0) {
      fVar1 = (float10)random_range(0.0,10.0);
      if (fVar1 < (float10)*(float *)(in_esi + 0x9c)) {
        random_int = random_int_range(0,3);
        reg_eax_00 = RwFrameGetLTM(*(RwFrame **)
                                    (*(int *)(in_esi + 0x1c + (random_int + 4) * 4) + 4));
        fx_spawn_r(reg_eax_00,in_esi + 0x50);
        if (*(float *)(in_esi + 0x60) < k_f_0_5) {
          *(float *)(in_esi + 0xa0) = 1.0;
          return;
        }
        *(float *)(in_esi + 0xa0) = 0.2;
        return;
      }
    }
  }
  return;
}

/* fx_spawn_r: C -> original fx_spawn (registers eax,edi) */
__declspec(naked) int fx_spawn_r(RwMatrix * p0, float * p1)
{
    __asm {
        push edi
        mov eax, dword ptr [esp + 8]
        mov edi, dword ptr [esp + 12]
        call fx_spawn
        pop edi
        ret
    }
}

/* line_of_sight_between_r: C -> original line_of_sight_between (registers eax,edi,s0,s1) */
__declspec(naked) undefined4 line_of_sight_between_r(int p0, float * p1, float p2, float p3)
{
    __asm {
        push edi
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        mov eax, dword ptr [esp + 16]
        mov edi, dword ptr [esp + 20]
        call line_of_sight_between
        add esp, 8
        pop edi
        ret
    }
}

/* entry thunk at the original address of fx_random_burst: register inputs (esi,ecx) -> natural C fx_random_burst_n; ret 0 */
__declspec(naked) void fx_random_burst(void)
{
    __asm {
        push ecx
        push esi
        call fx_random_burst_n
        add esp, 8
        ret
    }
}
