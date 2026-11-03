/* RwaObjInterfaceDefCreate (MFL.exe 0x005bd960): RenderWare Audio (Criterion Software), rwa.dll 0.951
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_243B40.c, function sub_243E20), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_240B40 -> rwa_uuid_005be6b0; sub_243670 -> rwa_unit243670_005bec50; sub_243E20 -> RwaObjInterfaceDefCreate), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bd960 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern RwUInt8 unk_50708C[0x24];
extern rwaLink dword_5070B0;
static __forceinline RwaObjInterfaceDef *prepareInterfaceDefinition(RwaObjInterfaceDef *def)
{
    if (!def)
    {
        def = (RwaObjInterfaceDef *)rwa_unit243670_005bec50(unk_50708C, rwaMEMHINTDUR_EVENT | rwaID_80A);
        if (!def)
        {
            return NULL;
        }

        def->flags = 0;
    }
    else
    {
        def->flags = 2;
    }

    return def;
}

RwaObjInterfaceDef *
RwaObjInterfaceDefCreate(RwaObjInterfaceDef *def)
{
    def = prepareInterfaceDefinition(def);
    if (!def)
    {
        return NULL;
    }

    def->noInputs = 0;
    def->noOutputs = 0;
    def->inputs = NULL;
    def->outputs = NULL;
    def->unregister = NULL;
    rwa_uuid_005be6b0(&def->uniqueID);

    def->link.prev = NULL;
    def->link.next = NULL;
    def->link.next = dword_5070B0.next;
    def->link.prev = &dword_5070B0;
    dword_5070B0.next->prev = &def->link;
    dword_5070B0.next = &def->link;

    return def;
}
