/* rwa_streamsourceobj_005c91f0 and its static helpers: RenderWare Audio (Criterion Software)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwastreamsourceobj.c, functions sub_241410 (0x005c8fa0, static), sub_241660 (0x005c91f0)), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; these functions stay in one file because cl 13.00 compiles a static function and its callers in the same
 * file with a register calling convention. Changes: function names (sub_23D740 -> RwaObjDefSetInterface; sub_23DB90 -> RwaObjDefDestroy; sub_240C70 -> rwa_streamsourceobj_005c8840; sub_240D80 -> rwa_streamsourceobj_005c8950; sub_2412F0 -> rwa_streamsourceobj_005c8e80; sub_241660 -> rwa_streamsourceobj_005c91f0), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * P7c: byte-identical to MFL.exe (tools/cmpfn.py, cl 13.10: see the compile marker; MFL-specific changes are noted in the code). Static functions keep their
 * linkage: the overlay replaces only the external ones, the static copies serve the callers in this file. */
/* compile: cl1310 */
#include "rwa.h"
#include "rwaint.h"
extern rwaObjDef unk_506B70;        /* the stream source definition */
extern RwUInt8 unk_506BCC[];
extern RwUInt8 unk_506BE4[];
extern rwaObjDef *dword_516C60;
extern RwBool RwaObjDefDestroy(rwaObjDef *def);
extern rwaObjDef *RwaObjDefAssignID(rwaObjDef *def, const void *uuid, const void *textID);
extern rwaObjDef *sub_23DFB0(void *parent, RwInt32 a, void *b, void *c, void *d, RwUInt32 hint);
extern RwBool _rwaObjDefSetupParams(rwaObjDef *def, void *sets, RwInt32 noSets, void *gets, RwInt32 noGets);
extern void *RwaStreamSourceInterfaceRegister(void);
extern rwaObjDef *RwaObjDefSetInterface(rwaObjDef *def, void *iface);
extern const RwaUUID unk_3ED29C;
extern void *unk_42E8A0[30];
extern void *unk_42E918[32];
extern void *unk_42E998[6];
extern rwaObjDef unk_506B70;
extern RwUInt8 unk_506BCC[24];
extern RwUInt8 unk_506BE4[8];
extern rwaDecoder * rwa_streamsourceobj_005c8840(rwaDecoder *obj);
extern rwaDecoder * rwa_streamsourceobj_005c8950(rwaDecoder *obj, rwaStreamNodeParams *params);
extern void sub_2411F0(rwaDecoder *obj, void *a, void *b);
extern rwaDecoder * rwa_streamsourceobj_005c8e80(rwaDecoder *obj, rwaInstance *parent, rwaStreamNodeParams *params, void *a, void *b);

/* MFL.exe 0x005c8fa0 */
static void
sub_241410(rwaObjDef *def)
{
    RwaObjDefAssignID(def, &unk_3ED29C, NULL);

    def->flags = sizeof(rwaDecoder) - sizeof(rwaInstance);
    rwaObjDefSetStateAlignment(def, rwaPow2(0x28));

    def->create = (RwBool (*)(rwaInstance *, rwaInstance *, void *, void *, void *))rwa_streamsourceobj_005c8e80;
    def->unlink = (void (*)(rwaInstance *, void *, void *))sub_2411F0;
    def->unk2C = (rwaInstance *(*)(rwaInstance *))rwa_streamsourceobj_005c8840;
    def->query = (void (*)(void *, void *))rwa_streamsourceobj_005c8950;
    def->paramsSize = sizeof(rwaStreamNodeParams);
    def->unk54 = unk_42E998;
}

/* MFL.exe 0x005c91f0 */
rwaObjDef *
rwa_streamsourceobj_005c91f0(void)
{
    rwaObjDef  *def = sub_23DFB0(dword_516C60, 0xC, &unk_506B70, unk_506BCC, unk_506BE4,
                                 rwaMEMHINTDUR_EVENT | rwaID_80E);
    void       *iface;

    if (!def)
    {
        return NULL;
    }

    sub_241410(def);

    if (!_rwaObjDefSetupParams(def, unk_42E8A0, 5, unk_42E918, 5))
    {
        RwaObjDefDestroy(def);

        return NULL;
    }

    iface = RwaStreamSourceInterfaceRegister();
    if (!iface)
    {
        RwaObjDefDestroy(def);

        return NULL;
    }

    RwaObjDefSetInterface(def, iface);

    return def;
}
