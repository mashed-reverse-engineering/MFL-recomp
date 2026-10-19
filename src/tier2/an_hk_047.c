#include "mfl_tier2.h"
/*00494630*/ extern undefined4 hermite_interpolate(float p0, void *p1, void *p2, void *p3, void *p4, void *p5);
/*004c5b10*/ extern void FUN_004c5b10(undefined4 p0, undefined4 p1, undefined4 p2);
/*004c5c40*/ extern void FUN_004c5c40(undefined4 p0, undefined4 p1, undefined4 p2);
/*005dd378*/ extern float k_f_0_1;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;

#define HERMITE_SEGMENTS (10)
#define FLOAT_0_25_BITS (0x3e800000)


/* ghost_draw_markers */

/* "draws the ghost car's wire markers (Im3D lines) in grey/white" */

/*
 * Draws the ghost car's wire markers as Im3D lines (inferred from the render calls).
 * ghost_base: address of the ghost car's data block (uint, not a pointer). The same
 * variable is reused as the segment index for the hermite loop further down.
 * Returns nothing; leaves the z-test render state enabled again.
 */

void ghost_draw_markers(uint ghost_base)

{
  uint next_segment;
  undefined1 colours[20];       /* 5 RGBA colours in one frame object: FUN_004c5c40 reads 4 bytes through each pointer */
  undefined4 marker_vecs[18];       /* six 3-float vectors in one frame object, passed by address */

  /* colour table: RGBA bytes, five entries of four bytes each */
  colours[2] = 0xc0;
  colours[10] = 0xc0;
  colours[12] = 0xc0;
  colours[13] = 0xc0;
  colours[14] = 0xc0;
  colours[3] = 0xff;
  colours[4] = 0xff;
  colours[5] = 0xff;
  colours[6] = 0xff;
  colours[7] = 0xff;
  colours[8] = 0xff;
  colours[11] = 0xff;
  colours[15] = 0xff;
  colours[19] = 0xff;
  colours[0] = 0x20;
  colours[1] = 0x20;
  colours[9] = 0x20;
  colours[16] = 0x40;
  colours[17] = 0x40;
  colours[18] = 0x40;
  /* turn z-test off so the markers draw on top of the scene (inferred) */
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
  /* five marker sets (inferred): the calls below pass a 0.25f size for each */
  FUN_004c5c40(ghost_base + 0x68,0,FLOAT_0_25_BITS);
  FUN_004c5c40(ghost_base + 0xa8,0,FLOAT_0_25_BITS);
  FUN_004c5c40(ghost_base + 0x28,colours,FLOAT_0_25_BITS);
  FUN_004c5c40(ghost_base + 0xe8,colours + 4,FLOAT_0_25_BITS);
  FUN_004c5c40(ghost_base + 0x128,colours + 8,FLOAT_0_25_BITS);
  marker_vecs[3] = *(undefined4 *)(ghost_base + 0x98);
  marker_vecs[4] = *(undefined4 *)(ghost_base + 0x9c);
  marker_vecs[5] = *(undefined4 *)(ghost_base + 0xa0);
  marker_vecs[0] = *(undefined4 *)(ghost_base + 0xd8);
  marker_vecs[1] = *(undefined4 *)(ghost_base + 0xdc);
  marker_vecs[2] = *(undefined4 *)(ghost_base + 0xe0);
  FUN_004c5b10(marker_vecs + 3,marker_vecs,0);
  marker_vecs[15] = *(undefined4 *)(ghost_base + 0x158);
  marker_vecs[16] = *(undefined4 *)(ghost_base + 0x15c);
  marker_vecs[17] = *(undefined4 *)(ghost_base + 0x160);
  marker_vecs[12] = *(undefined4 *)(ghost_base + 0x98);
  marker_vecs[13] = *(undefined4 *)(ghost_base + 0x9c);
  marker_vecs[14] = *(undefined4 *)(ghost_base + 0xa0);
  marker_vecs[9] = *(undefined4 *)(ghost_base + 0xd8);
  marker_vecs[10] = *(undefined4 *)(ghost_base + 0xdc);
  marker_vecs[11] = *(undefined4 *)(ghost_base + 0xe0);
  marker_vecs[6] = *(undefined4 *)(ghost_base + 0x118);
  marker_vecs[7] = *(undefined4 *)(ghost_base + 0x11c);
  marker_vecs[8] = *(undefined4 *)(ghost_base + 0x120);
  /* walk HERMITE_SEGMENTS segments along the curve; each segment is one line between two
     interpolated points, coloured from the colour table by segment parity (inferred) */
  ghost_base = 0;
  do {
    hermite_interpolate((float)(int)ghost_base * k_f_0_1,marker_vecs + 3,marker_vecs + 15,marker_vecs + 12,marker_vecs + 9,
                        marker_vecs + 6);
    next_segment = ghost_base + 1;
    hermite_interpolate((float)(int)next_segment * k_f_0_1,marker_vecs,marker_vecs + 15,marker_vecs + 12,marker_vecs + 9,
                        marker_vecs + 6);
    FUN_004c5b10(marker_vecs + 3,marker_vecs,colours + 12 + (ghost_base & 1) * 4);
    ghost_base = next_segment;
  } while ((int)next_segment < HERMITE_SEGMENTS);
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
  return;
}
