/* rwa_unit2452a0_005c0620 (MFL.exe 0x005c0620): RenderWare Audio (Criterion Software), neutral (codfh sub_245B60)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_2452A0.c, function sub_245B60), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_245B60 -> rwa_unit2452a0_005c0620), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c0620 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern RwBool RwaObjDefGetDefaultCreationParams(rwaObjDef *def, void *params);
extern rwaDecoder *sub_23F4C0(rwaObjDef *def, rwaInstance *parent, RwUInt32 flags, void *params,
                              void *a, void *b);

/* Creates a conversion node from format from to format to for the track
 * index. */
rwaDecoder *
rwa_unit2452a0_005c0620(rwaObjDef *def, const RwaWaveFormat *from, const RwaWaveFormat *to, RwUInt32 index,
           rwaStreamPlayer *player)
{
    rwaConversionParams params;

    RwaObjDefGetDefaultCreationParams(def, &params);
    params.info = player->info;
    params.unk58 = player->unk10;
    params.unk54 = player->info->tracks[index].unk20;
    params.from = *from;
    params.to = *to;
    return sub_23F4C0(def, NULL, 4, &params, player->unk50, player->unk58);
}
