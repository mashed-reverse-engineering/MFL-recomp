/* compile: /G5 */
/* _rpWorldObjRegisterExtensions and its same-file callees: RenderWare Graphics 3.7 (Criterion Software)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rw/world/baworobj.c, functions WorldCopyAtomicExt (0x0053eac0, static), readGeometryMesh (0x004f42d0, static), writeSectorMesh (0x004f43c0, static), readSectorMesh (0x004f43f0, static), sizeSectorMesh (0x004f4420, static), _rpWorldObjRegisterExtensions (0x004f3590)), a byte-identical decompilation of the RenderWare
 * build of an Xbox game; the Win32 branches it kept are RenderWare 3.7.2 source. Used with credit; these functions stay
 * in one file because cl 13.00 compiles a static function and its callers in the same file with a register calling
 * convention and inlines same-file callees (/Ob2). The declarations they need are copied from the codfh headers (PC build).
 * Changes: no renames; data definitions of the unit are extern declarations here.
 * Byte-identical to MFL.exe (tools/cmpfn.py /O2 /G5, relocations masked): WorldCopyAtomicExt 0x0053eac0, readGeometryMesh 0x004f42d0, writeSectorMesh 0x004f43c0, readSectorMesh 0x004f43f0, sizeSectorMesh 0x004f4420, _rpWorldObjRegisterExtensions 0x004f3590. Static functions keep their linkage:
 * the overlay replaces only the external ones, the static copies serve the callers in this file. */
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
typedef signed char RwInt8;
typedef char RwChar;
typedef float RwReal;
typedef RwInt32 RwBool;
#define RWALIGN(type, x)   __declspec(align(x)) type
#define rwMATRIXALIGNMENT 16
#define RWFORCEENUMSIZEINT ((RwInt32)((~((RwUInt32)0))>>1))
#define __RWUNUSED__           /* No op */
#define __RWUNUSEDRELEASE__           /* No op */
#define FALSE 0
#define TRUE !FALSE
#define _RWSTRING(x) x
#define RWSTRING(x) _RWSTRING(x)
typedef struct RwV2d RwV2d;
struct RwV2d
{
    RwReal x;   /**< X value*/
    RwReal y;   /**< Y value */
};
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
typedef struct RwLinkList RwLinkList;
struct RwLinkList
{
    RwLLLink link;
};
typedef struct RwSurfaceProperties RwSurfaceProperties;
struct RwSurfaceProperties
{
    RwReal ambient;   /**< ambient reflection coefficient */
    RwReal specular;  /**< specular reflection coefficient */
    RwReal diffuse;   /**< reflection coefficient */
};
typedef struct RwPlane RwPlane;
struct RwPlane
{
    RwV3d normal;    /**< Normal to the plane */
    RwReal distance; /**< Distance to plane from origin in normal direction*/
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
typedef RwStream *(*RwPluginDataChunkWriteCallBack)(RwStream *stream, RwInt32 binaryLength, const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef RwStream *(*RwPluginDataChunkReadCallBack)(RwStream *stream, RwInt32 binaryLength, void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef RwInt32(*RwPluginDataChunkGetSizeCallBack)(const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef void *(*RwPluginObjectConstructor)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef void *(*RwPluginObjectCopy)(void *dstObject, const void *srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject);
typedef void *(*RwPluginObjectDestructor)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
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
typedef struct RwRGBA RwRGBA;
struct RwRGBA
{
    RwUInt8 red;    /**< red component */
    RwUInt8 green;  /**< green component */
    RwUInt8 blue;   /**< blue component */
    RwUInt8 alpha;  /**< alpha component */
};
extern RwInt32 RwEngineRegisterPlugin(RwInt32 size, RwUInt32 pluginID,
                                  RwPluginObjectConstructor initCB,
                                  RwPluginObjectDestructor termCB);
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
typedef struct RwObjectHasFrame RwObjectHasFrame;
typedef RwObjectHasFrame * (*RwObjectHasFrameSyncFunction)(RwObjectHasFrame *object);
struct RwObjectHasFrame
{
    RwObject                     object;
    RwLLLink                     lFrame;
    RwObjectHasFrameSyncFunction sync;
};
typedef struct RwBBox RwBBox;
struct RwBBox
{
    /* Must be in this order */
    RwV3d sup;   /**< Supremum vertex. */
    RwV3d inf;   /**< Infimum vertex. */
};
enum RwCameraProjection
{
    rwNACAMERAPROJECTION = 0,   /**<Invalid projection */
    rwPERSPECTIVE = 1,          /**<Perspective projection */
    rwPARALLEL = 2,             /**<Parallel projection */
    rwCAMERAPROJECTIONFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCameraProjection RwCameraProjection;
typedef struct RwCamera RWALIGN(RwCamera, rwMATRIXALIGNMENT);
typedef RwCamera   *(*RwCameraBeginUpdateFunc) (RwCamera * camera);
typedef RwCamera   *(*RwCameraEndUpdateFunc) (RwCamera * camera);
typedef struct RwFrustumPlane RwFrustumPlane;
struct RwFrustumPlane
{
        RwPlane             plane;
        RwUInt8             closestX;
        RwUInt8             closestY;
        RwUInt8             closestZ;
        RwUInt8             pad;
};
struct RwCamera
{
        RwObjectHasFrame    object;

        /* Parallel or perspective projection */
        RwCameraProjection  projectionType;

        /* Start/end update functions */
        RwCameraBeginUpdateFunc beginUpdate;
        RwCameraEndUpdateFunc endUpdate;

        /* The view matrix */
        RwMatrix            viewMatrix;

        /* The cameras image buffer */
        RwRaster           *frameBuffer;

        /* The Z buffer */
        RwRaster           *zBuffer;

        /* Cameras mathmatical characteristics */
        RwV2d               viewWindow;
        RwV2d               recipViewWindow;
        RwV2d               viewOffset;
        RwReal              nearPlane;
        RwReal              farPlane;
        RwReal              fogPlane;

        /* Transformation to turn camera z or 1/z into a Z buffer z */
        RwReal              zScale, zShift;

        /* The clip-planes making up the viewing frustum */
        RwFrustumPlane      frustumPlanes[6];
        RwBBox              frustumBoundBox;

        /* Points on the tips of the view frustum */
        RwV3d               frustumCorners[8];
};
extern RwInt32      RwCameraRegisterPlugin(RwInt32 size,
                                           RwUInt32 pluginID,
                                           RwPluginObjectConstructor
                                           constructCB,
                                           RwPluginObjectDestructor
                                           destructCB,
                                           RwPluginObjectCopy copyCB);
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
extern RwStream *_rpReadMaterialRights(RwStream *s, RwInt32 len, void *obj,
                                     RwInt32 off, RwInt32 size);
extern RwStream *_rpWriteMaterialRights(RwStream *s, RwInt32 len, const void *obj,
                                      RwInt32 off, RwInt32 size);
extern RwInt32 _rpSizeMaterialRights(const void *obj, RwInt32 off, RwInt32 size);
extern RwInt32 RpMaterialRegisterPlugin(RwInt32 size, RwUInt32 pluginID,
                                        RwPluginObjectConstructor constructCB,
                                        RwPluginObjectDestructor destructCB,
                                        RwPluginObjectCopy copyCB);
extern RwInt32 RpMaterialRegisterPluginStream(RwUInt32 pluginID,
                                              RwPluginDataChunkReadCallBack readCB,
                                              RwPluginDataChunkWriteCallBack writeCB,
                                              RwPluginDataChunkGetSizeCallBack getSizeCB);
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
extern RwStream    *_rpMeshWrite(const RpMeshHeader * meshHeader,
                                 const void *object,
                                 RwStream * stream,
                                 const RpMaterialList * matList);
extern RpMeshHeader *_rpMeshRead(RwStream * stream,
                                 const void *object,
                                 const RpMaterialList * matList);
extern RwInt32      _rpMeshSize(const RpMeshHeader *meshHeader,
                                const void *object);
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
extern RwInt32
RpGeometryRegisterPlugin(RwInt32 size,
                         RwUInt32 pluginID,
                         RwPluginObjectConstructor constructCB,
                         RwPluginObjectDestructor destructCB,
                         RwPluginObjectCopy copyCB);
extern RwInt32
RpGeometryRegisterPluginStream(RwUInt32 pluginID,
                               RwPluginDataChunkReadCallBack readCB,
                               RwPluginDataChunkWriteCallBack writeCB,
                               RwPluginDataChunkGetSizeCallBack getSizeCB);
typedef struct RpVertexNormal RpVertexNormal;
struct RpVertexNormal
{
    RwInt8          x;
    RwInt8          y;
    RwInt8          z;
    RwUInt8         pad; /* pad character to force alignment */
};
typedef struct RpWorldSector RpWorldSector;
struct RpWorldSector
{
    RwInt32                 type;

    RpTriangle              *triangles;             /* Triangles */

    RwV3d                   *vertices;              /* Vertex positions */
    RpVertexNormal          *normals;               /* Vertex normals */

    RwTexCoords             *texCoords[rwMAXTEXTURECOORDS]; /* Texture coordinates */

    RwRGBA                  *preLitLum;             /* Pre lit luminances */

    /* Pointer to memory allocated for vertex and poly pointers */
    RwResEntry              *repEntry;

    /* Atomics in this sectors */
    /* The pointers are frigged so they look like they are pointing to
       Atomics when they are pointing to here */
    RwLinkList              collAtomicsInWorldSector;       /* Coll priority */

    /* Lights in an atomic sector */
    RwLinkList              lightsInWorldSector;

    /* Outer bounding box of sector based on BSP planes */
    RwBBox                  boundingBox;

    /* Bounding box tightly enclosing geometry */
    RwBBox                  tightBoundingBox;

    /* The mesh which groups same material triangles together */
    RpMeshHeader            *mesh;

    /* The WorldSector object pipeline for this WorldSector */
    RxPipeline    *pipeline;

    /* Material list window base
     * (triangles in a given sector can "see"
     * the 256 materials from
     * MatList[matListWindowBase] -> MatList[matListWindowBase + 255])
     */
    RwUInt16                matListWindowBase;

    RwUInt16                numVertices;            /* Num vertices */
    RwUInt16                numTriangles;           /* Num triangles */
    RwUInt16                pad;
};
typedef struct RpSector RpSector;
struct RpSector
{
    RwInt32                 type;
};
extern RwInt32 RpWorldSectorRegisterPlugin(RwInt32 size, RwUInt32 pluginID,
                                           RwPluginObjectConstructor constructCB,
                                           RwPluginObjectDestructor destructCB,
                                           RwPluginObjectCopy copyCB);
extern RwInt32 RpWorldSectorRegisterPluginStream(RwUInt32 pluginID,
                                                 RwPluginDataChunkReadCallBack readCB,
                                                 RwPluginDataChunkWriteCallBack writeCB,
                                                 RwPluginDataChunkGetSizeCallBack getSizeCB);
extern RwInt32 RpLightRegisterPlugin(RwInt32 size,
                                     RwUInt32 pluginID,
                                     RwPluginObjectConstructor constructCB,
                                     RwPluginObjectDestructor destructCB,
                                     RwPluginObjectCopy copyCB);
extern RwStream *_rpReadAtomicRights(RwStream *s, RwInt32 len, void *obj,
                                     RwInt32 off, RwInt32 size);
extern RwStream *_rpWriteAtomicRights(RwStream *s, RwInt32 len, const void *obj,
                                      RwInt32 off, RwInt32 size);
extern RwInt32 _rpSizeAtomicRights(const void *obj, RwInt32 off, RwInt32 size);
extern RwInt32
RpAtomicRegisterPlugin(RwInt32 size,
                       RwUInt32 pluginID,
                       RwPluginObjectConstructor constructCB,
                       RwPluginObjectDestructor destructCB,
                       RwPluginObjectCopy copyCB);
extern RwInt32
RpClumpRegisterPlugin(RwInt32 size,
                      RwUInt32 pluginID,
                      RwPluginObjectConstructor constructCB,
                      RwPluginObjectDestructor destructCB,
                      RwPluginObjectCopy copyCB);
extern RwInt32
RpAtomicRegisterPluginStream(RwUInt32 pluginID,
                             RwPluginDataChunkReadCallBack
                             readCB,
                             RwPluginDataChunkWriteCallBack
                             writeCB,
                             RwPluginDataChunkGetSizeCallBack
                             getSizeCB);
#define RWRETURN(value) return(value)
#define RWFUNCTION(name)
#define RWASSERT(condition)
enum RwCriterionWorldID
{
    /* Guard value that should not be used. */
    rwID_NAWORLDID = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x00),

    /* The valid plugin IDs */
    /**< RpMaterial pluginID */
    rwID_MATERIALMODULE      = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x01),
    /**< RpMesh pluginID */
    rwID_MESHMODULE          = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x02),
    /**< RpGeometry pluginID */
    rwID_GEOMETRYMODULE      = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x03),
    /**< RpClump pluginID */
    rwID_CLUMPMODULE         = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x04),
    /**< RpLight pluginID */
    rwID_LIGHTMODULE         = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x05),
    /* Not used */
    rwID_COLLISIONMODULE     = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x06),
    /**< RpWorld pluginID */
    rwID_WORLDMODULE         = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x07),
    /* Not used */
    rwID_RANDOMMODULE        = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x08),
    /**< PluginID for RpWorld's objects */
    rwID_WORLDOBJMODULE      = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x09),
    /**< RpWorldSector pluginID */
    rwID_SECTORMODULE        = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x0A),
    /**< Binary RpWorld pluginID */
    rwID_BINWORLDMODULE      = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x0B),
    /**< RpWorld pipeline pluginID */
    rwID_WORLDPIPEMODULE     = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x0D),
    /**< Binary RpMesh pluginID */
    rwID_BINMESHPLUGIN       = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x0E),
    /**< RpWorld device pluginID */
    rwID_RXWORLDDEVICEMODULE = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x0F),
    /**< PluginID for platform specific serialization data */
    rwID_NATIVEDATAPLUGIN    = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x10),
    /**< \if xbox Vertex format pluginID \endif */
    /**< \if gcn  Vertex format pluginID \endif */
    rwID_VERTEXFMTPLUGIN     = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x11),
    rwCRITERIONWORLDIDFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCriterionWorldID RwCriterionWorldID;
enum RpWorldRenderOrder
{
    rpWORLDRENDERNARENDERORDER = 0,
    rpWORLDRENDERFRONT2BACK,  /**<Renders nearest sectors first */
    rpWORLDRENDERBACK2FRONT, /**<Renders furthest sectors first */
    rpWORLDRENDERORDERFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpWorldRenderOrder RpWorldRenderOrder;
typedef RpWorldSector *(*RpWorldSectorCallBackRender) (RpWorldSector *
                                                       worldSector);
typedef struct RpWorld RpWorld;
struct RpWorld
{
    RwObject            object;

    RwUInt32            flags;

    RpWorldRenderOrder  renderOrder;

    /* Materials */
    RpMaterialList      matList;

    /* The world stored as a BSP tree */
    RpSector           *rootSector;

    /* The number of texture coordinate sets in each sector */
    RwInt32             numTexCoordSets;

    /* Render frame used when last rendered */
    RwInt32             numClumpsInWorld;
    RwLLLink           *currentClumpLink;

    /* All the clumps in the world */
    RwLinkList          clumpList;

    /* All of the lights in the world */
    RwLinkList          lightList;

    /* Directional lights in the world */
    RwLinkList          directionalLightList;

    /* The worlds origin offset */
    RwV3d               worldOrigin;

    /* Bounding box around the whole world */
    RwBBox              boundingBox;

    /* The callbacks functions */
    RpWorldSectorCallBackRender renderCallBack;

    RxPipeline         *pipeline;
};
extern RpWorld     *RpWorldSectorGetWorld(const RpWorldSector * sector);
extern RwInt32      RpWorldRegisterPlugin(RwInt32 size, RwUInt32 pluginID,
                                          RwPluginObjectConstructor
                                          constructCB,
                                          RwPluginObjectDestructor
                                          destructCB,
                                          RwPluginObjectCopy copyCB);
extern RwInt32      RpWorldRegisterPluginStream(RwUInt32 pluginID,
                                                RwPluginDataChunkReadCallBack
                                                readCB,
                                                RwPluginDataChunkWriteCallBack
                                                writeCB,
                                                RwPluginDataChunkGetSizeCallBack
                                                getSizeCB);
extern RwBool _rpWorldPipeAttach(void);
extern RwStream *_rpReadWorldRights(RwStream *s, RwInt32 len, void *obj,
                                    RwInt32 off, RwInt32 size);
extern RwStream *_rpWriteWorldRights(RwStream *s, RwInt32 len, const void *obj,
                                     RwInt32 off, RwInt32 size);
extern RwInt32 _rpSizeWorldRights(const void *obj, RwInt32 off, RwInt32 size);
extern RwStream *_rpReadSectRights(RwStream *s, RwInt32 len, void *obj,
                                   RwInt32 off, RwInt32 size);
extern RwStream *_rpWriteSectRights(RwStream *s, RwInt32 len, const void *obj,
                                    RwInt32 off, RwInt32 size);
extern RwInt32 _rpSizeSectRights(const void *obj, RwInt32 off, RwInt32 size);
extern RwBool _rpWorldObjRegisterExtensions(void);
typedef struct RpWorldCameraExt RpWorldCameraExt;
struct RpWorldCameraExt
{
    /* List of atomic sectors within the frustum */
    RpWorldSector     **frustumSectors;
    RwInt32             space;
    RwInt32             position;

    /* The world the camera lives in */
    RpWorld            *world;

    /* Old start update/end update functions */
    RwCameraBeginUpdateFunc oldBeginUpdate;
    RwCameraEndUpdateFunc oldEndUpdate;

    /* Old sync function */
    RwObjectHasFrameSyncFunction oldSync;
};
typedef struct RpWorldAtomicExt RpWorldAtomicExt;
struct RpWorldAtomicExt
{
    /* The world the atomic lives in */
    RpWorld            *world;

    /* Old sync function */
    RwObjectHasFrameSyncFunction oldSync;
};
typedef struct RpWorldClumpExt RpWorldClumpExt;
struct RpWorldClumpExt
{
    /* The world the clump lives in */
    RpWorld            *world;

    /* clumpsInFrustumID - used to prevent multiple invocations
     * of the clump frustum callback
     */
    RwUInt32            clumpsInFrustumID;
};
typedef struct RpWorldLightExt RpWorldLightExt;
struct RpWorldLightExt
{
    /* The world the light lives in */
    RpWorld            *world;

    /* Old sync function */
    RwObjectHasFrameSyncFunction oldSync;
};
typedef struct rpWorldObjGlobals rpWorldObjGlobals;
struct rpWorldObjGlobals
{
    RwFreeList         *tieFreeList;
    RwFreeList         *lightTieFreeList;
    RwUInt32            clumpsInFrustumID;
};
extern RwInt32 cameraExtOffset;
extern RwInt32 atomicExtOffset;
extern RwInt32 clumpExtOffset;
extern RwInt32 lightExtOffset;
extern void * WorldObjectOpen(void *instance, RwInt32 offset, RwInt32 size __RWUNUSED__);
extern void * WorldObjectClose(void *instance, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__);
extern void * WorldInitCameraExt(void *object, RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__);
extern void * WorldCopyCameraExt(void *dstObject, const void *srcObject, RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__);
extern void * WorldDeInitCameraExt(void *object, RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__);
extern void * WorldInitAtomicExt(void *object, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__);
extern void * WorldDeInitAtomicExt(void *object, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__);
extern void * WorldInitClumpExt(void *object, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__);
extern void * WorldCopyClumpExt(void *dstObject, const void *srcObject, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__);
extern void * WorldDeInitClumpExt(void *object, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__);
extern void * WorldInitLightExt(void *object, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__);
extern void * WorldCopyLightExt(void *dstObject, const void *srcObject, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__);
extern void * WorldDeInitLightExt(void *object, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__);
extern RwStream * writeGeometryMesh(RwStream * stream, RwInt32 binaryLength __RWUNUSED__, const void *object, RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__);
extern RwInt32 sizeGeometryMesh(const void *object, RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__);
extern RwStream * writeGeometryNative(RwStream *stream, RwInt32 binaryLength __RWUNUSED__, const void *object, RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__);
extern RwStream * readGeometryNative(RwStream *stream, RwInt32 binaryLength __RWUNUSED__, void *object, RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__);
extern RwInt32 sizeGeometryNative(const void *object, RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__);
extern RwStream * writeWorldSectorNative(RwStream *stream, RwInt32 binaryLength __RWUNUSED__, const void *object, RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__);
extern RwStream * readWorldSectorNative(RwStream *stream, RwInt32 binaryLength __RWUNUSED__, void *object, RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__);
extern RwInt32 sizeWorldSectorNative(const void *object, RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__);

/******************************************************************************
 WorldCopyAtomicExt
 Copy from another atomic
 On entry   : Object, object, offset in object, size in object
 On exit    : Object pointer on success
 */
/* MFL.exe 0x0053eac0 */
static void        *
WorldCopyAtomicExt(void *dstObject,
                   const void * srcObject __RWUNUSEDRELEASE__,
                   RwInt32 offset __RWUNUSED__,
                   RwInt32 size __RWUNUSED__)
{
    RWFUNCTION(RWSTRING("WorldCopyAtomicExt"));

    RWASSERT(dstObject);
    RWASSERT(srcObject);

    /* Currently cloning an atomic does not clone the frame.  Adding an atomic
     * with no frame to the world is meaningless.
     */

    /* Assume that sync function is OK */

    RWRETURN(dstObject);
}

/****************************************************************************
 readGeometryMesh
 Reads a geometry's mesh from the given stream
 On entry   : Stream to read from
            : Size of meshed when serialised (in bytes)
            : Object (geometry)
            : Plugin data offset (not used)
            : Plugin data size (not used)
 On exit    : Stream pointer on success
 */
/* MFL.exe 0x004f42d0 */
static RwStream    *
readGeometryMesh(RwStream * stream,
                 RwInt32 binaryLength __RWUNUSED__,
                 void *object,
                 RwInt32 offsetInObject __RWUNUSED__,
                 RwInt32 sizeInObject __RWUNUSED__)
{
    RpGeometry         *geometry = (RpGeometry *) object;

    RWFUNCTION(RWSTRING("readGeometryMesh"));

    RWASSERT(geometry);
    RWASSERT(stream);

    geometry->mesh = _rpMeshRead(stream, (const void *) geometry,
                        &geometry->matList);
    if (geometry->mesh)
    {
        RWRETURN(stream);
    }
    else
    {
        RWRETURN((RwStream *)NULL);
    }
}

/****************************************************************************
 writeSectorMesh
 Writes a sector's mesh to the given stream
 On entry   : Stream to read from
            : Size of meshed when serialised (in bytes) (not used)
            : Object (world sector)
            : Plugin data offset (not used)
            : Plugin data size (not used)
 On exit    : Stream pointer on success
 */
/* MFL.exe 0x004f43c0 */
static RwStream    *
writeSectorMesh(RwStream * stream,
                RwInt32 binaryLength __RWUNUSED__,
                const void *object,
                RwInt32 offsetInObject __RWUNUSED__,
                RwInt32 sizeInObject __RWUNUSED__)
{
    const RpWorldSector *sector = (const RpWorldSector *) object;
    const RpWorld      *world;

    RWFUNCTION(RWSTRING("writeSectorMesh"));

    RWASSERT(sector);
    RWASSERT(stream);

    world = RpWorldSectorGetWorld(sector);
    RWRETURN(_rpMeshWrite(sector->mesh, (const void *) world,
                          stream, &world->matList));
}

/****************************************************************************
 readSectorMesh
 Reads a sector's mesh from the given stream
 On entry   : Stream to read from
            : Size of meshed when serialised (in bytes)
            : Object (world sector)
            : Plugin data offset (not used)
            : Plugin data size (not used)
 On exit    : Stream pointer on success
 */
/* MFL.exe 0x004f43f0 */
static RwStream    *
readSectorMesh(RwStream * stream,
               RwInt32 binaryLength __RWUNUSED__,
               void *object,
               RwInt32 offsetInObject __RWUNUSED__,
               RwInt32 sizeInObject __RWUNUSED__)
{
    RpWorldSector      *sector = (RpWorldSector *) object;
    const RpWorld      *world;

    RWFUNCTION(RWSTRING("readSectorMesh"));

    RWASSERT(sector);
    RWASSERT(stream);

    world = RpWorldSectorGetWorld(sector);
    sector->mesh =
        _rpMeshRead(stream, (const void *) world, &world->matList);
    if (sector->mesh)
    {
        RWRETURN(stream);
    }
    else
    {
        RWRETURN((RwStream *)NULL);
    }
}

/****************************************************************************
 sizeSectorMesh
 Gets the size of a sector's mesh (when serialised)
 On entry   : Object (world sector)
            : Plugin data offset (not used)
            : Plugin data size (not used)
 On exit    : Size of mesh when serialised (in bytes)
 */
/* MFL.exe 0x004f4420 */
static RwInt32
sizeSectorMesh(const void *object,
               RwInt32 offsetInObject __RWUNUSED__,
               RwInt32 sizeInObject __RWUNUSED__)
{
    const RpWorldSector *sector = (const RpWorldSector *)object;

    RWFUNCTION(RWSTRING("sizeSectorMesh"));
    RWASSERT(sector);

    RWRETURN(_rpMeshSize(sector->mesh,
             (RwObject *)RpWorldSectorGetWorld(sector)));
}

/******************************************************************************
 _rpWorldObjRegisterExtensions
 Registers the necessary memory/functions to maintain objects in the world
 On entry   :
 On exit    : TRUE on success
 */
/* MFL.exe 0x004f3590 */
RwBool
_rpWorldObjRegisterExtensions(void)
{
    RwInt32             status;

    RWFUNCTION(RWSTRING("_rpWorldObjRegisterExtensions"));

    /* Just try and register everything, then see later if we failed. */

    /* This registers global memory and the module with the engine for instance intialization */
    status = RwEngineRegisterPlugin(sizeof(rpWorldObjGlobals),
                                    rwID_WORLDOBJMODULE,
                                    WorldObjectOpen, WorldObjectClose);

    /* This attaches the necessary data to lights, clumps, atomics and cameras
     * to facilitate their placement and maintenance within a world.
     * None saves binary data, so use not a TK ID
     */
    cameraExtOffset = RwCameraRegisterPlugin(sizeof(RpWorldCameraExt),
                                             rwID_WORLDOBJMODULE,
                                             WorldInitCameraExt,
                                             WorldDeInitCameraExt,
                                             WorldCopyCameraExt);
    status |= cameraExtOffset;

    atomicExtOffset = RpAtomicRegisterPlugin(sizeof(RpWorldAtomicExt),
                                             rwID_WORLDOBJMODULE,
                                             WorldInitAtomicExt,
                                             WorldDeInitAtomicExt,
                                             WorldCopyAtomicExt);
    status |= atomicExtOffset;

    clumpExtOffset = RpClumpRegisterPlugin(sizeof(RpWorldClumpExt),
                                           rwID_WORLDOBJMODULE,
                                           WorldInitClumpExt,
                                           WorldDeInitClumpExt,
                                           WorldCopyClumpExt);
    status |= clumpExtOffset;

    lightExtOffset = RpLightRegisterPlugin(sizeof(RpWorldLightExt),
                                           rwID_WORLDOBJMODULE,
                                           WorldInitLightExt,
                                           WorldDeInitLightExt,
                                           WorldCopyLightExt);
    status |= lightExtOffset;

    /* And now the mesh serialisation modules for geometry and sectors.
     * These are used just to register something to hand serialisation
     * callbacks from.
     */
    status |=
        RpGeometryRegisterPlugin(0,
                                 rwID_BINMESHPLUGIN,
                                 (RwPluginObjectConstructor)NULL,
                                 (RwPluginObjectDestructor)NULL,
                                 (RwPluginObjectCopy)NULL);

    status |=
        RpWorldSectorRegisterPlugin(0,
                                    rwID_BINMESHPLUGIN,
                                    (RwPluginObjectConstructor)NULL,
                                    (RwPluginObjectDestructor)NULL,
                                    (RwPluginObjectCopy)NULL);
    status |=
        RpGeometryRegisterPluginStream(rwID_BINMESHPLUGIN,
                                       readGeometryMesh,
                                       writeGeometryMesh,
                                       sizeGeometryMesh);
    status |=
        RpWorldSectorRegisterPluginStream(rwID_BINMESHPLUGIN,
                                          readSectorMesh,
                                          writeSectorMesh,
                                          sizeSectorMesh);

    /* Platform specific serialization */
    status |= RpGeometryRegisterPlugin(0, rwID_NATIVEDATAPLUGIN,
                                       (RwPluginObjectConstructor)NULL,
                                       (RwPluginObjectDestructor)NULL,
                                       (RwPluginObjectCopy)NULL);

    status |= RpWorldSectorRegisterPlugin(0, rwID_NATIVEDATAPLUGIN,
                                          (RwPluginObjectConstructor)NULL,
                                          (RwPluginObjectDestructor)NULL,
                                          (RwPluginObjectCopy)NULL);

    status |= RpGeometryRegisterPluginStream(rwID_NATIVEDATAPLUGIN,
                                             readGeometryNative,
                                             writeGeometryNative,
                                             sizeGeometryNative);

    status |= RpWorldSectorRegisterPluginStream(rwID_NATIVEDATAPLUGIN,
                                                readWorldSectorNative,
                                                writeWorldSectorNative,
                                                sizeWorldSectorNative);

    /* We now attached the rights serialisation plugin. */
    status |= RpAtomicRegisterPlugin(0, rwID_RIGHTTORENDER,
                                     (RwPluginObjectConstructor)NULL,
                                     (RwPluginObjectDestructor)NULL,
                                     (RwPluginObjectCopy)NULL);

    status |= RpAtomicRegisterPluginStream(rwID_RIGHTTORENDER,
                                           _rpReadAtomicRights,
                                           _rpWriteAtomicRights,
                                           _rpSizeAtomicRights);

    status |= RpWorldRegisterPlugin(0, rwID_RIGHTTORENDER,
                                    (RwPluginObjectConstructor)NULL,
                                    (RwPluginObjectDestructor)NULL,
                                    (RwPluginObjectCopy)NULL);

    status |= RpWorldRegisterPluginStream(rwID_RIGHTTORENDER,
                                          _rpReadWorldRights,
                                          _rpWriteWorldRights,
                                          _rpSizeWorldRights);

    status |= RpWorldSectorRegisterPlugin(0, rwID_RIGHTTORENDER,
                                          (RwPluginObjectConstructor)NULL,
                                          (RwPluginObjectDestructor)NULL,
                                          (RwPluginObjectCopy)NULL);

    status |= RpWorldSectorRegisterPluginStream(rwID_RIGHTTORENDER,
                                                _rpReadSectRights,
                                                _rpWriteSectRights,
                                                _rpSizeSectRights);

    status |= RpMaterialRegisterPlugin(0, rwID_RIGHTTORENDER,
                                          (RwPluginObjectConstructor)NULL,
                                          (RwPluginObjectDestructor)NULL,
                                          (RwPluginObjectCopy)NULL);

    status |= RpMaterialRegisterPluginStream(rwID_RIGHTTORENDER,
                                                _rpReadMaterialRights,
                                                _rpWriteMaterialRights,
                                                _rpSizeMaterialRights);

    if (status < 0)
    {
        /* Something went wrong */
        RWRETURN(FALSE);
    }

    /* And the pipe specific extensions */
    if (!_rpWorldPipeAttach())
    {
        /* Oooops */
        RWRETURN(FALSE);
    }

    /* Success */
    RWRETURN(TRUE);
}
