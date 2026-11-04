/* rwa_unit23cc10_005b9f20 and its static helpers: RenderWare Audio (Criterion Software)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_23CC10.c, functions sub_23D450 (0x005b9ce0, static), sub_23D690 (0x005b9f20)), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; these functions stay in one file because cl 13.00 compiles a static function and its callers in the same
 * file with a register calling convention. Changes: function names (sub_23D690 -> rwa_unit23cc10_005b9f20; sub_23D740 -> RwaObjDefSetInterface; sub_23DB90 -> RwaObjDefDestroy), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * P7c: byte-identical to MFL.exe (tools/cmpfn.py, cl 13.10: see the compile marker; MFL-specific changes are noted in the code). Static functions keep their
 * linkage: the overlay replaces only the external ones, the static copies serve the callers in this file. */
/* compile: cl1310 */
#include "rwa.h"
#include "rwaint.h"
extern RwUInt8 unk_506858[];
extern RwUInt32 dword_5068CC;
extern RwInt32 RwaObjDefDestroy(void *module);
extern const RwaUUID unk_3ED278;
extern void *unk_42E488[48];
extern void *unk_42E548[56];
extern RwUInt8 unk_506850[8];
extern RwUInt8 unk_506858[92];
extern RwUInt8 unk_5068B4[24];
extern rwaListener * sub_23CC70(rwaListener *listener);
extern rwaObjDef *RwaObjDefAssignID(rwaObjDef *def, const void *uuid, const void *textID);
extern rwaListener *sub_23CC70(rwaListener *listener);
extern rwaObjDef *dword_516C60;
extern RwUInt8 unk_506850[];
extern RwUInt8 unk_5068B4[];
extern rwaObjDef *sub_23DFB0(void *parent, RwInt32 a, void *b, void *c, void *d, RwUInt32 hint);
extern RwBool _rwaObjDefSetupParams(rwaObjDef *def, void *sets, RwInt32 noSets, void *gets, RwInt32 noGets);
extern void *RwaListenerInterfaceRegister_0(void);
extern rwaObjDef *RwaObjDefSetInterface(rwaObjDef *def, void *iface);

/* MFL.exe 0x005b9ce0 */
static void
sub_23D450(rwaObjDef *def)
{
    RwaObjDefAssignID(def, &unk_3ED278, NULL);

    def->flags = 0x4C;
    rwaObjDefSetStateAlignment(def, rwaPow2(0x28));

    def->create = (RwBool (*)(rwaInstance *, rwaInstance *, void *, void *, void *))sub_23CC70;
    def->paramsSize = 0;
    def->maxInstances = 1;
}

/* MFL.exe 0x005b9f20 */
rwaObjDef *
rwa_unit23cc10_005b9f20(void)
{
    rwaObjDef  *def = sub_23DFB0(dword_516C60, 0xC, unk_506858, unk_5068B4, unk_506850,
                                 rwaMEMHINTDUR_EVENT | rwaID_80B);
    void       *iface;

    if (!def)
    {
        return NULL;
    }

    sub_23D450(def);

    if (!_rwaObjDefSetupParams(def, unk_42E488, 8, unk_42E548, 9))
    {
        RwaObjDefDestroy(def);

        return NULL;
    }

    iface = RwaListenerInterfaceRegister_0();
    if (!iface)
    {
        RwaObjDefDestroy(def);

        return NULL;
    }

    RwaObjDefSetInterface(def, iface);
    dword_5068CC = 1;

    return def;
}
