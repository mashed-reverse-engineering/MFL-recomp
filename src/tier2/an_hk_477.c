#include "mfl_tier2.h"
/*004817b0*/ extern int atomic_plugin_get_bbox();
/*00481fd0*/ extern double random_range();
/*0049ff00*/ extern undefined __cdecl particle_emit_textured(float *p0, undefined4 *p1, int p2, int p3, struct RwTexture *p4, float p5, float *p6);
/*004c40c0*/ extern int FUN_004c40c0(undefined4 p0);
/*004c40f0*/ extern void * FUN_004c40f0(undefined4 p0);
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);

#define PARTICLE_COUNT (7)
#define PARTICLE_DEFAULT_COLOUR (0xffffffff) /* white, opaque */
#define OPAQUE_ALPHA (0xff)
#define BBOX_CORNER_COUNT (2) /* min and max corner */
#define RANDOM_SIZE_MIN (0x3f4ccccd) /* float bits of 0.8f (inferred) */
#define RANDOM_SIZE_MAX (0x3fa66666) /* float bits of 1.3f (inferred) */

/* fx_object_break_into_particles */

/* "emits 7 textured particles along the atomic's world bbox diagonal" */

/* Breaks an atomic into particles: PARTICLE_COUNT textured particles spaced along the diagonal
 * from the bbox min corner to the max corner, both transformed to world space by the atomic's frame LTM.
 * atomic: the atomic being broken apart (inferred).
 * emit_vector: float[3] passed unchanged to particle_emit_textured as its last argument (inferred: per-particle direction).
 * Returns nothing.
 */
void fx_object_break_into_particles(int atomic,float *emit_vector)

{
  float fraction;
  char *iVar2;
  float *pfVar3;
  RwTexture *texture;
  RwMatrix *matrix;
  int particle_index;
  float10 particle_size;
  struct {
    undefined4 colour;
    float particle_pos_x;
    float particle_pos_y;
    float particle_pos_z;
    float diagonal_x;
    float diagonal_y;
    float diagonal_z;
    RwV3d bbox_min;
    float bbox_max_x;
    float bbox_max_y;
    float bbox_max_z;
  } emit;

  emit.colour = PARTICLE_DEFAULT_COLOUR;
  particle_index = 0;
  /* iVar2 holds the bbox pointer here, and is reused below for the FUN_004c40c0 result (colour source). */
  iVar2 = atomic_plugin_get_bbox(atomic);
  emit.bbox_min.x = *(float *)(iVar2 + 0xc);
  emit.bbox_min.y = *(float *)(iVar2 + 0x10);
  emit.bbox_min.z = *(float *)(iVar2 + 0x14);
  /* Max corner: pfVar3 holds the bbox pointer here, and is reused below for emit_vector. */
  pfVar3 = (float *)atomic_plugin_get_bbox(atomic);
  emit.bbox_max_x = *pfVar3;
  emit.bbox_max_y = pfVar3[1];
  emit.bbox_max_z = pfVar3[2];
  texture = (RwTexture *)FUN_004c40f0(atomic);
  RwFrameGetLTM(*(RwFrame **)(atomic + 4));
  /* Colour from FUN_004c40c0 (inferred: atomic colour) when it returns non-null; alpha forced opaque. */
  iVar2 = FUN_004c40c0(atomic);
  if (iVar2 != 0) {
    emit.colour = *(undefined4 *)(iVar2 + 4);
  }
  emit.colour = CONCAT13(OPAQUE_ALPHA,(undefined3)emit.colour);
  /* Transform both bbox corners to world space (they are contiguous in emit: bbox_min, then bbox_max). */
  matrix = RwFrameGetLTM(*(RwFrame **)(atomic + 4));
  RwV3dTransformPoints(&emit.bbox_min,&emit.bbox_min,BBOX_CORNER_COUNT,matrix);
  emit.diagonal_x = emit.bbox_max_x - emit.bbox_min.x;
  emit.diagonal_y = emit.bbox_max_y - emit.bbox_min.y;
  emit.diagonal_z = emit.bbox_max_z - emit.bbox_min.z;
  /* One particle per step. Possible decompiler artifact, unverified (idiv vs fdiv not checked in the exe): particle_index / PARTICLE_COUNT is integer division as decompiled,
   * so fraction is 0 for every index below PARTICLE_COUNT and all particles spawn at the min corner.
   * Left as-is: fixing it changes the compiled code. */
  do {
    fraction = (float)(particle_index / PARTICLE_COUNT);
    emit.particle_pos_x = emit.diagonal_x * fraction + emit.bbox_min.x;
    emit.particle_pos_y = emit.diagonal_y * fraction + emit.bbox_min.y;
    emit.particle_pos_z = fraction * emit.diagonal_z + emit.bbox_min.z;
    pfVar3 = emit_vector;
    particle_size = (float10)random_range(RANDOM_SIZE_MIN,RANDOM_SIZE_MAX);
    particle_emit_textured(&emit.particle_pos_x,&emit.colour,0,0,texture,(float)particle_size,pfVar3);
    particle_index = particle_index + 1;
  } while (particle_index < PARTICLE_COUNT);
  return;
}


