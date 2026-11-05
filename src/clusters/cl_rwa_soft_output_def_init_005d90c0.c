// cluster-entries: rwa_soft_output_def_init_005d90c0 rwa_soft_output_def_create_005d92d0 rwa_soft_output_def_register_005d9300
/* Installed cluster rwa_soft_output_def_init_005d90c0 (tools/cluster_install.py from output/cluster_pilot_au5/rwa_soft_output_def_init_005d90c0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* Software output object definition (RWA soft mixer): the definition filler (register function, definition in esi) and
   its two callers, the create callbacks of the plain and the registered variant (entries of the register tables).
   Cluster: rwa_soft_output_def_init_005d90c0 is entered through the generated esi thunk. */
#include "mfl_tier2.h"

/*005ba000*/ extern void RwaObjDefAssignID(void *def, const void *uuid, int a3);
/*005bf560*/ extern int RwaFindLSB(RwUInt32 value);
/*005ba8d0*/ extern void *sub_23DFB0(void *parent, int nParams, void *a3, void *a4, void *a5, RwUInt32 a6);      /* RwaObjDefCreate */
/*005ba620*/ extern int _rwaObjDefSetupParams(void *def, const void *params, int nParams, const void *methods, int nMethods);
/*005db4b0*/ extern void *rwa_soft_dsp2_interface_register_005db4b0(void);
/*005b9fd0*/ extern void RwaObjDefSetInterface(void *def, void *intf);
/*005ba420*/ extern void RwaObjDefDestroy(void *def);
/*005d8f10*/ extern void FUN_005d8f10(void);      /* the output's init callback (AUD_softOutputInit, stays original: rwatap hook) */
/*005d8e00*/ extern void rwa_soft_output_free_005d8e00(void);
/*005d8e50*/ extern void *rwa_soft_output_start_execute_005d8e50(char *output);
/*005d8e80*/ extern void gap_5d8e80(void);
/*0062e590*/ extern signed char _rwMsbBit[];      /* 256 entries: 1-based index of the highest set bit of a byte (0 for 0) */
/*005f8f6c*/ extern undefined g_rwa_soft_mixer_uuid;
/*00949d5c*/ extern void *g_data_949d5c;      /* the parent definition (RWA output base) */
/*007dc370*/ extern undefined g_rwa_soft_output_def_7dc370;
/*007dc350*/ extern undefined g_rwa_soft_output_def_7dc350;
/*007dc368*/ extern undefined g_rwa_soft_output_def_7dc368;
/*0064c518*/ extern undefined g_rwa_soft_output_param_table;      /* 6 parameter descriptors */
/*0064c5a8*/ extern undefined g_rwa_soft_output_method_table;     /* 5 methods */
/*007dc3cc*/ extern int g_rwa_soft_output_registered;

#define RWA_DEF_FLAGS 0x3080c

/* RWA's alignment-shift macros, expanded the way RWA's source writes them: every use of ALIGN_POW2 calls RwaFindLSB again.
   ALIGN_POW2 = the lowest set bit of 0x28 as a 16-bit power of two (1 when that is 0); LOG2_32 = index of its highest set bit. */
#define ALIGN_BIT()   ((RwUInt16)(1 << RwaFindLSB(0x28)))
#define ALIGN_POW2()  (ALIGN_BIT() ? ALIGN_BIT() : 1)
#define LOG2_16(y)    (((y) & 0xff00) ? _rwMsbBit[(y) >> 8] + 7 : _rwMsbBit[(y)] - 1)
#define LOG2_32(x)    (((x) & 0xffff0000) ? LOG2_16((x) >> 16) + 16 : LOG2_16(x))

/* object size 0x48 (alignment shift in the top nibble of +0x40), init / free / start-execute / +0x3c callbacks */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_soft_output_def_init_005d90c0_n(char *def)
{
  RwaObjDefAssignID(def, &g_rwa_soft_mixer_uuid, 0);
  *(RwUInt16 *)(def + 0x46) = 1;
  *(RwUInt32 *)(def + 0x40) = 0x48;
  *(RwUInt32 *)(def + 0x40) |= (RwUInt32)LOG2_32(ALIGN_POW2()) << 28;
  *(void **)(def + 0x28) = (void *)FUN_005d8f10;
  *(void **)(def + 0x34) = (void *)rwa_soft_output_free_005d8e00;
  *(void **)(def + 0x2c) = (void *)rwa_soft_output_start_execute_005d8e50;
  *(void **)(def + 0x30) = 0;
  *(void **)(def + 0x3c) = (void *)gap_5d8e80;
  *(RwUInt16 *)(def + 0x44) = 0x10;
}

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void *rwa_soft_output_def_create_005d92d0(void)
{
  char *def = (char *)sub_23DFB0(g_data_949d5c, 0, &g_rwa_soft_output_def_7dc370, 0, 0, RWA_DEF_FLAGS);

  if (!def)
    return 0;
  rwa_soft_output_def_init_005d90c0_n(def);
  return def;
}

/* the same with the 6 parameters / 5 methods and the dsp interface; sets g_rwa_soft_output_registered */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void *rwa_soft_output_def_register_005d9300(void)
{
  char *def = (char *)sub_23DFB0(g_data_949d5c, 0xc, &g_rwa_soft_output_def_7dc370, &g_rwa_soft_output_def_7dc350,
                                 &g_rwa_soft_output_def_7dc368, RWA_DEF_FLAGS);
  void *intf;

  if (!def)
    return 0;
  rwa_soft_output_def_init_005d90c0_n(def);
  if (_rwaObjDefSetupParams(def, &g_rwa_soft_output_param_table, 6, &g_rwa_soft_output_method_table, 5) &&
      (intf = rwa_soft_dsp2_interface_register_005db4b0()) != 0) {
    RwaObjDefSetInterface(def, intf);
    g_rwa_soft_output_registered = 1;
    return def;
  }
  RwaObjDefDestroy(def);
  return 0;
}

/* entry thunk at the original address of rwa_soft_output_def_init_005d90c0: register inputs (esi) -> natural C rwa_soft_output_def_init_005d90c0_n; ret 0 */
__declspec(naked) void rwa_soft_output_def_init_005d90c0(void)
{
    __asm {
        push esi
        call rwa_soft_output_def_init_005d90c0_n
        add esp, 4
        ret
    }
}
