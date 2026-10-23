// cluster-entries: weapon_effects_particles_update
/* Installed cluster weapon_effects_particles_update (tools/cluster_install.py from output/cluster_pilot_dtrig/weapon_effects_particles_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern double sin(double);
/*00465250*/ extern void *material_uv_scroll_update(void *atomic, void *data);
/*004c56c0*/ extern void FUN_004c56c0(void *clump, unsigned char *colour_out);
/*004f6810*/ extern void *RpClumpForAllAtomics(void *clump, void *callback, void *data);
/*00475760*/ extern float *aux_camera_get_position(int viewport);
/*004867f0*/ extern void particles_set_position(float *xyz);
/*00486820*/ extern void particles_set_size(float *size_xy);
/*00486840*/ extern void particles_set_color(unsigned char *rgba);
/*00486b50*/ extern void particles_add(void *particle_batch);
/*00486c40*/ extern void particles_flush(void *particle_batch);

/* One glowing weapon effect (88 bytes). */
typedef struct WeaponEffectSlot {
    int reserved0[4];
    int active;             /* 0: slot unused */
    int glow_visible;       /* non-zero: draw the pulsing glow sprites */
    int reserved18;
    float scale;
    int reserved20;
    float position[3];      /* world position of the glow */
    float pulse_phase;      /* radians, advances 0.1 per update */
    unsigned char colour[4];/* rgb written by FUN_004c56c0 from the clump */
    int reserved38[7];
    void *clump;
} WeaponEffectSlot;

#define WEAPON_EFFECT_COUNT 4
/*006a9420*/ extern WeaponEffectSlot g_weapon_6a9420[WEAPON_EFFECT_COUNT];
/*006a93e0*/ extern char g_particles_6a93e0[];

#define WEAPON_PULSE_STEP 0.1f              /* phase advance per update */
#define WEAPON_GLOW_SIZE_BASE 0.4f          /* glow size = base + amplitude * sin(phase) */
#define WEAPON_GLOW_SIZE_AMPLITUDE 0.05f
#define WEAPON_GLOW_TOWARDS_CAMERA 0.1f     /* the first sprite sits this fraction of the way to the aux camera */
#define WEAPON_CORE_SIZE_SCALE 0.3f         /* the second, white core sprite is smaller and moved on by the same fraction again */
#define WEAPON_CORE_ALPHA 200

/* Per active weapon effect: pulses its phase, scales the effect model by the camera distance, tints it, and when it
   glows adds two sprites (coloured glow, smaller white core) shifted towards the aux camera; finally refreshes the
   model's material UV scroll. */
static void weapon_effect_scale_by_camera_distance_r(struct WeaponEffectSlot * p0);
extern void weapon_effect_scale_by_camera_distance(void);
void weapon_effects_particles_update(void)
{
    unsigned char colour[4];
    float size[2];
    struct { float position[3]; float towards_camera[3]; } frame;   /* one frame object: keeps the rounded float temporaries in memory */
    float *camera_position;
    int i;

    for (i = WEAPON_EFFECT_COUNT - 1; i >= 0; i--) {
        WeaponEffectSlot *slot = &g_weapon_6a9420[i];

        if (slot->active == 0)
            continue;
        slot->pulse_phase = slot->pulse_phase + WEAPON_PULSE_STEP;
        weapon_effect_scale_by_camera_distance_r(slot);
        FUN_004c56c0(slot->clump, slot->colour);
        if (slot->glow_visible != 0) {
            colour[0] = slot->colour[0];
            colour[1] = slot->colour[1];
            colour[2] = slot->colour[2];
            colour[3] = 255;
            particles_set_color(colour);
            size[0] = (float)(sin(slot->pulse_phase) * WEAPON_GLOW_SIZE_AMPLITUDE + WEAPON_GLOW_SIZE_BASE);
            size[1] = size[0];
            particles_set_size(size);

            frame.position[0] = slot->position[0];
            frame.position[1] = slot->position[1];
            frame.position[2] = slot->position[2];
            camera_position = aux_camera_get_position(0);
            frame.towards_camera[0] = camera_position[0] - frame.position[0];
            camera_position = aux_camera_get_position(0);
            frame.towards_camera[1] = camera_position[1] - frame.position[1];
            camera_position = aux_camera_get_position(0);
            frame.towards_camera[0] = frame.towards_camera[0] * WEAPON_GLOW_TOWARDS_CAMERA;
            frame.towards_camera[1] = frame.towards_camera[1] * WEAPON_GLOW_TOWARDS_CAMERA;
            frame.towards_camera[2] = (camera_position[2] - frame.position[2]) * WEAPON_GLOW_TOWARDS_CAMERA;
            frame.position[0] = frame.towards_camera[0] + frame.position[0];
            frame.position[1] = frame.towards_camera[1] + frame.position[1];
            frame.position[2] = frame.towards_camera[2] + frame.position[2];
            particles_set_position(frame.position);
            particles_add(g_particles_6a93e0);

            size[0] = size[0] * WEAPON_CORE_SIZE_SCALE;
            size[1] = size[1] * WEAPON_CORE_SIZE_SCALE;
            particles_set_size(size);
            colour[0] = 255;
            colour[1] = 255;
            colour[2] = 255;
            colour[3] = WEAPON_CORE_ALPHA;
            particles_set_color(colour);
            frame.towards_camera[2] = frame.towards_camera[2] * WEAPON_GLOW_TOWARDS_CAMERA;
            frame.position[0] = frame.towards_camera[0] * WEAPON_GLOW_TOWARDS_CAMERA + frame.position[0];
            frame.position[1] = frame.towards_camera[1] * WEAPON_GLOW_TOWARDS_CAMERA + frame.position[1];
            frame.position[2] = frame.towards_camera[2] + frame.position[2];
            particles_set_position(frame.position);
            particles_add(g_particles_6a93e0);
        }
        RpClumpForAllAtomics(slot->clump, material_uv_scroll_update, 0);
    }
    particles_flush(g_particles_6a93e0);
}

/* weapon_effect_scale_by_camera_distance_r: C -> original weapon_effect_scale_by_camera_distance (registers edi) */
__declspec(naked) void weapon_effect_scale_by_camera_distance_r(struct WeaponEffectSlot * p0)
{
    __asm {
        push edi
        mov edi, dword ptr [esp + 8]
        call weapon_effect_scale_by_camera_distance
        pop edi
        ret
    }
}
