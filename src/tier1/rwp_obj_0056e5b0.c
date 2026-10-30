/* compile: /G5 */
#include "mfl_tier2.h"
#define BIT_TEST(bits, id) ((bits)[(id) >> 5] & (1 << ((id) & 0x1f)))
typedef struct RwpWorld { char pad[0x60]; unsigned *bitsA; } RwpWorld;
typedef struct RwpPoseRef { int pad; void *pose; RwMatrix *matrix; } RwpPoseRef;
typedef struct RwpBody { void *vtbl; char pad[0xc]; RwpPoseRef *poses; char pad2[0xc]; unsigned short id; char pad3[2]; RwpWorld *owner; } RwpBody;
extern void rwp_sphere_0056bdd0(void *pose, RwMatrix *matrix);
/* If the object is in an active world, applies the pose entry with the given index to the object's pose. (Rwp library) */
void rwp_obj_0056e5b0(RwpBody *b, unsigned short pose_index)
{
  if (b->owner && BIT_TEST(b->owner->bitsA, b->id))
    rwp_sphere_0056bdd0(b->poses[pose_index].pose, b->poses[pose_index].matrix);
}
