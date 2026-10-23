// cluster-entries: course_highway_cb0
/* Installed cluster course_highway_cb0 (tools/cluster_install.py from output/cluster_pilot_uc3/course_highway_cb0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
#define COURSE_HIGHWAYN (12)
typedef int CourseId;
/*0042b110*/ extern int current_course_id();
/*00453ca0*/ extern int course_smoke_particles_create_00453ca0();
/*00453cb0*/ extern int load_headlight();
/*00453ff0*/ extern int course_car_shadow_particles_create();
/*00459810*/ extern int course_object_offsets_init();
/*0045aa00*/ extern int __cdecl course_highway_physics_objects_create(undefined4 p0);
/*004c3f60*/ extern RpAtomic * clump_first_atomic(RpClump * p0);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*005dd340*/ extern float k_f_0_5;
/*006a2690*/ extern int g_course_highway_6a2690;
/*006a2694*/ extern int g_course_highway_6a2694;
/*006a2698*/ extern int g_course_highway_6a2698;
/*006a269c*/ extern int g_course_highway_6a269c;
/*006a26a0*/ extern int g_course_highway_6a26a0;
/*008c3f60*/ extern int g_course_highway_8c3f60;
/*008c3f64*/ extern int g_course_8c3f64;
/*008c3f68*/ extern int g_course_highway_8c3f68;
/*008c3fa0*/ extern int g_course_8c3fa0;
/*008c3fa4*/ extern int g_course_highway_8c3fa4;
/*008c3fa8*/ extern int g_course_highway_8c3fa8;
/*008c3fac*/ extern int g_course_highway_8c3fac;
/*008c3fb0*/ extern int g_course_highway_8c3fb0;
/*008c3fb4*/ extern int g_course_highway_8c3fb4;
/*008c3fb8*/ extern int g_course_highway_8c3fb8;
/*008c3fbc*/ extern int g_course_highway_8c3fbc;
/*008c3fc0*/ extern int g_course_8c3fc0;
/*008c3fc4*/ extern int g_course_highway_8c3fc4;
/*008c3fc8*/ extern int g_course_highway_8c3fc8;
/*008c3fcc*/ extern int g_course_highway_8c3fcc;
/*008c3fd0*/ extern int g_course_highway_8c3fd0;
/*008c3fd4*/ extern int g_course_highway_8c3fd4;
/*008c3fd8*/ extern int g_course_highway_8c3fd8;
/*008c3fdc*/ extern int g_course_highway_8c3fdc;
/*008c4300*/ extern int g_course_highway_8c4300;
/*008c4308*/ extern undefined4 g_course_highway_8c4308[0x1000];
/*008c4440*/ extern int g_course_highway_8c4440;
/*008c4444*/ extern int g_course_highway_8c4444;
/*008c4448*/ extern int g_course_highway_8c4448;
/*008c444c*/ extern int g_course_highway_8c444c;
/*008c4450*/ extern int g_course_highway_8c4450;
/*008c4454*/ extern int g_course_highway_8c4454;
/*008c4458*/ extern int g_course_highway_8c4458;
/*008c445c*/ extern int g_course_highway_8c445c;
/*008c4460*/ extern int g_course_highway_8c4460;
/*008c4470*/ extern int g_course_highway_8c4470;
/*008c4474*/ extern int g_course_highway_8c4474;
/*008c4478*/ extern int g_course_highway_8c4478;
/*008c4720*/ extern int g_course_highway_8c4720;
/*008c4724*/ extern int g_course_highway_8c4724;
/*008c4728*/ extern int g_course_highway_8c4728;
/*008c472c*/ extern int g_course_highway_8c472c;
/*008c4730*/ extern int g_course_highway_8c4730;
/*008c4734*/ extern int g_course_highway_8c4734;
/*008c4750*/ extern int g_course_highway_8c4750;
/*008c4754*/ extern int g_course_highway_8c4754;
/*008c4758*/ extern int g_course_highway_8c4758;
/*008c475c*/ extern int g_course_highway_8c475c;
/*008c4760*/ extern int g_course_highway_8c4760;
/*008c4764*/ extern int g_course_highway_8c4764;
/*008c4780*/ extern int g_course_highway_8c4780;
/*008c4784*/ extern int g_course_highway_8c4784;
/*008c4788*/ extern int g_course_highway_8c4788;
/*008c478c*/ extern int g_course_highway_8c478c;
/*008c4790*/ extern int g_course_highway_8c4790;
/*008c4794*/ extern int g_course_highway_8c4794;
/*008c4798*/ extern int g_course_highway_8c4798;
/*008c479c*/ extern int g_course_highway_8c479c;
/*008c47a0*/ extern int g_course_highway_8c47a0;








/* course_highway_cb0 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

static int course_highway_lod_atomic_clone_a_r(undefined4 * p0, int p1);
extern void course_highway_lod_atomic_clone_a(void);
static int course_highway_lod_atomic_clone_b_r(int p0, int p1);
extern void course_highway_lod_atomic_clone_b(void);
static int course_highway_lod_atomic_scale_by_index_r(RpAtomic * p0, int p1);
extern void course_highway_lod_atomic_scale_by_index(void);
static int course_highway_lod_objects_setup_r(int p0, int p1);
extern void course_highway_lod_objects_setup(void);
void course_highway_cb0(int param_1)

{
  int *piVar1;
  int *piVar2;
  undefined4 uVar3;
  int iVar4;
  CourseId CVar5;
  int *piVar6;
  int course_id;
  int iVar7;
  int iVar8;
  struct {
    int iStack_174;
    int iStack_170;
    int iStack_16c;
    int iStack_168;
    int iStack_164;
    int iStack_160;
    undefined4 uStack_15c;
    undefined4 uStack_158;
    undefined4 uStack_154;
    undefined4 uStack_150;
    undefined4 uStack_14c;
    undefined4 uStack_148;
    undefined4 uStack_144;
    undefined4 auStack_140 [80];
  } frame;
  
  iVar7 = 0;
  frame.auStack_140[0] = 9;
  frame.auStack_140[1] = 6;
  frame.auStack_140[2] = 10;
  frame.auStack_140[3] = 1;
  frame.auStack_140[4] = 7;
  frame.auStack_140[5] = 0xb;
  frame.auStack_140[6] = 2;
  frame.auStack_140[7] = 8;
  frame.auStack_140[8] = 0xc;
  frame.auStack_140[9] = 3;
  frame.auStack_140[10] = 0xb;
  frame.auStack_140[0xb] = 8;
  frame.auStack_140[0xc] = 0xc;
  frame.auStack_140[0xd] = 6;
  frame.auStack_140[0xe] = 9;
  frame.auStack_140[0xf] = 10;
  frame.auStack_140[0x10] = 0xb;
  frame.auStack_140[0x11] = 6;
  frame.auStack_140[0x12] = 1;
  frame.auStack_140[0x13] = 7;
  frame.auStack_140[0x14] = 1;
  frame.auStack_140[0x15] = 7;
  frame.auStack_140[0x16] = 0xb;
  frame.auStack_140[0x17] = 2;
  frame.auStack_140[0x18] = 8;
  frame.auStack_140[0x19] = 0xc;
  frame.auStack_140[0x1a] = 3;
  frame.auStack_140[0x1b] = 9;
  frame.auStack_140[0x1c] = 0;
  frame.auStack_140[0x1d] = 7;
  frame.auStack_140[0x1e] = 10;
  frame.auStack_140[0x1f] = 8;
  frame.auStack_140[0x20] = 6;
  frame.auStack_140[0x21] = 0xb;
  frame.auStack_140[0x22] = 0;
  frame.auStack_140[0x23] = 6;
  frame.auStack_140[0x24] = 10;
  frame.auStack_140[0x25] = 1;
  frame.auStack_140[0x26] = 7;
  frame.auStack_140[0x27] = 0xb;
  frame.auStack_140[0x28] = 0;
  frame.auStack_140[0x29] = 6;
  frame.auStack_140[0x2a] = 10;
  frame.auStack_140[0x2b] = 1;
  frame.auStack_140[0x2c] = 7;
  frame.auStack_140[0x2d] = 0xb;
  frame.auStack_140[0x2e] = 2;
  frame.auStack_140[0x2f] = 8;
  frame.auStack_140[0x30] = 0xc;
  frame.auStack_140[0x31] = 3;
  frame.auStack_140[0x32] = 2;
  frame.auStack_140[0x33] = 8;
  frame.auStack_140[0x34] = 0xc;
  frame.auStack_140[0x35] = 7;
  frame.auStack_140[0x36] = 9;
  frame.auStack_140[0x37] = 10;
  frame.auStack_140[0x38] = 0xb;
  frame.auStack_140[0x39] = 6;
  frame.auStack_140[0x3a] = 1;
  frame.auStack_140[0x3b] = 7;
  frame.auStack_140[0x3c] = 1;
  frame.auStack_140[0x3d] = 7;
  frame.auStack_140[0x3e] = 0xb;
  frame.auStack_140[0x3f] = 2;
  frame.auStack_140[0x40] = 8;
  frame.auStack_140[0x41] = 0xc;
  frame.auStack_140[0x42] = 3;
  frame.auStack_140[0x43] = 9;
  frame.auStack_140[0x44] = 0;
  frame.auStack_140[0x45] = 0xc;
  frame.auStack_140[0x46] = 10;
  frame.auStack_140[0x4c] = 10;
  frame.auStack_140[0x49] = 0xb;
  frame.auStack_140[0x4f] = 0xb;
  frame.auStack_140[0x47] = 8;
  frame.auStack_140[0x48] = 6;
  frame.auStack_140[0x4a] = 3;
  frame.auStack_140[0x4b] = 6;
  frame.auStack_140[0x4d] = 1;
  frame.auStack_140[0x4e] = 7;
  uVar3 = (undefined4)(clump_first_atomic((RpClump *)(*(undefined4 *)(param_1 + 0xc1e4))));
  g_course_highway_6a2690 =
       func_0x004a5d50(uVar3,*(undefined4 *)(param_1 + 0xc004),param_1 + 0xc00,
                       *(undefined4 *)(param_1 + 0xc9e0),*(undefined4 *)(param_1 + 0xc9e4));
  uVar3 = (undefined4)(clump_first_atomic((RpClump *)(*(undefined4 *)(param_1 + 0xc1f0))));
  g_course_highway_6a2694 =
       func_0x004a5d50(uVar3,*(undefined4 *)(param_1 + 0xc010),param_1 + 0x3000,
                       *(undefined4 *)(param_1 + 0xc9e0),*(undefined4 *)(param_1 + 0xc9e4));
  uVar3 = (undefined4)(clump_first_atomic((RpClump *)(*(undefined4 *)(param_1 + 0xc1fc))));
  g_course_highway_6a2698 =
       func_0x004a5d50(uVar3,*(undefined4 *)(param_1 + 0xc01c),param_1 + 0x5400,
                       *(undefined4 *)(param_1 + 0xc9e0),*(undefined4 *)(param_1 + 0xc9e4));
  uVar3 = (undefined4)(clump_first_atomic((RpClump *)(*(undefined4 *)(param_1 + 0xc200))));
  g_course_highway_6a269c =
       func_0x004a5d50(uVar3,*(undefined4 *)(param_1 + 0xc020),param_1 + 0x6000,
                       *(undefined4 *)(param_1 + 0xc9e0),*(undefined4 *)(param_1 + 0xc9e4));
  uVar3 = (undefined4)(clump_first_atomic((RpClump *)(*(undefined4 *)(param_1 + 0xc208))));
  g_course_highway_6a26a0 =
       func_0x004a5d50(uVar3,*(undefined4 *)(param_1 + 0xc028),param_1 + 0x7800,
                       *(undefined4 *)(param_1 + 0xc9e0),*(undefined4 *)(param_1 + 0xc9e4));
  g_course_8c3fc0 = 0x40800000;
  g_course_highway_8c3fc4 = 0x40400000;
  g_course_highway_8c3fc8 = 0x40000000;
  g_course_highway_8c3fcc = 0x3f800000;
  g_course_highway_8c3fd0 = 0xc0800000;
  g_course_highway_8c3fd4 = 0xc0400000;
  g_course_highway_8c3fd8 = 0xc0000000;
  g_course_highway_8c3fdc = 0xbf800000;
  g_course_8c3fa0 = 0x40900000;
  g_course_highway_8c3fa4 = 0x40a00000;
  g_course_highway_8c3fa8 = 0x40b00000;
  g_course_highway_8c3fac = 0x40c00000;
  g_course_highway_8c3fb0 = 0x40900000;
  g_course_highway_8c3fb4 = 0x40a00000;
  g_course_highway_8c3fb8 = 0x40b00000;
  g_course_highway_8c3fbc = 0x40c00000;
  g_course_highway_8c4440 = 0x3f0d14e4;
  g_course_highway_8c4444 = 0x3f6f4880;
  g_course_highway_8c4448 = 0x40277b4a;
  g_course_highway_8c444c = 0x3f058794;
  g_course_highway_8c4450 = 0x3f5b7e91;
  g_course_highway_8c4454 = 0x3f9f1de7;
  g_course_highway_8c4458 = 0x3f0d42c4;
  g_course_highway_8c445c = 0x3f86f34d;
  g_course_highway_8c4460 = 0x4044fc50;
  course_id = 0;
  do {
    iVar4 = course_id + 0xc;
    *(float *)((int)&g_course_highway_8c3f60 + course_id) =
         *(float *)((char *)((undefined *)&g_course_highway_8c4440) + course_id) * k_f_0_5;
    *(float *)((int)&g_course_8c3f64 + course_id) =
         *(float *)((char *)((undefined *)&g_course_highway_8c4444) + course_id) * k_f_0_5;
    *(float *)((int)&g_course_highway_8c3f68 + course_id) =
         *(float *)((char *)((undefined *)&g_course_highway_8c4448) + course_id) * k_f_0_5;
    course_id = iVar4;
  } while (iVar4 < 0x24);
  mem_zero(&g_course_highway_8c4720,0x90);
  mem_zero(&g_course_highway_8c4470,0xc);
  g_course_highway_8c4470 = 2;
  g_course_highway_8c4474 = 2;
  g_course_highway_8c4720 = 0;
  g_course_highway_8c4724 = 0xbeb2fec5;
  g_course_highway_8c4728 = 0x3f7e4f76;
  g_course_highway_8c472c = 0;
  g_course_highway_8c4730 = 0xbeb2fec5;
  g_course_highway_8c4734 = 0xbf89a9fc;
  g_course_highway_8c4750 = 0;
  g_course_highway_8c4754 = 0xbe9ed289;
  g_course_highway_8c4758 = 0x3e9dcc64;
  g_course_highway_8c475c = 0;
  g_course_highway_8c4760 = 0xbe9ed289;
  g_course_highway_8c4764 = 0xbeeeb1c4;
  g_course_highway_8c4780 = 0;
  g_course_highway_8c4784 = 0xbed0e560;
  g_course_highway_8c4788 = 0x3f9d5cfb;
  g_course_highway_8c478c = 0;
  g_course_highway_8c4790 = 0xbed0e560;
  g_course_highway_8c4794 = 0x3ee8f5c3;
  g_course_highway_8c4798 = 0;
  g_course_highway_8c479c = 0xbed0e560;
  g_course_highway_8c47a0 = 0xbfadf3b6;
  g_course_highway_8c4478 = 3;
  frame.iStack_160 = 0x8bf0a0;
  frame.iStack_170 = 0;
  frame.iStack_174 = 0;
  do {
    frame.iStack_16c = 0;
    do {
      frame.iStack_168 = 0;
      do {
        iVar8 = 0;
        iVar4 = 0;
        course_id = 0;
        mem_zero(&frame.uStack_15c,0x10);
        switch(*(undefined4 *)((int)frame.auStack_140 + frame.iStack_174)) {
        case 0:
          iVar4 = 1;
          iVar8 = 0;
          course_id = 2;
          iVar7 = 1;
          break;
        case 1:
          iVar8 = 3;
          iVar4 = 4;
          course_id = 5;
          iVar7 = 1;
          break;
        case 2:
          iVar8 = 6;
          iVar4 = 7;
          course_id = 8;
          iVar7 = 1;
          break;
        case 3:
          iVar8 = 9;
          iVar4 = 10;
          course_id = 0xb;
          iVar7 = 1;
          break;
        case 4:
          iVar8 = 0xc;
          iVar4 = 0xd;
          course_id = 0xe;
          iVar7 = 1;
          break;
        case 5:
          iVar8 = 0xf;
          iVar4 = 0x10;
          course_id = 0x11;
          iVar7 = 1;
          break;
        case 6:
          iVar8 = 0x12;
          iVar4 = 0x13;
          course_id = 0x14;
          iVar7 = 0;
          break;
        case 7:
          iVar8 = 0x15;
          iVar4 = 0x16;
          course_id = 0x17;
          iVar7 = 0;
          break;
        case 8:
          iVar8 = 0x18;
          iVar4 = 0x19;
          course_id = 0x1a;
          iVar7 = 0;
          break;
        case 9:
          iVar8 = 0x1b;
          iVar4 = 0x1c;
          course_id = 0x1d;
          iVar7 = 0;
          break;
        case 10:
          iVar8 = 0x1e;
          iVar4 = 0x1f;
          course_id = 0x20;
          goto LAB_0045b28a;
        case 0xb:
          iVar8 = 0x21;
          iVar4 = 0x22;
          course_id = 0x23;
          goto LAB_0045b28a;
        case 0xc:
          iVar8 = 0x24;
          iVar4 = 0x25;
          course_id = 0x26;
LAB_0045b28a:
          iVar7 = 2;
        }
        if (iVar7 == 0) {
          frame.iStack_164 = 0x2e;
        }
        else if (iVar7 == 1) {
          frame.iStack_164 = 0x2f;
        }
        else if (iVar7 == 2) {
          frame.iStack_164 = 0x30;
        }
        frame.uStack_14c = (undefined4)(clump_first_atomic((RpClump *)(*(undefined4 *)(param_1 + 0xc110 + iVar8 * 4))));
        frame.uStack_148 = (undefined4)(clump_first_atomic((RpClump *)(*(undefined4 *)(param_1 + 0xc110 + iVar4 * 4))));
        frame.uStack_144 = (undefined4)(clump_first_atomic((RpClump *)(*(undefined4 *)(param_1 + 0xc110 + course_id * 4))));
        uVar3 = *(undefined4 *)(param_1 + 0xc1c0);
        frame.uStack_15c = (undefined4)(clump_first_atomic((RpClump *)uVar3));
        frame.uStack_158 = (undefined4)(clump_first_atomic((RpClump *)uVar3));
        frame.uStack_154 = (undefined4)(clump_first_atomic((RpClump *)uVar3));
        frame.uStack_150 = (undefined4)(clump_first_atomic((RpClump *)uVar3));
        uVar3 = (undefined4)(clump_first_atomic((RpClump *)(*(undefined4 *)(param_1 + 0xc110 + frame.iStack_164 * 4))));
        mem_zero((void *)frame.iStack_160,0xfc);
        *(int *)(frame.iStack_160 + 0xb8) = frame.iStack_168;
        *(int *)(frame.iStack_160 + 0xac) = iVar7;
        *(int *)(frame.iStack_160 + 0xb0) = frame.iStack_170;
        *(int *)(frame.iStack_160 + 0xb4) = frame.iStack_16c;
        course_highway_lod_atomic_clone_a_r(&frame.uStack_14c,frame.iStack_160);
        course_highway_lod_objects_setup_r(frame.iStack_160,(int)&frame.uStack_15c);
        course_highway_lod_atomic_clone_b_r(uVar3, frame.iStack_160);
        CVar5 = current_course_id();
        if (CVar5 == COURSE_HIGHWAYN) {
          course_highway_lod_atomic_scale_by_index_r((RpAtomic *)uVar3,frame.iStack_160);
        }
        *(int *)((int)&g_course_highway_8c4300 + frame.iStack_174) = frame.iStack_160;
        frame.iStack_160 = frame.iStack_160 + 0xfc;
        frame.iStack_168 = frame.iStack_168 + 1;
        frame.iStack_174 = frame.iStack_174 + 4;
      } while (frame.iStack_168 < 10);
      frame.iStack_16c = frame.iStack_16c + 1;
    } while (frame.iStack_16c < 4);
    frame.iStack_170 = frame.iStack_170 + 1;
    if (0x13f < frame.iStack_174) {
      piVar6 = g_course_highway_8c4308;
      do {
        course_id = 4;
        do {
          *(int *)(piVar6[-2] + 0xbc) = piVar6[-1];
          *(int *)(piVar6[-1] + 0xbc) = *piVar6;
          *(int *)(*piVar6 + 0xbc) = piVar6[1];
          *(int *)(piVar6[1] + 0xbc) = piVar6[2];
          *(int *)(piVar6[2] + 0xbc) = piVar6[3];
          *(int *)(piVar6[3] + 0xbc) = piVar6[4];
          *(int *)(piVar6[4] + 0xbc) = piVar6[5];
          *(int *)(piVar6[5] + 0xbc) = piVar6[6];
          *(int *)(piVar6[6] + 0xbc) = piVar6[7];
          piVar1 = piVar6 + 7;
          piVar2 = piVar6 + -2;
          piVar6 = piVar6 + 10;
          course_id = course_id + -1;
          *(int *)(*piVar1 + 0xbc) = *piVar2;
        } while (course_id != 0);
      } while ((int)piVar6 < 0x8c4448);
      course_object_offsets_init();
      course_highway_physics_objects_create(param_1);
      course_car_shadow_particles_create();
      course_smoke_particles_create_00453ca0();
      CVar5 = current_course_id();
      if (CVar5 != COURSE_HIGHWAYN) {
        return;
      }
      load_headlight();
      return;
    }
  } while( true );
}

/* course_highway_lod_atomic_clone_a_r: C -> original course_highway_lod_atomic_clone_a (registers eax,ebx) */
__declspec(naked) int course_highway_lod_atomic_clone_a_r(undefined4 * p0, int p1)
{
    __asm {
        push ebx
        mov eax, dword ptr [esp + 8]
        mov ebx, dword ptr [esp + 12]
        call course_highway_lod_atomic_clone_a
        pop ebx
        ret
    }
}

/* course_highway_lod_atomic_clone_b_r: C -> original course_highway_lod_atomic_clone_b (registers eax,esi) */
__declspec(naked) int course_highway_lod_atomic_clone_b_r(int p0, int p1)
{
    __asm {
        push esi
        mov eax, dword ptr [esp + 8]
        mov esi, dword ptr [esp + 12]
        call course_highway_lod_atomic_clone_b
        pop esi
        ret
    }
}

/* course_highway_lod_atomic_scale_by_index_r: C -> original course_highway_lod_atomic_scale_by_index (registers eax,ecx) */
__declspec(naked) int course_highway_lod_atomic_scale_by_index_r(RpAtomic * p0, int p1)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        mov ecx, dword ptr [esp + 8]
        call course_highway_lod_atomic_scale_by_index
        ret
    }
}

/* course_highway_lod_objects_setup_r: C -> original course_highway_lod_objects_setup (registers ebx,s0) */
__declspec(naked) int course_highway_lod_objects_setup_r(int p0, int p1)
{
    __asm {
        push ebx
        push dword ptr [esp + 12]
        mov ebx, dword ptr [esp + 12]
        call course_highway_lod_objects_setup
        add esp, 4
        pop ebx
        ret
    }
}
