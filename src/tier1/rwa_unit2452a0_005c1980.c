/* rwa_unit2452a0_005c1980 (MFL.exe 0x005c1980): RenderWare Audio (Criterion Software), neutral (codfh sub_246D20)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_2452A0.c, function sub_246D20), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_246D20 -> rwa_unit2452a0_005c1980), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c1980 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern void sub_245630(rwaStreamPlayer *player, void *unused, RwInt32 *states);
extern RwBool sub_246C20(rwaStreamPlayer *player, rwaNodeSet *set, rwaContainerCommand *command);

RwBool
rwa_unit2452a0_005c1980(rwaStreamContainer *container)
{
    RwBool      ready = sub_246C20(&container->player, &container->set, &container->command);

    sub_245630(&container->player, &container->set, container->states);
    return ready;
}
