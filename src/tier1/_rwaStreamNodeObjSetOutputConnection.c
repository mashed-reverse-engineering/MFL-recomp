/* _rwaStreamNodeObjSetOutputConnection: RenderWare Audio (Criterion Software)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_246FD0.c, functions sub_247130 (0x005c1d90, static), sub_2471D0 (0x005c1e30)), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; these functions stay in one file because cl 13.00 compiles a static function and its callers in the same
 * file with a register calling convention. Changes: function names (sub_2471D0 -> _rwaStreamNodeObjSetOutputConnection), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe (tools/cmpfn.py, relocations masked): sub_247130 0x005c1d90, _rwaStreamNodeObjSetOutputConnection 0x005c1e30. Static functions keep their linkage: the
 * overlay replaces only the external ones, the static copies serve the callers in this file. */
#include "rwa.h"
#include "rwaint.h"

/* MFL.exe 0x005c1d90 (returns 0).  Declared, not defined here: its address is stored, so it must resolve to the
 * original function, not to a static copy in the overlay (RELCHECK). */
extern RwUInt32 sub_247130(void);

/* The node's reader: its node, a reader returning nothing and the data. */
/* MFL.exe 0x005c1e30 */
rwaDecoder *
_rwaStreamNodeObjSetOutputConnection(rwaDecoder *obj, void *data, rwaStreamConnect *connect)
{
    connect->reader->unk00 = (RwUInt32)obj;
    connect->reader->read = (rwaQueueItem *(*)(rwaDecoder *))sub_247130;
    connect->reader->data = (rwaDecoder *)connect->unk00;
    return obj;
}
