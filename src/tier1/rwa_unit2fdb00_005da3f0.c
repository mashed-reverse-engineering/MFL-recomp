/* rwa_unit2fdb00_005da3f0 and its static helpers: RenderWare Audio (Criterion Software)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_2FDB00.c, functions sub_2FDFE0 (0x005d9860, static), sub_2FE000 (0x005d9880, static), sub_2FE8C0 (0x005da1a0, static), sub_2FEB10 (0x005da3f0)), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; these functions stay in one file because cl 13.00 compiles a static function and its callers in the same
 * file with a register calling convention. Changes: function names (sub_23D740 -> RwaObjDefSetInterface; sub_23DB90 -> RwaObjDefDestroy; sub_23F0D0 -> rwa_obj_005bac70; sub_243F40 -> rwa_unit243b40_005bda80; sub_2FDFE0 -> rwa_unit2fdb00_005d9860; sub_2FE000 -> rwa_unit2fdb00_005d9880; sub_2FEB10 -> rwa_unit2fdb00_005da3f0), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * P7c: byte-identical to MFL.exe (tools/cmpfn.py, cl 13.10: see the compile marker; MFL-specific changes are noted in the code). Static functions keep their
 * linkage: the overlay replaces only the external ones, the static copies serve the callers in this file. */
/* compile: cl1310 */
#include "rwa.h"
#include "rwaint.h"
extern rwaObjDef *sub_23DCA0(rwaObjDef *parent, const void *uuid, const void *textID);
extern RwBool RwaObjDefDestroy(rwaObjDef *def);
extern rwaObjDef *sub_23DFB0(void *parent, RwInt32 a, void *b, void *c, void *d, RwUInt32 hint);
extern RwBool _rwaObjDefSetupParams(rwaObjDef *def, void *sets, RwInt32 noSets, void *gets, RwInt32 noGets);
extern rwaObjDef *RwaObjDefSetInterface(rwaObjDef *def, void *iface);
extern rwaObjDef *RwaObjDefAssignID(rwaObjDef *def, const void *uuid, const void *textID);
extern rwaInstance *rwa_obj_005bac70(rwaInstance *obj, RwBool flag);
extern void *rwa_unit243b40_005bda80(const void *uuid);
extern rwaDecoder *RwaStreamNodeGetParams(rwaDecoder *obj, void *params);
extern rwaObjDef unk_512658;        /* the stream voice definition */
extern RwUInt8 unk_512638[];
extern RwUInt8 unk_512650[];
extern const RwUInt8 unk_3ED964[];  /* the stream node interface */
extern const RwUInt8 unk_3EDF5C[];
extern rwaNodeAllocator off_42FB3C; /* the default memory functions */
extern const RwaUUID unk_41AB64;
extern void *unk_438B98[6];
extern void *off_438BB0[30];
extern void *off_438C28[24];
extern RwUInt8 unk_512638[24];
extern RwUInt8 unk_512650[8];
extern rwaObjDef unk_512658;
extern rwaDecoder * sub_2FDBD0(rwaDecoder *obj, rwaInstance *parent, rwaStreamVoiceParams *params, void *a, void *b);
extern rwaDecoder * sub_2FE290(rwaInstance *base);
extern void sub_2FE820(rwaDecoder *obj, void *a, void *b);

/* MFL.exe 0x005d9860 */
static rwaDecoder *
rwa_unit2fdb00_005d9860(rwaDecoder *obj)
{
    rwa_obj_005bac70(&obj->inst, TRUE);

    return obj;
}

/* The default creation parameters. */
/* MFL.exe 0x005d9880 */
static rwaDecoder *
rwa_unit2fdb00_005d9880(rwaDecoder *obj, rwaStreamVoiceParams *params)
{
    RwaStreamNodeGetParams(obj, params);
    params->track = NULL;
    params->done = NULL;
    params->doneData = NULL;
    params->unk04 = 0;
    params->mono = FALSE;
    params->allocator = &off_42FB3C;
    params->unk00 = 1;
    params->noEntries = 2;

    return obj;
}

/* MFL.exe 0x005da1a0 */
static void
sub_2FE8C0(rwaObjDef *def)
{
    RwaObjDefAssignID(def, &unk_41AB64, NULL);

    def->flags = 0x234;     /* sizeof the stream voice state in MFL's PC build (codfh Xbox rwaStreamVoiceState: 0x298) */
    rwaObjDefSetStateAlignment(def, rwaPow2(0x28));

    def->create = (RwBool (*)(rwaInstance *, rwaInstance *, void *, void *, void *))sub_2FDBD0;
    def->unlink = (void (*)(rwaInstance *, void *, void *))sub_2FE820;
    def->unk2C = (rwaInstance *(*)(rwaInstance *))rwa_unit2fdb00_005d9860;
    def->unk30 = (rwaInstance *(*)(rwaInstance *))sub_2FE290;
    def->query = (void (*)(void *, void *))rwa_unit2fdb00_005d9880;
    def->paramsSize = sizeof(rwaStreamVoiceParams);
    def->unk54 = unk_438B98;
}

/* MFL.exe 0x005da3f0 */
rwaObjDef *
rwa_unit2fdb00_005da3f0(void)
{
    rwaObjDef  *def = sub_23DFB0(sub_23DCA0(NULL, unk_3EDF5C, NULL), 0xC, &unk_512658, unk_512638, unk_512650,
                                 rwaMEMHINTDUR_EVENT | rwaID_80E);

    if (!def)
    {
        return NULL;
    }

    sub_2FE8C0(def);

    if (!_rwaObjDefSetupParams(def, off_438BB0, 5, off_438C28, 4))
    {
        RwaObjDefDestroy(def);

        return NULL;
    }

    RwaObjDefSetInterface(def, rwa_unit243b40_005bda80(unk_3ED964));

    return def;
}
