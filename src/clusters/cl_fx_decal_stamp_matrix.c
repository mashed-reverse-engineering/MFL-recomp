// cluster-entries: fx_decal_stamp_matrix
/* Installed cluster fx_decal_stamp_matrix (tools/cluster_install.py from output/cluster_pilot_zy/fx_decal_stamp_matrix.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00752220*/ extern float g_particles_752220;
/*00752224*/ extern float g_particles_752224;
/*00752228*/ extern float g_particles_752228;
/*005ddafc*/ extern float k_f_0_05;
/*00481fd0*/ extern double random_range(float low, float high);
/*004d4870*/ extern RwMatrix *RwMatrixTranslate(RwMatrix *matrix, struct RwV3d *translation, enum RwOpCombineType combine);

static void fx_matrix_slot_store_r(int p0, RwMatrix * p1, float p2);
extern void fx_matrix_slot_store(void);
/* Stamps a decal for the matrix `source` (scale `scale`) when it is at least 0.05 (squared distance) away from the last stamp position: stores slot 1, then `count` jittered copies (random +-0.05 in x and z) in slot 0. */
void fx_decal_stamp_matrix(RwMatrix *source, float scale, int count)
{
  RwMatrix matrix;
  struct RwV3d jitter;
  double random_x;
  double random_z;

  matrix = *source;
  if (k_f_0_05 <= (matrix.pos.z - g_particles_752228) * (matrix.pos.z - g_particles_752228) +
                  (matrix.pos.x - g_particles_752220) * (matrix.pos.x - g_particles_752220) +
                  (matrix.pos.y - g_particles_752224) * (matrix.pos.y - g_particles_752224)) {
    g_particles_752220 = matrix.pos.x;
    g_particles_752224 = matrix.pos.y;
    g_particles_752228 = matrix.pos.z;
    fx_matrix_slot_store_r(1, &matrix, scale);
    for (; count != 0; count--) {
      random_x = random_range(-1.0f, 1.0f);
      jitter.x = (float)random_x;
      jitter.y = 0.0f;
      random_z = random_range(-1.0f, 1.0f);
      jitter.x = jitter.x * k_f_0_05;
      jitter.y = jitter.y * k_f_0_05;
      jitter.z = (float)(random_z * k_f_0_05);
      RwMatrixTranslate(&matrix, &jitter, rwCOMBINEPOSTCONCAT);
      fx_matrix_slot_store_r(0, &matrix, scale);
    }
  }
}

/* fx_matrix_slot_store_r: C -> original fx_matrix_slot_store (registers eax,s0,s1) */
__declspec(naked) void fx_matrix_slot_store_r(int p0, RwMatrix * p1, float p2)
{
    __asm {
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        mov eax, dword ptr [esp + 12]
        call fx_matrix_slot_store
        add esp, 8
        ret
    }
}
