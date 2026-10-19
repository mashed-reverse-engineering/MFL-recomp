// cluster-entries: weapon_flashbang_flare_particles weapon_flashbang_update
/* Installed cluster weapon_flashbang_flare_particles (tools/cluster_install.py from output/cluster_pilot3/weapon_flashbang_flare_particles.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/*00486b50*/ extern undefined particles_add();
/*00486c40*/ extern undefined particles_flush();
/*00486840*/ extern undefined particles_set_color();
/*004867f0*/ extern undefined particles_set_position();
/*00486820*/ extern undefined particles_set_size();
/* DECLS-END */
static undefined weapon_flashbang_state_step_r(undefined4 p0);
extern void weapon_flashbang_state_step(void);
/* Cluster weapon_flashbang_flare_particles: the light flares of a flashbang grenade (grow, then fade out) and the per-frame update of all flashbangs. */
typedef struct FlashbangFlare {
    float pos[3];          /* world position */
    int state;             /* 0 idle, 1 growing, 2 fading */
    float age;             /* seconds since the flare started */
    float fade;            /* seconds since the flare started to fade */
} FlashbangFlare;          /* 0x18 bytes */

typedef struct FlashbangState {
    char unknown_00[0x14];
} FlashbangState;          /* 0x14 bytes */

extern FlashbangFlare g_weapon_6a5d18[4];
extern FlashbangState g_weapon_6a5d78[4];
extern char g_weapon_8bf000[];      /* particle system of the flashbang */
extern float g_tick_seconds;

/* Advances one flare: grows the glow for 0.1 s, then fades it out for 2 s (was: ESI = flare). */
void weapon_flashbang_flare_particles_n(FlashbangFlare *flare)
{
    float size[2];
    unsigned char colour[4];
    float pos[3];

    if (flare->state == 1) {
        size[0] = flare->age * 10.0f;
        colour[0] = 0xff;
        colour[1] = 0xff;
        colour[2] = 0xff;
        colour[3] = 0xff;
        pos[0] = flare->pos[0];
        pos[1] = flare->pos[1] + 0.1f;
        pos[2] = flare->pos[2];
        flare->age = g_tick_seconds + flare->age;
        size[0] = size[0] * 4.0f;
        size[1] = size[0];
        particles_set_position(pos);
        particles_set_color(colour);
        particles_set_size(size);
        particles_add(g_weapon_8bf000);
        if (flare->age >= 0.1f) {
            flare->state = 2;
            flare->fade = 0.0f;
        }
    } else if (flare->state == 2) {
        flare->fade = g_tick_seconds + flare->fade;
        if (flare->fade >= 2.0f) {
            flare->state = 0;
        }
    }
}

/* Per-frame update: the state of every flashbang, its flares, then the flush of the flare particles. */
void weapon_flashbang_update(void)
{
    int i;

    for (i = 0; i < 4; i++) {
        weapon_flashbang_state_step_r(&g_weapon_6a5d78[i]);
    }
    for (i = 0; i < 4; i++) {
        weapon_flashbang_flare_particles_n(&g_weapon_6a5d18[i]);
    }
    particles_flush(g_weapon_8bf000);
}

/* weapon_flashbang_state_step_r: C -> original weapon_flashbang_state_step (registers eax) */
__declspec(naked) undefined weapon_flashbang_state_step_r(undefined4 p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call weapon_flashbang_state_step
        ret
    }
}

/* entry thunk at the original address of weapon_flashbang_flare_particles: register inputs (esi) -> natural C weapon_flashbang_flare_particles_n; ret 0 */
__declspec(naked) void weapon_flashbang_flare_particles(void)
{
    __asm {
        push esi
        call weapon_flashbang_flare_particles_n
        add esp, 4
        ret
    }
}
