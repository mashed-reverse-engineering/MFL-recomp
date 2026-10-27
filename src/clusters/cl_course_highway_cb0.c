// cluster-entries: course_highway_cb0
/* Installed cluster course_highway_cb0 (tools/cluster_install.py from output/cluster_pilot_uc3/course_highway_cb0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
#define COURSE_HIGHWAYN (12)
typedef int CourseId;
#define MTS_LIST_BYTES 0xc00 /* one matrix list of CourseWorld.mts_matrices (0x30 RwMatrix) */
/*0042b110*/ extern int current_course_id();
/*00453ca0*/ extern int course_smoke_particles_create_00453ca0();
/*00453cb0*/ extern int load_headlight();
/*00453ff0*/ extern int course_car_shadow_particles_create();
/*00459810*/ extern int course_object_offsets_init();
/*0045aa00*/ extern int __cdecl course_highway_physics_objects_create(struct CourseWorld *p0);
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


static int course_highway_lod_atomic_clone_a_r(undefined4 * p0, int p1);
extern void course_highway_lod_atomic_clone_a(void);
static int course_highway_lod_atomic_clone_b_r(int p0, int p1);
extern void course_highway_lod_atomic_clone_b(void);
static int course_highway_lod_atomic_scale_by_index_r(RpAtomic * p0, int p1);
extern void course_highway_lod_atomic_scale_by_index(void);
static int course_highway_lod_objects_setup_r(int p0, int p1);
extern void course_highway_lod_objects_setup(void);
/* Highway course load callback (inferred): builds the 5 big scenery objects (g_course_highway_6a2690..6a26a0), initialises the float tables (g_course_highway_8c3f60.., 8c4440.., 8c4720..), then creates the 2 x 4 x 10 grid (row, col, seg) of 0xfc byte LOD scenery objects (clump tables at course+0xc110, index triples chosen from variant_table, lod_tier 0..2 selects the far clump 0x2e..0x30), links each object to the next through +0xbc, and finally creates physics objects, shadow/smoke particles and, on COURSE_HIGHWAYN, the headlight. */
void course_highway_cb0(struct CourseWorld *course)

{
  int *chain_last;
  int *chain_first;
  undefined4 atomic;
  int idx_b;
  CourseId course_id;
  int *chain;
  int idx;
  int lod_tier;
  int idx_a;
  struct {
    int slot_off;
    int row;
    int col;
    int seg;
    int far_clump_idx;
    int obj;
    undefined4 shared_atomic_0;
    undefined4 shared_atomic_1;
    undefined4 shared_atomic_2;
    undefined4 shared_atomic_3;
    undefined4 lod_atomic_a;
    undefined4 lod_atomic_b;
    undefined4 lod_atomic_c;
    undefined4 variant_table [80];
  } frame;
  
  lod_tier = 0;
  /* variant_table: 80 entries of road piece kinds 0..0xc, consumed 4 bytes per piece (slot_off) in the loops below */
  frame.variant_table[0] = 9;
  frame.variant_table[1] = 6;
  frame.variant_table[2] = 10;
  frame.variant_table[3] = 1;
  frame.variant_table[4] = 7;
  frame.variant_table[5] = 0xb;
  frame.variant_table[6] = 2;
  frame.variant_table[7] = 8;
  frame.variant_table[8] = 0xc;
  frame.variant_table[9] = 3;
  frame.variant_table[10] = 0xb;
  frame.variant_table[0xb] = 8;
  frame.variant_table[0xc] = 0xc;
  frame.variant_table[0xd] = 6;
  frame.variant_table[0xe] = 9;
  frame.variant_table[0xf] = 10;
  frame.variant_table[0x10] = 0xb;
  frame.variant_table[0x11] = 6;
  frame.variant_table[0x12] = 1;
  frame.variant_table[0x13] = 7;
  frame.variant_table[0x14] = 1;
  frame.variant_table[0x15] = 7;
  frame.variant_table[0x16] = 0xb;
  frame.variant_table[0x17] = 2;
  frame.variant_table[0x18] = 8;
  frame.variant_table[0x19] = 0xc;
  frame.variant_table[0x1a] = 3;
  frame.variant_table[0x1b] = 9;
  frame.variant_table[0x1c] = 0;
  frame.variant_table[0x1d] = 7;
  frame.variant_table[0x1e] = 10;
  frame.variant_table[0x1f] = 8;
  frame.variant_table[0x20] = 6;
  frame.variant_table[0x21] = 0xb;
  frame.variant_table[0x22] = 0;
  frame.variant_table[0x23] = 6;
  frame.variant_table[0x24] = 10;
  frame.variant_table[0x25] = 1;
  frame.variant_table[0x26] = 7;
  frame.variant_table[0x27] = 0xb;
  frame.variant_table[0x28] = 0;
  frame.variant_table[0x29] = 6;
  frame.variant_table[0x2a] = 10;
  frame.variant_table[0x2b] = 1;
  frame.variant_table[0x2c] = 7;
  frame.variant_table[0x2d] = 0xb;
  frame.variant_table[0x2e] = 2;
  frame.variant_table[0x2f] = 8;
  frame.variant_table[0x30] = 0xc;
  frame.variant_table[0x31] = 3;
  frame.variant_table[0x32] = 2;
  frame.variant_table[0x33] = 8;
  frame.variant_table[0x34] = 0xc;
  frame.variant_table[0x35] = 7;
  frame.variant_table[0x36] = 9;
  frame.variant_table[0x37] = 10;
  frame.variant_table[0x38] = 0xb;
  frame.variant_table[0x39] = 6;
  frame.variant_table[0x3a] = 1;
  frame.variant_table[0x3b] = 7;
  frame.variant_table[0x3c] = 1;
  frame.variant_table[0x3d] = 7;
  frame.variant_table[0x3e] = 0xb;
  frame.variant_table[0x3f] = 2;
  frame.variant_table[0x40] = 8;
  frame.variant_table[0x41] = 0xc;
  frame.variant_table[0x42] = 3;
  frame.variant_table[0x43] = 9;
  frame.variant_table[0x44] = 0;
  frame.variant_table[0x45] = 0xc;
  frame.variant_table[0x46] = 10;
  frame.variant_table[0x4c] = 10;
  frame.variant_table[0x49] = 0xb;
  frame.variant_table[0x4f] = 0xb;
  frame.variant_table[0x47] = 8;
  frame.variant_table[0x48] = 6;
  frame.variant_table[0x4a] = 3;
  frame.variant_table[0x4b] = 6;
  frame.variant_table[0x4d] = 1;
  frame.variant_table[0x4e] = 7;
  atomic = (undefined4)(clump_first_atomic((RpClump *)course->clumps[53]));
  g_course_highway_6a2690 =
       func_0x004a5d50(atomic,course->mts_matrix_counts[1],course->mts_matrices + 1 * MTS_LIST_BYTES,
                       course->ambient_light,course->directional_light);
  atomic = (undefined4)(clump_first_atomic((RpClump *)course->clumps[56]));
  g_course_highway_6a2694 =
       func_0x004a5d50(atomic,course->mts_matrix_counts[4],course->mts_matrices + 4 * MTS_LIST_BYTES,
                       course->ambient_light,course->directional_light);
  atomic = (undefined4)(clump_first_atomic((RpClump *)course->clumps[59]));
  g_course_highway_6a2698 =
       func_0x004a5d50(atomic,course->mts_matrix_counts[7],course->mts_matrices + 7 * MTS_LIST_BYTES,
                       course->ambient_light,course->directional_light);
  atomic = (undefined4)(clump_first_atomic((RpClump *)course->clumps[60]));
  g_course_highway_6a269c =
       func_0x004a5d50(atomic,course->mts_matrix_counts[8],course->mts_matrices + 8 * MTS_LIST_BYTES,
                       course->ambient_light,course->directional_light);
  atomic = (undefined4)(clump_first_atomic((RpClump *)course->clumps[62]));
  g_course_highway_6a26a0 =
       func_0x004a5d50(atomic,course->mts_matrix_counts[10],course->mts_matrices + 10 * MTS_LIST_BYTES,
                       course->ambient_light,course->directional_light);
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
  /* float bit patterns (3 x 3 vectors) halved into g_course_highway_8c3f60.. below */
  g_course_highway_8c4440 = 0x3f0d14e4;
  g_course_highway_8c4444 = 0x3f6f4880;
  g_course_highway_8c4448 = 0x40277b4a;
  g_course_highway_8c444c = 0x3f058794;
  g_course_highway_8c4450 = 0x3f5b7e91;
  g_course_highway_8c4454 = 0x3f9f1de7;
  g_course_highway_8c4458 = 0x3f0d42c4;
  g_course_highway_8c445c = 0x3f86f34d;
  g_course_highway_8c4460 = 0x4044fc50;
  idx = 0;
  do {
    idx_b = idx + 0xc;
    *(float *)((int)&g_course_highway_8c3f60 + idx) =
         *(float *)((char *)((undefined *)&g_course_highway_8c4440) + idx) * k_f_0_5;
    *(float *)((int)&g_course_8c3f64 + idx) =
         *(float *)((char *)((undefined *)&g_course_highway_8c4444) + idx) * k_f_0_5;
    *(float *)((int)&g_course_highway_8c3f68 + idx) =
         *(float *)((char *)((undefined *)&g_course_highway_8c4448) + idx) * k_f_0_5;
    idx = idx_b;
  } while (idx_b < 0x24);
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
  frame.obj = 0x8bf0a0;
  frame.row = 0;
  frame.slot_off = 0;
  do {
    frame.col = 0;
    do {
      frame.seg = 0;
      do {
        idx_a = 0;
        idx_b = 0;
        idx = 0;
        /* per piece: choose three clump indices (idx_a/idx_b/idx) and the LOD tier from the piece kind */
        mem_zero(&frame.shared_atomic_0,0x10);
        switch(*(undefined4 *)((int)frame.variant_table + frame.slot_off)) {
        case 0:
          idx_b = 1;
          idx_a = 0;
          idx = 2;
          lod_tier = 1;
          break;
        case 1:
          idx_a = 3;
          idx_b = 4;
          idx = 5;
          lod_tier = 1;
          break;
        case 2:
          idx_a = 6;
          idx_b = 7;
          idx = 8;
          lod_tier = 1;
          break;
        case 3:
          idx_a = 9;
          idx_b = 10;
          idx = 0xb;
          lod_tier = 1;
          break;
        case 4:
          idx_a = 0xc;
          idx_b = 0xd;
          idx = 0xe;
          lod_tier = 1;
          break;
        case 5:
          idx_a = 0xf;
          idx_b = 0x10;
          idx = 0x11;
          lod_tier = 1;
          break;
        case 6:
          idx_a = 0x12;
          idx_b = 0x13;
          idx = 0x14;
          lod_tier = 0;
          break;
        case 7:
          idx_a = 0x15;
          idx_b = 0x16;
          idx = 0x17;
          lod_tier = 0;
          break;
        case 8:
          idx_a = 0x18;
          idx_b = 0x19;
          idx = 0x1a;
          lod_tier = 0;
          break;
        case 9:
          idx_a = 0x1b;
          idx_b = 0x1c;
          idx = 0x1d;
          lod_tier = 0;
          break;
        case 10:
          idx_a = 0x1e;
          idx_b = 0x1f;
          idx = 0x20;
          goto LAB_0045b28a;
        case 0xb:
          idx_a = 0x21;
          idx_b = 0x22;
          idx = 0x23;
          goto LAB_0045b28a;
        case 0xc:
          idx_a = 0x24;
          idx_b = 0x25;
          idx = 0x26;
LAB_0045b28a:
          lod_tier = 2;
        }
        if (lod_tier == 0) {
          /* far (lowest detail) clump index by tier */
          frame.far_clump_idx = 0x2e;
        }
        else if (lod_tier == 1) {
          frame.far_clump_idx = 0x2f;
        }
        else if (lod_tier == 2) {
          frame.far_clump_idx = 0x30;
        }
        frame.lod_atomic_a = (undefined4)(clump_first_atomic((RpClump *)course->clumps[idx_a]));
        frame.lod_atomic_b = (undefined4)(clump_first_atomic((RpClump *)course->clumps[idx_b]));
        frame.lod_atomic_c = (undefined4)(clump_first_atomic((RpClump *)course->clumps[idx]));
        atomic = course->clumps[44];
        frame.shared_atomic_0 = (undefined4)(clump_first_atomic((RpClump *)atomic));
        frame.shared_atomic_1 = (undefined4)(clump_first_atomic((RpClump *)atomic));
        frame.shared_atomic_2 = (undefined4)(clump_first_atomic((RpClump *)atomic));
        frame.shared_atomic_3 = (undefined4)(clump_first_atomic((RpClump *)atomic));
        atomic = (undefined4)(clump_first_atomic((RpClump *)course->clumps[frame.far_clump_idx]));
        mem_zero((void *)frame.obj,0xfc);
        *(int *)(frame.obj + 0xb8) = frame.seg;
        *(int *)(frame.obj + 0xac) = lod_tier;
        *(int *)(frame.obj + 0xb0) = frame.row;
        *(int *)(frame.obj + 0xb4) = frame.col;
        course_highway_lod_atomic_clone_a_r(&frame.lod_atomic_a,frame.obj);
        course_highway_lod_objects_setup_r(frame.obj,(int)&frame.shared_atomic_0);
        course_highway_lod_atomic_clone_b_r(atomic, frame.obj);
        course_id = current_course_id();
        if (course_id == COURSE_HIGHWAYN) {
          course_highway_lod_atomic_scale_by_index_r((RpAtomic *)atomic,frame.obj);
        }
        *(int *)((int)&g_course_highway_8c4300 + frame.slot_off) = frame.obj;
        frame.obj = frame.obj + 0xfc;
        frame.seg = frame.seg + 1;
        frame.slot_off = frame.slot_off + 4;
      } while (frame.seg < 10);
      frame.col = frame.col + 1;
    } while (frame.col < 4);
    frame.row = frame.row + 1;
    if (0x13f < frame.slot_off) {
      /* all 80 objects made (2 rows x 4 x 10): link them in chains of 10 (obj+0xbc = next object, ring per 10) */
      chain = g_course_highway_8c4308;
      do {
        idx = 4;
        do {
          *(int *)(chain[-2] + 0xbc) = chain[-1];
          *(int *)(chain[-1] + 0xbc) = *chain;
          *(int *)(*chain + 0xbc) = chain[1];
          *(int *)(chain[1] + 0xbc) = chain[2];
          *(int *)(chain[2] + 0xbc) = chain[3];
          *(int *)(chain[3] + 0xbc) = chain[4];
          *(int *)(chain[4] + 0xbc) = chain[5];
          *(int *)(chain[5] + 0xbc) = chain[6];
          *(int *)(chain[6] + 0xbc) = chain[7];
          chain_last = chain + 7;
          chain_first = chain + -2;
          chain = chain + 10;
          idx = idx + -1;
          *(int *)(*chain_last + 0xbc) = *chain_first;
        } while (idx != 0);
      } while ((int)chain < 0x8c4448);
      course_object_offsets_init();
      course_highway_physics_objects_create(course);
      course_car_shadow_particles_create();
      course_smoke_particles_create_00453ca0();
      course_id = current_course_id();
      if (course_id != COURSE_HIGHWAYN) {
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
