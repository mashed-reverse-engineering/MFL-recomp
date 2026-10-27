// cluster-entries: fx_reset_slot
/* Installed cluster fx_reset_slot (tools/cluster_install.py from output/cluster_pilot8/fx_reset_slot.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster fx_reset_slot: per-frame logic of an effect slot (a thrown / mounted weapon effect): clears the two velocity vectors; while the
   fuse (timer_a) is short the charge counts down and, once the line of sight to the target is free, explodes (bomb_explode); otherwise it
   rolls a random chance against the line of sight to start the fuse. */
typedef struct FxEffectSlot {
    char unknown_00[0x70];
    float chance;          /* 0x70: random range 0..10 is compared against this */
    float timer_a;         /* 0x74: fuse, restarts at 1.4 */
    float timer_b;         /* 0x78: charge, restarts at 0.05 */
    float velocity_a[3];   /* 0x7c */
    float velocity_b[3];   /* 0x88 */
} FxEffectSlot;

extern float random_range(float low, float high);
extern float k_f_0_05;
extern float k_f_1_4;
extern float k_f_0;
extern float g_tick_seconds;

static void bomb_explode_r(FxEffectSlot * p0, float * p1);
extern void bomb_explode(void);
static int line_of_sight_between_r(FxEffectSlot * p0, float * p1, float p2, float p3);
extern void line_of_sight_between(void);
/* was: ECX = slot, EAX = target position */
void fx_reset_slot_n(FxEffectSlot *slot, float *target)
{
    slot->velocity_a[2] = 0.0f;
    slot->velocity_a[1] = 0.0f;
    slot->velocity_a[0] = 0.0f;
    slot->velocity_b[2] = 0.0f;
    slot->velocity_b[1] = 0.0f;
    slot->velocity_b[0] = 0.0f;
    if (slot->timer_a >= k_f_1_4) {      /* a NaN timer takes the fuse branch below (original: test ah,1; jne) */
        if (line_of_sight_between_r(slot, target, 15.0f, 45.0f) != 0) {
            if (random_range(0.0f, 10.0f) < slot->chance)
                slot->timer_a = slot->timer_a - g_tick_seconds;
        }
    } else {
        if (slot->timer_b >= k_f_0_05) {
            if (line_of_sight_between_r(slot, target, 15.0f, 45.0f) != 0) {
                bomb_explode_r(slot, target);
                slot->timer_b = slot->timer_b - g_tick_seconds;
            }
        } else {
            slot->timer_b = slot->timer_b - g_tick_seconds;
            if (slot->timer_b <= k_f_0)
                slot->timer_b = 0.05f;
        }
        slot->timer_a = slot->timer_a - g_tick_seconds;
        if (slot->timer_a <= k_f_0)
            slot->timer_a = 1.4f;
    }
}

/* bomb_explode_r: C -> original bomb_explode (registers esi,s0) */
__declspec(naked) void bomb_explode_r(FxEffectSlot * p0, float * p1)
{
    __asm {
        push esi
        push dword ptr [esp + 12]
        mov esi, dword ptr [esp + 12]
        call bomb_explode
        add esp, 4
        pop esi
        ret
    }
}

/* line_of_sight_between_r: C -> original line_of_sight_between (registers eax,edi,s0,s1) */
__declspec(naked) int line_of_sight_between_r(FxEffectSlot * p0, float * p1, float p2, float p3)
{
    __asm {
        push edi
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        mov eax, dword ptr [esp + 16]
        mov edi, dword ptr [esp + 20]
        call line_of_sight_between
        add esp, 8
        pop edi
        ret
    }
}

/* entry thunk at the original address of fx_reset_slot: register inputs (ecx,eax) -> natural C fx_reset_slot_n; ret 0 */
__declspec(naked) void fx_reset_slot(void)
{
    __asm {
        push eax
        push ecx
        call fx_reset_slot_n
        add esp, 8
        ret
    }
}
