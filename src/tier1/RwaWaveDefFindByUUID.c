/* RwaWaveDefFindByUUID (MFL.exe 0x005bd2e0): RenderWare Audio (Criterion Software), rwa.dll 0.929
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_242910.c, function sub_243130), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_23D970 -> rwa_objdef_005ba200; sub_243130 -> RwaWaveDefFindByUUID), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bd2e0 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern void rwa_objdef_005ba200(void *a, void *b,
                       RwBool (*callback)(rwaObjDef *parent, rwaObjDef *objDef, void *data),
                       void *data, RwBool recurse);
extern RwBool sub_242C10(rwaObjDef *parent, rwaObjDef *objDef, rwaWaveDefFind *find);

RwaWaveDef *
RwaWaveDefFindByUUID(const RwaUUID *uuid)
{
    rwaWaveDefFind  find;

    find.uuid = uuid;
    find.found = NULL;
    rwa_objdef_005ba200(NULL, NULL, (RwBool (*)(rwaObjDef *, rwaObjDef *, void *))sub_242C10, &find,
               TRUE);

    return find.found;
}
