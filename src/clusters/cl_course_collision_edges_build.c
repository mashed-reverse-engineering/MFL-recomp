// cluster-entries: course_collision_edges_build
/* Installed cluster course_collision_edges_build (tools/cluster_install.py from output/cluster_pilot_zy/course_collision_edges_build.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
extern void *memset(void *, int, unsigned int);
#define EDGE_BYTES_PER_TRIANGLE (0x48) /* 3 edge records of 6 ints */
#define EDGE_RECORD_BYTES (0x18) /* 6 ints */
/* course_collision_edges_build */

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
  int remaining;
  int tri_index;
  int other_tri_or_word_count;
  int *other;
  int edge_count;
  int *dst;
  struct {
    int shared_edge_count;
    int outer_index;
    int outer_left;
  } frame;
  
  edge_count = tri_count * 3;
  frame.shared_edge_count = 0;
  mem = RwMalloc(tri_count * EDGE_BYTES_PER_TRIANGLE,0);
  memset(mem,0,tri_count * EDGE_BYTES_PER_TRIANGLE);
  tri_index = 0;
  edge = mem;
  if (0 < tri_count) {
    do {
      edge[4] = (uint)*tri_idx;
      edge[5] = (uint)tri_idx[1];
      *edge = (int)tri_idx;
      edge[1] = 0;
      edge[2] = tri_index;
      edge[3] = -1;
      edge[10] = (uint)tri_idx[1];
      edge[0xb] = (uint)tri_idx[2];
      edge[6] = (int)tri_idx;
      edge[8] = tri_index;
      edge[7] = 0;
      edge[9] = -1;
      edge[0x10] = (uint)tri_idx[2];
      edge[0x11] = (uint)*tri_idx;
      edge[0xc] = (int)tri_idx;
      edge[0xe] = tri_index;
      edge[0xd] = 0;
      edge[0xf] = -1;
      tri_index = tri_index + 1;
      tri_idx = tri_idx + 4;
      edge = edge + 0x12;
    } while (tri_index < tri_count);
  }
  frame.outer_left = edge_count + -1;
  if (0 < frame.outer_left) {
    frame.outer_index = 1;
    edge = mem;
    do {
      if ((*edge != 0) && (frame.outer_index < edge_count)) {
        remaining = edge_count - frame.outer_index;
        other = edge;
        do {
          other = other + 6;
          other_tri_or_word_count = *other;
          if (other_tri_or_word_count != 0) {
            if (collision_edge_shares_vertex_pair_r(edge,other) != 0) {
              edge[1] = other_tri_or_word_count;
              edge[3] = other[2];
              *other = 0;
              other[2] = -1;
            }
          }
          remaining = remaining + -1;
        } while (remaining != 0);
      }
      edge = edge + 6;
      frame.outer_index = frame.outer_index + 1;
      frame.outer_left = frame.outer_left + -1;
    } while (frame.outer_left != 0);
  }
  edge = mem;
  remaining = edge_count;
  if (0 < edge_count) {
    do {
      if ((*edge != 0) && (edge[1] != 0)) {
        frame.shared_edge_count = frame.shared_edge_count + 1;
      }
      remaining = remaining + -1;
      edge = edge + 6;
    } while (remaining != 0);
  }
  packed = RwMalloc(frame.shared_edge_count * EDGE_RECORD_BYTES,0);
  *out_edges = (int)packed;
  out_edges[1] = frame.shared_edge_count;
  if (0 < edge_count) {
    remaining = 0;
    edge = mem;
    do {
      if ((*edge != 0) && (edge[1] != 0)) {
        other = edge;
        dst = (int *)(*out_edges + remaining);
        for (other_tri_or_word_count = 6; other_tri_or_word_count != 0; other_tri_or_word_count = other_tri_or_word_count + -1) {
          *dst = *other;
          other = other + 1;
          dst = dst + 1;
        }
        remaining = remaining + EDGE_RECORD_BYTES;
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
