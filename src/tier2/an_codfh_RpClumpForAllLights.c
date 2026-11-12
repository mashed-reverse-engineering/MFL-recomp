/* RpClumpForAllLights (MFL.exe 004f68a0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baclump.c @e578672,
 * not byte-identical, changes: function cut out of the file by tools/codfh_loop.py, release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), the type / macro definitions it needs copied in from the codfh headers, extern declarations with MFL.exe addresses added. Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define MACRO_START do
#define MACRO_STOP while (0)

typedef struct RpClumpLightExt RpClumpLightExt;
typedef struct RwObjectHasFrame RwObjectHasFrame;
typedef struct RwLinkList RwLinkList;
typedef struct RwLLLink  RwLLLink;
struct RwLLLink
{
    RwLLLink *next;
    RwLLLink *prev;
};
#define rwLLLinkGetNext(linkvar)                                        \
    ((linkvar)->next)
#define rwLinkListGetFirstLLLink(list)                                  \
    ((list)->link.next)
#define rwLinkListGetTerminator(list)                                   \
    (&((list)->link))
#define offsetof(type, member)                                          \
    ((size_t)((RwUInt8 *)&((type *) 0)->member - (RwUInt8 *)((type *) 0)))
typedef RpClump    *(*RpClumpCallBack) (RpClump * clump, void *data);
typedef RwObjectHasFrame * (*RwObjectHasFrameSyncFunction)(RwObjectHasFrame *object);
struct RwObject
{
        RwUInt8 type;                
        RwUInt8 subType;             
        RwUInt8 flags;               
        RwUInt8 privateFlags;        
        void   *parent;              
                                     
};
struct RwObjectHasFrame
{
    RwObject                     object;
    RwLLLink                     lFrame;
    RwObjectHasFrameSyncFunction sync;
};
struct RwLinkList
{
    RwLLLink link;
};
struct RpClump
{
        RwObject            object;

        
        RwLinkList          atomicList;

        
        RwLinkList          lightList;
        RwLinkList          cameraList;

        
        RwLLLink            inWorldLink;

        
        RpClumpCallBack     callback;
};
struct RpClumpLightExt
{
    RpClump            *clump;
    RwLLLink            inClumpLink;
};
struct RpLight
{
        RwObjectHasFrame    object; 
        RwReal              radius; 
        RwRGBAReal          color;   
        RwReal              minusCosAngle;   
        RwLinkList          WorldSectorsInLight; 
        RwLLLink            inWorld; 
        RwUInt16            lightFrame; 
        RwUInt16            pad;
};
typedef RpLight    *(*RpLightCallBack) (RpLight * light, void *data);
/*007d2128*/ extern void *RwEngineInstance;

/*007d5360*/ extern undefined4 _rpClumpLightExtOffset;

/* TIER2: unvalidated authority=diffemu */
/* TIER2: validated 2026-11-12 authority=diffemu */
RpClump * RpClumpForAllLights(RpClump * clump, RpLightCallBack callback, void *pData)
{
    RwLLLink           *cur, *end, *next;

    cur = rwLinkListGetFirstLLLink(&clump->lightList);
    end = rwLinkListGetTerminator(&clump->lightList);

    while (cur != end)
    {
        RpLight *light = (RpLight *) ((RwUInt8 *)(cur) -
            offsetof(RpClumpLightExt, inClumpLink) - _rpClumpLightExtOffset);

        next = rwLLLinkGetNext(cur);

        if (!callback(light, pData))
        {

            return (clump);
        }

        cur = next;
    }

    return (clump);
}
