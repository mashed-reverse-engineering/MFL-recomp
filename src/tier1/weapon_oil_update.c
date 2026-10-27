#include "mfl_tier2.h"
/*00462d70*/ extern int weapon_oil_slicks_update_render();
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*006a7c80*/ extern undefined4 g_weapon_oil_6a7c80;
/*006a7cc0*/ extern char g_weapon_oil_6a7cc0[];

#define RW_OBJECT_FRAME_OFFSET 4    /* Offset to RwFrame pointer in RwObject / RpAtomic */
#define WEAPON_OIL_SLOT_STRIDE 4    /* Stride between oil weapon slots in dwords (16 bytes) */
#define WEAPON_OIL_TABLE_END ((int)&g_weapon_oil_6a7cc0) /* End address of the oil weapon slot table */


/* Updates active oil weapon slot frame transforms and updates/renders oil slicks. */

void weapon_oil_update(void)
{
  int *oil_slot;
  
  oil_slot = &g_weapon_oil_6a7c80;
  do {
    /* If oil slot is active, update model frame with slot transform matrix */
    if (oil_slot[-1] != 0) {
      RwFrameTransform(*(RwFrame **)(oil_slot[1] + RW_OBJECT_FRAME_OFFSET),(RwMatrix *)*oil_slot,rwCOMBINEREPLACE);
    }
    oil_slot = oil_slot + WEAPON_OIL_SLOT_STRIDE;
  } while ((int)oil_slot < WEAPON_OIL_TABLE_END);
  /* Update and render active oil slicks */
  weapon_oil_slicks_update_render();
  return;
}
