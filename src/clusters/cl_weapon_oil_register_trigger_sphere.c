// cluster-entries: weapon_oil_register_trigger_sphere
/* Installed cluster weapon_oil_register_trigger_sphere (tools/cluster_install.py from output/cluster_pilot6/weapon_oil_register_trigger_sphere.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster weapon_oil_register_trigger_sphere: registers the trigger sphere (type 0x12) of an oil slick at its position. */
typedef struct TriggerSphereDesc {
    float pos[3];              /* 0x00 */
    float radius;              /* 0x0c */
    int type;                  /* 0x10 */
    void *owner;               /* 0x14 */
    int slot;                  /* 0x18 */
    void (*callback)(void);    /* 0x1c: hit callback (original code) */
} TriggerSphereDesc;

extern void LAB_00462c70(void);
extern int trigger_sphere_register(TriggerSphereDesc *desc);

/* was: EAX = oil slick */
void weapon_oil_register_trigger_sphere_n(char *slick)
{
    TriggerSphereDesc desc;

    desc.pos[0] = *(float *)(slick + 0x48);
    desc.pos[1] = *(float *)(slick + 0x4c);
    desc.pos[2] = *(float *)(slick + 0x50);
    desc.radius = *(float *)(slick + 0x14);
    desc.owner = slick;
    desc.slot = 0;
    desc.type = 0x12;
    desc.callback = LAB_00462c70;
    trigger_sphere_register(&desc);
}

/* entry thunk at the original address of weapon_oil_register_trigger_sphere: register inputs (eax) -> natural C weapon_oil_register_trigger_sphere_n; ret 0 */
__declspec(naked) void weapon_oil_register_trigger_sphere(void)
{
    __asm {
        push eax
        call weapon_oil_register_trigger_sphere_n
        add esp, 4
        ret
    }
}
