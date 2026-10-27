// cluster-entries: hudpanel_load_assets_0041d0e0
/* Installed cluster hudpanel_load_assets_0041d0e0 (tools/cluster_install.py from output/cluster_pilot_pg/hudpanel_load_assets_0041d0e0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0040d0d0*/ extern void *__cdecl find_texture_dict_b(const char *name);
/*0041d070*/ extern void __cdecl load_smoke_texture(void);
/*0042f780*/ extern void *__cdecl clump_load_by_name(const char *name, int unused_a, int unused_b);
/*0042f860*/ extern void *__cdecl texdict_load_named_b(const char *name, int unused_a, int unused_b);
/*00486f10*/ extern void __cdecl particle_system_setup_ptank(unsigned int *ptank, unsigned int max_particles, unsigned int flags, void *texture);
/*004c3f60*/ extern void *__cdecl clump_first_atomic(void *clump);
/*004d0c50*/ extern void *__cdecl RwFrameCreate(void);
/*004d5e10*/ extern void *__cdecl RwTexDictionarySetCurrent(void *dict);
/*004f6ae0*/ extern void *__cdecl RpAtomicClone(void *atomic);
/*004f7f70*/ extern void *__cdecl RpAtomicSetFrame(void *atomic, void *frame);
/* hudpanel_load_assets_0041d0e0 */

#define HUDPANEL_TARGET_COUNT 2        /* "target" marker clumps (record stride 0x54) */
#define HUDPANEL_MISSILE_COUNT 4       /* cloned exocet missile atomics */
#define HUDPANEL_SLOT_COUNT 4          /* HUD panel slots (record stride 0x6c) */
#define HUDPANEL_PTANK_PARTICLES 0x80c /* argument of particle_system_setup_ptank (inferred: particle capacity) */
#define HUDPANEL_PTANK_FLAGS 8

typedef struct HudTargetRecord { unsigned char bytes[0x54]; } HudTargetRecord;

/* one missile record; the atomic pointer sits behind 0x40 unknown bytes (the symbol g_load_658300 labels it) */
typedef struct HudMissileRecord {
    unsigned char unknown_00[0x40];
    void *atomic;                  /* 0x40: RpAtomic clone of the exocet missile */
    unsigned char unknown_44[0x1c];
    int index;                     /* 0x60 */
} HudMissileRecord;

typedef struct HudPanelSlotRecord {
    unsigned char unknown_00[0x50];
    int index;                     /* 0x50 */
    unsigned char unknown_54[0x14];
    HudMissileRecord *missile;     /* 0x68 */
} HudPanelSlotRecord;

extern void *g_hudpanel_load_6582b8;
extern HudMissileRecord g_load_6582c0[HUDPANEL_MISSILE_COUNT];
extern HudTargetRecord g_hudpanel_658450[HUDPANEL_TARGET_COUNT];
extern unsigned int g_hudpanel_6584f8[];
extern HudPanelSlotRecord g_hudpanel_658538[HUDPANEL_SLOT_COUNT];
extern void *g_load_658b28;
extern void *g_load_658b2c;

/* Loads the weapon HUD assets: the shockwave / crosshair textures, the exocet missile clump (cloned once per missile record,
   each clone with its own frame), the airstrike texture dictionary, one "target" clump per target record, resets the HUD panel
   slots and finally loads the smoke texture. */
static void hudpanel_slot_init_r(void * p0);
extern void hudpanel_slot_init(void);
static void hudpanel_slot_reset_r(void * p0);
extern void hudpanel_slot_reset(void);
static void hudpanel_slot_attach_clump_r(void * p0, void * p1, int p2);
extern void hudpanel_slot_attach_clump(void);
void hudpanel_load_assets_0041d0e0(void)
{
    void *shockwave;
    void *missile_atomic;
    int i;

    shockwave = find_texture_dict_b("shockwave");
    particle_system_setup_ptank(g_hudpanel_6584f8, HUDPANEL_PTANK_PARTICLES, HUDPANEL_PTANK_FLAGS, shockwave);
    g_hudpanel_load_6582b8 = find_texture_dict_b("crosshair2");
    if (g_hudpanel_load_6582b8 == 0) {
        g_hudpanel_load_6582b8 = find_texture_dict_b("crosshair");
    }
    g_load_658b28 = clump_load_by_name("exocet.dff", 0, 0);
    g_load_658b2c = texdict_load_named_b("airstrike", 0, 0);
    RwTexDictionarySetCurrent(g_load_658b2c);
    for (i = 0; i < HUDPANEL_TARGET_COUNT; i++) {
        hudpanel_slot_attach_clump_r(clump_load_by_name("target", 0, 0), &g_hudpanel_658450[i], i);
    }
    missile_atomic = clump_first_atomic(g_load_658b28);
    for (i = 0; i < HUDPANEL_MISSILE_COUNT; i++) {
        g_load_6582c0[i].atomic = RpAtomicClone(missile_atomic);
        RpAtomicSetFrame(g_load_6582c0[i].atomic, RwFrameCreate());
        g_load_6582c0[i].index = i;
    }
    for (i = 0; i < HUDPANEL_SLOT_COUNT; i++) {
        hudpanel_slot_init_r(&g_hudpanel_658538[i]);
        hudpanel_slot_reset_r(&g_hudpanel_658538[i]);
        g_hudpanel_658538[i].index = i;
        g_hudpanel_658538[i].missile = &g_load_6582c0[i];
    }
    load_smoke_texture();
}

/* hudpanel_slot_init_r: C -> original hudpanel_slot_init (registers esi) */
__declspec(naked) void hudpanel_slot_init_r(void * p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call hudpanel_slot_init
        pop esi
        ret
    }
}

/* hudpanel_slot_reset_r: C -> original hudpanel_slot_reset (registers eax) */
__declspec(naked) void hudpanel_slot_reset_r(void * p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call hudpanel_slot_reset
        ret
    }
}

/* hudpanel_slot_attach_clump_r: C -> original hudpanel_slot_attach_clump (registers eax,ebx,s0) */
__declspec(naked) void hudpanel_slot_attach_clump_r(void * p0, void * p1, int p2)
{
    __asm {
        push ebx
        push dword ptr [esp + 16]
        mov eax, dword ptr [esp + 12]
        mov ebx, dword ptr [esp + 16]
        call hudpanel_slot_attach_clump
        add esp, 4
        pop ebx
        ret
    }
}
