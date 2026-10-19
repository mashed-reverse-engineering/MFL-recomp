// cluster-entries: weapon_mortar_register_trigger weapon_mortar_update
/* Installed cluster weapon_mortar_register_trigger (tools/cluster_install.py from output/cluster_pilot6/weapon_mortar_register_trigger.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/* DECLS-END */
/* Cluster weapon_mortar_register_trigger: the mortar update.  The 4 launcher slots (7 words each, from 0x6a29b8) get their matrix applied to the launcher
   frame and look for targets in a cone, the 32 mortar shells (0x110 bytes each, from 0x6a2a28) fly and register a trigger sphere at their position. */
typedef struct TriggerSphereDesc {
    float pos[3];              /* 0x00 */
    float radius;              /* 0x0c */
    int type;                  /* 0x10: 0x13 mortar shell */
    void *owner;               /* 0x14 */
    int unknown_18;            /* 0x18 */
    void (*callback)(void);    /* 0x1c: hit callback (original code) */
} TriggerSphereDesc;

typedef struct MortarLauncher {
    RwMatrix *matrix;          /* 0x00 */
    int value;                 /* 0x04: weapon_effect_get_slot_value_b */
    int active;                /* 0x08 */
    int unknown_0c;
    float timer;               /* 0x10 */
    struct { int unknown; RwFrame *frame; } *frame_a;     /* 0x14: the RwFrame is at +4 */
    struct { int unknown; RwFrame *frame; } *frame_b;     /* 0x18 */
} MortarLauncher;

extern MortarLauncher g_weapon_mortar_6a29b8[4];
extern char g_weapon_6a2a28[];
extern float g_weapon_62ac80[];
extern float k_f_0;
extern float g_tick_seconds;
extern void LAB_0045e8b0(void);
extern int trigger_sphere_register(TriggerSphereDesc *desc);
extern int weapon_effect_get_slot_value_b(int slot);
extern void weapon_find_targets_in_cone(int slot, float *position, float a, float b, unsigned int *mask);

static void weapon_mortar_shell_update_r(char * p0);
extern void weapon_mortar_shell_update(void);
/* was: EAX = mortar shell */
void weapon_mortar_register_trigger_n(char *shell)
{
    TriggerSphereDesc desc;

    desc.pos[0] = *(float *)(shell + 0x14);
    desc.pos[1] = *(float *)(shell + 0x18);
    desc.pos[2] = *(float *)(shell + 0x1c);
    desc.radius = 0.05f;
    desc.unknown_18 = 0;
    desc.callback = LAB_0045e8b0;
    desc.type = 0x13;
    desc.owner = shell;
    trigger_sphere_register(&desc);
}

void weapon_mortar_update(void)
{
    int i;
    char *shell;
    unsigned int mask;
    MortarLauncher *launcher = g_weapon_mortar_6a29b8;

    for (i = 0; i < 4; i++, launcher++) {
        mask = 0xffffff00;
        if (launcher->active != 0) {
            launcher->value = weapon_effect_get_slot_value_b(i);
            RwFrameTransform(launcher->frame_a->frame, launcher->matrix, rwCOMBINEREPLACE);
            weapon_find_targets_in_cone(i, (float *)((char *)launcher->matrix + 0x30), 15.0f, 20.0f, &mask);
            if (k_f_0 < launcher->timer) {
                RwFrameRotate(launcher->frame_b->frame, (RwV3d *)g_weapon_62ac80, 30.0f, rwCOMBINEPRECONCAT);
                launcher->timer = launcher->timer - g_tick_seconds;
            }
        }
    }
    shell = g_weapon_6a2a28;
    do {
        if (*(int *)(shell + 4) != 0) {
            weapon_mortar_shell_update_r(shell);
            weapon_mortar_register_trigger_n(shell);
        }
        shell = shell + 0x110;
    } while (shell < g_weapon_6a2a28 + 0x2200);
}

/* weapon_mortar_shell_update_r: C -> original weapon_mortar_shell_update (registers eax) */
__declspec(naked) void weapon_mortar_shell_update_r(char * p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call weapon_mortar_shell_update
        ret
    }
}

/* entry thunk at the original address of weapon_mortar_register_trigger: register inputs (eax) -> natural C weapon_mortar_register_trigger_n; ret 0 */
__declspec(naked) void weapon_mortar_register_trigger(void)
{
    __asm {
        push eax
        call weapon_mortar_register_trigger_n
        add esp, 4
        ret
    }
}
