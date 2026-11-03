/* RwaCoreUpdateStart (MFL.exe 0x005b6650): RenderWare Audio (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwabase.c, function RwaCoreUpdateStart), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_23F0D0 -> rwa_obj_005bac70), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005b6650 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern RwUInt32 dword_5069B8;
void sub_23E610(void);
extern rwaInstance *sub_23F2F0(void);
extern rwaInstance *rwa_obj_005bac70(rwaInstance *obj, RwBool skipSelf);
extern void sub_23EBD0(void);

void
RwaCoreUpdateStart(void)
{
    dword_5069B8 = 1;
    sub_23E610();
    rwa_obj_005bac70(sub_23F2F0(), FALSE);
    sub_23EBD0();
}
