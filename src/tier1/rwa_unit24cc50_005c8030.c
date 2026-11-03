/* rwa_unit24cc50_005c8030 (MFL.exe 0x005c8030): RenderWare Audio (Criterion Software), neutral (codfh sub_24CC80)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_24CC50.c, function sub_24CC80), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_24CC80 -> rwa_unit24cc50_005c8030), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c8030 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern rwaSplitter *rwa_unit24cc50_005c8030(rwaSplitter *obj);

/* Source shape: rwad.dll _RwaStreamFanOutStartExecute, 0x10067D61.
 * Keep the incoming object and typed local separate. */
rwaSplitter *
rwa_unit24cc50_005c8030(rwaSplitter *arg)
{
    rwaSplitter *obj = arg;
    if ((RwInt32)obj->state.node.numCreated > 0)
    {
        RwInt32     mode = obj->state.node.mode;

        switch (obj->state.node.status)
        {
            case 3:
                if (mode == 3 || mode == 2)
                {
                    obj->state.node.status = 4;
                }
                break;
            case 4:
            case 5:
            case 6:
                if (obj->state.starved)
                {
                    obj->state.node.status = 7;
                }
                if (mode == 3)
                {
                    obj->state.node.status = 4;
                }
                else if (mode == 1)
                {
                    obj->state.node.status = 3;
                }
                break;
            case 7:
                if (!obj->state.starved && mode == 2)
                {
                    obj->state.node.status = 6;
                }
                if (mode == 3)
                {
                    obj->state.node.status = 4;
                }
                if (mode == 1)
                {
                    obj->state.node.status = 3;
                }
                break;
            case 8:
                obj->state.node.status = 3;
                break;
        }
    }

    return obj;
}
