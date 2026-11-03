/* rwa_unit244790_005bb430 (MFL.exe 0x005bb430): RenderWare Audio (Criterion Software), neutral (codfh sub_244850)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_244790.c, function sub_244850), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_244850 -> rwa_unit244790_005bb430), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bb430 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern void *memcpy(void *dst, const void *src, RwUInt32 size);

RwaWave *
rwa_unit244790_005bb430(RwaWave *wave, const RwaWaveFormat *format, RwUInt32 offset, void *block)
{
    memcpy(block, wave->buffer + offset, format->length);

    return wave;
}
