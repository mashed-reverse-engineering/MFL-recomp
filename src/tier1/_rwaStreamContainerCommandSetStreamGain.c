/* _rwaStreamContainerCommandSetStreamGain (MFL.exe 0x005c1b30): RenderWare Audio (Criterion Software), rwa.dll 0.922
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_246D50.c, function sub_246ED0), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_246ED0 -> _rwaStreamContainerCommandSetStreamGain), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c1b30 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"

/* Sets value for the entries of the given track, channel and kind (-1:
 * any; kinds 0 to 3). */
/* Debug source-flow evidence: rwad.dll _rwaStreamContainerCommandSetStreamGain,
 * 0x100C04BA; void return and range initialization before the entry loop.
 * The gain arrives as raw 32-bit data, as declared by its caller in unit_23FAD0. */
void
_rwaStreamContainerCommandSetStreamGain(rwaContainerCommand *command, const rwaNodeSet *set, RwInt32 track, RwInt32 channel,
           RwInt32 kind, RwUInt32 value)
{
    RwInt32 trackStart = 0;
    RwInt32 kindStart = 0;
    RwInt32 kindEnd = 4;
    RwInt32 trackEnd;
    RwUInt32 i;

    trackEnd = set->numNodes;

    if (track != -1)
    {
        trackStart = track;
        trackEnd = trackStart + 1;
    }
    if (kind != -1)
    {
        kindStart = kind;
        kindEnd = kindStart + 1;
    }

    for (i = 0; i < set->numEntries; i++)
    {
        RwInt32     index = command->entries[i].track;

        index = (index & 0x80000000) ? index & ~0x80000000 : index;
        if (index >= trackStart && index < trackEnd
            && (RwInt32)command->entries[i].kind >= kindStart
            && (RwInt32)command->entries[i].kind < kindEnd
            && ((RwInt32)command->entries[i].channel == channel || channel < 0))
        {
            command->entries[i].value = *(RwReal *)&value;
            command->entries[i].track = index | 0x80000000;
        }
    }

    command->flags |= 0x10;
}
