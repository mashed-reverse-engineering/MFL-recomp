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
/* tri_idx: tri_count triangles of 4 ushorts (3 vertex indices + pad).  Pass 1 builds 3 edge records per triangle (0x48 bytes per triangle; one record is 6 ints:
   [0] triangle ptr (0 = dropped), [1] second triangle ptr sharing the edge, [2] triangle number, [3] second triangle number (-1), [4],[5] the two vertex indices).
   Pass 2 merges each record with the later record over the same vertex pair (collision_edge_shares_vertex_pair).  Pass 3 counts the edges shared by two triangles
   and copies exactly those into out_edges[0] (0x18 bytes each), out_edges[1] = their count. */
void course_collision_edges_build_n(int tri_count, int *out_edges, ushort *tri_idx)

{
  int *mem;
  void *packed;
  int *edge;
  int n;
  int tmp;
  int *other;
  int edge_count;
  int *dst;
  struct {
    int local_c;
    int iStack_8;
    int iStack_4;
  } frame;
  
  edge_count = tri_count * 3;
  frame.local_c = 0;
  mem = RwMalloc(tri_count * 0x48,0);
  memset(mem,0,tri_count * 0x48);
  n = 0;
  edge = mem;
  if (0 < tri_count) {
    do {
      edge[4] = (uint)*tri_idx;
      edge[5] = (uint)tri_idx[1];
      *edge = (int)tri_idx;
      edge[1] = 0;
      edge[2] = n;
      edge[3] = -1;
      edge[10] = (uint)tri_idx[1];
      edge[0xb] = (uint)tri_idx[2];
      edge[6] = (int)tri_idx;
      edge[8] = n;
      edge[7] = 0;
      edge[9] = -1;
      edge[0x10] = (uint)tri_idx[2];
      edge[0x11] = (uint)*tri_idx;
      edge[0xc] = (int)tri_idx;
      edge[0xe] = n;
      edge[0xd] = 0;
      edge[0xf] = -1;
      n = n + 1;
      tri_idx = tri_idx + 4;
      edge = edge + 0x12;
    } while (n < tri_count);
  }
  frame.iStack_4 = edge_count + -1;
  if (0 < frame.iStack_4) {
    frame.iStack_8 = 1;
    edge = mem;
    do {
      if ((*edge != 0) && (frame.iStack_8 < edge_count)) {
        n = edge_count - frame.iStack_8;
        other = edge;
        do {
          other = other + 6;
          tmp = *other;
          if (tmp != 0) {
            if (collision_edge_shares_vertex_pair_r(edge,other) != 0) {
              edge[1] = tmp;
              edge[3] = other[2];
              *other = 0;
              other[2] = -1;
            }
          }
          n = n + -1;
        } while (n != 0);
      }
      edge = edge + 6;
      frame.iStack_8 = frame.iStack_8 + 1;
      frame.iStack_4 = frame.iStack_4 + -1;
    } while (frame.iStack_4 != 0);
  }
  edge = mem;
  n = edge_count;
  if (0 < edge_count) {
    do {
      if ((*edge != 0) && (edge[1] != 0)) {
        frame.local_c = frame.local_c + 1;
      }
      n = n + -1;
      edge = edge + 6;
    } while (n != 0);
  }
  packed = RwMalloc(frame.local_c * 0x18,0);
  *out_edges = (int)packed;
  out_edges[1] = frame.local_c;
  if (0 < edge_count) {
    n = 0;
    edge = mem;
    do {
      if ((*edge != 0) && (edge[1] != 0)) {
        other = edge;
        dst = (int *)(*out_edges + n);
        for (tmp = 6; tmp != 0; tmp = tmp + -1) {
          *dst = *other;
          other = other + 1;
          dst = dst + 1;
        }
        n = n + 0x18;
      }
      edge = edge + 6;
      edge_count = edge_count + -1;
    } while (edge_count != 0);
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
