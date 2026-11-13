/* PTankAtomicDestruct (MFL.exe 00545100). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/ptank/rpptank.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
extern RpMaterial *RpMaterialSetTexture(RpMaterial *material, RwTexture *texture);
#define RpGeometryGetMaterialMacro(_geometry,_num) (((_geometry)->matList.materials)[(_num)])
#define RpGeometryGetMaterial(_geometry,_num) RpGeometryGetMaterialMacro(_geometry, _num)
#define RpPTankAtomicSetTextureMacro(atm_,tex_) MACRO_START{RpMaterialSetTexture(RpGeometryGetMaterial(RpAtomicGetGeometry(atm_),0), tex_);}MACRO_STOP
#define RpPTankAtomicSetTexture(atm_,tex_) RpPTankAtomicSetTextureMacro(atm_,tex_)
/* ---- end of the generated declarations ---- */

/* TIER2: unvalidated authority=diffemu */
void * PTankAtomicDestruct(void *object, RwInt32 __RWUNUSED__ offset, RwInt32 __RWUNUSED__ size)
{

    if( NULL != RPATOMICPTANKPLUGINDATA(object) )
    {
        RpPTankAtomicSetTexture((RpAtomic*)object,(RwTexture*)NULL);

        if( NULL != RPATOMICPTANKPLUGINDATA(object)->rawdata )
        {
            RwFree(RPATOMICPTANKPLUGINDATA(object)->rawdata);
        }

        RwFree(RPATOMICPTANKPLUGINDATA(object));
        RPATOMICPTANKPLUGINDATA(object) = NULL;
    }

    return (object);
}
