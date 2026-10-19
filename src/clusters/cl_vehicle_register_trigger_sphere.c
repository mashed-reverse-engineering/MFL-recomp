// cluster-entries: vehicle_register_trigger_sphere
/* Installed cluster vehicle_register_trigger_sphere (tools/cluster_install.py from output/cluster_pilot6/vehicle_register_trigger_sphere.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster vehicle_register_trigger_sphere: registers the trigger sphere of a vehicle (type 0xff00 + vehicle id) at its position. */
typedef struct TriggerSphereDesc {
    float pos[3];              /* 0x00 */
    float radius;              /* 0x0c */
    int type;                  /* 0x10 */
    void *owner;               /* 0x14 */
    int slot;                  /* 0x18 */
    void (*callback)(void);    /* 0x1c */
} TriggerSphereDesc;

extern int trigger_sphere_register(TriggerSphereDesc *desc);

/* was: EAX = vehicle record */
void vehicle_register_trigger_sphere_n(char *vehicle)
{
    TriggerSphereDesc desc;

    desc.pos[0] = *(float *)(vehicle + 0x258);
    desc.pos[1] = *(float *)(vehicle + 0x25c);
    desc.pos[2] = *(float *)(vehicle + 0x260);
    desc.type = *(int *)(vehicle + 0x290) + 0xff00;
    desc.radius = *(float *)(vehicle + 0x264);
    desc.owner = 0;
    desc.slot = 0;
    desc.callback = 0;
    trigger_sphere_register(&desc);
}

/* entry thunk at the original address of vehicle_register_trigger_sphere: register inputs (eax) -> natural C vehicle_register_trigger_sphere_n; ret 0 */
__declspec(naked) void vehicle_register_trigger_sphere(void)
{
    __asm {
        push eax
        call vehicle_register_trigger_sphere_n
        add esp, 4
        ret
    }
}
