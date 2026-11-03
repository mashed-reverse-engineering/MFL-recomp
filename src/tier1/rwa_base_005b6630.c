/* rwa_base_005b6630 (MFL.exe 0x005b6630): RenderWare Audio (Criterion Software), neutral (codfh sub_23E320)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwabase.c, function sub_23E320), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_23E220 -> rwa_base_005b6560; sub_23E320 -> rwa_base_005b6630), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005b6630 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern void nullsub_6(void);
void rwa_base_005b6560(void);
extern void rwa_base_005b6560(void);

void
rwa_base_005b6630(rwaModule *module, RwUInt32 index)
{
    if (module->close == rwa_base_005b6560)
    {
        nullsub_6();
    }
}
