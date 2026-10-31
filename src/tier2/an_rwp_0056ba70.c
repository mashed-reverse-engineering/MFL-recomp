#include "mfl_tier2.h"

/*0056ea00*/ extern int rwp_obj_0056ea00(float *dst, float *prop);

typedef struct RwpInertiaProps {
  float e[3];   /* 0x0 principal values (non zero) */
  float q[4];   /* 0xc quaternion */
} RwpInertiaProps;
typedef struct RwpTables {
  unsigned char pad00[0x20];
  unsigned char *table_a;   /* 0x20 */
  unsigned char pad24[4];
  unsigned char *table_b;   /* 0x28 */
} RwpTables;
typedef struct RwpOwner {
  unsigned char pad00[0x10];
  RwpTables *tables;   /* 0x10 */
} RwpOwner;
typedef struct RwpSlot {
  RwpOwner *owner;   /* 0x0 */
  int index;   /* 0x4 */
} RwpSlot;

/* Stores the world inertia tensor of props (R * diag(e) * R^T) and of the inverse (diag(1/sqrt(e)) with the same quaternion) as 3 rows of
   3 floats (16 bytes apart) into the two per-body tables of the owner, slot->index * 48 bytes in.  Returns 0 when one of the three values is 0. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_volume_0056ba70(RwpSlot *slot, RwpInertiaProps *props)
{
  RwpInertiaProps inv;
  float tensor[9], tensor_inv[9];
  float *row;
  int i;

  if (props->e[0] != 0.0f && props->e[1] != 0.0f && props->e[2] != 0.0f) {
    inv.q[0] = props->q[0];
    inv.q[1] = props->q[1];
    inv.q[2] = props->q[2];
    inv.q[3] = props->q[3];
    inv.e[0] = 1.0f / sqrt(props->e[0]);
    inv.e[1] = 1.0f / sqrt(props->e[1]);
    inv.e[2] = 1.0f / sqrt(props->e[2]);
    rwp_obj_0056ea00(tensor, (float *)props);
    rwp_obj_0056ea00(tensor_inv, (float *)&inv);
    for (i = 0; i < 3; i++) {
      row = (float *)(slot->owner->tables->table_a + slot->index * 48 + i * 16);
      row[0] = tensor[3 * i];
      row[1] = tensor[3 * i + 1];
      row[2] = tensor[3 * i + 2];
    }
    for (i = 0; i < 3; i++) {
      row = (float *)(slot->owner->tables->table_b + slot->index * 48 + i * 16);
      row[0] = tensor_inv[3 * i];
      row[1] = tensor_inv[3 * i + 1];
      row[2] = tensor_inv[3 * i + 2];
    }
    return 1;
  }
  return 0;
}
