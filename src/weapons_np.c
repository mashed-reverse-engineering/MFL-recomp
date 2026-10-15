/* MFL.exe weapons_np: functions moved verbatim from game_globals.c, weapons.c by tools/reorg.py (shared declarations: mfl_externs.h). */
#include "mfl_types.h"
#include "mfl_externs.h"

// FUNCTION: MFL 0x00460360
int weapon_slot_6a5d24_get(int slot)
{
    return g_weapon_6a5d24[slot * 6];
}

// FUNCTION: MFL 0x00461620
int rocket_slot_6a6000_get(int slot)
{
    return *(int *)(g_rocket_6a6000 + slot * 0x2c);
}

// FUNCTION: MFL 0x004603c0
int weapon_flashbang_is_finished(struct WeaponSlot *w)
{
    return *(int *)(w->state + 0xc) == 4;
}

// FUNCTION: MFL 0x00460400
float weapon_flashbang_ammo_fraction(struct WeaponSlot *w)
{
    if (*(int *)(w->state + 0xc) == 1)
        return 1.0f;
    return 0.0f;
}

// FUNCTION: MFL 0x004622c0
int weapon_machinegun_is_finished(struct WeaponSlot *w)
{
    return MachinegunState_ARRAY_006a60d0[w->player].ammo <= 0;
}

// FUNCTION: MFL 0x00462330
float weapon_machinegun_ammo_fraction(struct WeaponSlot *w)
{
    return (float)MachinegunState_ARRAY_006a60d0[w->player].ammo * 0.02f;
}

// FUNCTION: MFL 0x00463710
int weapon_drum_is_finished(struct WeaponSlot *w)
{
    return *(int *)(w->state + 8) == 0;
}

// FUNCTION: MFL 0x00463730
void weapon_drum_unequip(struct WeaponSlot *w)
{
    unsigned int s = w->state;
    *(int *)(s + 0x10) = 2;
    *(int *)(s + 0xc) = 0;
}

// FUNCTION: MFL 0x004603e0
void weapon_flashbang_unequip(struct WeaponSlot *w)
{
    unsigned int s = w->state;
    *(int *)(s + 0xc) = 5;
    *(int *)(s + 8) = 0;
}

// FUNCTION: MFL 0x00467110
void weapon_shotgun_unequip(struct WeaponSlot *w)
{
    unsigned int s = w->state;
    *(int *)(s + 0x18) = 2;
    *(int *)(s + 0x14) = 0;
}

// FUNCTION: MFL 0x004670e0
int weapon_shotgun_is_finished(struct WeaponSlot *w)
{
    if (*(int *)(w->state + 0x10) == 0 && *(int *)(w->state + 0xc) == 0)
        return 1;
    return 0;
}

// FUNCTION: MFL 0x00464dd0
int powerup_set_respawn_time(int index, float seconds)
{
    if (index >= 0 && index < 0x19) {
        struct Powerup *p = &g_powerups[index];
        p->respawn_time = seconds;
        return 1;
    }
    return 0;
}

int powerup_set_enabled(int index, int enabled)
{
    if (index >= 0 && index < 0x19) {
        struct Powerup *p = &g_powerups[index];
        if (enabled == 0) {
            p->state = 3;
        } else if (p->state == 3) {
            p->state = 1;
            p->respawn_timer = 0.0f;
            return 1;
        }
        p->respawn_timer = 0.0f;
        return 1;
    }
    return 0;
}
