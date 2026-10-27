#include "mfl_tier2.h"
/*004978d0*/ extern char *__cdecl fx_pool_alloc_particle(void *p0);
/*005ddc28*/ extern float k_f_0;
/*005e07f0*/ extern float k_f_0_80999994;
/*00745ce8*/ extern undefined g_particles_745ce8;
/*00745cec*/ extern undefined g_particles_745cec;
/*00745cf0*/ extern undefined g_particles_745cf0;
/*007519ac*/ extern float g_particles_7519ac;

/* Byte stride of one emission point in the g_particles_745ce8 table: sizeof(float[3]) (x, y, z). */
#define EMIT_POINT_STRIDE (0xc)

/* fx_trail_particle_spawn_interpolated */

/* "copies a template particle and lerps its position along the last emission point" */

/* Spawns one trail particle from the template particle into the particle pool.
 *   template_particle: float[] particle to copy; floats 0..2 are its position.
 *   interp_t: lerp fraction from the template position toward the stored emission point.
 *   extra_float: stored into the new particle's float slot 0x12 (0x48 bytes) (inferred meaning).
 *   emit_index: slot in the emission point table; 0 disables interpolation.
 * With emit_index != 0 the stored point for that slot is compared with the template position:
 *   interp_t <= 0: the stored point is reset to the template position and the particle spawns there.
 *   otherwise: if the squared distance to the stored point exceeds k_f_0_80999994 the spawn is skipped
 *     (the sqrt of k_f_0_80999994 is the radius); else the spawn point is lerped toward the stored point.
 * Returns nothing. If the pool has no free particle (alloc returns NULL), nothing is written. */
void fx_trail_particle_spawn_interpolated(float *template_particle,float interp_t,float extra_float,int emit_index)

{
  float delta_x;
  float delta_y;
  float delta_z;
  float *src_particle;
  int emit_offset;
  float *new_particle;
  float spawn_x;
  float spawn_z;
  float spawn_y;

  src_particle = template_particle;
  spawn_x = *template_particle;
  spawn_y = template_particle[1];
  spawn_z = template_particle[2];
  /* Interpolation against the last emission point stored for this slot. */
  if (emit_index != 0) {
    emit_offset = emit_index * EMIT_POINT_STRIDE;
    delta_x = *(float *)((char *)&g_particles_745ce8 + emit_offset) - spawn_x;
    delta_y = *(float *)((char *)&g_particles_745cec + emit_offset) - spawn_y;
    delta_z = *(float *)((char *)&g_particles_745cf0 + emit_offset) - spawn_z;
    /* No interpolation: store the template position as the new emission point. */
    if (!(interp_t > k_f_0)) {      /* NaN: no interpolation (original: fcomp, test ah,0x41, jne) */
      *(float *)((char *)&g_particles_745ce8 + emit_offset) = spawn_x;
      *(float *)((char *)&g_particles_745cec + emit_offset) = spawn_y;
      *(float *)((char *)&g_particles_745cf0 + emit_offset) = spawn_z;
    }
    else {
      /* Too far from the stored point (squared distance): skip this spawn entirely. */
      if (k_f_0_80999994 < delta_z * delta_z + delta_x * delta_x + delta_y * delta_y) {
        return;
      }
      /* Lerp the spawn point toward the stored emission point by interp_t. */
      spawn_x = delta_x * interp_t + spawn_x;
      spawn_y = delta_y * interp_t + spawn_y;
      spawn_z = delta_z * interp_t + spawn_z;
    }
  }
  /* Allocate from the pool. The argument is the address of the parameter (the decompiler artifact, inferred). */
  new_particle = (float *)fx_pool_alloc_particle(&template_particle);
  if (new_particle != (float *)0x0) {
    /* Copy the template's attributes, then overwrite position and extra_float. */
    new_particle[0xc] = src_particle[0xc];
    new_particle[0xd] = 0.0;
    *(undefined1 *)((char *)new_particle + 0x45) = *(undefined1 *)((char *)src_particle + 0x45);
    new_particle[0x10] = src_particle[0x10];
    new_particle[0xe] = src_particle[0xe];
    *new_particle = spawn_x;
    new_particle[1] = spawn_y;
    new_particle[2] = spawn_z;
    new_particle[6] = src_particle[6];
    new_particle[7] = src_particle[7];
    new_particle[8] = src_particle[8];
    new_particle[9] = src_particle[9];
    new_particle[10] = src_particle[10];
    new_particle[0xb] = src_particle[0xb];
    new_particle[4] = src_particle[4];
    new_particle[5] = src_particle[5];
    /* spawn_x is reused here as a temporary for the template's float slot 3. */
    spawn_x = src_particle[3];
    new_particle[0x12] = extra_float;
    new_particle[3] = spawn_x;
    new_particle[0xf] = g_particles_7519ac;
    /* Byte 0x44 gets bit 11 of the flags word at float slot 0x10 (inferred meaning). */
    *(byte *)(new_particle + 0x11) = (byte)(*(int *)&new_particle[0x10] >> 0xb) & 1;
  }
  return;
}


