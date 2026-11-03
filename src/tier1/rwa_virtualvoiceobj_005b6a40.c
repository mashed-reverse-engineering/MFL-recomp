/* rwa_virtualvoiceobj_005b6a40 (MFL.exe 0x005b6a40): RenderWare Audio (Criterion Software), neutral (codfh SoundEmitter_Stop)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwavirtualvoiceobj.c, function SoundEmitter_Stop), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (SoundEmitter_Stop -> rwa_virtualvoiceobj_005b6a40; SoundStopper_Transition -> rwa_voicemanager_005b7eb0), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005b6a40 (tools/cmpfn.py, relocations masked). */
#include <string.h>
#include "rwa.h"
#include "rwaint.h"
extern RwUInt32 dword_5069F8;
extern void rwa_voicemanager_005b7eb0(rwaSound *sound, RwInt32 state);
void rwa_virtualvoiceobj_005b6a40(rwaEmitter *emitter, RwInt32 now);

void
rwa_virtualvoiceobj_005b6a40(rwaEmitter *emitter, RwInt32 now)
{
    if (now)
    {
        rwa_voicemanager_005b7eb0(&emitter->sound, 0);
        emitter->sound.unk54 = dword_5069F8;
    }
    else
    {
        rwa_voicemanager_005b7eb0(&emitter->sound, 1);
    }
}
