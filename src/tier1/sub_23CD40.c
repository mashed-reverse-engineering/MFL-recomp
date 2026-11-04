/* sub_23CD40 (MFL.exe 0x005b9630): RenderWare Audio (Criterion Software), 
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_23CC10.c, function sub_23CD40), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466; MFL.exe was built with cl 13.10). Used with
 * credit; split into one file per function. Changes: function names (none), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005b9630 (tools/cmpfn.py, relocations masked). */
/* compile: cl1310 */
#include "rwa2.h"
#include "rwaint2.h"
#define RwaV3dSubMacro(o, a, b)                                  \
do                                                     \
{                                                               \
    (o)->x = (((a)->x) - ( (b)->x));                            \
    (o)->y = (((a)->y) - ( (b)->y));                            \
    (o)->z = (((a)->z) - ( (b)->z));                            \
}                                                               \
while(0)
#define RwaV3dDotProductMacro(a, b)                              \
    ((((( (((a)->x) * ((b)->x))) +                              \
        ( (((a)->y) * ((b)->y))))) +                            \
        ( (((a)->z) * ((b)->z)))))
#define RwaListenerPreviousPositionMacro(o,a,b) do { \
 (o)->x = ((*(const volatile RwReal *)&(a)->x) - ( *(const volatile RwReal *)&(b)->x)); \
 (o)->y = (((a)->y) - ( (b)->y)); \
 (o)->z = (((a)->z) - ( (b)->z)); \
 } while(0)
extern void sub_242050(RwReal *length, const RwaV3d *vector);
extern RwReal RwV3dNormalize(RwaV3d *out, const RwaV3d *in);

void
sub_23CD40(rwaInstance **listeners, RwUInt32 count,
           const rwa3DSource *source, RwReal *gains, RwUInt32 mode)
{
    rwaListenerState *listener = &((rwaListener *)*listeners)->state;
    RwReal doppler = *(RwReal *)&listener->unk6C;
    RwReal rolloff = *(RwReal *)&listener->unk64;
    const RwaV3d *at = &listener->at;
    RwReal velocityScale = *(RwReal *)&listener->unk68;
    RwaV3d direction, previousDirection;
    RwaV3d listenerPrevious, sourcePrevious;
    RwReal distance, previousDistance;
    RwReal delta, speed;


    RwaListenerPreviousPositionMacro(&listenerPrevious, &listener->pos, &listener->vel);
    RwaV3dSubMacro(&sourcePrevious, &source->position, &source->velocity);
    RwaV3dSubMacro(&previousDirection, &sourcePrevious, &listenerPrevious);
    sub_242050(&previousDistance, &previousDirection);

    RwaV3dSubMacro(&direction, &source->position, &listener->pos);
    sub_242050(&distance, &direction);
    delta = distance - previousDistance;
    if(delta > 150.0f)delta = 150.0f;
    else if(delta < -150.0f)delta = -150.0f;
    speed = doppler * 340.0f;
    gains[1] = (speed - delta * velocityScale) / speed;
    if(distance > source->maxDistance)gains[0] = 0.0f;
    else if(distance < source->minDistance)gains[0] = 1.0f;
    else
    {
        RwReal range;
        distance -= source->minDistance;
        range = source->maxDistance - source->minDistance;
        gains[0] = (range - distance) / (distance * rolloff + range);
    }

    if(direction.x == 0.0f && direction.y == 0.0f && direction.z == 0.0f)
        direction = *at;
    else RwV3dNormalize(&direction, &direction);
    if(previousDirection.x == 0.0f && previousDirection.y == 0.0f && previousDirection.z == 0.0f)
        previousDirection = *at;
    else RwV3dNormalize(&previousDirection, &previousDirection);

    gains[2] = 0.5f - RwaV3dDotProductMacro(&direction, &listener->right) * 0.5f;
    gains[2] = gains[2] < 0.0f ? 0.0f : gains[2] > 1.0f ? 1.0f : gains[2];
    gains[3] = 0.5f - RwaV3dDotProductMacro(&direction, at) * 0.5f;
    gains[3] = gains[3] < 0.0f ? 0.0f : gains[3] > 1.0f ? 1.0f : gains[3];
    gains[4] = 0.5f - RwaV3dDotProductMacro(&previousDirection, &listener->right) * 0.5f;
    gains[4] = gains[4] < 0.0f ? 0.0f : gains[4] > 1.0f ? 1.0f : gains[4];
    gains[5] = 0.5f - RwaV3dDotProductMacro(&previousDirection, at) * 0.5f;
    gains[5] = gains[5] < 0.0f ? 0.0f : gains[5] > 1.0f ? 1.0f : gains[5];
    switch(mode)
    {
    case 2:
        gains[2] -= 0.5f;
        gains[4] -= 0.5f;
        gains[2] *= 0.0f;
        gains[4] *= 0.0f;
        gains[2] += 0.5f;
        gains[4] += 0.5f;
        break;
    case 3:
        gains[2] -= 0.5f;
        gains[4] -= 0.5f;
        gains[2] *= 0.5f;
        gains[4] *= 0.5f;
        gains[2] += 0.5f;
        gains[4] += 0.5f;
        break;
    }
}
