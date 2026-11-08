/* compile: /G5 */
/* RpAtomicCreate (MFL.exe 0x004f68f0): RenderWare Graphics 3.7 (Criterion Software), codfh name, P7b near miss
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rw/world/baclump.c, function RpAtomicCreate), a byte-identical decompilation of the RenderWare
 * build of an Xbox game; the Win32 branches it kept are RenderWare 3.7.2 source. Used with credit; split into one file per
 * function, the declarations it needs copied from the codfh headers named in the comments (PC build: no _XBOX, no RWDEBUG).
 * Changes: no renames; data definitions of the unit are extern declarations here.
 * P7 delta classes for MFL's PC build: RWALIGN is empty (no __declspec(align(16))), RwGlobals has no RW 3.6 fileFuncs table,
 * Rt2dGlobals has no codfh `unknown` field; P7b: RenderWare library version 3.7.0.2 (stream range 0x35000..0x37002), static same-unit
 * callees the code calls or takes the address of are extern (bound to the MFL function by name / alias), not copied.
 * Byte-identical to MFL.exe 0x004f68f0 (tools/cmpfn.py /O2 /G5, relocations masked). */
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>
typedef int  RwInt32;
typedef unsigned int RwUInt32;
typedef short RwInt16;
typedef unsigned short RwUInt16;
typedef unsigned char RwUInt8;
typedef char RwChar;
typedef float RwReal;
typedef RwInt32 RwBool;
#define RWALIGN(type, x)   type   /* P7: no alignment in MFL's build */
#define rwMATRIXALIGNMENT 16
#define rwFRAMEALIGNMENT 16
#define RWFORCEENUMSIZEINT ((RwInt32)((~((RwUInt32)0))>>1))
#define MACRO_START do
#define MACRO_STOP while(0)
#define FALSE 0
#define TRUE !FALSE
#define _RWSTRING(x) x
#define RWSTRING(x) _RWSTRING(x)
typedef struct RwV3d RwV3d;
struct RwV3d
{
    RwReal x;   /**< X value */
    RwReal y;   /**< Y value */
    RwReal z;   /**< Z value */
};
typedef struct RwSphere RwSphere;
struct RwSphere
{
    RwV3d center;   /**< Sphere center */
    RwReal radius;  /**< Sphere radius */
};
#define rwMAXTEXTURECOORDS 8
typedef struct RwTexCoords RwTexCoords;
struct RwTexCoords
{
    RwReal u;   /**< U value */
    RwReal v;   /**< V value */
};
typedef struct RwLLLink  RwLLLink;                     /*** RwLLLink ***/
struct RwLLLink
{
    RwLLLink *next;
    RwLLLink *prev;
};
#define rwLLLinkInitialize(linkvar)                                     \
    ( (linkvar)->prev = (RwLLLink *)NULL,                               \
      (linkvar)->next = (RwLLLink *)NULL )
typedef struct RwLinkList RwLinkList;
struct RwLinkList
{
    RwLLLink link;
};
#define rwLinkListInitialize(list)                                      \
    ( (list)->link.next = ((RwLLLink *)(list)),                         \
      (list)->link.prev = ((RwLLLink *)(list)) )
typedef struct RwSurfaceProperties RwSurfaceProperties;
struct RwSurfaceProperties
{
    RwReal ambient;   /**< ambient reflection coefficient */
    RwReal specular;  /**< specular reflection coefficient */
    RwReal diffuse;   /**< reflection coefficient */
};
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
enum RwCorePluginID
{
    rwID_NAOBJECT               = MAKECHUNKID(rwVENDORID_CORE, 0x00),
    rwID_STRUCT                 = MAKECHUNKID(rwVENDORID_CORE, 0x01),
    rwID_STRING                 = MAKECHUNKID(rwVENDORID_CORE, 0x02),
    rwID_EXTENSION              = MAKECHUNKID(rwVENDORID_CORE, 0x03),

    rwID_CAMERA                 = MAKECHUNKID(rwVENDORID_CORE, 0x05),
/**< RwCamera chunk. See \ref RwCameraStreamRead */

    rwID_TEXTURE                = MAKECHUNKID(rwVENDORID_CORE, 0x06),
/**< RwTexture chunk. See \ref RwTextureStreamRead */

    rwID_MATERIAL               = MAKECHUNKID(rwVENDORID_CORE, 0x07),
/**< RpMaterial chunk. See \ref RpMaterialStreamRead. */

    rwID_MATLIST                = MAKECHUNKID(rwVENDORID_CORE, 0x08),
    rwID_ATOMICSECT             = MAKECHUNKID(rwVENDORID_CORE, 0x09),
    rwID_PLANESECT              = MAKECHUNKID(rwVENDORID_CORE, 0x0A),

    rwID_WORLD                  = MAKECHUNKID(rwVENDORID_CORE, 0x0B),
/**< RpWorld chunk. See \ref RpWorldStreamRead. */

    rwID_SPLINE                 = MAKECHUNKID(rwVENDORID_CORE, 0x0C),
/**< RpSpline chunk. See \ref RpSplineStreamRead */

    rwID_MATRIX                 = MAKECHUNKID(rwVENDORID_CORE, 0x0D),
/**< RwMatrix chunk. See \ref RwMatrixStreamRead */

    rwID_FRAMELIST              = MAKECHUNKID(rwVENDORID_CORE, 0x0E),

    rwID_GEOMETRY               = MAKECHUNKID(rwVENDORID_CORE, 0x0F),
/**< RpGeometry chunk. See \ref RpGeometryStreamRead. */

    rwID_CLUMP                  = MAKECHUNKID(rwVENDORID_CORE, 0x10),
/**< RpClump chunk. See \ref RpClumpStreamRead. */

    rwID_LIGHT                  = MAKECHUNKID(rwVENDORID_CORE, 0x12),
/**< RpLight chunk. See \ref RpLightStreamRead. */

    rwID_UNICODESTRING          = MAKECHUNKID(rwVENDORID_CORE, 0x13),

    rwID_ATOMIC                 = MAKECHUNKID(rwVENDORID_CORE, 0x14),
/**< RpAtomic chunk. See \ref RpAtomicStreamRead */

    rwID_TEXTURENATIVE          = MAKECHUNKID(rwVENDORID_CORE, 0x15),

    rwID_TEXDICTIONARY          = MAKECHUNKID(rwVENDORID_CORE, 0x16),
/**< RwTexDictionary - platform specific texture dictionary.
 * See \ref RwTexDictionaryStreamRead. */

    rwID_ANIMDATABASE           = MAKECHUNKID(rwVENDORID_CORE, 0x17),

    rwID_IMAGE                  = MAKECHUNKID(rwVENDORID_CORE, 0x18),
/**< RwImage chunk. See \ref RwImageStreamRead */

    rwID_SKINANIMATION          = MAKECHUNKID(rwVENDORID_CORE, 0x19),
    rwID_GEOMETRYLIST           = MAKECHUNKID(rwVENDORID_CORE, 0x1A),

    rwID_ANIMANIMATION          = MAKECHUNKID(rwVENDORID_CORE, 0x1B),
/**< RtAnimAnimation chunk. See \ref RtAnimAnimationStreamRead. */
    rwID_HANIMANIMATION         = MAKECHUNKID(rwVENDORID_CORE, 0x1B),
/**< RtAnimAnimation chunk. For backwards compatibility. See \ref rwID_ANIMANIMATION. */

    rwID_TEAM                   = MAKECHUNKID(rwVENDORID_CORE, 0x1C),
/**< \ref RpTeam chunk. See \ref RpTeamStreamRead */
    rwID_CROWD                  = MAKECHUNKID(rwVENDORID_CORE, 0x1D),

    rwID_DMORPHANIMATION        = MAKECHUNKID(rwVENDORID_CORE, 0x1E),
/**< RpDMorphAnimation - delta morph animation chunk. See \ref RpDMorphAnimationStreamRead */

    rwID_RIGHTTORENDER          = MAKECHUNKID(rwVENDORID_CORE, 0x1f),

    rwID_MTEFFECTNATIVE         = MAKECHUNKID(rwVENDORID_CORE, 0x20),
/**< \if xbox RpMTEffect - multi-texture effect chunk. See \ref RpMTEffectStreamRead \endif */
/**< \if gcn  RpMTEffect - multi-texture effect chunk. See \ref RpMTEffectStreamRead \endif */

    rwID_MTEFFECTDICT           = MAKECHUNKID(rwVENDORID_CORE, 0x21),
/**< \if xbox RpMTEffectDict - multi-texture effect dictionary chunk. See \ref RpMTEffectDictStreamRead \endif */
/**< \if gcn  RpMTEffectDict - multi-texture effect dictionary chunk. See \ref RpMTEffectDictStreamRead \endif */

    rwID_TEAMDICTIONARY         = MAKECHUNKID(rwVENDORID_CORE, 0x22),
/**< \ref RpTeamDictionary chunk. See \ref RpTeamDictionaryStreamRead */

    rwID_PITEXDICTIONARY        = MAKECHUNKID(rwVENDORID_CORE, 0x23),
/**< RwTexDictionary - platform independent texture dictionary. See \ref RtPITexDictionaryStreamRead. */

    rwID_TOC                    = MAKECHUNKID(rwVENDORID_CORE, 0x24),
/**< RtTOC chunk. See \ref RtTOCStreamRead */

    rwID_PRTSTDGLOBALDATA       = MAKECHUNKID(rwVENDORID_CORE, 0x25),
/**< RpPrtStdEmitterClass, RpPrtStdParticleClass and RpPrtStdPropertyTable chunks.
 *   See \ref RpPrtStdEClassStreamRead, \ref RpPrtStdPClassStreamRead
 *   \ref RpPrtStdPropTabStreamRead and \ref RpPrtStdGlobalDataStreamRead */

    rwID_ALTPIPE                = MAKECHUNKID(rwVENDORID_CORE, 0x26),
    rwID_PIPEDS                 = MAKECHUNKID(rwVENDORID_CORE, 0x27),
    rwID_PATCHMESH              = MAKECHUNKID(rwVENDORID_CORE, 0x28),
/**< RpPatchMesh chunk. See \ref RpPatchMeshStreamRead */

    rwID_CHUNKGROUPSTART        = MAKECHUNKID(rwVENDORID_CORE, 0x29),
    rwID_CHUNKGROUPEND          = MAKECHUNKID(rwVENDORID_CORE, 0x2A),

    rwID_UVANIMDICT             = MAKECHUNKID(rwVENDORID_CORE, 0x2B),
/**< UV anim dictionary chunk. See \ref RpUVAnimGetDictSchema */

    rwID_COLLTREE               = MAKECHUNKID(rwVENDORID_CORE, 0x2C),

    rwID_ENVIRONMENT            = MAKECHUNKID(rwVENDORID_CORE, 0x2D),
/**< RpEnvironment chunk is used internally to convey art package background
   < settings to the Visualizer.*/    

/* Insert before MAX and increment MAX */
    rwID_COREPLUGINIDMAX        = MAKECHUNKID(rwVENDORID_CORE, 0x2E),
    rwCOREPLUGINIDFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCorePluginID RwCorePluginID ;
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
#define rwObjectInitialize(o, t, s)                     \
MACRO_START                                             \
{                                                       \
    ((RwObject *)(o))->type = (RwUInt8)(t);             \
    ((RwObject *)(o))->subType = (RwUInt8)(s);          \
    ((RwObject *)(o))->flags = 0;                       \
    ((RwObject *)(o))->privateFlags = 0;                \
    ((RwObject *)(o))->parent = NULL;                   \
}                                                       \
MACRO_STOP
#define rwObjectSetFlags(o, f)              (((RwObject *)(o))->flags) = (RwUInt8)(f)
#define rwObjectGetPrivateFlags(c)          (((const RwObject *)(c))->privateFlags)
#define rwObjectSetPrivateFlags(c,f)        (((RwObject *)(c))->privateFlags) = (RwUInt8)(f)
enum RwMemoryHintDuration
{
    rwMEMHINTDUR_NADURATION = 0x00000000,
    /** Is used for allocating some temporary memory which
     *  will be freed inside the function scope.
     */
    rwMEMHINTDUR_FUNCTION   = 0x00010000,
    /** Is used for per frame memory allocations.
     */
    rwMEMHINTDUR_FRAME      = 0x00020000,
    /** Is used for allocations which persist for longer
     *  than per frame, but are not global. This can be per
     *  level or per event.
     */
    rwMEMHINTDUR_EVENT      = 0x00030000,
    /** Is used for all global allocations, happening on and
     *  before RwEngineStart() and which are freed on and after
     *  RwEngineStop()
     */
    rwMEMHINTDUR_GLOBAL     = 0x00040000,
    rwMEMHINTDUR_MASK       = 0x00FF0000,
    rwMEMHINTDURFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwMemoryHintDuration RwMemoryHintDuration;
struct RwMemoryFunctions
{
    /* c.f.
     * Program Files/Microsoft Visual Studio/VC98/Include/MALLOC.H
     */
    void *(*rwmalloc)(size_t size, RwUInt32 hint);
        /**< Allocates memory blocks.
         *  \param size Number of bytes to allocate. Should be greater
         *         then zero.
         *  \param hint A RwUInt32 value representing a memory hint.
         *  \return A void pointer to the allocated space, or NULL if
         *          there is insufficient memory available.
         */
    void  (*rwfree)(void *mem);
        /**< Deallocates or frees a memory block.
         *  \param mem Previously allocated memory block to be freed.
         *         Shouldn't be NULL pointer.
         */
    void *(*rwrealloc)(void *mem, size_t newSize, RwUInt32 hint);
        /**< Reallocate memory blocks.
         *  \param mem Pointer to previously allocated memory block.
         *  \param size New size in bytes. Should be greater then zero.
         *  \param hint A RwUInt32 value representing a memory hint.
         *  \return A void pointer to the allocated space, or NULL if
         *          there is insufficient memory available.
         */
    void *(*rwcalloc)(size_t numObj, size_t sizeObj, RwUInt32 hint);
        /**< Allocates an array in memory with elements initialized to 0.
         *  \param numObj Non-zero number of elements.
         *  \param sizeObj Non-zero length in bytes of each element.
         *  \param hint A RwUInt32 value representing a memory hint.
         *  \return A void pointer to the allocated space, or NULL if
         *          there is insufficient memory available.
         */
};
typedef struct RwMemoryFunctions RwMemoryFunctions;
typedef struct RwFreeList RwFreeList;
struct RwFreeList
{
    RwUInt32   entrySize;       /**<Size of an entry in the free list. */
    RwUInt32   entriesPerBlock; /**<Number of entries per free list block. */
    RwUInt32   heapSize;        /**<Size of the heap. */
    RwUInt32   alignment;       /**<Alignment of a free list entry. */
    RwLinkList blockList;       /**<List of data blocks. */
    RwUInt32   flags;           /**<Flags which affect the behavior of the
                                    free list. <BR>
                                    rwFREELISTFLAG_FREEBLOCKS */
    RwLLLink   link;            /**<Link to the free list linked list. */


//@{ 20050513 DDonSS : Threadsafe
//@} DDonSS
};
typedef void       *(*RwMemoryAllocFn)    (RwFreeList * fl, RwUInt32 hint);
typedef RwFreeList *(*RwMemoryFreeFn)     (RwFreeList * fl, void *pData);
#define RwFreeListAlloc(_f, _h)     \
    RWSRCGLOBAL(memoryAlloc)((_f), (_h))
enum RwStreamType
{
    rwNASTREAM = 0,     /**<Invalid stream type */
    rwSTREAMFILE,       /**<File */
    rwSTREAMFILENAME,   /**<File name */
    rwSTREAMMEMORY,     /**<Memory*/
    rwSTREAMCUSTOM,     /**<Custom */
    rwSTREAMTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwStreamType RwStreamType;
enum RwStreamAccessType
{
    rwNASTREAMACCESS = 0,   /**<Invalid stream access */
    rwSTREAMREAD,           /**<Read */
    rwSTREAMWRITE,          /**<Write */
    rwSTREAMAPPEND,         /**<Append */
    rwSTREAMACCESSTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwStreamAccessType RwStreamAccessType;
typedef struct RwStreamMemory RwStreamMemory;
struct RwStreamMemory
{
    RwUInt32            position; /* Current 'memory' position 0 is first byte */
    RwUInt32            nSize;  /* Space allocated currently */
    RwUInt8            *memBlock; /* Current memory block pointer */
};
typedef union RwStreamFile RwStreamFile;
union RwStreamFile
{
    void    *fpFile;               /**< file pointer */
    const void    *constfpFile;    /**< const file pointer */
};
typedef             RwBool(*rwCustomStreamFnClose) (void *data);
typedef             RwUInt32(*rwCustomStreamFnRead) (void *data, void *buffer,
                                                     RwUInt32 length);
typedef             RwBool(*rwCustomStreamFnWrite) (void *data,
                                                    const void *buffer,

                                                    RwUInt32 length);
typedef             RwBool(*rwCustomStreamFnSkip) (void *data,

                                                   RwUInt32 offset);
typedef struct RwStreamCustom RwStreamCustom;
struct RwStreamCustom
{
    rwCustomStreamFnClose sfnclose;
    rwCustomStreamFnRead sfnread;
    rwCustomStreamFnWrite sfnwrite;
    rwCustomStreamFnSkip sfnskip;
    void               *data;
};
typedef union RwStreamUnion RwStreamUnion;
union RwStreamUnion
{
    RwStreamMemory      memory; /**< memory */
    RwStreamFile        file; /**< file */
    RwStreamCustom      custom; /**< custom */
};
typedef struct RwStream RwStream;
struct RwStream
{
    RwStreamType        type;
    RwStreamAccessType  accessType;
    RwInt32             position;
    RwStreamUnion       Type;
    RwBool              rwOwned;
};
typedef int (*vecSprintfFunc)(RwChar *buffer,
                              const RwChar *format,
                              ...) /* __RWFORMAT__(printf, 2, 3) */;
typedef int (*vecVsprintfFunc)(RwChar *buffer,
                               const RwChar *format,
                               va_list argptr);
typedef RwChar *(*vecStrcpyFunc)(RwChar *dest,
                                 const RwChar *srce);
typedef RwChar *(*vecStrncpyFunc)(RwChar *dest,
                                  const RwChar *srce,
                                  size_t size);
typedef RwChar *(*vecStrcatFunc)(RwChar *dest,
                                 const RwChar *srce);
typedef RwChar *(*vecStrncatFunc)(RwChar *dest,
                                  const RwChar *srce,
                                  size_t size);
typedef RwChar *(*vecStrrchrFunc)(const RwChar *string,
                                  int findThis);
typedef RwChar *(*vecStrchrFunc)(const RwChar *string,
                                 int findThis);
typedef RwChar *(*vecStrstrFunc)(const RwChar *string,
                                 const RwChar *findThis);
typedef int (*vecStrcmpFunc)(const RwChar *string1,
                             const RwChar *string2);
typedef int (*vecStrncmpFunc)(const RwChar *string1,
                              const RwChar *string2,
                              size_t max_size);
typedef int (*vecStricmpFunc)(const RwChar *string1,
                              const RwChar *string2);
typedef size_t (*vecStrlenFunc)(const RwChar *string);
typedef RwChar *(*vecStruprFunc)(RwChar *string);
typedef RwChar *(*vecStrlwrFunc)(RwChar *string);
typedef RwChar *(*vecStrtokFunc)(RwChar *string, const RwChar *delimit);
typedef int (*vecSscanfFunc)(const RwChar *buffer,
                             const RwChar *format,
                             ...) /* __RWFORMAT__(scanf, 2, 3) */;
typedef struct RwStringFunctions RwStringFunctions;
struct RwStringFunctions
{
    vecSprintfFunc vecSprintf ;
    vecVsprintfFunc vecVsprintf;
    vecStrcpyFunc vecStrcpy;
    vecStrncpyFunc vecStrncpy;
    vecStrcatFunc vecStrcat;
    vecStrncatFunc vecStrncat;
    vecStrrchrFunc vecStrrchr;
    vecStrchrFunc vecStrchr;
    vecStrstrFunc vecStrstr;
    vecStrcmpFunc vecStrcmp;
    vecStrncmpFunc vecStrncmp;
    vecStricmpFunc vecStricmp;
    vecStrlenFunc vecStrlen;
    vecStruprFunc vecStrupr;
    vecStrlwrFunc vecStrlwr;
    vecStrtokFunc vecStrtok;
    vecSscanfFunc vecSscanf;
};
typedef RwStream *(*RwPluginDataChunkWriteCallBack)(RwStream *stream, RwInt32 binaryLength, const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef RwStream *(*RwPluginDataChunkReadCallBack)(RwStream *stream, RwInt32 binaryLength, void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef RwInt32(*RwPluginDataChunkGetSizeCallBack)(const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef RwBool(*RwPluginDataChunkAlwaysCallBack)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef RwBool(*RwPluginDataChunkRightsCallBack)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject, RwUInt32 extraData);
typedef void *(*RwPluginObjectConstructor)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef void *(*RwPluginObjectCopy)(void *dstObject, const void *srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef void *(*RwPluginObjectDestructor)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef void *(*RwPluginErrorStrCallBack)(void *);
typedef struct RwPluginRegistry RwPluginRegistry;
typedef struct RwPluginRegEntry RwPluginRegEntry;
struct RwPluginRegistry
{
    RwInt32          sizeOfStruct;
    RwInt32          origSizeOfStruct;
    RwInt32          maxSizeOfStruct;
    RwInt32          staticAlloc;
    RwPluginRegEntry *firstRegEntry;
    RwPluginRegEntry *lastRegEntry;
};
struct RwPluginRegEntry
{
    RwInt32         offset;
    RwInt32         size;
    RwUInt32        pluginID;
    RwPluginDataChunkReadCallBack readCB;
    RwPluginDataChunkWriteCallBack writeCB;
    RwPluginDataChunkGetSizeCallBack getSizeCB;
    RwPluginDataChunkAlwaysCallBack alwaysCB;
    RwPluginDataChunkRightsCallBack rightsCB;
    RwPluginObjectConstructor constructCB;
    RwPluginObjectDestructor destructCB;
    RwPluginObjectCopy copyCB;
    RwPluginErrorStrCallBack errStrCB;
    RwPluginRegEntry *nextRegEntry;
    RwPluginRegEntry *prevRegEntry;
    RwPluginRegistry *parentRegistry;
};
extern const
RwPluginRegistry *_rwPluginRegistryInitObject(const RwPluginRegistry * reg,
                                             void *object);
#define rwPluginRegistryInitObject(reg, object) \
       _rwPluginRegistryInitObject(reg, object)
struct RwMatrixTag
{
    /* These are padded to be 16 byte quantities per line */
    RwV3d               right;
    RwUInt32            flags;
    RwV3d               up;
    RwUInt32            pad1;
    RwV3d               at;
    RwUInt32            pad2;
    RwV3d               pos;
    RwUInt32            pad3;
};
typedef struct RwMatrixTag RWALIGN(RwMatrix, rwMATRIXALIGNMENT);
typedef struct RwRGBAReal RwRGBAReal;
struct RwRGBAReal
{
    RwReal red;     /**< red component */
    RwReal green;   /**< green component */
    RwReal blue;    /**< blue component */
    RwReal alpha;   /**< alpha component */
};
typedef struct RwRGBA RwRGBA;
struct RwRGBA
{
    RwUInt8 red;    /**< red component */
    RwUInt8 green;  /**< green component */
    RwUInt8 blue;   /**< blue component */
    RwUInt8 alpha;  /**< alpha component */
};
typedef struct _RwNull2DVertex RwNull2DVertex;
struct _RwNull2DVertex
{
    RwV3d           screen;  /* Screen space coordinate */
    RwRGBAReal      col;  /* Vertex color */
    RwReal          texU, texV;  /* Texture coordinates */
    RwReal          recipZ;  /* Reciprocal Z */
    RwV3d           camera;  /* Camera space coordinate */
};
typedef RwNull2DVertex  RwIm2DVertex;
typedef RwUInt16        RxVertexIndex;
typedef RxVertexIndex   RwImVertexIndex;
enum RwRenderState
{
    rwRENDERSTATENARENDERSTATE = 0,

    rwRENDERSTATETEXTURERASTER,
        /**<Raster used for texturing (normally used in immediate mode). 
         *  The value is a pointer to an \ref RwRaster.
         * Default: NULL.
         */
    rwRENDERSTATETEXTUREADDRESS,
        /**<\ref RwTextureAddressMode: wrap, clamp, mirror or border.
         * Default: rwTEXTUREADDRESSWRAP.
         */
    rwRENDERSTATETEXTUREADDRESSU,
        /**<\ref RwTextureAddressMode in u only.
         * Default: rwTEXTUREADDRESSWRAP.
         */
    rwRENDERSTATETEXTUREADDRESSV,
        /**<\ref RwTextureAddressMode in v only.
         * Default: rwTEXTUREADDRESSWRAP.
         */
    rwRENDERSTATETEXTUREPERSPECTIVE,
        /**<Perspective correction on/off (always enabled on many platforms).
         */
    rwRENDERSTATEZTESTENABLE,
        /**<Z-buffer test on/off.
         * Default: TRUE.
         */
    rwRENDERSTATESHADEMODE,
        /**<\ref RwShadeMode: flat or gouraud shading.
         * Default: rwSHADEMODEGOURAUD.
         */
    rwRENDERSTATEZWRITEENABLE,
        /**<Z-buffer write on/off.
         * Default: TRUE.
         */
    rwRENDERSTATETEXTUREFILTER,
        /**<\ref RwTextureFilterMode: point sample, bilinear, trilinear, etc.
         * Default: rwFILTERLINEAR.
         */
    rwRENDERSTATESRCBLEND,
        /**<\ref RwBlendFunction used to modulate the source pixel color
         *  when blending to the frame buffer.
         * Default: rwBLENDSRCALPHA.
         */
    rwRENDERSTATEDESTBLEND,
        /**<\ref RwBlendFunction used to modulate the destination pixel
         *  color in the frame buffer when blending. The resulting pixel
         *  color is given by the formula 
         *  (SRCBLEND * srcColor + DESTBLEND * destColor) for each RGB
         *  component. For a particular platform, not all combinations
         *  of blend function are allowed (see platform specific
         *  restrictions).
         * Default: rwBLENDINVSRCALPHA.
         */
    rwRENDERSTATEVERTEXALPHAENABLE,
        /**<Alpha blending on/off (always enabled on some platforms). 
         *  This is normally used in immediate mode to enable alpha blending 
         *  when vertex colors or texture rasters have transparency. Retained
         *  mode pipelines will usually set this state based on material colors 
         *  and textures.
         * Default: FALSE.
         */
    rwRENDERSTATEBORDERCOLOR,
        /**<Border color for \ref RwTextureAddressMode 
         *  \ref rwTEXTUREADDRESSBORDER. The value should be a packed 
         *  RwUInt32 in a platform specific format. The macro 
         *  RWRGBALONG(r, g, b, a) may be used to construct this using 
         *  8-bit color components.
         * Default: RWRGBALONG(0, 0, 0, 0).
         */
    rwRENDERSTATEFOGENABLE,
        /**<Fogging on/off (all polygons will be fogged).  
         * Default: FALSE.
         */
    rwRENDERSTATEFOGCOLOR,
        /**<Color used for fogging. The value should be a packed RwUInt32 
         *  in a platform specific format. The macro RWRGBALONG(r, g, b, a)
         *  may be used to construct this using 8-bit color components. 
         * Default: RWRGBALONG(0, 0, 0, 0).
         */
    rwRENDERSTATEFOGTYPE,
        /**<\ref RwFogType, the type of fogging to use. 
         * Default: rwFOGTYPELINEAR.
         */
    rwRENDERSTATEFOGDENSITY,
        /**<Fog density for \ref RwFogType of 
         *  \ref rwFOGTYPEEXPONENTIAL or \ref rwFOGTYPEEXPONENTIAL2. 
         *  The value should be a pointer to an RwReal in the 
         *  range 0 to 1.  
         * Default: 1.
         */
    rwRENDERSTATECULLMODE = 20,
        /**<\ref RwCullMode, for selecting front/back face culling, or
         *  no culling.
         * Default: rwCULLMODECULLBACK.
         */
    rwRENDERSTATESTENCILENABLE,
        /**<Stenciling on/off.
         *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
         * Default: FALSE.
         */
    rwRENDERSTATESTENCILFAIL,
        /**<\ref RwStencilOperation used when the stencil test passes.
         *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
         * Default: rwSTENCILOPERATIONKEEP.
         */
    rwRENDERSTATESTENCILZFAIL,
        /**<\ref RwStencilOperation used when the stencil test passes and 
         *  the depth test (z-test) fails. 
         *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
         * Default: rwSTENCILOPERATIONKEEP.
         */
    rwRENDERSTATESTENCILPASS,
        /**<\ref RwStencilOperation used when both the stencil and the depth 
         *  (z) tests pass. 
         *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
         * Default: rwSTENCILOPERATIONKEEP.
         */
    rwRENDERSTATESTENCILFUNCTION,
        /**<\ref RwStencilFunction for the stencil test. 
         *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
         * Default: rwSTENCILFUNCTIONALWAYS.
         */
    rwRENDERSTATESTENCILFUNCTIONREF,
        /**<Integer reference value for the stencil test. 
         *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
         * Default: 0.
         */
    rwRENDERSTATESTENCILFUNCTIONMASK,
        /**<Mask applied to the reference value and each stencil buffer 
         *  entry to determine the significant bits for the stencil test. 
         *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
         * Default: 0xffffffff.
         */
    rwRENDERSTATESTENCILFUNCTIONWRITEMASK,
        /**<Write mask applied to values written into the stencil buffer. 
         *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
         * Default: 0xffffffff.
         */
    rwRENDERSTATEALPHATESTFUNCTION,
        /**<\ref RwAlphaTestFunction for the alpha test. When a pixel fails,
         * neither the frame buffer nor the Z-buffer are updated.
         * Default: rwALPHATESTFUNCTIONGREATER (GameCube, Xbox, D3D8, D3D9
         * and OpenGL). The default PS2 behaviour is to always update the
         * frame buffer and update the Z-buffer only if a greater than or
         * equal test passes.
         */
    rwRENDERSTATEALPHATESTFUNCTIONREF,
        /**<Integer reference value for the alpha test. 
         *  <i> Range is 0 to 255, mapped to the platform's actual range </i>
         * Default: 128 (PS2) 0 (GameCube, Xbox, D3D8, D3D9 and OpenGL).
         */

    rwRENDERSTATEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwRenderState RwRenderState;
enum RwShadeMode
{
    rwSHADEMODENASHADEMODE = 0,
    rwSHADEMODEFLAT,                /**<Flat shading */
    rwSHADEMODEGOURAUD,             /**<Gouraud shading */
    rwSHADEMODEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwShadeMode RwShadeMode;
enum RwTextureFilterMode
{
    rwFILTERNAFILTERMODE = 0,
    rwFILTERNEAREST,                /**<Point sampled */
    rwFILTERLINEAR,                 /**<Bilinear */
    rwFILTERMIPNEAREST,             /**<Point sampled per pixel mip map */
    rwFILTERMIPLINEAR,              /**<Bilinear per pixel mipmap */
    rwFILTERLINEARMIPNEAREST,       /**<MipMap interp point sampled */
    rwFILTERLINEARMIPLINEAR,        /**<Trilinear */
    rwTEXTUREFILTERMODEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwTextureFilterMode RwTextureFilterMode;
enum RwFogType
{
    rwFOGTYPENAFOGTYPE = 0,
    rwFOGTYPELINEAR,            /**<Linear fog */
    rwFOGTYPEEXPONENTIAL,       /**<Exponential fog */
    rwFOGTYPEEXPONENTIAL2,      /**<Exponential^2 fog */
    rwFOGTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwFogType RwFogType;
enum RwBlendFunction
{
    rwBLENDNABLEND = 0,
    rwBLENDZERO,            /**<(0,    0,    0,    0   ) */
    rwBLENDONE,             /**<(1,    1,    1,    1   ) */
    rwBLENDSRCCOLOR,        /**<(Rs,   Gs,   Bs,   As  ) */
    rwBLENDINVSRCCOLOR,     /**<(1-Rs, 1-Gs, 1-Bs, 1-As) */
    rwBLENDSRCALPHA,        /**<(As,   As,   As,   As  ) */
    rwBLENDINVSRCALPHA,     /**<(1-As, 1-As, 1-As, 1-As) */
    rwBLENDDESTALPHA,       /**<(Ad,   Ad,   Ad,   Ad  ) */
    rwBLENDINVDESTALPHA,    /**<(1-Ad, 1-Ad, 1-Ad, 1-Ad) */
    rwBLENDDESTCOLOR,       /**<(Rd,   Gd,   Bd,   Ad  ) */
    rwBLENDINVDESTCOLOR,    /**<(1-Rd, 1-Gd, 1-Bd, 1-Ad) */
    rwBLENDSRCALPHASAT,     /**<(f,    f,    f,    1   )  f = min (As, 1-Ad) */
    rwBLENDFUNCTIONFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwBlendFunction RwBlendFunction;
enum RwTextureAddressMode
{
    rwTEXTUREADDRESSNATEXTUREADDRESS = 0,
    rwTEXTUREADDRESSWRAP,      /**<UV wraps (tiles) */
    rwTEXTUREADDRESSMIRROR,    /**<Alternate UV is flipped */
    rwTEXTUREADDRESSCLAMP,     /**<UV is clamped to 0-1 */
    rwTEXTUREADDRESSBORDER,    /**<Border color takes effect outside of 0-1 */
    rwTEXTUREADDRESSMODEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwTextureAddressMode RwTextureAddressMode;
enum RwPrimitiveType
{
    rwPRIMTYPENAPRIMTYPE = 0,   /**<Invalid primative type */
    rwPRIMTYPELINELIST = 1,     /**<Unconnected line segments, each line is specified by
                                 * both its start and end index, independently of other lines
                                 * (for example, 3 segments specified as 0-1, 2-3, 4-5) */
    rwPRIMTYPEPOLYLINE = 2,     /**<Connected line segments, each line's start index
                                 * (except the first) is specified by the index of the end of
                                 * the previous segment (for example, 3 segments specified as
                                 * 0-1, 1-2, 2-3) */
    rwPRIMTYPETRILIST = 3,      /**<Unconnected triangles: each triangle is specified by
                                 * three indices, independently of other triangles (for example,
                                 * 3 triangles specified as 0-1-2, 3-4-5, 6-7-8) */
    rwPRIMTYPETRISTRIP = 4,     /**<Connected triangles sharing an edge with, at most, one
                                 * other forming a series (for example, 3 triangles specified
                                 * as 0-2-1, 1-2-3-, 2-4-3) */
    rwPRIMTYPETRIFAN = 5 ,      /**<Connected triangles sharing an edge with, at most,
                                 * two others forming a fan (for example, 3 triangles specified
                                 * as 0-2-1, 0-3-2, 0-4-3) */
    rwPRIMTYPEPOINTLIST = 6,    /**<Points 1, 2, 3, etc. This is not
                                 * supported by the default RenderWare
                                 * immediate or retained-mode pipelines
                                 * (except on PlayStation 2), it is intended
                                 * for use by user-created pipelines */
    rwPRIMITIVETYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwPrimitiveType RwPrimitiveType;
#define rwSTANDARDNUMOFSTANDARD         29
typedef RwBool (*RwStandardFunc)(void *pOut,void *pInOut,RwInt32 nI);
typedef RwBool
    (*RwSystemFunc)(RwInt32 nOption,
                    void *pOut,
                    void *pInOut,
                    RwInt32 nIn);
typedef RwBool
    (*RwRenderStateSetFunction)(RwRenderState nState,void *pParam);
typedef RwBool
    (*RwRenderStateGetFunction)(RwRenderState nState,void *pParam);
typedef RwBool
    (*RwIm2DRenderLineFunction)(RwIm2DVertex *vertices,
                                RwInt32 numVertices,
                                RwInt32 vert1,
                                RwInt32 vert2);
typedef RwBool
    (*RwIm2DRenderTriangleFunction)(RwIm2DVertex *vertices,
                                    RwInt32 numVertices,
                                    RwInt32 vert1,
                                    RwInt32 vert2,
                                    RwInt32 vert3);
typedef RwBool
    (*RwIm2DRenderPrimitiveFunction)(RwPrimitiveType primType,
                                     RwIm2DVertex *vertices,
                                     RwInt32 numVertices);
typedef RwBool
    (*RwIm2DRenderIndexedPrimitiveFunction)(RwPrimitiveType primType,
                                            RwIm2DVertex *vertices,
                                            RwInt32 numVertices,
                                            RwImVertexIndex *indices,
                                            RwInt32 numIndices);
typedef RwBool
    (*RwIm3DRenderLineFunction)(RwInt32 vert1,
                                RwInt32 vert2);
typedef RwBool
    (*RwIm3DRenderTriangleFunction)(RwInt32 vert1,
                                    RwInt32 vert2,
                                    RwInt32 vert3);
typedef RwBool
    (*RwIm3DRenderPrimitiveFunction)(RwPrimitiveType primType);
typedef RwBool
    (*RwIm3DRenderIndexedPrimitiveFunction)(RwPrimitiveType primtype,
                                            RwImVertexIndex *indices,
                                            RwInt32 numIndices);
typedef struct RwDevice RwDevice;
struct RwDevice
{
    RwReal                                  gammaCorrection; /* Gamma correction  */
    RwSystemFunc                            fpSystem;  /* System handler */
    RwReal                                  zBufferNear; /* Near Z buffer value */
    RwReal                                  zBufferFar; /* Far Z buffer value */

    /* Immediate mode functions */
    RwRenderStateSetFunction                fpRenderStateSet; /* Internal Use */
    RwRenderStateGetFunction                fpRenderStateGet; /* Internal Use */

    /* Render functions */
    RwIm2DRenderLineFunction                fpIm2DRenderLine; /* Internal Use */
    RwIm2DRenderTriangleFunction            fpIm2DRenderTriangle; /* Internal Use */
    RwIm2DRenderPrimitiveFunction           fpIm2DRenderPrimitive; /* Internal Use */
    RwIm2DRenderIndexedPrimitiveFunction    fpIm2DRenderIndexedPrimitive; /* Internal Use */

    RwIm3DRenderLineFunction                fpIm3DRenderLine; /* Internal Use */
    RwIm3DRenderTriangleFunction            fpIm3DRenderTriangle; /* Internal Use */
    RwIm3DRenderPrimitiveFunction           fpIm3DRenderPrimitive; /* Internal Use */
    RwIm3DRenderIndexedPrimitiveFunction    fpIm3DRenderIndexedPrimitive; /* Internal Use */
};
typedef struct RwMetrics RwMetrics;
struct RwMetrics
{
    RwUInt32    numTriangles;           /**< The number of triangles processed. */
    RwUInt32    numProcTriangles;       /**< The number of mesh triangles processed. */
    RwUInt32    numVertices;            /**< The number of vertices processed. */
    RwUInt32    numTextureUploads;
    /**<
     * \if sky2 The number of textures used. \endif
     * \if d3d8 The number of times RwD3D8SetTexture is called. \endif
     * \if d3d9 The number of times RwD3D9SetTexture is called.  \endif
     * \if opengl The number of times RwOpenGLSetTexture is called. \endif
     * \if xbox The number of textures used. \endif
     * \if gcn The number of textures used. \endif
     * \if softras The number of times the \ref rwRENDERSTATETEXTURERASTER renderstate
     * is set with a valid (non-NULL) texture. \endif
     */
    RwUInt32    sizeTextureUploads;
    /**<
     * \if sky2 The number of bytes uploaded to the GS. \endif
     * \if d3d8 The size of the textures, in bytes, that have been passed to RwD3D8SetTexture. \endif
     * \if d3d9 The size of the textures, in bytes, that have been passed to RwD3D9SetTexture. \endif
     * \if opengl The size of the textures, in bytes, that have been passed to RwOpenGLSetTexture. \endif
     * \if xbox Size of textures swapped. \endif
     * \if gcn Size of textures swapped. \endif
     * \if softras The size of the textures, in bytes, that have been set with
     * the \ref rwRENDERSTATETEXTURERASTER renderstate. \endif
     */
    RwUInt32    numResourceAllocs;      /**< The number of resource blocks swapped. */
    void        *devSpecificMetrics;    /**< Device specific metrics. */
};
#define RWPLUGINOFFSET(_type, _base, _offset)                   \
   ((_type *)((RwUInt8 *)(_base) + (_offset)))
#define RWSRCGLOBAL(variable) \
   (((RwGlobals *)RwEngineInstance)->variable)
#define RWASSERTISTYPE(_f, _t) \
   RWASSERT((((const RwObject *)(_f))->type)==(_t))
enum RwEngineStatus
{
    rwENGINESTATUSIDLE = 0,                     /* This needs to be zero */
    rwENGINESTATUSINITED = 1,
    rwENGINESTATUSOPENED = 2,
    rwENGINESTATUSSTARTED = 3,
    rwENGINESTATUSFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwEngineStatus RwEngineStatus;
typedef RwBool (*rwFnFexist)(const RwChar *name);
typedef void *(*rwFnFopen)(const RwChar *name, const RwChar *access);
typedef int (*rwFnFclose)(void *fptr);
typedef size_t (*rwFnFread)(void *addr, size_t size, size_t count, void *fptr);
typedef size_t (*rwFnFwrite)(const void *addr, size_t size, size_t count, void *fptr);
typedef RwChar *(*rwFnFgets)(RwChar *buffer, int maxLen, void *fptr);
typedef int (*rwFnFputs)(const RwChar *buffer, void *fptr);
typedef int (*rwFnFeof)(void *fptr);
typedef int (*rwFnFseek)(void *fptr, long offset, int origin);
typedef int (*rwFnFflush)(void *fptr);
typedef int (*rwFnFtell)(void *fptr);
typedef struct RwFileFunctions RwFileFunctions;
struct RwFileFunctions
{
    rwFnFexist  rwfexist;
    rwFnFopen   rwfopen;
    rwFnFclose  rwfclose;
    rwFnFread   rwfread;
    rwFnFwrite  rwfwrite;
    rwFnFgets   rwfgets;
    rwFnFputs   rwfputs;
    rwFnFeof    rwfeof;
    rwFnFseek   rwfseek;
    rwFnFflush  rwfflush;
    rwFnFtell   rwftell;
};
typedef struct RwGlobals RwGlobals;
struct RwGlobals
{

		/* codfh-match: the drop's "Jaewon 20050603" debugLogFunc field is
		 * not in the image's RwGlobals (curCamera is its first word, at
		 * 0x5200E0), so it is left out here. */

        /* Current entities */
        void                *curCamera;             /* Current camera */
        void                *curWorld;              /* Current World */

        /* Checking counters */
        RwUInt16            renderFrame;            /* Camera display count */
        RwUInt16            lightFrame;             /* Used to ensure each light is applied only once. */
        RwUInt16            pad[2];                 /* Longword align it again */

        /* For the currently accessed device */
        RwDevice            dOpenDevice;

        /* Standard renderers and functions */
        RwStandardFunc      stdFunc[rwSTANDARDNUMOFSTANDARD];

        /* All of the frames which have been updated */
        RwLinkList          dirtyFrameList;
        /* P7: no RW 3.6 fileFuncs table here in MFL (stringFuncs at +0xc4 (vecStrlen +0xf4), memoryFuncs at +0x108, memoryAlloc +0x118) */
		
        /* The string functions */
        RwStringFunctions   stringFuncs;

        /* The memory allocation functions */
        RwMemoryFunctions   memoryFuncs;

        /* virtual memory alloc/free functions */
        RwMemoryAllocFn         memoryAlloc;
        RwMemoryFreeFn          memoryFree;

        RwMetrics           *metrics;

        /* Current engine status */
        RwEngineStatus      engineStatus;

        /* Resource arena init size. */
        RwUInt32            resArenaInitSize;


//@{ 2005414 DDonSS : Threadsafe



//@} DDonSS

};
typedef struct RwModuleInfo RwModuleInfo;
struct RwModuleInfo
{
        RwInt32     globalsOffset;
        RwInt32     numInstances;
};
extern void         *RwEngineInstance;
typedef struct RwResEntry RwResEntry;
typedef void        (*RwResEntryDestroyNotify) (RwResEntry * resEntry);
struct RwResEntry
{
    RwLLLink            link;   /* Node in the list of resource elements */
    RwInt32             size;   /* Size of this node */
    void               *owner;  /* Owner of this node */
    RwResEntry        **ownerRef; /* Pointer to pointer to this (enables de-alloc) */
    RwResEntryDestroyNotify destroyNotify; /* This is called right before destruction */
};
typedef struct rxHeapFreeBlock rxHeapFreeBlock;
typedef struct rxHeapSuperBlockDescriptor rxHeapSuperBlockDescriptor;
typedef struct RxHeap RxHeap;
typedef struct rxHeapBlockHeader rxHeapBlockHeader;
struct rxHeapFreeBlock
{
    RwUInt32            size;
    rxHeapBlockHeader  *ptr;
};
struct rxHeapSuperBlockDescriptor
{
    void *start;
    RwUInt32 size;
    rxHeapSuperBlockDescriptor *next;
};
struct RxHeap
{
    RwUInt32            superBlockSize; /**< Granularity of heap growth */
    rxHeapSuperBlockDescriptor *head;   /**< Internally used superblock pointer */
    rxHeapBlockHeader  *headBlock;      /**< Internally used block pointer */
    rxHeapFreeBlock    *freeBlocks;     /**< Internally used free blocks pointer */
    RwUInt32            entriesAlloced; /**< Number of entries allocated */
    RwUInt32            entriesUsed;    /**< Number of entries used */
    RwBool              dirty;          /**< Internally used boolean, flags whether
                                         *   the heap needs resetting or not. */
};
struct rxHeapBlockHeader
{
    /* present in all blocks (used & unused) */
    rxHeapBlockHeader  *prev, *next;
    RwUInt32            size;
    rxHeapFreeBlock    *freeEntry; /* (or null) */
    RwUInt32            pad[4]; /* alignment padding to 32 bytes */
};
enum RxClusterValidityReq
{
    rxCLREQ_DONTWANT = 0, /**<The cluster is required but any data within it is
                           * not wanted and will be overwritten */
    rxCLREQ_REQUIRED = 1, /**<The cluster is required and it must contain
                           * valid data */
    rxCLREQ_OPTIONAL = 2, /**<The cluster will be used if it is present and
                           * contains valid data, otherwise the node will
                           * make do without it. */
    rxCLUSTERVALIDITYREQFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
enum RxClusterValid
{
    rxCLVALID_NOCHANGE = 0, /**<The cluster and its data will not change in
                             * validity on passing through this node */
    rxCLVALID_VALID = 1,    /**<The cluster and its data will be valid on
                             * exit from this node */
    rxCLVALID_INVALID = 2,  /**<The cluster's data will be invalid on
                             * exit from this node */
    rxCLUSTERVALIDFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef struct RxClusterDefinition RxClusterDefinition;
typedef struct rxReq rxReq;
typedef enum RxClusterValidityReq RxClusterValidityReq;
typedef enum RxClusterValid RxClusterValid;
typedef struct RxOutputSpec RxOutputSpec;
typedef struct RxClusterRef RxClusterRef;
typedef struct RxIoSpec RxIoSpec;
typedef struct RxNodeMethods RxNodeMethods;
typedef struct RxNodeDefinition RxNodeDefinition;
typedef struct RxCluster RxCluster;
typedef struct RxPipelineCluster RxPipelineCluster;
typedef struct RxPacket RxPacket;
typedef struct RxPipelineNode RxPipelineNode;
typedef struct RxPipelineNodeTopSortData RxPipelineNodeTopSortData;
typedef struct RxPipelineNodeParam RxPipelineNodeParam;
typedef struct RxPipelineRequiresCluster RxPipelineRequiresCluster;
typedef struct RxPipeline RxPipeline;
struct RxClusterDefinition
{
    RwChar             *name;                  /**< Name */
    RwUInt32            defaultStride;         /**< Default stride */
    RwUInt32            defaultAttributes;     /**< Default attributes */
    const RwChar         *attributeSet;           /**< Attribute set */
};
struct RxOutputSpec
{
     RwChar             *name;                  /**< Name */
     RxClusterValid     *outputClusters;        /**< States of clusters of interest on output */
     RxClusterValid      allOtherClusters;      /**< States of clusters not of interest on output */
};
enum RxClusterForcePresent
{
    rxCLALLOWABSENT = FALSE, /**<The cluster is allowed to be absent if no prior
                              *  nodes create it and no subsequent nodes require it */
    rxCLFORCEPRESENT = TRUE, /**<The cluster must be present even if no prior
                              *  nodes create it and no subsequent nodes require it */

    rxCLUSTERFORCEPRESENTFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RxClusterForcePresent RxClusterForcePresent;
struct RxClusterRef
{
    RxClusterDefinition  *clusterDef;           /**< Cluster definition */
    RxClusterForcePresent forcePresent;         /**< Specifies whether the cluster should be forced present */
    RwUInt32              reserved;             /**< Omit or initialize to zero */
};
struct RxIoSpec
{
    RwUInt32              numClustersOfInterest;/**< Number of Clusters of interest */
    RxClusterRef         *clustersOfInterest;   /**< Clusters of interest array */
    RxClusterValidityReq *inputRequirements;    /**< Requirements of the clusters of interest on input to the node */
    RwUInt32              numOutputs;           /**< Number of outputs from the node */
    RxOutputSpec         *outputs;              /**< Output specification array */
};
typedef RwBool (*RxNodeBodyFn) (RxPipelineNode * self,
                                const RxPipelineNodeParam *params);
typedef RwBool (*RxNodeInitFn) (RxNodeDefinition * self);
typedef void   (*RxNodeTermFn) (RxNodeDefinition * self);
typedef RwBool (*RxPipelineNodeInitFn) (RxPipelineNode * self);
typedef void   (*RxPipelineNodeTermFn) (RxPipelineNode * self);
typedef RwBool (*RxPipelineNodeConfigFn) (RxPipelineNode * self,
                                          RxPipeline * pipeline);
typedef RwUInt32 (*RxConfigMsgHandlerFn) (RxPipelineNode * self,
                                          RwUInt32 msg,
                                          RwUInt32 intparam,
                                          void *ptrparam);
struct RxNodeMethods
{
    RxNodeBodyFn         nodeBody;              /**< Node body function */
    RxNodeInitFn         nodeInit;              /**< Node initialization function */
    RxNodeTermFn         nodeTerm;              /**< Node termination function */
    RxPipelineNodeInitFn pipelineNodeInit;      /**< Pipeline node initialization function */
    RxPipelineNodeTermFn pipelineNodeTerm;      /**< Pipeline node termination function */
    RxPipelineNodeConfigFn pipelineNodeConfig;  /**< Pipleline node configuation function */
    RxConfigMsgHandlerFn configMsgHandler;      /**< Configuaraton message handler function */
};
enum RxNodeDefEditable
{
    /**<This node definition should not be freed */
    rxNODEDEFCONST = FALSE,
    /**< This node definition is a temporary,
     * modified copy of another and can be freed */
    rxNODEDEFEDITABLE = TRUE,
    rxNODEDEFEDITABLEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RxNodeDefEditable RxNodeDefEditable;
struct RxNodeDefinition
{
    RwChar             *name;                   /**< Name */
    RxNodeMethods       nodeMethods;            /**< Node methods */
    RxIoSpec            io;                     /**< Input/output specification */
    RwUInt32            pipelineNodePrivateDataSize; /**< Size in bytes of node's private data  */
    RxNodeDefEditable   editable;               /**< Flags whether a node is editable */
    RwInt32             InputPipesCnt;          /**< Count of the unlocked pipelines containing this node */
};
struct RxPipelineCluster
{
    RxClusterDefinition *clusterRef;            /**< Cluster refererence */
    RwUInt32             creationAttributes;    /**< Creation Attributes */
};
struct RxCluster
{
    RwUInt16            flags;                  /**< Bitfield of flags e.g. modification permissions */
    RwUInt16            stride;                 /**< Stride in bytes of the cluster's data */
    void               *data;                   /**< The Cluster's data */
    void               *currentData;            /**< A 'cursor', referencing the current element in the data */
    RwUInt32            numAlloced;             /**< Allocated count */
    RwUInt32            numUsed;                /**< Used count */
    RxPipelineCluster  *clusterRef;             /**< Valid after a cluster has been locked for writing,
                                                 *   otherwise NULL. Analog of \ref RxPipelineNode and
                                                 *   its 'nodeDef' member. */
    RwUInt32            attributes;             /**< Attributes */
};
struct RxPacket
{
    RwUInt16            flags;                  /**< Flags to guide pipeline execution */
    RwUInt16            numClusters;            /**< Maximum number of clusters simultanesouly present in the current pipeline */
    RxPipeline         *pipeline;               /**< The pipeline in which this packet is embedded (the current pipeline) */
    RwUInt32           *inputToClusterSlot;     /**< LUT to locate clusters of interest in the packet */
    RwUInt32           *slotsContinue;          /**< Bitfields specifying clusters persisting to output node */
    RxPipelineCluster **slotClusterRefs;        /**< Internal Use */
    RxCluster           clusters[1];            /**< Cluster array large enough for widest part of the pipeline */
};
struct RxPipelineNode
{
    RxNodeDefinition   *nodeDef;            /**< Node definition reference */
    RwUInt32            numOutputs;         /**< Output count */
    RwUInt32           *outputs;            /**< Output array, indexing the pipeline's array of pipeline nodes */
    RxPipelineCluster **slotClusterRefs;    /**< For packets passing through this node, SlotClusterRefs[n]
                                             * identifies the cluster to be found at index n in the packet's
                                             * Clusters[] array. These assignments are determined at pipeline
                                             * Unlock() time and are good for the life of the pipeline. */
    RwUInt32           *slotsContinue;      /**< Cluster persistence table for the node's outputs */
    void               *privateData;        /**< Pointer to the pipeline Node's private data */
    RwUInt32           *inputToClusterSlot; /**< LUT to locate clusters of interest in packets passing through this pipeline Node */
    RxPipelineNodeTopSortData *topSortData; /**< Internal Use */
    void               *initializationData; /**< Used by the pipeline node initialisation
                                             * function in setting up the Private data. */
    RwUInt32        initializationDataSize; /**< Present so that if the node is cloned
                                             * we can copy the initialisation data. */
};
struct RxPipelineNodeTopSortData
{
    RwUInt32            numIns;                 /**< Input count */
    RwUInt32            numInsVisited;          /**< Count of inputs visited during dependencies
                                                 * propagation/cluster slot allocation */
    rxReq              *req;                    /**< Req used in dependencies propagation/cluster
                                                 * slot allocation */
};
struct RxPipelineNodeParam
{
    void   *dataParam;   /**< The data pointer passed in to \ref RxPipelineExecute */
    RxHeap *heap;        /**< The heap associated with the current pipeline exeuction */
};
enum rxEmbeddedPacketState
{
    rxPKST_PACKETLESS = 0, /* Packet not created */
    rxPKST_UNUSED     = 1, /* Packet created and then destroyed */
    rxPKST_INUSE      = 2, /* Packet created but not yet destroyed and fetched but not yet dispatched */
    rxPKST_PENDING    = 3, /* Packet created but not destroyed and dispatched but not yet fetched */
    rxEMBEDDEDPACKETSTATEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum rxEmbeddedPacketState rxEmbeddedPacketState;
struct RxPipelineRequiresCluster
{
    RxClusterDefinition *clusterDef;    /**< Reference to a Cluster definition */
    RxClusterValidityReq rqdOrOpt;      /**< Cluster validity requirement (rxCLREQ_DONTWANT, rxCLREQ_REQUIRED or rxCLREQ_OPTIONAL) */
    RwUInt32             slotIndex;     /**< Index into the packet's cluster array within this pipeline */
};
struct RxPipeline
{
    RwBool                     locked;                /**< Flags whether the pipeline is locked for editing */
    RwUInt32                   numNodes;              /**< Node count */
    RxPipelineNode            *nodes;                 /**< Topologically sorted array of pipeline nodes */
    RwUInt32                   packetNumClusterSlots; /**< Number of slots allocated for clusters */
    rxEmbeddedPacketState      embeddedPacketState;   /**< The state of this pipeline's embedded packet */
    RxPacket                  *embeddedPacket;        /**< This pipeline's embedded packet */

    RwUInt32                   numInputRequirements;  /**< Input requirements count */
    RxPipelineRequiresCluster *inputRequirements;     /**< Input requirements array */

    void                      *superBlock;            /**< Internally used block of memory */
    RwUInt32                   superBlockSize;        /**< Internally used block of memory */

    RwUInt32                   entryPoint;            /**< The index of the node which is the entry point of this pipeline */
    RwUInt32                   pluginId;              /**< If required, the Id of the plugin owning this pipeline, or 0 */
    RwUInt32                   pluginData;            /**< Rights callback extra data */
};
extern RxPipeline *
RxPipelineExecute(RxPipeline  * pipeline,
                  void        * data,
                  RwBool       heapReset);
typedef struct RwRaster RwRaster;
struct RwRaster
{
    RwRaster           *parent; /* Top level raster if a sub raster */
    RwUInt8            *cpPixels; /* Pixel pointer when locked */
    RwUInt8            *palette; /* Raster palette */
    RwInt32             width, height, depth; /* Dimensions of raster */
    RwInt32             stride; /* Lines bytes of raster */
    RwInt16             nOffsetX, nOffsetY; /* Sub raster offset */
    RwUInt8             cType;  /* Type of raster */
    RwUInt8             cFlags; /* Raster flags */
    RwUInt8             privateFlags; /* Raster private flags */
    RwUInt8             cFormat; /* Raster format */

    RwUInt8            *originalPixels;
    RwInt32             originalWidth;
    RwInt32             originalHeight;
    RwInt32             originalStride;
};
typedef struct RxRenderStateVector RxRenderStateVector;
struct RxRenderStateVector
{
    RwUInt32             Flags;         /**< A load of the boolean renderstate options */
    RwShadeMode          ShadeMode;     /**< Flat or Gouraud currently */
    RwBlendFunction      SrcBlend;      /**< Src  alpha, 1-src  alpha, etc */
    RwBlendFunction      DestBlend;     /**< Dest alpha, 1-dest alpha, etc */
    RwRaster            *TextureRaster; /**< texture raster */
    RwTextureAddressMode AddressModeU;  /**< U addressing mode - WRAP, MIRROR, CLAMP, BORDER */
    RwTextureAddressMode AddressModeV;  /**< V addressing mode - WRAP, MIRROR, CLAMP, BORDER */
    RwTextureFilterMode  FilterMode;    /**< filtering mode - combos of NEAREST LINEAR MIP */
    RwRGBA               BorderColor;   /**< Border color for texturing address mode border */
    RwFogType            FogType;       /**< Select the type of fogging to use */
    RwRGBA               FogColor;      /**< Color used for fogging */
};
#define rwTEXTUREBASENAMELENGTH     32
typedef struct RwTexDictionary RwTexDictionary;
typedef struct RwTexture RwTexture;
struct RwTexDictionary
{
    RwObject            object; /* Homogeneous type */
    RwLinkList          texturesInDict; /* List of textures in dictionary */
    RwLLLink            lInInstance; /* Link list of all dicts in system */
};
struct RwTexture
{
    RwRaster           *raster; /** pointer to RwRaster with data */
    RwTexDictionary    *dict;   /* Dictionary this texture is in */
    RwLLLink            lInDictionary; /* List of textures in this dictionary */

    RwChar              name[rwTEXTUREBASENAMELENGTH];  /* Name of the texture */
    RwChar              mask[rwTEXTUREBASENAMELENGTH];  /* Name of the textures mask */

    /* 31 [xxxxxxxx xxxxxxxx vvvvuuuu ffffffff] 0 */
    RwUInt32            filterAddressing; /* Filtering & addressing mode flags */

    RwInt32             refCount; /* Reference count, surprisingly enough */
};
struct RwFrame
{
    RwObject            object;

    RwLLLink            inDirtyListLink;

    /* Put embedded matrices here to ensure they remain 16-byte aligned */
    RwMatrix            modelling;
    RwMatrix            ltm;

    RwLinkList          objectList; /* List of objects connected to a frame */

    struct RwFrame      *child;
    struct RwFrame      *next;
    struct RwFrame      *root;   /* Root of the tree */


//@{ 20050513 DDonSS : Threadsafe
};
typedef struct RwFrame RWALIGN(RwFrame, rwFRAMEALIGNMENT);
typedef struct RwObjectHasFrame RwObjectHasFrame;
typedef RwObjectHasFrame * (*RwObjectHasFrameSyncFunction)(RwObjectHasFrame *object);
struct RwObjectHasFrame
{
    RwObject                     object;
    RwLLLink                     lFrame;
    RwObjectHasFrameSyncFunction sync;
};
extern void _rwObjectHasFrameSetFrame(void *object, RwFrame *frame);
#define rwObjectHasFrameInitialize(o, type, subtype, syncFunc)  \
MACRO_START                                                     \
{                                                               \
    rwObjectInitialize(o, type, subtype);                       \
    ((RwObjectHasFrame *)o)->sync = syncFunc;                   \
}                                                               \
MACRO_STOP
#define rwObjectHasFrameSetFrame(object, frame) \
        _rwObjectHasFrameSetFrame(object, frame)
struct rwPipeGlobals
{
    RwFreeList          *pipesFreeList;   /* Save mallocs, use a freelist */
    RxRenderStateVector  defaultRenderState;
    RwLinkList           allPipelines;    /* Unused as of yet, meant to be used to keep track of all
                                           * created pipelines (for CBs and maybe cleanup) */
    RwUInt32             maxNodesPerPipe; /* Initialised from _rxPipelineMaxNodes at startup. Used to
                                           * allow conservative, single allocations during pipelock */

    /* NOTE: Rw and RpWorld PowerPipe globals kept together for simplicity */

    /* The current default pipelines (used if pipe == NULL for an object) */
    RxPipeline          *currentAtomicPipeline;
    RxPipeline          *currentWorldSectorPipeline;
    RxPipeline          *currentMaterialPipeline;
    /* Generic C-based pipes that run on all platforms
     * - these are set as the current pipes at startup unless
     *   platform-specific pipes (below) are created */
    RxPipeline          *genericAtomicPipeline;
    RxPipeline          *genericWorldSectorPipeline;
    RxPipeline          *genericMaterialPipeline;
    /* Platforms that have their own non-generic pipelines
     * (OPENGL, D3D8, SKY2, KAMUI2, DOLPHIN) put them here: */
    RxPipeline          *platformAtomicPipeline;
    RxPipeline          *platformWorldSectorPipeline;
    RxPipeline          *platformMaterialPipeline;
};
typedef struct rwPipeGlobals rwPipeGlobals;
#define RXPIPELINEGLOBAL(var) (RWPLUGINOFFSET(rwPipeGlobals, RwEngineInstance, _rxPipelineGlobalsOffset)->var)
extern RwInt32      _rxPipelineGlobalsOffset;
typedef struct RpMaterial RpMaterial;
struct RpMaterial
{
        RwTexture           *texture; /**< texture */
        RwRGBA              color; /**< color */              
        RxPipeline          *pipeline; /**< pipeline */     
        RwSurfaceProperties surfaceProps; /**< surfaceProps */
        RwInt16             refCount;          /* C.f. rwsdk/world/bageomet.h:RpGeometry */
        RwInt16             pad;
};
typedef struct RpMaterialList RpMaterialList;
struct RpMaterialList
{
    RpMaterial     **materials;
    RwInt32        numMaterials;
    RwInt32        space;
};
typedef struct RpMeshHeader RpMeshHeader;
struct RpMeshHeader
{
    RwUInt32            flags;    /**< \see RpMeshHeaderFlags */
    RwUInt16            numMeshes; /**< Number of meshes in object */
    RwUInt16            serialNum; /**< Determine if mesh has changed
                                    * since last instance */
    RwUInt32            totalIndicesInMesh; /**< Total triangle index
                                             * count in all meshes
                                             */
    RwUInt32            firstMeshOffset; /**< Offset in bytes from end this
                                          * structure RpMeshHeader
                                          * to the first mesh
                                          */
};
typedef struct RpGeometry RpGeometry;
typedef struct RpMorphTarget RpMorphTarget;
struct RpMorphTarget
{
        RpGeometry *parentGeom; 
        RwSphere   boundingSphere;
        RwV3d      *verts;
        RwV3d      *normals;
};
typedef struct RpTriangle RpTriangle;
struct RpTriangle
{
    RwUInt16            vertIndex[3];   /**< vertex indices */
    RwUInt16            matIndex;       /**< Index into material list */
};
struct RpGeometry
{
    RwObject            object;     /* Generic type */

    RwUInt32            flags;      /* Geometry flags */

    RwUInt16            lockedSinceLastInst; /* What has been locked since we last instanced - for re-instancing */
    RwInt16             refCount;   /* Reference count (for keeping track of atomics referencing geometry) */

    RwInt32             numTriangles; /* Quantity of various things (polys, verts and morph targets) */
    RwInt32             numVertices;
    RwInt32             numMorphTargets;
    RwInt32             numTexCoordSets;

    RpMaterialList      matList;

    RpTriangle         *triangles;  /* The triangles */

    RwRGBA             *preLitLum;  /* The pre-lighting values */

    RwTexCoords        *texCoords[rwMAXTEXTURECOORDS]; /* Texture coordinates */

    RpMeshHeader       *mesh;   /* The mesh - groups polys of the same material */

    RwResEntry         *repEntry;       /* Information for an instance */

    RpMorphTarget      *morphTarget;    /* The Morph Target */
    
//@{ 20050513 DDonSS : Threadsafe
};
#define rpATOMIC 1
enum RpInterpolatorFlag
{
    rpINTERPOLATORDIRTYINSTANCE = 0x01,
    rpINTERPOLATORDIRTYSPHERE = 0x02,
    rpINTERPOLATORNOFRAMEDIRTY = 0x04,
    rpINTERPOLATORFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
enum RpAtomicFlag
{
    rpATOMICCOLLISIONTEST = 0x01, /**<A generic collision flag to indicate
                                   * that the atomic should be considered
                                   * in collision tests.
                                   */
    rpATOMICRENDER = 0x04,      /**<The atomic is rendered if it is
                                 * in the view frustum.
                                 */

    rpATOMICFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpAtomicFlag RpAtomicFlag;
enum RpAtomicPrivateFlag
{
    rpATOMICPRIVATEWORLDBOUNDDIRTY = 0x01,
    rpATOMICPRIVATEFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef struct rpClumpGlobals rpClumpGlobals;
struct rpClumpGlobals
{
    RwFreeList         *atomicFreeList;
    RwFreeList         *clumpFreeList;
};
typedef struct RpAtomic RpAtomic;
typedef struct RpInterpolator RpInterpolator;
struct RpInterpolator
{
        RwInt32             flags; /**< flags */
        RwInt16             startMorphTarget; /**< startMorphTarget */
        RwInt16             endMorphTarget; /**< endMorphTarget */
        RwReal              time; /**< time */
        RwReal              recipTime; /**< recipTime */
        RwReal              position; /**< position */
};
typedef struct RpClump RpClump;
typedef RpClump    *(*RpClumpCallBack) (RpClump * clump, void *data);
struct RpClump
{
        RwObject            object;

        /* Information about all the Atomics */
        RwLinkList          atomicList;

        /* Lists of lights and cameras */
        RwLinkList          lightList;
        RwLinkList          cameraList;

        /* The clump in a world */
        RwLLLink            inWorldLink;

        /* Clump frustum callback */
        RpClumpCallBack     callback;
};
typedef RpAtomic   *(*RpAtomicCallBackRender) (RpAtomic * atomic);
struct RpAtomic
{
    RwObjectHasFrame    object;

    /* Information for an instance */
    RwResEntry         *repEntry;

    /* Triangles making the object */
    RpGeometry         *geometry;

    /* Interpolated bounding sphere (in object space and world space) */
    RwSphere            boundingSphere;
    RwSphere            worldBoundingSphere;

    /* Connections to other atomics */
    RpClump            *clump;
    RwLLLink            inClumpLink;

    /* callbacks */
    RpAtomicCallBackRender renderCallBack;

    /* Interpolation animation pointer */
    RpInterpolator      interpolator;

    /* Counter for checks of "render has occurred already" */
    RwUInt16            renderFrame;
    RwUInt16            pad;

    /* Connections to sectors */
    RwLinkList          llWorldSectorsInAtomic;

    /* The Atomic object pipeline for this Atomic */
    RxPipeline         *pipeline;
};
#define RpAtomicSetRenderCallBackMacro(_atomic, _callback)              \
MACRO_START                                                             \
{                                                                       \
    (_atomic)->renderCallBack = (_callback);                            \
    if (!(_atomic)->renderCallBack)                                     \
    {                                                                   \
        (_atomic)->renderCallBack = AtomicDefaultRenderCallBack;        \
    }                                                                   \
}                                                                       \
MACRO_STOP
#define RpAtomicSetFlagsMacro(_atomic, _flags)                          \
    (rwObjectSetFlags(_atomic, _flags),                                 \
     (_atomic))
#define RpAtomicSetRenderCallBack(_atomic, _callback) \
    RpAtomicSetRenderCallBackMacro(_atomic, _callback)
#define RpAtomicSetFlags(_atomic, _flags) \
    RpAtomicSetFlagsMacro(_atomic, _flags)
extern RpAtomic *
AtomicDefaultRenderCallBack(RpAtomic * atomic);
extern void
_rpAtomicResyncInterpolatedSphere(RpAtomic * atomic);
static RpAtomic *
RpAtomicSetFrame(RpAtomic * atomic,
                 RwFrame * frame);
extern RpAtomic *
RpAtomicCreate(void);
#define RWRETURN(value) return(value)
#define RWFUNCTION(name)
#define RWAPIFUNCTION(name)
#define RWASSERT(condition)
#define RWCLUMPGLOBAL(var)                              \
   (RWPLUGINOFFSET(rpClumpGlobals,                      \
                   RwEngineInstance,                    \
                   clumpModule.globalsOffset)->var)
extern RwPluginRegistry atomicTKList;
extern RwModuleInfo clumpModule;
extern RwObjectHasFrame * AtomicSync(RwObjectHasFrame * object);      /* P7b: static in codfh; binds to the MFL function (alias), not copied */
extern RpAtomic * AtomicDefaultRenderCallBack(RpAtomic *atomic);      /* P7b: static in codfh; binds to the MFL function (alias), not copied */
extern void _rpAtomicResyncInterpolatedSphere(RpAtomic * atomic);
static RpAtomic           *
RpAtomicSetFrame(RpAtomic * atomic, RwFrame * frame)
{
    RWAPIFUNCTION(RWSTRING("RpAtomicSetFrame"));
    RWASSERT(clumpModule.numInstances);
    RWASSERT(atomic);
    RWASSERTISTYPE(atomic, rpATOMIC);

    rwObjectHasFrameSetFrame(atomic, frame);

    /* World bounding sphere is no longer valid */
    rwObjectSetPrivateFlags(atomic, rwObjectGetPrivateFlags(atomic) |
                            rpATOMICPRIVATEWORLDBOUNDDIRTY);

    RWRETURN(atomic);
}

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                            Atomic handling
 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/**
 * \ingroup rpatomic
 * \ref RpAtomicCreate is used to create and initialize a new atomic.
 * An atomic can be thought of as an instance of geometry which defines the
 * atomic's physical structure. The geometry may hold one
 * or more morph targets each containing vertex position and vertex normal
 * data. The atomic's structure in any one instance is defined by
 * its interpolator which linearly blends the morph target data according
 * to the current interpolator value and scale.
 * If only one morph target is defined
 * the atomic's structure is defined solely by that morph target.
 * The interpolator
 * for a newly created atomic is set such that the start and end morph targets
 * point to the first morph target in the atomic's geometry and the
 * interpolator's
 * value and scale are zero and one, respectively.
 *
 * Before the atomic can be used in any rendering it must be attached
 * to a frame
 * and added to a world (unless it is part of a clump, then adding the clump
 * to the world will suffice). The frame enables the atomic to be positioned
 * and oriented within the world. The frame may also be attached to an
 * existing frame hierarchy containing other atomics grouped together into
 * an articulated clump.
 *
 * The world plugin must be attached before using this function.
 *
 * \return Returns a pointer to the new atomic if successful or NULL if
 * there is an error.
 *
 * \see RpAtomicClone
 * \see RpAtomicSetGeometry
 * \see RpAtomicGetGeometry
 * \see RpAtomicSetFrame
 * \see RpAtomicGetFrame
 * \see RpAtomicGetInterpolator
 * \see RpAtomicGetBoundingSphere
 * \see RpAtomicSetFlags
 * \see RpAtomicGetFlags
 * \see RpAtomicDestroy
 * \see RpAtomicSetRenderCallBack
 * \see RpAtomicGetRenderCallBack
 * \see RpWorldAddAtomic
 * \see RpAtomicRender
 * \see RpClumpAddAtomic
 * \see RpGeometryCreate
 * \see RpWorldPluginAttach
 *
 */
RpAtomic           *
RpAtomicCreate(void)
{
    RpAtomic           *atomic;

    RWAPIFUNCTION(RWSTRING("RpAtomicCreate"));
    RWASSERT(clumpModule.numInstances);

    RWASSERT(RWCLUMPGLOBAL(atomicFreeList));
    atomic = (RpAtomic *) RwFreeListAlloc(RWCLUMPGLOBAL(atomicFreeList),
                                      rwID_ATOMIC | rwMEMHINTDUR_EVENT);
    if (!atomic)
    {
        RWRETURN((RpAtomic *)NULL);
    }

    /* We don't care about the sub type */
    rwObjectHasFrameInitialize(atomic, rpATOMIC, 0, AtomicSync);

    /* NOTE: the repEntry is set to NULL because (a) if the geom is
     * non-animated then IT will hold the repEntry and (b) if the geom is
     * animated then the atomic will have a repEntry created during its
     * next pipeline execute. [A similar argument applies to the PS2
     * rwMeshCache plugin set up during rwPluginRegistryInitObject, below] */
    atomic->repEntry = (RwResEntry *)NULL;

    /* Flag the atomic as dirty -> ie vertices need expanding,
     * and the world space bounding sphere too...
     */
    RpAtomicSetFlags(atomic, rpATOMICCOLLISIONTEST | rpATOMICRENDER);
    rwObjectSetPrivateFlags(atomic, rpATOMICPRIVATEWORLDBOUNDDIRTY);

    /* No frame yet */
    RpAtomicSetFrame(atomic, (RwFrame *)NULL);

    /* Set up clump status */
    atomic->geometry = (RpGeometry *)NULL;

    /* And the bounding sphere's (these get updated during frame
     * synchronization and instancing)
     */
    atomic->boundingSphere.radius = (RwReal) (0);
    atomic->boundingSphere.center.x = (RwReal) (0);
    atomic->boundingSphere.center.y = (RwReal) (0);
    atomic->boundingSphere.center.z = (RwReal) (0);

    atomic->worldBoundingSphere.radius = (RwReal) (0);
    atomic->worldBoundingSphere.center.x = (RwReal) (0);
    atomic->worldBoundingSphere.center.y = (RwReal) (0);
    atomic->worldBoundingSphere.center.z = (RwReal) (0);

    /* Set up the default render callback */
    RpAtomicSetRenderCallBack(atomic, AtomicDefaultRenderCallBack);

    /* Set on the first frame of the geometry */
    atomic->interpolator.startMorphTarget = 0;
    atomic->interpolator.endMorphTarget = 0;
    atomic->interpolator.time = (RwReal) (1.0);
    atomic->interpolator.recipTime = (RwReal) (1.0);
    atomic->interpolator.position = (RwReal) (0.0);
    atomic->interpolator.flags = (RwInt32)
        (rpINTERPOLATORDIRTYINSTANCE | rpINTERPOLATORDIRTYSPHERE);

    /* membership of clump */
    rwLLLinkInitialize(&atomic->inClumpLink);

    atomic->clump = (RpClump *)NULL;

    /* use the default atomic object pipeline */
    atomic->pipeline = (RxPipeline *)NULL;

    /* Not in any atomic sectors */
    rwLinkListInitialize(&atomic->llWorldSectorsInAtomic);

    /* Initialize memory allocated to toolkits */
    rwPluginRegistryInitObject(&atomicTKList, atomic);


    RWRETURN(atomic);
}
