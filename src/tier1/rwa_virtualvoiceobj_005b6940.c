/* rwa_virtualvoiceobj_005b6940 (MFL.exe 0x005b6940): RenderWare Audio (Criterion Software), neutral (codfh sub_23B7B0)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwavirtualvoiceobj.c, function sub_23B7B0), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_23B7B0 -> rwa_virtualvoiceobj_005b6940; sub_23E8A0 -> rwa_voicemanager_005b7860), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005b6940 (tools/cmpfn.py, relocations masked). */
#include <string.h>
#include "rwa.h"
#include "rwaint.h"
extern void rwa_voicemanager_005b7860(rwaEmitter *emitter);

rwaEmitter *
rwa_virtualvoiceobj_005b6940(rwaEmitter *emitter, const RwaUUID *param, const RwUInt32 *flags)
{
    rwaSound   *sound = &emitter->sound;

    memset(sound, 0, sizeof(rwaSound));
    sound->prev = NULL;
    sound->next = NULL;

    rwa_voicemanager_005b7860(emitter);

    sound->flags = *flags & 0x18;
    sound->unk40 = 0x3F800000;
    sound->unk70 = 0x3F800000;
    sound->unk44 = 0;
    sound->unk54 = 0;
    sound->unk48 = 0;
    sound->unk4C = 0;
    sound->unk80 = 0;
    sound->handle = NULL;
    sound->unk84 = 0;
    sound->unk78 = 0x3F000000;
    sound->unk7C = 0x3F000000;
    sound->unk3C = 22050.0f;
    sound->unk74 = 0x461C4000;

    return emitter;
}
