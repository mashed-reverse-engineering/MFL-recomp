/* sub_23CC70 (MFL.exe 0x005b9560): RenderWare Audio (Criterion Software), 
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_23CC10.c, function sub_23CC70), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466; MFL.exe was built with cl 13.10). Used with
 * credit; split into one file per function. Changes: function names (none), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005b9560 (tools/cmpfn.py, relocations masked). */
/* compile: cl1310 */
#include "rwa.h"
#include "rwaint.h"
#define RwaV3dCrossProductMacro(o, a, b)                         \
do                                                     \
{                                                               \
    (o)->x =                                                    \
        (( (((a)->y) * ( (b)->z))) -                            \
         ( (((a)->z) * ( (b)->y))));                            \
    (o)->y =                                                    \
        (( (((a)->z) * ( (b)->x))) -                            \
         ( (((a)->x) * ( (b)->z))));                            \
    (o)->z =                                                    \
        (( (((a)->x) * ( (b)->y))) -                            \
         ( (((a)->y) * ( (b)->x))));                            \
}                                                               \
while(0)
extern rwaListener *sub_23CC70(rwaListener *listener);

rwaListener *
sub_23CC70(rwaListener *listener)
{
    RwaV3d  v;

    v.x = 0.0f;
    v.y = 0.0f;
    v.z = 0.0f;
    listener->pos = v;
    listener->vel = v;

    v.z = v.x = listener->at.y = listener->at.x = 0.0f;
    v.y = listener->at.z = 1.0f;
    listener->up = v;

    RwaV3dCrossProductMacro(&listener->right, &listener->up, &listener->at);

    listener->unk68 = 0x3F800000;
    listener->unk6C = 0x3F800000;
    listener->unk64 = 0x41200000;
    listener->dirty = 0x7B;

    return listener;
}
