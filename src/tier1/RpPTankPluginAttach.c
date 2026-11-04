/* compile: /G5 */
/* RpPTankPluginAttach (MFL.exe 0x00545090): RenderWare Graphics 3.7 (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rw/plugin/ptank/rpptank.c, function RpPTankPluginAttach), a byte-identical decompilation of the RenderWare
 * build of an Xbox game; the Win32 branches it kept are RenderWare 3.7.2 source. Used with credit; split into one file per
 * function, the declarations it needs copied from the codfh headers named in the comments (PC build: no _XBOX, no RWDEBUG).
 * Changes: no renames; data definitions of the unit are extern declarations here.
 * Byte-identical to MFL.exe 0x00545090 (tools/cmpfn.py /O2 /G5, relocations masked). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <math.h>
typedef int  RwInt32;
typedef unsigned int RwUInt32;
typedef RwInt32 RwBool;
#define RWFORCEENUMSIZEINT ((RwInt32)((~((RwUInt32)0))>>1))
#define __RWUNUSED__           /* No op */
#define FALSE 0
#define _RWSTRING(x) x
#define RWSTRING(x) _RWSTRING(x)
#define MAKECHUNKID(vendorID, chunkID) (((vendorID & 0xFFFFFF) << 8) | (chunkID & 0xFF))
enum RwPluginVendor
{
    rwVENDORID_CORE             = 0x000000L,
    rwVENDORID_CRITERIONTK      = 0x000001L,
    rwVENDORID_REDLINERACER     = 0x000002L,
    rwVENDORID_CSLRD            = 0x000003L,
    rwVENDORID_CRITERIONINT     = 0x000004L,
    rwVENDORID_CRITERIONWORLD   = 0x000005L,
    rwVENDORID_BETA             = 0x000006L,
    rwVENDORID_CRITERIONRM      = 0x000007L,
    rwVENDORID_CRITERIONRWA     = 0x000008L, /* RenderWare Audio */
    rwVENDORID_CRITERIONRWP     = 0x000009L, /* RenderWare Physics */
    rwPLUGINVENDORFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwPluginVendor RwPluginVendor;
typedef void *(*RwPluginObjectConstructor)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef void *(*RwPluginObjectCopy)(void *dstObject, const void *srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef void *(*RwPluginObjectDestructor)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
extern RwInt32 RwEngineRegisterPlugin(RwInt32 size, RwUInt32 pluginID,
                                  RwPluginObjectConstructor initCB,
                                  RwPluginObjectDestructor termCB);
extern RwInt32
RpAtomicRegisterPlugin(RwInt32 size,
                       RwUInt32 pluginID,
                       RwPluginObjectConstructor constructCB,
                       RwPluginObjectDestructor destructCB,
                       RwPluginObjectCopy copyCB);
#define rwID_PTANKPLUGIN     MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x2f)
#define RWRETURN(value) return(value)
#define RWAPIFUNCTION(name)
extern RwInt32 _rpPTankAtomicDataOffset; /* Offset in RpAtomic */
extern RwInt32 _rpPTankGlobalsOffset; /* Offset in RwEngine */
extern RwBool
RpPTankPluginAttach(void);
extern void        *
PTankOpen(void *object,
                    RwInt32 __RWUNUSED__ offset,
                    RwInt32 __RWUNUSED__ size);
extern void        *
PTankClose(void *object,
                    RwInt32 __RWUNUSED__ offset,
                    RwInt32 __RWUNUSED__ size);
extern RwInt32 _rpPTankAtomicDataOffset;
extern RwInt32 _rpPTankGlobalsOffset;
extern void * PTankAtomicInit(void *object, RwInt32 __RWUNUSED__ offset, RwInt32 __RWUNUSED__ size);
extern void * PTankAtomicDestruct(void *object, RwInt32 __RWUNUSED__ offset, RwInt32 __RWUNUSED__ size);

/**
 * \ingroup rpptank
 * \ref RpPTankPluginAttach is used to attach the PTank plugin
 * to the RenderWare system. The PTank plugin must be attached
 * between initializing the system with \ref RwEngineInit and opening
 * it with \ref RwEngineOpen.
 *
 * \return TRUE on success, FALSE otherwise.
 *
 * \see RwEngineInit
 * \see RwEngineOpen
 * \see RwEngineStart
 */
RwBool
RpPTankPluginAttach(void)
{
    RWAPIFUNCTION(RWSTRING("RpPTankPluginAttach"));

    /* Atomic plugin */
    _rpPTankGlobalsOffset =
        RwEngineRegisterPlugin(sizeof(RwInt32),
                               rwID_PTANKPLUGIN,
                               PTankOpen,
                               PTankClose);

    if( _rpPTankGlobalsOffset >= 0 )
    {
        _rpPTankAtomicDataOffset =
            RpAtomicRegisterPlugin(sizeof(RwInt32),
                                   rwID_PTANKPLUGIN,
                                   PTankAtomicInit,
                                   PTankAtomicDestruct,
                                   (RwPluginObjectCopy)NULL);

        RWRETURN( _rpPTankAtomicDataOffset >= 0 );
    }


    RWRETURN(FALSE);
}
