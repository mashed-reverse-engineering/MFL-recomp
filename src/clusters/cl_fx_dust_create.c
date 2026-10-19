// cluster-entries: fx_dust_create fx_dust_create_default
/* Installed cluster fx_dust_create (tools/cluster_install.py from output/cluster_pilot5/fx_dust_create.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/*00486f10*/ extern /* RET? */ undefined particle_system_setup_ptank(uint *p0, uint p1, uint p2, struct RwTexture *p3);
/*00486b00*/ extern /* RET? */ undefined ptank_set_blend_modes();
/* DECLS-END */
/* Cluster fx_dust_create: creates the dust particle tank (0x80c particles) with the texture and blend modes 5 / 6. */
extern unsigned int g_fx_dust_759658[];

/* was: EAX = texture, ECX = id (low 16 bits used) */
void fx_dust_create_n(RwTexture *texture, unsigned int id)
{
    particle_system_setup_ptank(g_fx_dust_759658, 0x80c, id & 0xffff, texture);
    ptank_set_blend_modes(g_fx_dust_759658, 5, 6);
}

void fx_dust_create_default(unsigned int id, RwTexture *texture, int unused)
{
    fx_dust_create_n(texture, id);
}

/* entry thunk at the original address of fx_dust_create: register inputs (eax,ecx) -> natural C fx_dust_create_n; ret 0 */
__declspec(naked) void fx_dust_create(void)
{
    __asm {
        push ecx
        push eax
        call fx_dust_create_n
        add esp, 8
        ret
    }
}
