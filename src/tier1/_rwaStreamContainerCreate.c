/* _rwaStreamContainerCreate (MFL.exe 0x005c1820): RenderWare Audio (Criterion Software), rwa.dll 0.96
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_2452A0.c, function sub_246BD0), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_245DC0 -> _rwaStreamContainerStateCreate; sub_246BD0 -> _rwaStreamContainerCreate), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c1820 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern rwaStreamPlayer * _rwaStreamContainerStateCreate(rwaStreamPlayer *player, rwaNodeSet *set, rwaContainerCommand *command, RwUInt32 a4, RwUInt32 a5, void *a6, void *a7, void *a8);

rwaStreamContainer *
_rwaStreamContainerCreate(rwaStreamContainer *container, RwUInt32 a2, RwUInt32 a3, void *a4, void *a5, void *a6)
{
    rwaStreamContainer *result = (rwaStreamContainer *)_rwaStreamContainerStateCreate(&container->player,
                                                                  &container->set,
                                                                  &container->command,
                                                                  a2, a3, a4, a5, a6);

    if (result)
    {
        result->states[1] = 0;
        result->states[2] = 0;
        result->states[0] = 0;
    }

    return result;
}
