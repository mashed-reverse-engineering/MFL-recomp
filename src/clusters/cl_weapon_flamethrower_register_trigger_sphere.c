// cluster-entries: weapon_flamethrower_register_trigger_sphere
/* Installed cluster weapon_flamethrower_register_trigger_sphere (tools/cluster_install.py from output/cluster_pilot6/weapon_flamethrower_register_trigger_sphere.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster weapon_flamethrower_register_trigger_sphere: registers the trigger sphere (type 0x10, radius 0.15) of a flame particle at its position. */
typedef struct TriggerSphereDesc {
    float pos[3];              /* 0x00 */
    float radius;              /* 0x0c */
    int type;                  /* 0x10 */
    void *owner;               /* 0x14 */
    int slot;                  /* 0x18 */
    void (*callback)(void);    /* 0x1c: hit callback (original code) */
} TriggerSphereDesc;

extern void LAB_00466270(void);
extern int trigger_sphere_register(TriggerSphereDesc *desc);

/* was: EAX = position, the slot index is the one stack argument */
void weapon_flamethrower_register_trigger_sphere_n(const float *position, int slot)
{
    TriggerSphereDesc desc;

    desc.pos[0] = position[0];
    desc.pos[1] = position[1];
    desc.pos[2] = position[2];
    desc.radius = 0.15f;
    desc.owner = 0;
    desc.slot = slot;
    desc.type = 0x10;
    desc.callback = LAB_00466270;
    trigger_sphere_register(&desc);
}

/* entry thunk at the original address of weapon_flamethrower_register_trigger_sphere: register inputs (eax,s0) -> natural C weapon_flamethrower_register_trigger_sphere_n; ret 0 */
__declspec(naked) void weapon_flamethrower_register_trigger_sphere(void)
{
    __asm {
        push dword ptr [esp + 4]
        push eax
        call weapon_flamethrower_register_trigger_sphere_n
        add esp, 8
        ret
    }
}
