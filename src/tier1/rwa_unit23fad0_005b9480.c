/* rwa_unit23fad0_005b9480 and its static helpers: RenderWare Audio (Criterion Software)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_23FAD0.c, functions sub_2405F0 (0x005b9150, static), sub_240900 (0x005b9480)), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; these functions stay in one file because cl 13.00 compiles a static function and its callers in the same
 * file with a register calling convention. Changes: function names (sub_23D740 -> RwaObjDefSetInterface; sub_23DB90 -> RwaObjDefDestroy; sub_23FE10 -> rwa_unit23fad0_005b8960; sub_240900 -> rwa_unit23fad0_005b9480; sub_2416D0 -> RwaSemaphoreCreate; sub_243F40 -> rwa_unit243b40_005bda80), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * P7c: byte-identical to MFL.exe (tools/cmpfn.py, cl 13.00 and 13.10; MFL-specific changes are noted in the code). Static functions keep their
 * linkage: the overlay replaces only the external ones, the static copies serve the callers in this file. */
#include "rwa.h"
#include "rwaint.h"
extern RwUInt32 dword_506A94;
extern rwaLink dword_506AB4;
extern void *hSemaphore;
extern void *hObject;
extern rwaObjDef unk_506AD8;        /* the stream object definition */
extern RwUInt8 unk_506A98[];        /* the worker thread */
extern RwUInt8 unk_506AAC[];
extern RwUInt8 unk_506ABC[];
extern RwUInt32 dword_506B40;       /* the worker's priority */
extern const RwUInt8 unk_3ED864[];
extern const RwUInt8 unk_3EDF5C[];
extern __declspec(dllimport) RwBool __stdcall CloseHandle(void *handle);
extern void sub_23FC00(rwaStream *stream);
extern rwaObjDef *RwaObjDefAssignID(rwaObjDef *def, const void *uuid, const void *textID);
extern void *sub_23DCA0(void *a, const void *uuid, void *b);
extern rwaObjDef *sub_23DFB0(void *parent, RwInt32 a, void *b, void *c, void *d, RwUInt32 hint);
extern RwBool _rwaObjDefSetupParams(rwaObjDef *def, void *sets, RwInt32 noSets, void *gets,
                                    RwInt32 noGets);
extern rwaObjDef *RwaObjDefSetInterface(rwaObjDef *def, void *iface);
extern void *rwa_unit243b40_005bda80(const void *uuid);
extern RwBool RwaSemaphoreCreate(void **handle, RwInt32 a, RwInt32 b);
extern RwBool _RwaThreadCreate(void *thread, RwUInt32 (*func)(void *), RwUInt32 priority,
                               void *data);
extern RwBool sub_241770(void *thread, RwInt32 a);
extern RwBool FUN_005bfc50(void);
extern void FUN_005bfcf0(void);
extern RwBool RwaObjDefDestroy(rwaObjDef *def);
extern const RwaUUID unk_3ED28C;
extern void *unk_42E708[36];
extern void *unk_42E798[66];
extern RwUInt8 unk_506A98[20];
extern RwUInt8 unk_506AAC[8];
extern RwUInt8 unk_506ABC[28];
extern rwaObjDef unk_506AD8;
extern RwUInt32 dword_506B40;
extern rwaStream * sub_23FB40(rwaStream *stream, rwaInstance *parent, RwUInt32 *params);
extern void sub_23FC00(rwaStream *volatile input);
extern RwUInt32 sub_23FC60(void *data);
extern rwaStream * sub_23FD30(rwaStream *stream);
extern void * rwa_unit23fad0_005b8960(void *obj, RwUInt32 *params);

/* MFL.exe 0x005b9150 */
static void
sub_2405F0(rwaObjDef *def)
{
    RwaObjDefAssignID(def, &unk_3ED28C, NULL);

    def->flags = sizeof(rwaStream) - sizeof(rwaInstance);
    rwaObjDefSetStateAlignment(def, rwaPow2(0x28));

    def->create = (RwBool (*)(rwaInstance *, rwaInstance *, void *, void *, void *))sub_23FB40;
    def->unlink = (void (*)(rwaInstance *, void *, void *))sub_23FC00;
    def->unk30 = (rwaInstance *(*)(rwaInstance *))sub_23FD30;
    def->query = (void (*)(void *, void *))rwa_unit23fad0_005b8960;
    def->paramsSize = 0xC;

    dword_506AB4.next = &dword_506AB4;
    dword_506AB4.prev = &dword_506AB4;

    /* MFL's PC build: the stream worker setup is bracketed by FUN_005bfc50 (must succeed first) and FUN_005bfcf0 (undo on failure);
       the codfh Xbox build has neither call */
    if (!FUN_005bfc50())
    {
        return;
    }

    if (RwaSemaphoreCreate(&hSemaphore, 1, 1))
    {
        if (RwaSemaphoreCreate(&hObject, 0, 1))
        {
            dword_506A94 = 0;
            if (_RwaThreadCreate(unk_506A98, sub_23FC60, dword_506B40, NULL) &&
                sub_241770(unk_506A98, 0))
            {
                return;
            }

            CloseHandle(hObject);
        }

        CloseHandle(hSemaphore);
    }

    FUN_005bfcf0();
}

/* MFL.exe 0x005b9480 */
rwaObjDef *
rwa_unit23fad0_005b9480(void)
{
    rwaObjDef  *def = sub_23DFB0(sub_23DCA0(NULL, unk_3EDF5C, NULL), 0xC, &unk_506AD8,
                                 unk_506ABC, unk_506AAC, rwaMEMHINTDUR_EVENT | rwaID_80E);

    if (!def)
    {
        return NULL;
    }

    sub_2405F0(def);

    if (!_rwaObjDefSetupParams(def, unk_42E708, 6, unk_42E798, 9))
    {
        RwaObjDefDestroy(def);

        return NULL;
    }

    RwaObjDefSetInterface(def, rwa_unit243b40_005bda80(unk_3ED864));

    return def;
}
