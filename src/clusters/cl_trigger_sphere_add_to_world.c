// cluster-entries: trigger_sphere_add_to_world trigger_sphere_register_in_grid
/* Installed cluster trigger_sphere_add_to_world (tools/cluster_install.py from output/cluster_pilot5/trigger_sphere_add_to_world.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster trigger_sphere_add_to_world: an 11x11 grid of trigger worlds (0x1b8 bytes each) over the course; a sphere is registered in the cell of its centre
   and in the neighbouring cells its radius touches.  Worlds and spheres link each other. */
typedef struct TriggerWorld TriggerWorld;
typedef struct TriggerSphere {
    float x;                   /* 0x00 */
    float y;
    float z;                   /* 0x08 */
    float radius;              /* 0x0c: in course units */
    int owner;                 /* 0x10: owner id, a world with spheres of two owners gets `mixed` */
    char unknown_14[0x10];
    TriggerWorld *worlds[16];  /* 0x24 */
    int world_count;           /* 0x64 */
} TriggerSphere;

struct TriggerWorld {
    TriggerSphere *spheres[100];   /* 0x000 */
    int sphere_count;              /* 0x190 */
    char unknown_194[0x18];
    int owner;                     /* 0x1ac: owner id of the first sphere */
    int mixed;                     /* 0x1b0: spheres of more than one owner */
    int full;                      /* 0x1b4: no more spheres accepted */
};                                 /* 0x1b8 bytes */

extern TriggerWorld g_trigger_sphere_7036e0[121];
extern float *g_trigger_sphere_7106d8;
extern float k_f_24_75;
extern float k_f_0_02020202;
extern float k_f_11;
extern float k_f_0_22222222_5e06d8;
extern double __cdecl floor(double x);

/* was: EAX = world, ECX = sphere.  Returns 1 when the sphere was added. */
int trigger_sphere_add_to_world_n(TriggerWorld *world, TriggerSphere *sphere)
{
    if (world->full != 0 || world->sphere_count >= 100) {
        return 0;
    }
    world->spheres[world->sphere_count] = sphere;
    world->sphere_count++;
    if (world->mixed == 0) {
        if (world->owner == 0) {
            world->owner = sphere->owner;
        }
        if (world->owner != sphere->owner) {
            world->mixed = 1;
            world->owner = sphere->owner;
        }
    }
    sphere->worlds[sphere->world_count] = world;
    sphere->world_count++;
    return 1;
}

int trigger_sphere_register_in_grid(TriggerSphere *sphere)
{
    float *centre = g_trigger_sphere_7106d8;
    float a, b, sa, sb, radius;
    int cx, cz, x0, x1, z0, z1, x, z;

    if (centre == 0) {
        return 0;
    }
    a = (k_f_24_75 + centre[0]) - sphere->x;
    b = (k_f_24_75 + centre[2]) - sphere->z;
    sa = k_f_0_02020202 * a * k_f_11;
    cx = (int)floor((double)sa);
    sb = k_f_0_02020202 * b * k_f_11;
    cz = (int)floor((double)sb);
    if (cx < 0 || cx >= 11 || cz < 0 || cz >= 11) {
        return 0;
    }
    if (trigger_sphere_add_to_world_n(&g_trigger_sphere_7036e0[cx * 11 + cz], sphere) == 0) {
        return 0;
    }
    radius = sphere->radius * k_f_0_22222222_5e06d8;
    x0 = (int)(sa - radius);
    x1 = (int)(sa + radius);
    z0 = (int)(sb - radius);
    z1 = (int)(radius + sb);
    if (x0 < 0) x0 = 0;
    if (x1 > 10) x1 = 10;
    if (z0 < 0) z0 = 0;
    if (z1 > 10) z1 = 10;
    for (x = x0; x <= x1; x++) {
        for (z = z0; z <= z1; z++) {
            if (x != cx || z != cz) {
                trigger_sphere_add_to_world_n(&g_trigger_sphere_7036e0[x * 11 + z], sphere);
            }
        }
    }
    return 1;
}

/* entry thunk at the original address of trigger_sphere_add_to_world: register inputs (eax,ecx) -> natural C trigger_sphere_add_to_world_n; ret 0 */
__declspec(naked) void trigger_sphere_add_to_world(void)
{
    __asm {
        push ecx
        push ecx
        push eax
        call trigger_sphere_add_to_world_n
        add esp, 8
        pop ecx
        ret
    }
}
