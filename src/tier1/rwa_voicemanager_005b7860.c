/* rwa_voicemanager_005b7860 (MFL.exe 0x005b7860): RenderWare Audio (Criterion Software), neutral (codfh sub_23E8A0)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwavoicemanager.c, function sub_23E8A0), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_23E8A0 -> rwa_voicemanager_005b7860), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005b7860 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern rwaLink dword_5069C8;    /* free voices */
#define rwaVoiceLink(_sound) ((rwaLink *)(_sound))

RwBool
rwa_voicemanager_005b7860(rwaEmitter *emitter)
{
    rwaSound   *sound = &emitter->sound;

    rwaVoiceLink(sound)->prev = NULL;
    rwaVoiceLink(sound)->next = NULL;
    rwaVoiceLink(sound)->next = dword_5069C8.next;
    rwaVoiceLink(sound)->prev = &dword_5069C8;
    dword_5069C8.next->prev = rwaVoiceLink(sound);
    dword_5069C8.next = rwaVoiceLink(sound);

    sound->flags &= ~1;

    return TRUE;
}
