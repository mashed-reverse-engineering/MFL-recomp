// cluster-entries: rwa_soft_dsp_def_init_005da1a0 rwa_soft_dsp_def_register_005da3f0
/* Installed cluster rwa_soft_dsp_def_init_005da1a0 (tools/cluster_install.py from output/cluster_pilot_au5/rwa_soft_dsp_def_init_005da1a0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* Software dsp node object definition (RWA soft mixer): the definition filler (register function, definition in esi) and
   its caller, the create callback of the registered definition (an entry of the register tables).
   Cluster: rwa_soft_dsp_def_init_005da1a0 is entered through the generated esi thunk. */
#include "mfl_tier2.h"

/*005ba000*/ extern void RwaObjDefAssignID(void *def, const void *uuid, int a3);
/*005bf560*/ extern int RwaFindLSB(RwUInt32 value);
/*005ba5c0*/ extern void *sub_23DCA0();      /* RwaObjDefFindByUUID(a1, uuid, out): 3 slots read; declared `()` because
                                                cl_sound_card_info_log.c over-declares it with 4 (defectlint cross-file arity) */
/*005ba8d0*/ extern void *sub_23DFB0(void *parent, int nParams, void *a3, void *a4, void *a5, RwUInt32 a6);      /* RwaObjDefCreate */
/*005ba620*/ extern int _rwaObjDefSetupParams(void *def, const void *params, int nParams, const void *methods, int nMethods);
/*005bda80*/ extern void *rwa_unit243b40_005bda80(const void *uuid);
/*005b9fd0*/ extern void RwaObjDefSetInterface(void *def, void *intf);
/*005ba420*/ extern void RwaObjDefDestroy(void *def);
/*005d9440*/ extern char *rwa_soft_dsp_create_005d9440(char *dsp, void *output, char *params, void *lookup, void *arg);
/*005da0f0*/ extern void *rwa_soft_dsp_destroy_005da0f0(char *dsp, void *lookup, void *arg);
/*005d9ad0*/ extern char *rwa_soft_dsp_process_005d9ad0(char *node, void *unused);
/*005d9860*/ extern char *rwa_soft_dsp_start_execute_005d9860(char *dsp);
/*005d9880*/ extern void *rwa_soft_dsp_get_params_005d9880(void *def, char *params);
/*0062e590*/ extern signed char _rwMsbBit[];      /* 256 entries: 1-based index of the highest set bit of a byte (0 for 0) */
/*005f8f7c*/ extern undefined g_data_5f8f7c;      /* the soft dsp uuid */
/*005f835c*/ extern undefined g_sound_5f835c;     /* the parent definition's uuid */
/*005f82ec*/ extern undefined g_data_5f82ec;      /* the interface uuid */
/*0064c620*/ extern undefined g_data_64c620;
/*007dc3f0*/ extern undefined g_data_7dc3f0;
/*007dc3d0*/ extern undefined g_rwa_soft_dsp_def_7dc3d0;
/*007dc3e8*/ extern undefined g_rwa_soft_dsp_def_7dc3e8;
/*0064c638*/ extern undefined g_rwa_soft_dsp_param_table;      /* 5 parameter descriptors */
/*0064c6b0*/ extern undefined g_rwa_soft_dsp_method_table;     /* 4 methods */

#define RWA_DEF_FLAGS 0x3080e

/* RWA's alignment-shift macros, expanded the way RWA's source writes them: every use of ALIGN_POW2 calls RwaFindLSB again.
   ALIGN_POW2 = the lowest set bit of 0x28 as a 16-bit power of two (1 when that is 0); LOG2_32 = index of its highest set bit. */
#define ALIGN_BIT()   ((RwUInt16)(1 << RwaFindLSB(0x28)))
#define ALIGN_POW2()  (ALIGN_BIT() ? ALIGN_BIT() : 1)
#define LOG2_16(y)    (((y) & 0xff00) ? _rwMsbBit[(y) >> 8] + 7 : _rwMsbBit[(y)] - 1)
#define LOG2_32(x)    (((x) & 0xffff0000) ? LOG2_16((x) >> 16) + 16 : LOG2_16(x))

/* object size 0x234 (alignment shift in the top nibble of +0x40), create / destroy / start / process / get-params callbacks */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_soft_dsp_def_init_005da1a0_n(char *def)
{
  RwaObjDefAssignID(def, &g_data_5f8f7c, 0);
  *(RwUInt32 *)(def + 0x40) = 0x234;
  *(RwUInt32 *)(def + 0x40) |= (RwUInt32)LOG2_32(ALIGN_POW2()) << 28;
  *(void **)(def + 0x28) = (void *)rwa_soft_dsp_create_005d9440;
  *(void **)(def + 0x34) = (void *)rwa_soft_dsp_destroy_005da0f0;
  *(void **)(def + 0x2c) = (void *)rwa_soft_dsp_start_execute_005d9860;
  *(void **)(def + 0x30) = (void *)rwa_soft_dsp_process_005d9ad0;
  *(void **)(def + 0x3c) = (void *)rwa_soft_dsp_get_params_005d9880;
  *(RwUInt16 *)(def + 0x44) = 0x30;
  *(void **)(def + 0x54) = &g_data_64c620;
}

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void *rwa_soft_dsp_def_register_005da3f0(void)
{
  char *def = (char *)sub_23DFB0(sub_23DCA0(0, &g_sound_5f835c, 0), 0xc, &g_data_7dc3f0, &g_rwa_soft_dsp_def_7dc3d0,
                                 &g_rwa_soft_dsp_def_7dc3e8, RWA_DEF_FLAGS);

  if (!def)
    return 0;
  rwa_soft_dsp_def_init_005da1a0_n(def);
  if (!_rwaObjDefSetupParams(def, &g_rwa_soft_dsp_param_table, 5, &g_rwa_soft_dsp_method_table, 4)) {
    RwaObjDefDestroy(def);
    return 0;
  }
  RwaObjDefSetInterface(def, rwa_unit243b40_005bda80(&g_data_5f82ec));
  return def;
}

/* entry thunk at the original address of rwa_soft_dsp_def_init_005da1a0: register inputs (esi) -> natural C rwa_soft_dsp_def_init_005da1a0_n; ret 0 */
__declspec(naked) void rwa_soft_dsp_def_init_005da1a0(void)
{
    __asm {
        push esi
        call rwa_soft_dsp_def_init_005da1a0_n
        add esp, 4
        ret
    }
}
