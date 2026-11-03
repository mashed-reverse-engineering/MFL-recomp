/* rwa_virtualvoiceobj_005b69e0 (MFL.exe 0x005b69e0): RenderWare Audio (Criterion Software), neutral (codfh sub_23B850)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwavirtualvoiceobj.c, function sub_23B850), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (SoundStopper_Transition -> rwa_voicemanager_005b7eb0; sub_23B850 -> rwa_virtualvoiceobj_005b69e0; sub_23F960 -> RwaObjGetParamUInt32), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005b69e0 (tools/cmpfn.py, relocations masked). */
#include <string.h>
#include "rwa.h"
#include "rwaint.h"
extern void rwa_voicemanager_005b7eb0(rwaSound *sound, RwInt32 state);
extern RwInt32 RwaObjGetParamUInt32(RwaObjHandle *handle, RwInt32 param, RwInt32 type, RwInt32 value);

rwaEmitter *
rwa_virtualvoiceobj_005b69e0(rwaEmitter *emitter)
{
    rwaSound   *sound = &emitter->sound;

    if (sound->flags & 2)
    {
        RwInt32 state = 2;

        if (sound->handle)
        {
            if (!RwaObjGetParamUInt32(sound->handle, 6, 2, 0))
            {
                state = 1;
            }
        }

        rwa_voicemanager_005b7eb0(sound, state);
    }

    sound->unk84 = 0;

    return emitter;
}
