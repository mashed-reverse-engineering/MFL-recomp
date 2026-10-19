// cluster-entries: weapon_pickup_slot_clear_b weapon_pickups_clear
/* Installed cluster weapon_pickup_slot_clear_b (tools/cluster_install.py from output/cluster_pilot/weapon_pickup_slot_clear_b.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster weapon_pickup_slot_clear_b: clears all weapon pickup slots (4 small ones, 32 large ones with a clump in the world). */
typedef struct WeaponPickupSmall {
    char unknown_00[0x1c];
} WeaponPickupSmall;

typedef struct WeaponPickup {
    char unknown_00[4];
    int field_04;
    char unknown_08[0x3c];
    int field_44;
    RpClump *clump;        /* model, removed from its world on clear */
    int field_4c;
    char unknown_50[4];
    int field_54;
    char unknown_58[0xb8];
} WeaponPickup;            /* 0x110 bytes */

extern WeaponPickupSmall g_weapon_6a29b8[4];
extern WeaponPickup g_weapon_6a2a28[32];
extern RpWorld *FUN_004f4920(RpClump *clump);      /* RpClumpGetWorld */
extern RpWorld *RpWorldRemoveClump(RpWorld *world, RpClump *clump);

static void weapon_pickup_slot_clear_a_r(WeaponPickupSmall * p0);
extern void weapon_pickup_slot_clear_a(void);
/* Resets the fields of one slot and removes its clump from the world it is in (was: ESI = slot). */
void weapon_pickup_slot_clear_b_n(WeaponPickup *slot)
{
    RpWorld *world;

    slot->field_04 = 0;
    slot->field_4c = 0;
    slot->field_44 = 0;
    slot->field_54 = 0;
    world = FUN_004f4920(slot->clump);
    if (world != 0) {
        RpWorldRemoveClump(world, slot->clump);
    }
}

/* Clears every weapon pickup slot. */
void weapon_pickups_clear(void)
{
    int i;

    for (i = 0; i < 4; i++) {
        weapon_pickup_slot_clear_a_r(&g_weapon_6a29b8[i]);
    }
    for (i = 0; i < 32; i++) {
        weapon_pickup_slot_clear_b_n(&g_weapon_6a2a28[i]);
    }
}

/* weapon_pickup_slot_clear_a_r: C -> original weapon_pickup_slot_clear_a (registers esi) */
__declspec(naked) void weapon_pickup_slot_clear_a_r(WeaponPickupSmall * p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call weapon_pickup_slot_clear_a
        pop esi
        ret
    }
}

/* entry thunk at the original address of weapon_pickup_slot_clear_b: register inputs (esi) -> natural C weapon_pickup_slot_clear_b_n; ret 0 */
__declspec(naked) void weapon_pickup_slot_clear_b(void)
{
    __asm {
        push esi
        call weapon_pickup_slot_clear_b_n
        add esp, 4
        ret
    }
}
