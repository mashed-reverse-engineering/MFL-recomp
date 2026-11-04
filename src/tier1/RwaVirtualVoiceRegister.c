/* RwaVirtualVoiceRegister and its static helpers: RenderWare Audio (Criterion Software)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwavirtualvoiceobj.c, functions sub_23BD10 (0x005b6ee0, static), RwaVirtualVoiceRegister (0x005b7120)), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; these functions stay in one file because cl 13.00 compiles a static function and its callers in the same
 * file with a register calling convention. Changes: function names (sub_23B7B0 -> rwa_virtualvoiceobj_005b6940; sub_23B850 -> rwa_virtualvoiceobj_005b69e0; sub_23B8A0 -> rwa_virtualvoiceobj_005b6a30; sub_23D740 -> RwaObjDefSetInterface; sub_23DB90 -> RwaObjDefDestroy), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * P7c: byte-identical to MFL.exe (tools/cmpfn.py, cl 13.10: see the compile marker; MFL-specific changes are noted in the code). Static functions keep their
 * linkage: the overlay replaces only the external ones, the static copies serve the callers in this file. */
/* compile: cl1310 */
#include <string.h>
#include "rwa.h"
#include "rwaint.h"
extern RwUInt8 unk_506798[];
extern RwUInt32 dword_5067FC;
extern RwInt32 RwaObjDefDestroy(void *module);
extern const RwaUUID unk_3ED218;
extern void *unk_42E1C8[84];
extern void *unk_42E318[90];
extern RwUInt8 unk_506780[24];
extern RwUInt8 unk_506798[92];
extern RwUInt8 unk_5067F4[8];
extern rwaEmitter * rwa_virtualvoiceobj_005b6940(rwaEmitter *emitter, const RwaUUID *param, const RwUInt32 *flags);
extern void sub_23B820(rwaEmitter *emitter);
extern rwaEmitter * rwa_virtualvoiceobj_005b69e0(rwaEmitter *emitter);
extern rwaEmitter * rwa_virtualvoiceobj_005b6a30(rwaEmitter *emitter, RwUInt32 *value);
extern rwaObjDef *RwaObjDefAssignID(rwaObjDef *def, const void *uuid, const void *textID);
extern rwaObjDef *dword_516C60;
extern RwUInt8 unk_506780[];
extern RwUInt8 unk_5067F4[];
extern rwaObjDef *sub_23DFB0(void *parent, RwInt32 a, void *b, void *c, void *d, RwUInt32 hint);
extern RwBool _rwaObjDefSetupParams(rwaObjDef *def, void *sets, RwInt32 noSets, void *gets, RwInt32 noGets);
extern void *RwaVirtualVoiceInterfaceRegister(void);
extern rwaObjDef *RwaObjDefSetInterface(rwaObjDef *def, void *iface);

/* MFL.exe 0x005b6ee0 */
static void
sub_23BD10(rwaObjDef *def)
{
    RwaObjDefAssignID(def, &unk_3ED218, NULL);

    def->flags = 0x64;
    rwaObjDefSetStateAlignment(def, rwaPow2(0x28));

    def->create = (RwBool (*)(rwaInstance *, rwaInstance *, void *, void *, void *))rwa_virtualvoiceobj_005b6940;
    def->unlink = (void (*)(rwaInstance *, void *, void *))sub_23B820;
    def->unk2C = (rwaInstance *(*)(rwaInstance *))rwa_virtualvoiceobj_005b69e0;
    def->query = (void (*)(void *, void *))rwa_virtualvoiceobj_005b6a30;
    def->paramsSize = 4;
}

/* MFL.exe 0x005b7120 */
rwaObjDef *
RwaVirtualVoiceRegister(void)
{
    rwaObjDef  *def = sub_23DFB0(dword_516C60, 0xC, unk_506798, unk_506780, unk_5067F4,
                                 rwaMEMHINTDUR_EVENT | rwaID_810);
    void       *iface;

    if (!def)
    {
        return NULL;
    }

    sub_23BD10(def);

    if (!_rwaObjDefSetupParams(def, unk_42E1C8, 14, unk_42E318, 15))
    {
        RwaObjDefDestroy(def);

        return NULL;
    }

    iface = RwaVirtualVoiceInterfaceRegister();
    if (!iface)
    {
        RwaObjDefDestroy(def);

        return NULL;
    }

    RwaObjDefSetInterface(def, iface);
    dword_5067FC = 1;

    return def;
}
