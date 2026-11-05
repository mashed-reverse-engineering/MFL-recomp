/* rwa_dxaudio_output_def_init_005cb2d0 (0x005cb2d0): DirectSound output object definition: id, object size / alignment, callbacks */
#include "mfl_tier2.h"

/*005ba000*/ extern void RwaObjDefAssignID(void *def, const void *uuid, int a3);
/*005bf560*/ extern int RwaFindLSB(RwUInt32 value);
/*0062e590*/ extern signed char _rwMsbBit[];      /* 256 entries: 1-based index of the highest set bit of a byte (0 for 0) */
/*005f835c*/ extern undefined g_sound_5f835c;     /* the output definition uuid */
/*005cb570*/ extern void gap_5cb570(void);        /* create (open) */
/*005cbb20*/ extern void gap_5cbb20(void);        /* destroy */
/*005cbb90*/ extern void gap_5cbb90(void);        /* finish execute */
/*005cc0d0*/ extern void gap_5cc0d0(void);        /* get creation parameters */

/* RWA's alignment-shift macros, expanded the way RWA's source writes them: every use of ALIGN_POW2 calls RwaFindLSB again.
   ALIGN_POW2 = the lowest set bit of 0x28 as a 16-bit power of two (1 when that is 0); LOG2_32 = index of its highest set bit. */
#define ALIGN_BIT()   ((RwUInt16)(1 << RwaFindLSB(0x28)))
#define ALIGN_POW2()  (ALIGN_BIT() ? ALIGN_BIT() : 1)
#define LOG2_16(y)    (((y) & 0xff00) ? _rwMsbBit[(y) >> 8] + 7 : _rwMsbBit[(y)] - 1)
#define LOG2_32(x)    (((x) & 0xffff0000) ? LOG2_16((x) >> 16) + 16 : LOG2_16(x))

/* object size 0xf8 (alignment shift in the top nibble of +0x40), create / destroy / finish-execute / get-params callbacks */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_dxaudio_output_def_init_005cb2d0(char *def)
{
  RwaObjDefAssignID(def, &g_sound_5f835c, 0);
  *(RwUInt32 *)(def + 0x40) = 0xf8;
  *(RwUInt32 *)(def + 0x40) |= (RwUInt32)LOG2_32(ALIGN_POW2()) << 28;
  *(void **)(def + 0x28) = (void *)gap_5cb570;
  *(void **)(def + 0x34) = (void *)gap_5cbb20;
  *(void **)(def + 0x30) = (void *)gap_5cbb90;
  *(void **)(def + 0x3c) = (void *)gap_5cc0d0;
  *(RwUInt16 *)(def + 0x44) = 0x28;
  *(RwUInt16 *)(def + 0x46) = 1;
}
