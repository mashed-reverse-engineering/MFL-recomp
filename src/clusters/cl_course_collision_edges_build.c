// cluster-entries: course_collision_edges_build
/* Installed cluster course_collision_edges_build (tools/cluster_install.py from output/cluster_pilot_zy/course_collision_edges_build.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
extern void *memset(void *, int, unsigned int);
/* course_collision_edges_build */

/* WARNING: Unknown calling convention */
/* "allocates and fills the 0x48 byte per triangle edge records (3 per triangle)" */

static int collision_edge_shares_vertex_pair_r(int * p0, int * p1);
extern void collision_edge_shares_vertex_pair(void);
void course_collision_edges_build_n(int in_eax, int *param_2, ushort *param_3)

{
  int *mem;
  void *pvVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  int iVar6;
  int *piVar7;
  struct {
    int local_c;
    int iStack_8;
    int iStack_4;
  } frame;
  
  iVar6 = in_eax * 3;
  frame.local_c = 0;
  mem = RwMalloc(in_eax * 0x48,0);
  memset(mem,0,in_eax * 0x48);
  iVar3 = 0;
  piVar2 = mem;
  if (0 < in_eax) {
    do {
      piVar2[4] = (uint)*param_3;
      piVar2[5] = (uint)param_3[1];
      *piVar2 = (int)param_3;
      piVar2[1] = 0;
      piVar2[2] = iVar3;
      piVar2[3] = -1;
      piVar2[10] = (uint)param_3[1];
      piVar2[0xb] = (uint)param_3[2];
      piVar2[6] = (int)param_3;
      piVar2[8] = iVar3;
      piVar2[7] = 0;
      piVar2[9] = -1;
      piVar2[0x10] = (uint)param_3[2];
      piVar2[0x11] = (uint)*param_3;
      piVar2[0xc] = (int)param_3;
      piVar2[0xe] = iVar3;
      piVar2[0xd] = 0;
      piVar2[0xf] = -1;
      iVar3 = iVar3 + 1;
      param_3 = param_3 + 4;
      piVar2 = piVar2 + 0x12;
    } while (iVar3 < in_eax);
  }
  frame.iStack_4 = iVar6 + -1;
  if (0 < frame.iStack_4) {
    frame.iStack_8 = 1;
    piVar2 = mem;
    do {
      if ((*piVar2 != 0) && (frame.iStack_8 < iVar6)) {
        iVar3 = iVar6 - frame.iStack_8;
        piVar5 = piVar2;
        do {
          piVar5 = piVar5 + 6;
          iVar4 = *piVar5;
          if (iVar4 != 0) {
            if (collision_edge_shares_vertex_pair_r(piVar2,piVar5) != 0) {
              piVar2[1] = iVar4;
              piVar2[3] = piVar5[2];
              *piVar5 = 0;
              piVar5[2] = -1;
            }
          }
          iVar3 = iVar3 + -1;
        } while (iVar3 != 0);
      }
      piVar2 = piVar2 + 6;
      frame.iStack_8 = frame.iStack_8 + 1;
      frame.iStack_4 = frame.iStack_4 + -1;
    } while (frame.iStack_4 != 0);
  }
  piVar2 = mem;
  iVar3 = iVar6;
  if (0 < iVar6) {
    do {
      if ((*piVar2 != 0) && (piVar2[1] != 0)) {
        frame.local_c = frame.local_c + 1;
      }
      iVar3 = iVar3 + -1;
      piVar2 = piVar2 + 6;
    } while (iVar3 != 0);
  }
  pvVar1 = RwMalloc(frame.local_c * 0x18,0);
  *param_2 = (int)pvVar1;
  param_2[1] = frame.local_c;
  if (0 < iVar6) {
    iVar3 = 0;
    piVar2 = mem;
    do {
      if ((*piVar2 != 0) && (piVar2[1] != 0)) {
        piVar5 = piVar2;
        piVar7 = (int *)(*param_2 + iVar3);
        for (iVar4 = 6; iVar4 != 0; iVar4 = iVar4 + -1) {
          *piVar7 = *piVar5;
          piVar5 = piVar5 + 1;
          piVar7 = piVar7 + 1;
        }
        iVar3 = iVar3 + 0x18;
      }
      piVar2 = piVar2 + 6;
      iVar6 = iVar6 + -1;
    } while (iVar6 != 0);
  }
  RwFree(mem);
  return;
}

/* collision_edge_shares_vertex_pair_r: C -> original collision_edge_shares_vertex_pair (registers esi,edx) */
__declspec(naked) int collision_edge_shares_vertex_pair_r(int * p0, int * p1)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        mov edx, dword ptr [esp + 12]
        call collision_edge_shares_vertex_pair
        pop esi
        ret
    }
}

/* entry thunk at the original address of course_collision_edges_build: register inputs (eax,s0,s1) -> natural C course_collision_edges_build_n; ret 0 */
__declspec(naked) void course_collision_edges_build(void)
{
    __asm {
        push dword ptr [esp + 8]
        push dword ptr [esp + 8]
        push eax
        call course_collision_edges_build_n
        add esp, 12
        ret
    }
}
