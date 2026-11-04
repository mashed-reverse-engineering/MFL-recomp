/* rwa_unit24cc50_005c8760 and its static helpers: RenderWare Audio (Criterion Software)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_24CC50.c, functions sub_24CC50 (0x005c8000, static), sub_24CD20 (0x005c80d0, static), sub_24D1B0 (0x005c8510, static), sub_24D400 (0x005c8760)), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; these functions stay in one file because cl 13.00 compiles a static function and its callers in the same
 * file with a register calling convention. Changes: function names (sub_23DB90 -> RwaObjDefDestroy; sub_247120 -> rwa_unit246fd0_005c1d80; sub_24CC50 -> rwa_unit24cc50_005c8000; sub_24CC80 -> rwa_unit24cc50_005c8030; sub_24CD20 -> rwa_unit24cc50_005c80d0; sub_24CFF0 -> rwa_unit24cc50_005c8350; sub_24D400 -> rwa_unit24cc50_005c8760), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * P7c: byte-identical to MFL.exe (tools/cmpfn.py, cl 13.10: see the compile marker; MFL-specific changes are noted in the code). Static functions keep their
 * linkage: the overlay replaces only the external ones, the static copies serve the callers in this file. */
/* compile: cl1310 */
#include "rwa.h"
#include "rwaint.h"
extern rwaDecoder *RwaStreamNodeGetParams(rwaDecoder *obj, rwaStreamNodeParams *params);
extern void rwa_unit246fd0_005c1d80(rwaDecoder *obj, void *a, void *b);
extern rwaObjDef *RwaObjDefAssignID(rwaObjDef *def, const void *uuid, const void *textID);
extern rwaObjDef *sub_23DFB0(void *parent, RwInt32 a, void *b, void *c, void *d, RwUInt32 hint);
extern RwBool _rwaObjDefSetupParams(rwaObjDef *def, void *sets, RwInt32 noSets, void *gets, RwInt32 noGets);
extern RwBool RwaObjDefDestroy(rwaObjDef *def);
extern rwaObjDef *dword_516C60;
extern rwaObjDef unk_507BA8;        /* the splitter definition */
extern RwUInt8 unk_507C04[];
extern RwUInt8 unk_507C0C[];
extern const RwUInt8 unk_3EDEBC[];  /* the splitter */
extern rwaNodeAllocator off_42FB3C; /* the default memory functions */
extern rwaSplitter *rwa_unit24cc50_005c8030(rwaSplitter *obj);
extern rwaSplitter *rwa_unit24cc50_005c8350(rwaSplitter *obj, rwaInstance *parent, rwaSplitterParams *params,
                               void *a, void *b);
extern void *off_42FF88[18];
extern void *off_42FFD0[48];
extern rwaObjDef unk_507BA8;
extern RwUInt8 unk_507C04[8];
extern RwUInt8 unk_507C0C[64];
extern rwaSplitter * rwa_unit24cc50_005c8030(rwaSplitter *arg);
extern rwaSplitter * rwaSplitterUpdate(rwaSplitter *obj);
extern rwaSplitter * rwa_unit24cc50_005c8350(rwaSplitter *obj, rwaInstance *parent, rwaSplitterParams *params, void *a, void *b);

/* MFL.exe 0x005c8000 */
static void
rwa_unit24cc50_005c8000(rwaSplitter *obj, void *a, void *b)
{
    obj->state.allocator->free(obj->state.masks, obj->state.allocator->data);
    rwa_unit246fd0_005c1d80((rwaDecoder *)obj, a, b);
}

/* The default creation parameters. */
/* MFL.exe 0x005c80d0 */
static void *
rwa_unit24cc50_005c80d0(rwaSplitter *obj, rwaSplitterParams *params)
{
    RwaStreamNodeGetParams((rwaDecoder *)obj, (rwaStreamNodeParams *)params);
    params->unk00 = 1;
    params->unk04 = 1;
    params->allocator = &off_42FB3C;

    return obj;
}

/* MFL.exe 0x005c8510 */
static void
sub_24D1B0(rwaObjDef *def)
{
    RwaObjDefAssignID(def, unk_3EDEBC, NULL);

    def->flags = sizeof(rwaSplitterState);
    rwaObjDefSetStateAlignment(def, rwaPow2(0x28));

    def->create = (RwBool (*)(rwaInstance *, rwaInstance *, void *, void *, void *))rwa_unit24cc50_005c8350;
    def->unlink = (void (*)(rwaInstance *, void *, void *))rwa_unit24cc50_005c8000;
    def->unk2C = (rwaInstance *(*)(rwaInstance *))rwa_unit24cc50_005c8030;
    def->unk30 = (rwaInstance *(*)(rwaInstance *))rwaSplitterUpdate;
    def->query = (void (*)(void *, void *))rwa_unit24cc50_005c80d0;
    def->paramsSize = sizeof(rwaSplitterParams);
}

/* MFL.exe 0x005c8760 */
rwaObjDef *
rwa_unit24cc50_005c8760(void)
{
    rwaObjDef  *def = sub_23DFB0(dword_516C60, 0xC, &unk_507BA8, unk_507C0C, unk_507C04,
                                 rwaMEMHINTDUR_EVENT | rwaID_80E);

    if (!def)
    {
        return NULL;
    }

    sub_24D1B0(def);

    if (!_rwaObjDefSetupParams(def, off_42FF88, 3, off_42FFD0, 2))
    {
        RwaObjDefDestroy(def);

        return NULL;
    }

    return def;
}
