#include "mfl_tier2.h"
/*0048e6d0*/ extern int rwp_object_get_handle();
/*0056c140*/ extern int __cdecl FUN_0056c140(int a1, int a2, void *a3);
/*0056c3b0*/ extern int __cdecl FUN_0056c3b0(int a1, int a2, int a3, int a4, void *a5);
/*0058c8a0*/ extern int rwp_owner_object_get(int p0);
typedef struct { float x, y, z; } RwpV3;
typedef struct { char setup[16]; char header[4]; RwpV3 point; RwpV3 normal; int pad; } RwpRayHit;
/* rwp_object_query_ray_hit */
/* Ray test against the physics body of an object that is currently in the world; on a hit copies the hit point and normal out and returns 1. */
int __cdecl rwp_object_query_ray_hit(undefined4 object_ref,undefined4 ray_origin,undefined4 ray_direction,RwpV3 *hit_point,RwpV3 *hit_normal)
{
  int owner_block_word2;
  int query_handle;
  int owner_or_hit;
  RwpRayHit hit_buffer;

  query_handle = rwp_object_get_handle(object_ref);
  owner_or_hit = rwp_owner_object_get(query_handle);
  query_handle = **(int **)(owner_or_hit + 0x10);
  owner_block_word2 = (*(int **)(owner_or_hit + 0x10))[2];
  if ((*(int *)(owner_or_hit + 0x24) != 0) &&
     ((*(uint *)(*(char **)(*(char **)(owner_or_hit + 0x24) + 100) + (uint)(*(ushort *)(owner_or_hit + 0x20) >> 5) * 4) & 1 << (*(ushort *)(owner_or_hit + 0x20) & 0x1f)) != 0)) {
    FUN_0056c140(query_handle,owner_block_word2,hit_buffer.setup);
    owner_or_hit = FUN_0056c3b0(query_handle,owner_block_word2,ray_origin,ray_direction,hit_buffer.header);
    if (owner_or_hit != 0) {
      *hit_point = hit_buffer.point;
      *hit_normal = hit_buffer.normal;
      return 1;
    }
  }
  return 0;
}
