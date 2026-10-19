// cluster-entries: rocket_explode
/* Installed cluster rocket_explode (tools/cluster_install.py from output/cluster_pilot8/rocket_explode.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster rocket_explode: a rocket ends: two sparks bursts at the position (moved towards the aux camera by 0.2), the rocket's clump is
   removed, an explosion with random strength is queued at the rocket position and the rocket's voice is stopped. */
typedef struct Rocket {
    char unknown_00[0x0c];
    float position[3];     /* 0x0c */
    char unknown_18[0x24];
    int voice;             /* 0x3c */
} Rocket;

extern float g_car_62ac74;       /* up axis */
extern void point_lerp_to_aux_camera(const float *point, float *out, float t);
extern float rp_random_float_range(float low, float high);
extern void fx_emitter_spawn_b_default(const float *position, const float *axis, float angle, float scale);
extern void explosion_queue(const float *explosion, float a, float b, float c, int kind);
extern void sfx_voice_is_active(int sound, int voice);

static void weapon_fx_clump_remove_b_r(Rocket * p0);
extern void weapon_fx_clump_remove_b(void);
/* was: EAX = rocket */
void rocket_explode_n(Rocket *rocket)
{
    float spark_position[3];
    struct {
        float position[3];
        float radius;
    } explosion;

    point_lerp_to_aux_camera(rocket->position, spark_position, 0.2f);
    fx_emitter_spawn_b_default(spark_position, &g_car_62ac74, 360.0f, rp_random_float_range(1.0f, 1.25f));
    fx_emitter_spawn_b_default(spark_position, &g_car_62ac74, 360.0f, rp_random_float_range(1.5f, 2.0f));
    weapon_fx_clump_remove_b_r(rocket);
    explosion.position[0] = rocket->position[0];
    explosion.position[1] = rocket->position[1];
    explosion.position[2] = rocket->position[2];
    explosion.radius = 1.9f;
    explosion_queue(explosion.position, rp_random_float_range(35.0f, 45.0f), rp_random_float_range(150.0f, 250.0f), rp_random_float_range(200.0f, 300.0f), 2);
    sfx_voice_is_active(0x14, rocket->voice);
}

/* weapon_fx_clump_remove_b_r: C -> original weapon_fx_clump_remove_b (registers esi) */
__declspec(naked) void weapon_fx_clump_remove_b_r(Rocket * p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call weapon_fx_clump_remove_b
        pop esi
        ret
    }
}

/* entry thunk at the original address of rocket_explode: register inputs (eax) -> natural C rocket_explode_n; ret 0 */
__declspec(naked) void rocket_explode(void)
{
    __asm {
        push eax
        call rocket_explode_n
        add esp, 4
        ret
    }
}
