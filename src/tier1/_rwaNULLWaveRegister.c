/* _rwaNULLWaveRegister (MFL.exe 0x005bb460): RenderWare Audio (Criterion Software), rwa.dll 1.0
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_244790.c, function sub_244880), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_244800 -> rwa_unit244790_005bb3e0; sub_244820 -> rwa_unit244790_005bb400; sub_244850 -> rwa_unit244790_005bb430; sub_244880 -> _rwaNULLWaveRegister), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bb460 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern RwaWaveDef unk_5070F0;           /* the memory wave definition */
extern RwaWaveDef *sub_243090(rwaObjDef *objDef, RwaWaveDef *waveDef);
extern RwaWaveDef *RwaWaveDefAssignID(RwaWaveDef *waveDef, const RwaUUID *uuid,
                                      const RwChar *name);
extern const RwaUUID unk_3ED794;
extern RwaWaveDef unk_5070F0;
extern RwaWave * sub_2447B0(RwaWave *wave);
extern void rwa_unit244790_005bb3e0(RwaWave *wave);
extern RwaWave * rwa_unit244790_005bb400(RwaWave *wave, RwUInt32 offset, const void *block, RwUInt32 size);
extern RwaWave * rwa_unit244790_005bb430(RwaWave *wave, const RwaWaveFormat *format, RwUInt32 offset, void *block);

RwaWaveDef *
_rwaNULLWaveRegister(void)
{
    RwaWaveDef *waveDef = sub_243090(NULL, &unk_5070F0);

    if (!waveDef)
    {
        return NULL;
    }

    RwaWaveDefAssignID(waveDef, &unk_3ED794, NULL);
    waveDef->create = (RwBool (*)(RwaWave *, void *))sub_2447B0;
    waveDef->destroy = rwa_unit244790_005bb3e0;
    waveDef->unk14 = rwa_unit244790_005bb400;
    waveDef->unk18 = rwa_unit244790_005bb430;

    return waveDef;
}
