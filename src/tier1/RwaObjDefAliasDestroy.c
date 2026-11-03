/* RwaObjDefAliasDestroy (MFL.exe 0x005c44c0): RenderWare Audio (Criterion Software), rwa.dll 0.933
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwasymboltable.c, function sub_244600), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_244600 -> RwaObjDefAliasDestroy), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c44c0 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern void sub_2421B0(void *mem);
extern RwaUniqueID *sub_240C30(RwaUniqueID *uniqueID);

void
RwaObjDefAliasDestroy(RwaParamType *paramType)
{
    paramType->link.prev->next = paramType->link.next;
    paramType->link.next->prev = paramType->link.prev;
    sub_240C30(&paramType->uniqueID);
    sub_2421B0(paramType);
}
