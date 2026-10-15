/* MFL.exe weapons: functions moved verbatim from batch11.c, batch21.c by tools/reorg.py (shared declarations: mfl_externs.h). */
#pragma auto_inline(off)
#include "mfl_types.h"
#include "mfl_externs.h"
extern char g_weapon_shotgun_6aac2c[];

// FUNCTION: MFL 0x00460ca0
int weapon_rocket_is_finished(struct WeaponSlot *w)
{
    struct RocketState *r = &g_rocket_states[w->player];
    if (r->active != 0)
        return 0;
    return r->ammo <= 0;
}

// FUNCTION: MFL 0x00465f60
char *weapon_effect_get_slot_record(int i)
{
    if (i >= 0 && i < 4)
        return g_weapon_6a9444 + i * 0x58;
    return 0;
}

// FUNCTION: MFL 0x00466710
int weapon_flamethrower_is_finished(struct WeaponSlot *w)
{
    return *(int *)((char *)g_weapon_6a9588 + w->player * 0x68) == 0;
}

// FUNCTION: MFL 0x00467150
void weapon_shotgun_free_assets(void)
{
    int *p = g_weapon_shotgun_6aab9c;
    do {
        RpClumpDestroy(*p);
        p += 9;
    } while ((int)p < ((int)&g_weapon_shotgun_6aac2c));
    RpClumpDestroy(g_weapon_shotgun_6aac10);
}

// FUNCTION: MFL 0x00467570
void weapon_shotgun_fire(struct WeaponSlot *w, int phase)
{
    int st = w->state;
    if (phase == 2 && *(int *)(st + 0x10) != 0)
        weapon_shotgun_fire_shot(st);
}
