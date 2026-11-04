/* compile: /G5 */
/* RpLightSetColor (MFL.exe 0x004f4a20): RenderWare Graphics 3.7 (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rw/world/balight.c, function RpLightSetColor), a byte-identical decompilation of the RenderWare
 * build of an Xbox game; the Win32 branches it kept are RenderWare 3.7.2 source. Used with credit; split into one file per
 * function, the declarations it needs copied from the codfh headers named in the comments (PC build: no _XBOX, no RWDEBUG).
 * Changes: no renames; data definitions of the unit are extern declarations here.
 * Byte-identical to MFL.exe 0x004f4a20 (tools/cmpfn.py /O2 /G5, relocations masked). */
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
#define rwObjectSetPrivateFlags(c,f)        (((RwObject *)(c))->privateFlags) = (RwUInt8)(f)
typedef struct RwRGBAReal RwRGBAReal;
struct RwRGBAReal
{
    RwReal red;     /**< red component */
    RwReal green;   /**< green component */
    RwReal blue;    /**< blue component */
    RwReal alpha;   /**< alpha component */
};
#define RwRGBARealAssign(_target, _source)        \
    ( *(_target) = *(_source) )
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
enum rpLightPrivateFlag
{
    rpLIGHTPRIVATENOCHROMA = 0x01,
    rpLIGHTPRIVATEFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum rpLightPrivateFlag rpLightPrivateFlag;
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
extern RpLight *RpLightSetColor(RpLight *light, const RwRGBAReal *color);
#define RWRETURN(value) return(value)
#define RWAPIFUNCTION(name)
#define RWASSERT(condition)
extern RwModuleInfo lightModule;

/**
 * \ingroup rplight
 * \ref RpLightSetColor is used to specify the color of the given light.
 * Light colors are specified as real values within the range 0 to 1 in
 * each color channel.
 *
 * The default color is white (red = 1.0, green = 1.0, blue = 1.0).
 *
 * The world plugin must be attached before using this function.
 *
 * \param light  Pointer to the light
 * \param color  A pointer to a RwRGBAReal value equal to the light's color.
 *
 * \return Returns pointer to the light if successful or NULL if there
 * is an error.
 *
 * \see RpLightGetColor
 * \see RpLightSetRadius
 * \see RpLightGetRadius
 * \see RpLightSetConeAngle
 * \see RpLightGetConeAngle
 * \see RpWorldPluginAttach
 *
 */
RpLight            *
RpLightSetColor(RpLight * light, const RwRGBAReal * color)
{
    RWAPIFUNCTION(RWSTRING("RpLightSetColor"));
    RWASSERT(lightModule.numInstances);
    RWASSERT(light);
    RWASSERTISTYPE(light, rpLIGHT);
    RWASSERT(color);

    RwRGBARealAssign(&light->color, color);

    if ((light->color.red == light->color.green) &&
        (light->color.red == light->color.blue))
    {
        /* It's a shade of grey, so set the 'grey' flag (no chroma) */
        rwObjectSetPrivateFlags(light, rpLIGHTPRIVATENOCHROMA);
    }
    else
    {
        /* It's got color, turn off the optimisation */
        rwObjectSetPrivateFlags(light, 0);
    }

    RWRETURN(light);
}
