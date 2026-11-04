/* compile: /G5 */
/* RpLightSetConeAngle (MFL.exe 0x004f4cb0): RenderWare Graphics 3.7 (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rw/world/balight.c, function RpLightSetConeAngle), a byte-identical decompilation of the RenderWare
 * build of an Xbox game; the Win32 branches it kept are RenderWare 3.7.2 source. Used with credit; split into one file per
 * function, the declarations it needs copied from the codfh headers named in the comments (PC build: no _XBOX, no RWDEBUG).
 * Changes: no renames; data definitions of the unit are extern declarations here. P7 fix: rpLIGHTMAXCONEANGLE is rwPI/2 in
 * MFL's build (the original compares against 0x3fc90fdb = pi/2; codfh's Xbox header has rwPI).
 * Byte-identical to MFL.exe 0x004f4cb0 (tools/cmpfn.py /O2 /G5, relocations masked). */
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>
typedef int  RwInt32;
typedef unsigned int RwUInt32;
typedef unsigned short RwUInt16;
typedef unsigned char RwUInt8;
typedef float RwReal;
#define RwCos(_x)               cos(_x)
#define RWFORCEENUMSIZEINT ((RwInt32)((~((RwUInt32)0))>>1))
#define _RWSTRING(x) x
#define RWSTRING(x) _RWSTRING(x)
typedef struct RwLLLink  RwLLLink;                     /*** RwLLLink ***/
struct RwLLLink
{
    RwLLLink *next;
    RwLLLink *prev;
};
typedef struct RwLinkList RwLinkList;
struct RwLinkList
{
    RwLLLink link;
};
#define rwPI ((RwReal)(3.1415926535f))
typedef struct RwObject RwObject;
struct RwObject
{
        RwUInt8 type;                /**< Internal Use */
        RwUInt8 subType;             /**< Internal Use */
        RwUInt8 flags;               /**< Internal Use */
        RwUInt8 privateFlags;        /**< Internal Use */
        void   *parent;              /**< Internal Use */
                                     /* Often a Frame  */
};
enum RwDebugType
{
    rwNADEBUGTYPE = 0,          /**<Invalid */
    rwDEBUGASSERT,              /**<Send an assert message */
    rwDEBUGERROR,               /**<Send an error message */
    rwDEBUGMESSAGE,             /**<Send an informational message */
    rwDEBUGTRACE,               /**<Send a trace message */
    rwDEBUGTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwDebugType RwDebugType;
#define RwDebugSendMessage(type, funcName, message)
typedef struct RwRGBAReal RwRGBAReal;
struct RwRGBAReal
{
    RwReal red;     /**< red component */
    RwReal green;   /**< green component */
    RwReal blue;    /**< blue component */
    RwReal alpha;   /**< alpha component */
};
#define RWASSERTISTYPE(_f, _t) \
   RWASSERT((((const RwObject *)(_f))->type)==(_t))
typedef struct RwModuleInfo RwModuleInfo;
struct RwModuleInfo
{
        RwInt32     globalsOffset;
        RwInt32     numInstances;
};
typedef struct RwObjectHasFrame RwObjectHasFrame;
typedef RwObjectHasFrame * (*RwObjectHasFrameSyncFunction)(RwObjectHasFrame *object);
struct RwObjectHasFrame
{
    RwObject                     object;
    RwLLLink                     lFrame;
    RwObjectHasFrameSyncFunction sync;
};
#define rpLIGHT 3
#define rpLIGHTMINCONEANGLE ((RwReal)0.0f)
#define rpLIGHTMAXCONEANGLE (rwPI/2)
typedef struct RpLight RpLight;
struct RpLight
{
        RwObjectHasFrame    object; /**< object */
        RwReal              radius; /**< radius */
        RwRGBAReal          color; /**< color */  /* Light color */
        RwReal              minusCosAngle; /**< minusCosAngle */  
        RwLinkList          WorldSectorsInLight; /**< WorldSectorsInLight */
        RwLLLink            inWorld; /**< inWorld */
        RwUInt16            lightFrame; /**< lightFrame */
        RwUInt16            pad;
};
extern RpLight *RpLightSetConeAngle(RpLight * ight, RwReal angle);
#define RWRETURN(value) return(value)
#define RWAPIFUNCTION(name)
#define RWASSERT(condition)
extern RwModuleInfo lightModule;

/**
 * \ingroup rplight
 * \ref RpLightSetConeAngle is used to specify the cone angle at which a
 * spot light illuminates objects (measured in radians from the direction
 * vector of the light). This function is only valid for lights of type
 * rpLIGHTTYPESPOT or rpLIGHTTYPESPOTSOFT.
 *
 * The default cone angle is zero. The maximum allowed cone angle (this
 * value being platform-dependent) is given by rpLIGHTMAXCONEANGLE.
 *
 * The world plugin must be attached before using this function.
 *
 * \param light  Pointer to the light.
 * \param angle  A RwReal value equal to the cone angle (in radians).
 *
 * \return Returns pointer to the light if successful or NULL if there
 * is an error.
 *
 * \see RpLightGetConeAngle
 * \see RpLightSetRadius
 * \see RpLightGetRadius
 * \see RpLightSetColor
 * \see RpLightGetColor
 * \see RpWorldPluginAttach
 *
 */
RpLight            *
RpLightSetConeAngle(RpLight * light, RwReal angle)
{
    RwReal minusCosAngle;

    RWAPIFUNCTION(RWSTRING("RpLightSetConeAngle"));
    RWASSERT(lightModule.numInstances);
    RWASSERT(light);
    RWASSERTISTYPE(light, rpLIGHT);

    if ((angle < rpLIGHTMINCONEANGLE) ||
        (angle > rpLIGHTMAXCONEANGLE) )
    {
        /* Might this bloat use code w/ platform-predication?
         * NO: if behaviour needs to be the same on two platforms,
         * then they need to use the smallest valid cone angle for both.
         * If behaviour is different, behaviour is different :) */
        RwDebugSendMessage(rwDEBUGERROR,
                           "RpLightSetConeAngle",
                           "Light cone angle invalid (on this platform).");
        RWRETURN((RpLight *)NULL);
    }


    minusCosAngle = - (RwReal) RwCos(angle);
    light->minusCosAngle = minusCosAngle;

    RWRETURN(light);
}
