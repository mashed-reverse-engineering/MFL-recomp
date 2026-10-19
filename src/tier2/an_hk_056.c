#include "mfl_tier2.h"
/*00658b50*/ extern char *g_btb_panel_658b50;
/*00658b54*/ extern char *g_btb_panel_658b54;
/*00658b58*/ extern char *g_btb_panel_658b58;
/*00658b5c*/ extern char *g_btb_panel_658b5c;
/*00658b60*/ extern char *g_btb_panel_658b60;
/*00658b64*/ extern char *g_btb_panel_658b64;
/*00658b68*/ extern char *g_btb_panel_658b68;
/*00658b70*/ extern char *g_btb_panel_658b70;
/*00658b74*/ extern char *g_btb_panel_658b74;
/*00658b78*/ extern char *g_btb_panel_658b78;
/*00658b80*/ extern char *g_btb_panel_658b80;
/*00658b84*/ extern char *g_btb_panel_658b84;
/*00658b88*/ extern char *g_btb_panel_658b88;

#define SLOT_STRIDE (0xc4)          /* bytes per panel slot record (inferred) */
#define MATRIX_FLOAT_COUNT (0x10)   /* 4x4 matrix = 16 floats */
#define FLOAT_ONE_BITS (0x3f800000) /* IEEE-754 bit pattern of 1.0f */

/* btb_panel_slot_set_matrix */

/* "(slot, matrix*): copies 16 floats" */

/* Stores a 4x4 matrix into panel slot `slot` (slot records are SLOT_STRIDE bytes apart).
   If matrix is NULL, instead resets the slot: diagonal entries 0, 5 and 10 are set to 1.0,
   the other entries it touches are set to 0, and the flag word gets bits 0x20003 OR'd in. (inferred) */
void btb_panel_slot_set_matrix(int slot,undefined4 *matrix)

{
  int floats_left;
  undefined4 *dst;

  /* slot index becomes a byte offset into the panel table from here on */
  slot = slot * SLOT_STRIDE;
  if (matrix != (undefined4 *)0x0) {
    /* copy the 16 matrix floats into the slot */
    dst = (undefined4 *)((char *)&g_btb_panel_658b50 + slot);
    for (floats_left = MATRIX_FLOAT_COUNT; floats_left != 0; floats_left = floats_left + -1) {
      *dst = *matrix;
      matrix = matrix + 1;
      dst = dst + 1;
    }
    return;
  }
  /* matrix == NULL: reset the slot to identity-style values */
  *(undefined4 *)((char *)&g_btb_panel_658b60 + slot) = 0;
  *(undefined4 *)((char *)&g_btb_panel_658b58 + slot) = 0;
  *(undefined4 *)((char *)&g_btb_panel_658b54 + slot) = 0;
  *(undefined4 *)((char *)&g_btb_panel_658b74 + slot) = 0;
  *(undefined4 *)((char *)&g_btb_panel_658b70 + slot) = 0;
  *(undefined4 *)((char *)&g_btb_panel_658b68 + slot) = 0;
  *(undefined4 *)((char *)&g_btb_panel_658b88 + slot) = 0;
  *(undefined4 *)((char *)&g_btb_panel_658b84 + slot) = 0;
  *(undefined4 *)((char *)&g_btb_panel_658b80 + slot) = 0;
  *(undefined4 *)((char *)&g_btb_panel_658b78 + slot) = FLOAT_ONE_BITS;
  *(undefined4 *)((char *)&g_btb_panel_658b64 + slot) = FLOAT_ONE_BITS;
  *(undefined4 *)((char *)&g_btb_panel_658b50 + slot) = FLOAT_ONE_BITS;
  /* flag word at offset 0x0c of the slot */
  *(uint *)((char *)&g_btb_panel_658b5c + slot) = *(uint *)((char *)&g_btb_panel_658b5c + slot) | 0x20003;
  return;
}


