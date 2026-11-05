// cluster-entries: rwa_soft_voice_def_init_005d8920 rwa_soft_voice_def_create_005d8b60 rwa_soft_voice_def_register_005d8ba0
/* Installed cluster rwa_soft_voice_def_init_005d8920 (tools/cluster_install.py from output/cluster_pilot_au5/rwa_soft_voice_def_init_005d8920.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* Software mixer voice object definition (RWA soft mixer): the definition filler (register function, definition in esi)
   and its two callers, the create callbacks of the plain and the registered variant (entries of the register tables).
   Cluster: rwa_soft_voice_def_init_005d8920 is entered through the generated esi thunk. */
#include "mfl_tier2.h"

/*005ba000*/ extern void RwaObjDefAssignID(void *def, const void *uuid, int a3);
/*005bf560*/ extern int RwaFindLSB(RwUInt32 value);
/*005ba5c0*/ extern void *sub_23DCA0();      /* RwaObjDefFindByUUID(a1, uuid, out): 3 slots read; declared `()` because
                                                cl_sound_card_info_log.c over-declares it with 4 (defectlint cross-file arity) */
/*005ba8d0*/ extern void *sub_23DFB0(void *parent, int nParams, void *a3, void *a4, void *a5, RwUInt32 a6);      /* RwaObjDefCreate */
/*005ba620*/ extern int _rwaObjDefSetupParams(void *def, const void *params, int nParams, const void *methods, int nMethods);
/*005da990*/ extern void *rwa_soft_dsp_interface_register_005da990(void);
/*005b9fd0*/ extern void RwaObjDefSetInterface(void *def, void *intf);
/*005ba420*/ extern void RwaObjDefDestroy(void *def);
/*005d8170*/ extern char *rwa_soft_voice_init_005d8170(char *voice, char *params);
/*005d8880*/ extern void rwa_soft_voice_remove_005d8880(void);
/*005d82b0*/ extern char *rwa_soft_voice_update_005d82b0(char *voice);
/*0062e590*/ extern signed char _rwMsbBit[];      /* 256 entries: 1-based index of the highest set bit of a byte (0 for 0) */
/*005f8f5c*/ extern undefined g_rwa_soft_voice_uuid;
/*005f8f6c*/ extern undefined g_rwa_soft_mixer_uuid;
/*007dc2d0*/ extern undefined g_rwa_soft_voice_def_7dc2d0;
/*007dc2c8*/ extern undefined g_rwa_soft_voice_def_7dc2c8;
/*007dc32c*/ extern undefined g_rwa_soft_voice_def_7dc32c;
/*0064c290*/ extern undefined g_rwa_soft_voice_param_table;      /* 13 parameter descriptors */
/*0064c3c8*/ extern undefined g_rwa_soft_voice_method_table;     /* 14 methods */
/*007dc344*/ extern int g_rwa_soft_mixer_registered;

#define RWA_DEF_FLAGS 0x3080d

/* RWA's alignment-shift macros, expanded the way RWA's source writes them: every use of ALIGN_POW2 calls RwaFindLSB again
   (the original calls it up to six times).  ALIGN_POW2 = the lowest set bit of the voice alignment 0x28 as a 16-bit
   power of two (1 when that is 0); LOG2_32 = index of its highest set bit. */
#define ALIGN_BIT()   ((RwUInt16)(1 << RwaFindLSB(0x28)))
#define ALIGN_POW2()  (ALIGN_BIT() ? ALIGN_BIT() : 1)
#define LOG2_16(y)    (((y) & 0xff00) ? _rwMsbBit[(y) >> 8] + 7 : _rwMsbBit[(y)] - 1)
#define LOG2_32(x)    (((x) & 0xffff0000) ? LOG2_16((x) >> 16) + 16 : LOG2_16(x))

/* object size 0xb0, alignment 8 (shift in the top nibble of +0x40), init / remove / update callbacks of the voice */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_soft_voice_def_init_005d8920_n(char *def)
{
  RwaObjDefAssignID(def, &g_rwa_soft_voice_uuid, 0);
  *(RwUInt16 *)(def + 0x46) = 8;
  *(RwUInt32 *)(def + 0x40) = 0xb0;
  *(RwUInt32 *)(def + 0x40) |= (RwUInt32)LOG2_32(ALIGN_POW2()) << 28;
  *(void **)(def + 0x28) = (void *)rwa_soft_voice_init_005d8170;
  *(void **)(def + 0x34) = (void *)rwa_soft_voice_remove_005d8880;
  *(void **)(def + 0x2c) = (void *)rwa_soft_voice_update_005d82b0;
  *(RwUInt16 *)(def + 0x44) = 0;
}

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void *rwa_soft_voice_def_create_005d8b60(void)
{
  char *def = (char *)sub_23DFB0(sub_23DCA0(0, &g_rwa_soft_mixer_uuid, 0), 0, &g_rwa_soft_voice_def_7dc2d0, 0, 0, RWA_DEF_FLAGS);

  if (!def)
    return 0;
  rwa_soft_voice_def_init_005d8920_n(def);
  return def;
}

/* the same with the 13 parameters / 14 methods and the dsp interface; sets g_rwa_soft_mixer_registered */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void *rwa_soft_voice_def_register_005d8ba0(void)
{
  char *def = (char *)sub_23DFB0(sub_23DCA0(0, &g_rwa_soft_mixer_uuid, 0), 0xc, &g_rwa_soft_voice_def_7dc2d0,
                                 &g_rwa_soft_voice_def_7dc32c, &g_rwa_soft_voice_def_7dc2c8, RWA_DEF_FLAGS);
  void *intf;

  if (!def)
    return 0;
  rwa_soft_voice_def_init_005d8920_n(def);
  if (_rwaObjDefSetupParams(def, &g_rwa_soft_voice_param_table, 0xd, &g_rwa_soft_voice_method_table, 0xe) &&
      (intf = rwa_soft_dsp_interface_register_005da990()) != 0) {
    RwaObjDefSetInterface(def, intf);
    g_rwa_soft_mixer_registered = 1;
    return def;
  }
  RwaObjDefDestroy(def);
  return 0;
}

/* entry thunk at the original address of rwa_soft_voice_def_init_005d8920: register inputs (esi) -> natural C rwa_soft_voice_def_init_005d8920_n; ret 0 */
__declspec(naked) void rwa_soft_voice_def_init_005d8920(void)
{
    __asm {
        push esi
        call rwa_soft_voice_def_init_005d8920_n
        add esp, 4
        ret
    }
}
