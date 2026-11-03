/* RwaObjDefDestroy and its static helpers: RenderWare Audio (Criterion Software)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwaobjdef.c, functions sub_23DAA0 (0x005ba330, static), sub_23DB30 (0x005ba3c0, static), sub_23DB90 (0x005ba420)), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; these functions stay in one file because cl 13.00 compiles a static function and its callers in the same
 * file with a register calling convention. Changes: function names (sub_23DAA0 -> rwa_objdef_005ba330; sub_23DB30 -> rwa_objdef_005ba3c0; sub_23DB90 -> RwaObjDefDestroy; sub_2437A0 -> rwa_unit243670_005bed80; sub_2438E0 -> rwa_unit243670_005beec0; sub_243B80 -> RwaObjInterfaceRemove; sub_244600 -> RwaObjDefAliasDestroy), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe (tools/cmpfn.py, relocations masked): rwa_objdef_005ba330 0x005ba330, rwa_objdef_005ba3c0 0x005ba3c0, RwaObjDefDestroy 0x005ba420. Static functions keep their linkage: the
 * overlay replaces only the external ones, the static copies serve the callers in this file. */
#include "rwa.h"
#include "rwaint.h"
#include <string.h>
#include <stddef.h>
extern RwBool RwaObjInterfaceRemove(rwaObjInterface *objInterface);
extern void RwaObjDefAliasDestroy(RwaParamType *paramType);
extern void rwa_unit243670_005beec0(void *freeList);
extern void sub_2443F0(void *pool);
extern void sub_240C30(void *uniqueID);
extern RwUInt8 unk_5068F8[0x5C];
extern RwUInt8 unk_506954[0x24];
extern RwUInt8 unk_506978[0x24];
extern RwUInt8 unk_5068D0[0x24];
extern void rwa_unit243670_005bed80(void *freeList, void *entry);
extern void sub_2421B0(void *entry);
extern RwBool RwaObjDefDestroy(rwaObjDef *def);

/* Qualified parameter-table pointer reads preserve their loads before
 * the count tests. Original qualifiers are unknown. */
/* MFL.exe 0x005ba330 */
static rwaObjDef *
rwa_objdef_005ba330(rwaObjDef *def)
{
    if (def->flags0C & 4)
    {
        {
            rwaParamTables *tables = (rwaParamTables *)def->interfaces;
            rwaParamDef *param = *(rwaParamDef *volatile *)&tables->inputs;
            RwUInt16 count = tables->noInputs;

            if (count > 0)
            {
                RwUInt32 i;

                param += count;
                i = count;
                do
                {
                    --param;
                    sub_240C30(param);
                }
                while (--i > 0);
            }
        }
        {
            rwaParamTables *tables = (rwaParamTables *)def->interfaces;
            rwaParamDef *param = *(rwaParamDef *volatile *)&tables->outputs;
            RwUInt16 count = tables->noOutputs;

            if (count > 0)
            {
                RwUInt32 i;

                param += count;
                i = count;
                do
                {
                    --param;
                    sub_240C30(param);
                }
                while (--i > 0);
            }
        }
        if (!(def->flags0C & 1))
        {
            sub_2421B0(((rwaParamTables *)def->interfaces)->inputs);
        }

        ((rwaParamTables *)def->interfaces)->inputs = NULL;
        ((rwaParamTables *)def->interfaces)->outputs = NULL;
    }

    return def;
}

/* MFL.exe 0x005ba3c0 */
static void
rwa_objdef_005ba3c0(rwaObjDef *def)
{
    rwa_objdef_005ba330(def);
    sub_240C30(def);

    if ((def->flags0C & 8) && !(def->flags0C & 0x20))
    {
        rwa_unit243670_005bed80(unk_506954, def->unk50);
    }

    if ((def->flags0C & 4) && !(def->flags0C & 0x10))
    {
        rwa_unit243670_005bed80(unk_506978, def->interfaces);
    }

    if (def->flags0C & 2)
    {
        rwa_unit243670_005bed80(unk_5068D0, def);
    }
}

/* MFL.exe 0x005ba420 */
RwBool
RwaObjDefDestroy(rwaObjDef *def)
{
    rwaLink *list;
    rwaLink *child;

    while ((child = def->children.next) != &def->children)
    {
        if (!RwaObjDefDestroy((rwaObjDef *)((RwUInt8 *)child - offsetof(rwaObjDef, sibling))))
        {
            return FALSE;
        }
    }

    if (def->unregister)
    {
        def->unregister(def);
    }

    if (def->flags0C & 4)
    {
        rwaParamTables *tables = (rwaParamTables *)def->interfaces;
        child = tables->interfaces.next;
        list = &tables->interfaces;
        if (child != list)
        do
        {
            RwaObjInterfaceRemove((rwaObjInterface *)((RwUInt8 *)child - offsetof(rwaObjInterface, link)));
            child = list->next;
        }
        while (child != list);
    }

    if (def->flags0C & 8)
    {
        list = (rwaLink *)def->unk50;
        while (list->next != list)
        {
            RwaObjDefAliasDestroy((RwaParamType *)((RwUInt8 *)list->next - offsetof(RwaParamType, link)));
        }
    }

    if (def->instances)
    {
        if (def->flags0C & 0x40)
        {
            rwa_unit243670_005beec0(def->instances);
            def->instances = NULL;
        }
        else if (def->flags0C & 0x80)
        {
            sub_2443F0(def->instances);
            def->instances = NULL;
        }
        else
        {
            def->instances = NULL;
        }
    }

    if (def != (rwaObjDef *)unk_5068F8)
    {
        def->sibling.prev->next = def->sibling.next;
        def->sibling.next->prev = def->sibling.prev;
    }

    rwa_objdef_005ba3c0(def);

    return TRUE;
}
