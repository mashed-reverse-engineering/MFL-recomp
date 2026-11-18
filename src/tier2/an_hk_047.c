#include "mfl_tier2.h"
/*00494630*/ extern undefined4 hermite_interpolate(float p0, void *p1, void *p2, void *p3, void *p4, void *p5);
/*004c5b10*/ extern void im3d_draw_line(void *p0, void *p1, void *p2);
/*004c5c40*/ extern void im3d_draw_matrix_axes(void *p0, void *p1, undefined4 p2);
/*005dd378*/ extern float k_f_0_1;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;

#define HERMITE_SEGMENTS (10)
#define FLOAT_0_25_BITS (0x3e800000)

/* one 0x40-byte frame matrix of the ghost block; only its position (+0x30) is read here, copied as raw words */
typedef struct GhostMatrix {
  char unknown_00[0x30];
  undefined4 pos[3];
  char unknown_3c[4];
} GhostMatrix;

/* ghost car data block: the five frame matrices drawn as axes start at +0x28 */
typedef struct GhostBlock {
  char unknown_00[0x28];
  GhostMatrix matrix[5];
} GhostBlock;

/* ghost_draw_markers */

/* "draws the ghost car's wire markers (Im3D lines) in grey/white" */

/*
 * Draws the ghost car's wire markers as Im3D lines (inferred from the render calls).
 * ghost: the ghost car's data block.
 * Returns nothing; leaves the z-test render state enabled again.
 */

void ghost_draw_markers(GhostBlock *ghost)

{
  uint segment;
  uint next_segment;
  undefined1 colours[20];       /* 5 RGBA colours in one frame object: im3d_draw_matrix_axes reads 4 bytes through each pointer */
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
  im3d_draw_matrix_axes(&ghost->matrix[1],0,FLOAT_0_25_BITS);
  im3d_draw_matrix_axes(&ghost->matrix[2],0,FLOAT_0_25_BITS);
  im3d_draw_matrix_axes(&ghost->matrix[0],colours,FLOAT_0_25_BITS);
  im3d_draw_matrix_axes(&ghost->matrix[3],colours + 4,FLOAT_0_25_BITS);
  im3d_draw_matrix_axes(&ghost->matrix[4],colours + 8,FLOAT_0_25_BITS);
  marker_vecs[3] = ghost->matrix[1].pos[0];
  marker_vecs[4] = ghost->matrix[1].pos[1];
  marker_vecs[5] = ghost->matrix[1].pos[2];
  marker_vecs[0] = ghost->matrix[2].pos[0];
  marker_vecs[1] = ghost->matrix[2].pos[1];
  marker_vecs[2] = ghost->matrix[2].pos[2];
  im3d_draw_line(marker_vecs + 3,marker_vecs,0);
  marker_vecs[15] = ghost->matrix[4].pos[0];
  marker_vecs[16] = ghost->matrix[4].pos[1];
  marker_vecs[17] = ghost->matrix[4].pos[2];
  marker_vecs[12] = ghost->matrix[1].pos[0];
  marker_vecs[13] = ghost->matrix[1].pos[1];
  marker_vecs[14] = ghost->matrix[1].pos[2];
  marker_vecs[9] = ghost->matrix[2].pos[0];
  marker_vecs[10] = ghost->matrix[2].pos[1];
  marker_vecs[11] = ghost->matrix[2].pos[2];
  marker_vecs[6] = ghost->matrix[3].pos[0];
  marker_vecs[7] = ghost->matrix[3].pos[1];
  marker_vecs[8] = ghost->matrix[3].pos[2];
  /* walk HERMITE_SEGMENTS segments along the curve; each segment is one line between two
     interpolated points, coloured from the colour table by segment parity (inferred) */
  segment = 0;
  do {
    hermite_interpolate((float)(int)segment * k_f_0_1,marker_vecs + 3,marker_vecs + 15,marker_vecs + 12,marker_vecs + 9,
                        marker_vecs + 6);
    next_segment = segment + 1;
    hermite_interpolate((float)(int)next_segment * k_f_0_1,marker_vecs,marker_vecs + 15,marker_vecs + 12,marker_vecs + 9,
                        marker_vecs + 6);
    im3d_draw_line(marker_vecs + 3,marker_vecs,colours + 12 + (segment & 1) * 4);
    segment = next_segment;
  } while ((int)next_segment < HERMITE_SEGMENTS);
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
  return;
}
