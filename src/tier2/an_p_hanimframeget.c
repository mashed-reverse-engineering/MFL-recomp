/* rwframe_plugin_object_get (MFL.exe 00544440): RpHAnimFrameGetHierarchy, the HAnim frame plugin's hierarchy pointer (the word at +4
 * of the frame's HAnim extension; the extension offset is the first word of RpHAnimAtomicGlobals, 0x947ac8).  Written from the MFL.exe
 * disassembly (lane P W8); the recomp name stays the one recomp/aliases.txt and cl_gap_4c47c0.c use.  Compared with MFL.exe by
 * differential emulation (tier 2). */
#include "rw37_slim.h"

typedef struct rpHAnimFrameExtension rpHAnimFrameExtension;
struct rpHAnimFrameExtension
{
    RwInt32           id;
    RpHAnimHierarchy *hierarchy;
};
RW_LAYOUT_ASSERT(p_hfx_0, RW_OFFSETOF(rpHAnimFrameExtension, hierarchy) == 4);      /* MFL.exe 0x54444a [ecx+eax+4] */

/* the first word of RpHAnimAtomicGlobals: the frame plugin offset of the HAnim extension */
extern RwInt32 RpHAnimAtomicGlobals;
#define RPHANIMFRAMEGETEXT(frame) ((rpHAnimFrameExtension *)(((RwUInt8 *)(frame)) + RpHAnimAtomicGlobals))

/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RpHAnimHierarchy * rwframe_plugin_object_get(RwFrame *frame)
{
    return (RPHANIMFRAMEGETEXT(frame)->hierarchy);
}
