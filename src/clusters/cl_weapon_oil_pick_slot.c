// cluster-entries: weapon_oil_pick_slot weapon_oil_pick_slot_forward
/* Installed cluster weapon_oil_pick_slot (tools/cluster_install.py from output/cluster_pilot5/weapon_oil_pick_slot.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster weapon_oil_pick_slot: oil slick slots (64 slots of 0x68 bytes at g_weapon_6a6238, used-bit map at g_weapon_6a7d28).  A new slick takes the first free slot,
   or (all used) the slot with the largest timer; it starts at the car matrix position with the full 4x4 matrix copied behind it. */
typedef struct OilSlot {
    float timer;           /* 0x00 */
    int unknown_04;
    float pos[3];          /* 0x08 */
    float scale;           /* 0x14 */
    int matrix[16];        /* 0x18 */
    float unknown_58;      /* 0x58 */
    float unknown_5c;      /* 0x5c */
    float unknown_60;      /* 0x60 */
    int unknown_64;
} OilSlot;                 /* 0x68 bytes */

extern OilSlot g_weapon_6a6238[64];
extern unsigned char g_weapon_6a7d28[];

/* was: EAX = source matrix (16 dwords; the position is the dword triple at +0x30) */
void weapon_oil_pick_slot_n(const int *matrix)
{
    OilSlot *best = g_weapon_6a6238;
    OilSlot *p = g_weapon_6a6238;
    OilSlot *slot;
    int i = 0;
    int k;

    for (;;) {
        if (p->timer > best->timer) {
            best = p;
        }
        if ((g_weapon_6a7d28[i >> 3] & (1 << (i & 7))) == 0) {
            slot = &g_weapon_6a6238[i];
            break;
        }
        p++;
        i++;
        if (p >= g_weapon_6a6238 + 64) {
            slot = best;
            break;
        }
    }
    g_weapon_6a7d28[i >> 3] |= (unsigned char)(1 << (i & 7));
    slot->unknown_60 = 0.0f;
    slot->timer = 0.0f;
    slot->unknown_5c = -1.0f;
    slot->unknown_58 = 0.0f;
    slot->scale = 0.25f;
    slot->pos[0] = ((const float *)matrix)[12];
    slot->pos[1] = ((const float *)matrix)[13];
    slot->pos[2] = ((const float *)matrix)[14];
    for (k = 0; k < 16; k++) {
        slot->matrix[k] = matrix[k];
    }
}

void weapon_oil_pick_slot_forward(const int *matrix)
{
    weapon_oil_pick_slot_n(matrix);
}

/* entry thunk at the original address of weapon_oil_pick_slot: register inputs (eax) -> natural C weapon_oil_pick_slot_n; ret 0 */
__declspec(naked) void weapon_oil_pick_slot(void)
{
    __asm {
        push eax
        call weapon_oil_pick_slot_n
        add esp, 4
        ret
    }
}
