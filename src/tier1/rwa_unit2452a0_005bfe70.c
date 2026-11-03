/* rwa_unit2452a0_005bfe70 (MFL.exe 0x005bfe70): RenderWare Audio (Criterion Software), neutral (codfh sub_245380)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_2452A0.c, function sub_245380), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_245380 -> rwa_unit2452a0_005bfe70), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bfe70 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"

/* Calls func on each source. */
void
rwa_unit2452a0_005bfe70(rwaStreamPlayer *player, rwaContainerNodeFunc func, void *data)
{
    rwaLink    *link = player->sources.next;

    while (link != &player->sources)
    {
        rwaDecoder *node = rwaDecoderFromLink(link);

        link = link->next;
        func(node, data);
    }
}
