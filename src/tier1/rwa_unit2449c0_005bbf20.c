/* rwa_unit2449c0_005bbf20 (MFL.exe 0x005bbf20): RenderWare Audio (Criterion Software), neutral (codfh sub_245130)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_2449C0.c, function sub_245130), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_245130 -> rwa_unit2449c0_005bbf20), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bbf20 (tools/cmpfn.py, relocations masked).
 * The callee at 0x005bfbf0 is the audio stream file close (waits for the stream event, then calls the real RwFclose 0x00560e70);
 * codfh calls it RwFclose, here rwaFileClose_005bfbf0: the name RwFclose belongs to the RenderWare function (lib_names.txt / mflrw.dll). */
#include "rwa.h"
#include "rwaint.h"
extern RwInt32 rwaFileClose_005bfbf0(void *file);
extern void sub_244F60(rwaCacheClient *client);

void
rwa_unit2449c0_005bbf20(rwaCacheClient *client)
{
    sub_244F60(client);

    if (client->holder->type == 2)
    {
        rwaFileClose_005bfbf0(client->holder->file);
    }
}
