/* RwaObjDefDestroy and its static helpers: RenderWare Audio (Criterion Software)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwaobjdef.c, functions sub_23DAA0 (0x005ba330, static), sub_23DB30 (0x005ba3c0, static), sub_23DB90 (0x005ba420), sub_23DE20 (0x005ba740, static), sub_23DF00 (0x005ba820, static), sub_23DFB0 (0x005ba8d0)), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466; MFL.exe was built with cl 13.10). Used with
 * credit; these functions stay in one file because cl 13.00 compiles a static function and its callers in the same
 * file with a register calling convention. Changes: function names (sub_23DAA0 -> rwa_objdef_005ba330; sub_23DB30 -> rwa_objdef_005ba3c0; sub_23DB90 -> RwaObjDefDestroy; sub_23DE20 -> rwa_objdef_005ba740; sub_23DF00 -> rwa_objdef_005ba820; sub_240B40 -> rwa_uuid_005be6b0; sub_243670 -> rwa_unit243670_005bec50; sub_2437A0 -> rwa_unit243670_005bed80; sub_2438E0 -> rwa_unit243670_005beec0; sub_243B80 -> RwaObjInterfaceRemove; sub_244600 -> RwaObjDefAliasDestroy), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe (tools/cmpfn.py, relocations masked): rwa_objdef_005ba330 0x005ba330, rwa_objdef_005ba3c0 0x005ba3c0, RwaObjDefDestroy 0x005ba420, rwa_objdef_005ba740 0x005ba740, rwa_objdef_005ba820 0x005ba820, sub_23DFB0 0x005ba8d0. Static functions keep their linkage: the
 * overlay replaces only the external ones, the static copies serve the callers in this file. */
/* compile: cl1310 */
#include "rwa.h"
#include "rwaint.h"
#include <string.h>
#include <stddef.h>
extern RwBool RwaObjInterfaceRemove(rwaObjInterface *objInterface);
extern void RwaObjDefAliasDestroy(RwaParamType *paramType);
extern void rwa_unit243670_005beec0(void *freeList);
extern void sub_2443F0(void *pool);
extern void *TkFSManagerGetValue(void *obj, RwUInt32 index, void *data);
extern void sub_240C30(void *uniqueID);
extern void rwa_uuid_005be6b0(void *uniqueID);
extern RwUInt8 unk_516C50[];
extern void *sub_244750(void *types, RwInt32 index);
extern rwaObjDef *dword_516C60;
extern RwUInt8 unk_5068F8[0x5C];
extern RwUInt8 unk_506954[0x24];
extern RwUInt8 unk_506978[0x24];
extern RwUInt8 unk_5068D0[0x24];
extern RwUInt32 dword_5069A4;
extern RwUInt8 unk_516C50[12];
extern void *rwa_unit243670_005bec50(void *freeList, RwUInt32 hint);
extern void rwa_unit243670_005bed80(void *freeList, void *entry);
extern void sub_2421B0(void *entry);
extern RwBool RwaObjDefDestroy(rwaObjDef *def);
extern rwaObjDef *sub_23DFB0(rwaObjDef *parent, RwUInt32 flags, rwaObjDef *def, void *tables,
                             void *list, RwUInt32 hint);

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

/* MFL.exe 0x005ba740 */
static void
rwa_objdef_005ba740(rwaObjDef *input, RwInt32 io)
{
    rwaObjDef *def = input;
    void           *type = sub_244750(unk_516C50, 0);
    rwaParamDef    *param;
    RwUInt16        i;

    if (io == 1)
    {
        {
            rwaParamTables *tables = (rwaParamTables *)def->interfaces;
            rwaParamDef *params = tables->inputs;
            RwUInt16 count = tables->noInputs;

            for (i = 0; i < count; ++i)
            {
                param = &params[i];
                param->type = type;
                param->typeIndex = 0;
                param->handler = TkFSManagerGetValue;
                rwa_uuid_005be6b0(param);
            }
        }
    }
    else
    {
        {
            rwaParamTables *tables = (rwaParamTables *)def->interfaces;
            rwaParamDef *params = tables->outputs;
            RwUInt16 count = tables->noOutputs;

            for (i = 0; i < count; ++i)
            {
                param = &params[i];
                param->type = type;
                param->typeIndex = 0;
                param->handler = TkFSManagerGetValue;
                rwa_uuid_005be6b0(param);
            }
        }
    }
}

/* Clear the packed size and alignment separately. These stores and the
 * alignment macro retain the original OR with the zeroed flag word. */
/* MFL.exe 0x005ba820 */
static void
rwa_objdef_005ba820(rwaObjDef *def)
{
    if (def->flags0C & 4)
    {
        rwa_objdef_005ba740(def, 1);
        rwa_objdef_005ba740(def, 2);
    }

    rwa_uuid_005be6b0(def);

    if (def->flags0C & 4)
    {
        rwaParamTables *tables = (rwaParamTables *)def->interfaces;
        rwaLink        *list = &tables->interfaces;

        list->next = list;
        tables->interfaces.prev = list;
        tables->unk00 = NULL;
        tables->inputs = NULL;
        tables->outputs = NULL;
        tables->noInputs = 0;
        tables->noOutputs = 0;
    }

    if (def->flags0C & 8)
    {
        rwaLink    *list = def->unk50;      /* void * field */

        list->next = list;
        list->prev = list;
    }

    /* Packed state-size/alignment word at +0x40. */
    def->unk40.stateSize = 0;
    def->unk40.stateAlign = 0;
    def->instances = NULL;
    rwaObjDefSetStateAlignment(def, 1);
    def->children.next = &def->children;
    def->children.prev = &def->children;
    def->maxInstances = 8;
    def->paramsSize = 0;
    def->unk54 = NULL;
    def->sibling.prev = NULL;
    def->sibling.next = NULL;
    def->waveDefs.next = &def->waveDefs;
    def->waveDefs.prev = &def->waveDefs;
    def->create = NULL;
    def->unk2C = NULL;
    def->unk30 = NULL;
    def->unlink = NULL;
    def->unregister = NULL;
    def->query = NULL;
}

/* MFL.exe 0x005ba8d0 */
rwaObjDef *
sub_23DFB0(rwaObjDef *parent, RwUInt32 flags, rwaObjDef *def, void *tables, void *list,
           RwUInt32 hint)
{
    if (!parent && !dword_5069A4)
    {
        parent = dword_516C60;
    }

    if (!def)
    {
        def = (rwaObjDef *)rwa_unit243670_005bec50(unk_5068D0, rwaMEMHINTDUR_EVENT | rwaID_809);
        if (!def)
        {
            return NULL;
        }

        def->flags0C = 2;
    }
    else
    {
        def->flags0C = 0;
    }

    def->hint = hint;

    if ((RwUInt8)flags & 4)
    {
        if (!tables)
        {
            def->interfaces = rwa_unit243670_005bec50(unk_506978, rwaMEMHINTDUR_EVENT | rwaID_809);
            if (!def->interfaces)
            {
                rwa_objdef_005ba3c0(def);

                return NULL;
            }
        }
        else
        {
            def->interfaces = tables;
            def->flags0C |= 0x10;
        }

        memset(def->interfaces, 0, sizeof(rwaParamTables));
        def->flags0C |= 4;
    }

    if ((RwUInt8)flags & 8)
    {
        if (!list)
        {
            def->unk50 = rwa_unit243670_005bec50(unk_506954, rwaMEMHINTDUR_EVENT | rwaID_809);
            if (!def->unk50)
            {
                rwa_objdef_005ba3c0(def);

                return NULL;
            }
        }
        else
        {
            def->unk50 = list;
            def->flags0C |= 0x20;
        }

        memset(def->unk50, 0, sizeof(rwaLink));
        def->flags0C |= 8;
    }

    rwa_objdef_005ba820(def);

    if (parent)
    {
        def->sibling.next = parent->children.next;
        def->sibling.prev = &parent->children;
        parent->children.next->prev = &def->sibling;
        parent->children.next = &def->sibling;
    }

    return def;
}
