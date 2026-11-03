/* rwa_unit242910_005bcbf0 (MFL.exe 0x005bcbf0): RenderWare Audio (Criterion Software), neutral (codfh sub_2429D0)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_242910.c, function sub_2429D0), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_2429D0 -> rwa_unit242910_005bcbf0), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bcbf0 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"

RwBool
rwa_unit242910_005bcbf0(const RwaWaveFormat *format1, const RwaWaveFormat *format2)
{
    if (!RwaUUIDCompare(format1->dataType, format2->dataType) &&
        format1->sampleRate == format2->sampleRate &&
        format1->bitDepth == format2->bitDepth &&
        format1->noChannels == format2->noChannels &&
        !((format1->flags ^ format2->flags) & ~2))
    {
        return TRUE;
    }

    return FALSE;
}
