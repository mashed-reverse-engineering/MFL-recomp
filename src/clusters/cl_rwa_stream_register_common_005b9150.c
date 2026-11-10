// cluster-entries: rwa_stream_register_common_005b9150 rwa_stream_register_005b9440
/* Installed cluster rwa_stream_register_common_005b9150 (tools/cluster_install.py from output/cluster_pilot_a4/rwa_stream_register_common_005b9150.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* RenderWare Audio stream object definition setup (MFL.exe 0x005b9150, rwa.dll unit 23FAD0): _RwaStreamRegisterCommon (register function:
   definition in eax, entered through the generated thunk) fills the stream definition (uuid, state size 0xd78, state alignment 0x28 as
   log2 in the top nibble of +0x40, the create / unlink / execute / query callbacks, 0xc parameter bytes), empties the active stream list,
   then opens the container module and starts the stream thread with its lock and exit semaphore (undone on failure);
   RwaStreamRegister (0x005b9440, its caller) creates the definition.  Written from the MFL.exe disassembly (tools/disasm_va.py). */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwBool __stdcall CloseHandle(void *handle);

typedef struct RwaLink { struct RwaLink *next, *prev; } RwaLink;

/*005ba000*/ extern void *RwaObjDefAssignID(void *def, const void *uuid, const void *textID);
/*005ba5c0*/ extern void *RwaObjDefFindByUUID();      /* (parent, uuid, out): 3 slots read; declared `()` because cl_sound_card_info_log.c
                                                          over-declares it with 4 (defectlint cross-file arity), as in cl_rwa_soft_voice_def_init_005d8920.c */
/*005ba8d0*/ extern void *RwaObjDefCreate(void *parent, RwInt32 a2, void *def, void *a4, void *a5, RwUInt32 hint);
/*005bf560*/ extern RwInt32 RwaFindLSB(RwUInt32 value);
/*005bfc50*/ extern RwBool FUN_005bfc50(void);      /* stream container module open */
/*005bfcf0*/ extern void FUN_005bfcf0(void);        /* stream container module close */
/*005c4340*/ extern RwBool RwaSemaphoreCreate(void **handle, RwInt32 initial, RwInt32 maximum);
/*005c43a0*/ extern RwBool _RwaThreadCreate(void *thread, void *func, RwUInt32 priority, void *data);
/*005c4400*/ extern RwBool _RwaThreadStart(void *thread, RwInt32 a2);
/*005b8680*/ extern char *rwa_stream_container_attach_005b8680(char *stream, char *media, RwUInt32 *params);
/*005b8740*/ extern RwBool rwa_stream_unlink_005b8740(char *stream);
/*005b8880*/ extern char *rwa_stream_execute_005b8880(char *stream);
/*005b8960*/ extern void *rwa_unit23fad0_005b8960(void *obj, RwUInt32 *params);      /* query (rwa.dll similarity name _RwaDSUnknownInit is a mismatch: not DirectSound) */
/*005b87b0*/ extern RwUInt32 rwa_stream_update_thread_005b87b0(void *data);

/*005f766c*/ extern const RwUInt8 g_data_5f766c[];      /* stream definition uuid */
/*005f835c*/ extern const RwUInt8 g_sound_5f835c[];     /* parent definition uuid */
/*0062e590*/ extern const signed char g_data_62e590[];  /* bit index table of the log2 macro */
/*00649614*/ extern RwUInt32 g_data_649614;             /* stream thread priority */
/*007dac00*/ extern void *rwastream_objDefRegistered;   /* stream list lock */
/*007dac04*/ extern RwInt32 g_data_7dac04;              /* stream thread quit request */
/*007dac08*/ extern RwUInt8 g_data_7dac08[];            /* stream thread */
/*007dac24*/ extern RwaLink g_data_7dac24;              /* active stream list head (next, prev at 7dac28) */
/*007dac48*/ extern RwUInt8 g_data_7dac48[];            /* stream definition storage */
/*007daca4*/ extern void *g_data_7daca4;                /* stream thread exit semaphore */

#define DEF_FLAGS(d)    (*(RwUInt32 *)((char *)(d) + 0x40))
/* the RWA alignment macros, kept as macros: the original evaluates RwaFindLSB once per expansion of the argument */
#define RWA_POW2(n)          ((RwUInt16)(1 << RwaFindLSB(n)))
#define RWA_POW2_MIN1(n)     (RWA_POW2(n) ? RWA_POW2(n) : 1)
#define RWA_LOG2(v)          (((v) & 0xffff0000) ? (((v) & 0xff000000) ? g_data_62e590[(v) >> 24] + 7 + 16 : g_data_62e590[(v) >> 16] - 1 + 16) \
                                                 : (((v) & 0xff00) ? g_data_62e590[(v) >> 8] + 7 : g_data_62e590[v] - 1))
#define STREAM_STATE_ALIGN 0x28

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void rwa_stream_register_common_005b9150_n(void *def)
{
  RwaObjDefAssignID(def, g_data_5f766c, 0);
  DEF_FLAGS(def) = 0xd78;      /* state size */
  DEF_FLAGS(def) |= (RwUInt32)RWA_LOG2(RWA_POW2_MIN1(STREAM_STATE_ALIGN)) << 28;
  *(void **)((char *)def + 0x28) = (void *)rwa_stream_container_attach_005b8680;      /* create */
  *(void **)((char *)def + 0x34) = (void *)rwa_stream_unlink_005b8740;               /* unlink */
  *(void **)((char *)def + 0x30) = (void *)rwa_stream_execute_005b8880;              /* execute */
  *(void **)((char *)def + 0x3c) = (void *)rwa_unit23fad0_005b8960;                  /* query */
  *(RwUInt16 *)((char *)def + 0x44) = 0xc;                                            /* parameter bytes */
  g_data_7dac24.next = &g_data_7dac24;
  g_data_7dac24.prev = &g_data_7dac24;

  if (!FUN_005bfc50())
    return;
  if (RwaSemaphoreCreate(&rwastream_objDefRegistered, 1, 1)) {
    if (RwaSemaphoreCreate(&g_data_7daca4, 0, 1)) {
      g_data_7dac04 = 0;
      if (_RwaThreadCreate(g_data_7dac08, (void *)rwa_stream_update_thread_005b87b0, g_data_649614, 0) && _RwaThreadStart(g_data_7dac08, 0))
        return;
      CloseHandle(g_data_7daca4);
    }
    CloseHandle(rwastream_objDefRegistered);
  }
  FUN_005bfcf0();
}

/* RwaStreamRegister: create the stream object definition under its parent and set it up */
/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *rwa_stream_register_005b9440(void)
{
  void *def = RwaObjDefCreate(RwaObjDefFindByUUID(0, g_sound_5f835c, 0), 0, g_data_7dac48, 0, 0, 0x3080e);

  if (!def)
    return 0;
  rwa_stream_register_common_005b9150_n(def);
  return def;
}

/* entry thunk at the original address of rwa_stream_register_common_005b9150: register inputs (eax) -> natural C rwa_stream_register_common_005b9150_n; ret 0 */
__declspec(naked) void rwa_stream_register_common_005b9150(void)
{
    __asm {
        push eax
        call rwa_stream_register_common_005b9150_n
        add esp, 4
        ret
    }
}
