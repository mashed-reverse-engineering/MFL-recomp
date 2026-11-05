/* rwa_dxaudio_voice_def_init_005c99f0 (0x005c99f0): DirectSound voice object definition: id, object size / alignment, callbacks */
#include "mfl_tier2.h"

/*005ba000*/ extern void RwaObjDefAssignID(void *def, const void *uuid, int a3);
/*005bf560*/ extern int RwaFindLSB(RwUInt32 value);
/*0062e590*/ extern signed char _rwMsbBit[];      /* 256 entries: 1-based index of the highest set bit of a byte (0 for 0) */
/*005f834c*/ extern undefined g_data_5f834c;      /* the voice definition uuid */
/*005c9c80*/ extern void *rwa_dxaudio_voice_init_005c9c80(void *v, char *outputObj, RwUInt32 *params, int a4, int a5);
/*005c9f50*/ extern void FUN_005c9f50(void);      /* destroy */
/*005c9fb0*/ extern void *rwa_dxaudio_voice_finish_execute_005c9fb0(void *voice);
/*005ca640*/ extern void gap_5ca640(void);        /* get creation parameters */

/* RWA's alignment-shift macros, expanded the way RWA's source writes them: every use of ALIGN_POW2 calls RwaFindLSB again.
   ALIGN_POW2 = the lowest set bit of 0x28 as a 16-bit power of two (1 when that is 0); LOG2_32 = index of its highest set bit. */
#define ALIGN_BIT()   ((RwUInt16)(1 << RwaFindLSB(0x28)))
#define ALIGN_POW2()  (ALIGN_BIT() ? ALIGN_BIT() : 1)
#define LOG2_16(y)    (((y) & 0xff00) ? _rwMsbBit[(y) >> 8] + 7 : _rwMsbBit[(y)] - 1)
#define LOG2_32(x)    (((x) & 0xffff0000) ? LOG2_16((x) >> 16) + 16 : LOG2_16(x))

/* object size 0x100 (alignment shift in the top nibble of +0x40), create / destroy / finish-execute / get-params callbacks */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_dxaudio_voice_def_init_005c99f0(char *def)
{
  RwaObjDefAssignID(def, &g_data_5f834c, 0);
  *(RwUInt32 *)(def + 0x40) = 0x100;
  *(RwUInt32 *)(def + 0x40) |= (RwUInt32)LOG2_32(ALIGN_POW2()) << 28;
  *(void **)(def + 0x28) = (void *)rwa_dxaudio_voice_init_005c9c80;
  *(void **)(def + 0x34) = (void *)FUN_005c9f50;
  *(void **)(def + 0x30) = (void *)rwa_dxaudio_voice_finish_execute_005c9fb0;
  *(void **)(def + 0x3c) = (void *)gap_5ca640;
  *(RwUInt16 *)(def + 0x44) = 4;
}
