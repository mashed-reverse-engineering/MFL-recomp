#include "mfl_tier2.h"
/*00498f40*/ extern int fx_ring_add_point();
/*006fcdd8*/ extern undefined4 g_rw_6fcdd8;


/* Contact types (unknown) */
#define CONTACT_TYPE_UNKNOWN_10        0x10
#define CONTACT_TYPE_UNKNOWN_11        0x11
#define CONTACT_TYPE_UNKNOWN_19        0x19

/* Structure offsets */
#define CONTACT_OFFSET_TYPE            0x10
#define CONTACT_OFFSET_FLAG_INDEX      0x18
#define OBJECT_OFFSET_RING_OWNER       0x14
#define RING_OWNER_OFFSET_UNKNOWN_10   0x10
#define RING_OFFSET_UNKNOWN_8          8
#define RING_POINTS_BASE_OFFSET        0x30

/* FX ring parameters */
#define FX_RING_POINT_SCALE_ONE        0x3f800000 /* 1.0f in IEEE-754 */

/* Contact flags */
#define CONTACT_FLAG_PROCESSED         1


/* Adds three FX ring contact points and sets the processed flag for specific vehicle collision contact types. */
void rwpcar_ring_add_points_for_contact(char *car_object, char *contact_record)
{
  int contact_type_or_ring_base;

  contact_type_or_ring_base = *(int *)(contact_record + CONTACT_OFFSET_TYPE);
  /* Check for qualifying contact types and verify ring owner exists */
  if ((((contact_type_or_ring_base == CONTACT_TYPE_UNKNOWN_19) || (contact_type_or_ring_base == CONTACT_TYPE_UNKNOWN_11)) || (contact_type_or_ring_base == CONTACT_TYPE_UNKNOWN_10)) && (*(int *)(car_object + OBJECT_OFFSET_RING_OWNER) != 0))
  {
    /* Traverse ring owner hierarchy (unknown structures) to reach ring points base */
    contact_type_or_ring_base = *(int *)(*(char **)(*(char **)(car_object + OBJECT_OFFSET_RING_OWNER) + RING_OWNER_OFFSET_UNKNOWN_10) + RING_OFFSET_UNKNOWN_8) + RING_POINTS_BASE_OFFSET;
    fx_ring_add_point(contact_type_or_ring_base, FX_RING_POINT_SCALE_ONE);
    fx_ring_add_point(contact_type_or_ring_base, FX_RING_POINT_SCALE_ONE);
    fx_ring_add_point(contact_type_or_ring_base, FX_RING_POINT_SCALE_ONE);
    /* Mark contact as processed in global flag table */
    (&g_rw_6fcdd8)[*(int *)(contact_record + CONTACT_OFFSET_FLAG_INDEX)] = (&g_rw_6fcdd8)[*(int *)(contact_record + CONTACT_OFFSET_FLAG_INDEX)] | CONTACT_FLAG_PROCESSED;
  }
  return;
}
