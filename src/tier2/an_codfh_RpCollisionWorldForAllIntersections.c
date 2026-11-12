/* RpCollisionWorldForAllIntersections (MFL.exe 00549290). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/collis/collquery.c @e578672,
 * not byte-identical, changes: function cut out of the file by tools/codfh_loop.py, release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), the type / macro definitions it needs copied in from the codfh headers, extern declarations with MFL.exe addresses added. Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

typedef struct RpIntersection RpIntersection;
typedef union RpIntersectData RpIntersectData;
typedef struct RpCollisionTriangle RpCollisionTriangle;
typedef struct RwLine RwLine;
typedef enum RpIntersectType RpIntersectType;
struct RpCollisionTriangle
{
    RwV3d      normal; 
    RwV3d      point;  
    RwInt32    index;  
    RwV3d      *vertices[3]; 
};
struct RwLine
{
    RwV3d start;    
    RwV3d end;      
};
union RpIntersectData
{
    RwLine              line;       
    RwV3d               point;      
    RwSphere            sphere;     
    RwBBox              box;        
    void               *object;     

};
#define RWFORCEENUMSIZEINT ((RwInt32)((~((RwUInt32)0))>>1))
enum RpIntersectType
{
    rpINTERSECTNONE = 0,
    rpINTERSECTLINE,   
    rpINTERSECTPOINT,  
    rpINTERSECTSPHERE, 
    rpINTERSECTBOX,    
    rpINTERSECTATOMIC, 
    rpINTERSECTTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
struct RpIntersection
{
    RpIntersectData     t; 

    RpIntersectType     type; 

};
typedef RpCollisionTriangle * (*RpIntersectionCallBackWorldTriangle)
    (RpIntersection * intersection,
     RpWorldSector * sector,
     RpCollisionTriangle * collTriangle, RwReal distance, void *data);
/*007d2128*/ extern void *RwEngineInstance;

/*004f6240*/ extern struct RwSphere * RpAtomicGetWorldBoundingSphere(struct RpAtomic *p0);
/*00549f10*/ extern undefined __cdecl WorldForAllTriIntsecBBox(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*00549370*/ extern undefined __cdecl WorldForAllTriIntsecLine(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*0054a4d0*/ extern undefined WorldForAllTriIntsecSphere();

/* TIER2: unvalidated authority=diffemu */
/* TIER2: validated 2026-11-12 authority=diffemu */
RpWorld * RpCollisionWorldForAllIntersections(RpWorld *world, RpIntersection *intersection, RpIntersectionCallBackWorldTriangle callBack, void *data)
{

    switch (intersection->type)
    {
    case rpINTERSECTLINE:
        {
            
            WorldForAllTriIntsecLine(
                world, intersection, callBack, data);
            return (world);
        }

    case rpINTERSECTSPHERE:
        {
            WorldForAllTriIntsecSphere(
                world, &intersection->t.sphere, intersection, callBack, data);
            return (world);
        }

    case rpINTERSECTBOX:
        {
            WorldForAllTriIntsecBBox(
                world, intersection, callBack, data);
            return (world);
        }

    case (rpINTERSECTATOMIC):
        {
            RpAtomic *atomic = (RpAtomic *)intersection->t.object;
            RwSphere sphere;

            sphere = *RpAtomicGetWorldBoundingSphere(atomic);

            WorldForAllTriIntsecSphere(
                world, &sphere, intersection, callBack, data);
            return (world);
        }

    default:
        {

            return (world);
        }
    }
}
