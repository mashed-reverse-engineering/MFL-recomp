// cluster-entries: fx_matrix_slot_store
/* Installed cluster fx_matrix_slot_store (tools/cluster_install.py from output/cluster_pilot8/fx_matrix_slot_store.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster fx_matrix_slot_store: appends a decal matrix to a ring of 256 slots (0x74 bytes): the matrix is scaled by 0.75 * scale in x and y and
   copied when the caller passes one, the position, scale and a random factor 0.6..1.3 are stored, and the slot's bit in the "used" mask is set. */
typedef struct MatrixSlot {
    float position[3];     /* 0x00 */
    int unknown_0c;        /* 0x0c: cleared */
    int unknown_10;
    float scale;           /* 0x14 */
    float random_factor;   /* 0x18 */
    int unknown_1c;        /* 0x1c: cleared */
    int unknown_20;
    int unknown_24;
    RwMatrix matrix;       /* 0x28 */
    int used;              /* 0x68: set to 1 */
    int has_matrix;        /* 0x6c */
    int unknown_70;        /* 0x70: cleared */
} MatrixSlot;

extern float k_f_0_75;
extern float random_range(float low, float high);
extern MatrixSlot g_sound_752230[256];            /* slot ring */
extern unsigned char g_particles_752200[32];      /* bit mask of the slots in use */
extern int g_particles_759630;                    /* next slot */

/* was: EAX = has matrix flag, stack = matrix, scale */
void fx_matrix_slot_store_n(int has_matrix, RwMatrix *matrix, float scale)
{
    RwV3d factor;
    MatrixSlot *slot;
    int index;

    factor.x = 1.0f;
    factor.y = 1.0f;
    factor.z = 1.0f;
    if (g_particles_759630 > 0xff)
        g_particles_759630 = 0;
    index = g_particles_759630;
    slot = &g_sound_752230[index];
    if (has_matrix == 0) {
        slot->has_matrix = 0;
    } else {
        RwMatrix *scaled;

        factor.x = scale * k_f_0_75;
        factor.y = factor.x;
        scaled = RwMatrixScale(matrix, &factor, rwCOMBINEPRECONCAT);
        slot->matrix = *scaled;
        slot->has_matrix = 1;
    }
    slot->used = 1;
    slot->position[0] = matrix->pos.x;
    slot->position[1] = matrix->pos.y;
    slot->position[2] = matrix->pos.z;
    slot->scale = scale;
    slot->unknown_0c = 0;
    slot->random_factor = random_range(0.6f, 1.3f);
    slot->unknown_1c = 0;
    slot->unknown_70 = 0;
    g_particles_752200[g_particles_759630 >> 3] |= (unsigned char)(1 << (g_particles_759630 & 7));
    g_particles_759630 = g_particles_759630 + 1;
}

/* entry thunk at the original address of fx_matrix_slot_store: register inputs (eax,s0,s1) -> natural C fx_matrix_slot_store_n; ret 0 */
__declspec(naked) void fx_matrix_slot_store(void)
{
    __asm {
        push dword ptr [esp + 8]
        push dword ptr [esp + 8]
        push eax
        call fx_matrix_slot_store_n
        add esp, 12
        ret
    }
}
