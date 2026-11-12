/* RpClumpForAllCameras (MFL.exe 004f6850). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baclump.c @e578672,
 * not byte-identical, changes: function cut out of the file by tools/codfh_loop.py, release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), the type / macro definitions it needs copied in from the codfh headers, extern declarations with MFL.exe addresses added. Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define MACRO_START do
#define MACRO_STOP while (0)

typedef struct RpClumpCameraExt RpClumpCameraExt;
typedef struct RwObjectHasFrame RwObjectHasFrame;
typedef struct RwFrustumPlane RwFrustumPlane;
typedef struct RwPlane RwPlane;
typedef struct RwLinkList RwLinkList;
typedef RwCamera   *(*RwCameraBeginUpdateFunc) (RwCamera * camera);
typedef RwCamera   *(*RwCameraEndUpdateFunc) (RwCamera * camera);
typedef RwObjectHasFrame * (*RwObjectHasFrameSyncFunction)(RwObjectHasFrame *object);
struct RwObject
{
        RwUInt8 type;                
        RwUInt8 subType;             
        RwUInt8 flags;               
        RwUInt8 privateFlags;        
        void   *parent;              
                                     
};
typedef struct RwLLLink  RwLLLink;
struct RwLLLink
{
    RwLLLink *next;
    RwLLLink *prev;
};
struct RwObjectHasFrame
{
    RwObject                     object;
    RwLLLink                     lFrame;
    RwObjectHasFrameSyncFunction sync;
};
struct RwPlane
{
    RwV3d normal;    
    RwReal distance; 
};
struct RwFrustumPlane
{
        RwPlane             plane;
        RwUInt8             closestX;
        RwUInt8             closestY;
        RwUInt8             closestZ;
        RwUInt8             pad;
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
typedef short RwInt16;
struct RwRaster
{
    RwRaster           *parent; 
    RwUInt8            *cpPixels; 
    RwUInt8            *palette; 
    RwInt32             width, height, depth; 
    RwInt32             stride; 
    RwInt16             nOffsetX, nOffsetY; 
    RwUInt8             cType;  
    RwUInt8             cFlags; 
    RwUInt8             privateFlags; 
    RwUInt8             cFormat; 

    RwUInt8            *originalPixels;
    RwInt32             originalWidth;
    RwInt32             originalHeight;
    RwInt32             originalStride;
};
struct RwCamera
{
        RwObjectHasFrame    object;

        
        RwCameraProjection  projectionType;

        
        RwCameraBeginUpdateFunc beginUpdate;
        RwCameraEndUpdateFunc endUpdate;

        
        RwMatrix            viewMatrix;

        
        RwRaster           *frameBuffer;

        
        RwRaster           *zBuffer;

        
        RwV2d               viewWindow;
        RwV2d               recipViewWindow;
        RwV2d               viewOffset;
        RwReal              nearPlane;
        RwReal              farPlane;
        RwReal              fogPlane;

        
        RwReal              zScale, zShift;

        
        RwFrustumPlane      frustumPlanes[6];
        RwBBox              frustumBoundBox;

        
        RwV3d               frustumCorners[8];
};
typedef RwCamera *(*RwCameraCallBack)(RwCamera *camera, void *data);
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
struct RpClumpCameraExt
{
    RpClump            *clump;
    RwLLLink            inClumpLink;
};
/*007d2128*/ extern void *RwEngineInstance;

/*007d535c*/ extern undefined4 _rpClumpCameraExtOffset;

/* TIER2: unvalidated authority=diffemu */
/* TIER2: validated 2026-11-12 authority=diffemu */
RpClump * RpClumpForAllCameras(RpClump * clump, RwCameraCallBack callback, void *pData)
{
    RwLLLink           *cur, *end, *next;

    cur = rwLinkListGetFirstLLLink(&clump->cameraList);
    end = rwLinkListGetTerminator(&clump->cameraList);

    while (cur != end)
    {
        RwCamera *camera = (RwCamera *) ( (RwUInt8 *)(cur) -
            offsetof(RpClumpCameraExt, inClumpLink) - _rpClumpCameraExtOffset);

        next = rwLLLinkGetNext(cur);

        if (!callback(camera, pData))
        {

            return (clump);
        }

        cur = next;
    }

    return (clump);
}
