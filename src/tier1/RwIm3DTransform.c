/* compile: /G5 */
/* RwIm3DTransform (MFL.exe 0x004dc920): RenderWare Graphics 3.7 (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rw/src/pipe/p2/baim3d.c, function RwIm3DTransform), a byte-identical decompilation of the RenderWare
 * build of an Xbox game; the Win32 branches it kept are RenderWare 3.7.2 source. Used with credit; split into one file per
 * function, the declarations it needs copied from the codfh headers named in the comments (PC build: no _XBOX, no RWDEBUG).
 * Changes: no renames; data definitions of the unit are extern declarations here.
 * Byte-identical to MFL.exe 0x004dc920 (tools/cmpfn.py /O2 /G5, relocations masked). */
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <stdarg.h>
typedef int  RwInt32;
typedef unsigned int RwUInt32;
typedef short RwInt16;
typedef unsigned short RwUInt16;
typedef unsigned char RwUInt8;
typedef char RwChar;
typedef float RwReal;
typedef RwInt32 RwBool;
#define RWALIGN(type, x)   __declspec(align(x)) type
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
enum RwCriterionPluginID
{
    rwID_COREPLUGIN             = 0x01,
    rwID_WORLDPLUGIN            = 0x02,
    rwID_TOOLPLUGIN             = 0x03,
    rwID_TOOL2PLUGIN            = 0x04,
    rwCRITERIONPLUGINIDFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCriterionPluginID RwCriterionPluginID;
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
extern RwBool
RwFrameDestroy(RwFrame * frame);
#define RWECODE(a,b) a,
enum RwErrorCodePlugin_errcore 
{
RWECODE(E_RW_BADWINDOW,
        "Invalid view window dimensions supplied")
RWECODE(E_RW_CHUNKTYPEGET,
        "Unable to get a chunk of the given type")
RWECODE(E_RW_DEVICEERROR,
        "Device specific error: %s")
RWECODE(E_RW_DEVICEOPEN,
        "Request to open device system refused")
RWECODE(E_RW_DEVICESTART,
        "Attempt to start Device failed")
RWECODE(E_RW_ENDOFSTREAM,
        "At the end of the stream.")
RWECODE(E_RW_FRAMEDESTROYSTATIC,
        "RwFrameDestroy called on static frame.")
RWECODE(E_RW_FRAMEBUFFERMISMATCH,
        "Resolutions of parent rasters of frame buffer and Z buffer are different")
RWECODE(E_RW_FREELISTFREED,
        "Free List value already on the free list")
RWECODE(E_RW_FREELISTINVADDRESS,
        "Invalid Free List memory address")
RWECODE(E_RW_FREELISTINVMEMBOUND,
        "Invalid Free List memory boundary")
RWECODE(E_RW_FREELISTTRASH,
        "An unused Free List entry has been overwritten")
RWECODE(E_RW_INSUFFICIENTRESOURCES,
        "Insufficient resources to satisfy the allocation of %d bytes.")
RWECODE(E_RW_INVSTREAMACCESSTYPE,
        "Invalid stream access type.")
RWECODE(E_RW_INVSTREAMTYPE,
        "Invalid stream type.")
RWECODE(E_RW_NEGCLIPPLANE,
        "Negative positioned clip planes are invalid")
RWECODE(E_RW_NOCAMERA,
        "Cannot render until a Camera has been created")
RWECODE(E_RW_NOFRAMEBUFFER,
        "Camera has no frame buffer raster")
RWECODE(E_RW_NOPARENT,
        "The given object has no parent")
RWECODE(E_RW_RASTERRECT,
        "Rectangle is not totally within Raster")
RWECODE(E_RW_RASTERSTACK,
        "Insufficient Raster stack space available")
RWECODE(E_RW_RASTERSTACKEMPTY,
        "No Raster Currently on Stack")
RWECODE(E_RW_READTEXMASK,
        "Unable to read Texture %s / Mask %s")
RWECODE(E_RW_STREAMOPEN,
        "Unable to open stream : %s")
RWECODE(E_RW_SYSTEMREQUEST,
        "A system request call has failed, request code : 0x%x")
RWECODE(E_RW_ZEROLENGTH,
        "A Vector of Zero Length was passed for normalizing")
RWECODE(E_RX_CANNOT_TRANSFER_BETWEEN_NODES,
        "Node %s cannot output to node %s")
RWECODE(E_RX_CANNOT_TRANSFER_FROM_NODE_TO_PIPELINE,
        "Node %s cannot output to specified pipeline")
RWECODE(E_RX_CYCLICPIPELINE,
        "Pipeline contains cycles; illegal")
RWECODE(E_RX_DEP_DEPENDENCIESMISMATCH,
        "\n*** dependencies cannot be satisfied.\n*** rxCLREQ_REQUIRED on cluster %s, originating\n*** with node %s, not serviced by a rxCLVALID_VALID -\n*** blocked at node %s, output #%ld (\"%s\").")
RWECODE(E_RX_DEP_DUPLICATECLUSTERDEFS,
        "\n*** Node %s specifies RxClusterDefinition for cluster %s more than once in\n*** clusters of interest array. Distinct clusters within a pipeline must reference\n*** distinct RxClusterDefinitions, even if the clusters contain the same data type\n")
RWECODE(E_RX_DEP_NULLCLUSTERDEF,
        "Node %s specified with RxClusterDefinition pointer NULL for cluster of interest %d\n")
RWECODE(E_RX_DEP_OUTOFMEMORY,
        "Dependency chasing; memory alloc failed")
RWECODE(E_RX_EMPTYPIPELINE,
        "RwPipeline2Execute cannot execute a pipeline with no nodes :)")
RWECODE(E_RX_FRAGMENTEDPIPELINE,
        "Pipeline forms two or more unconnected graphs; illegal")
RWECODE(E_RX_IM3DNOTACTIVE,
        "Cannot render Im3D primitives outside of a RwIm3dTransform()/RwIm3dEnd() pair")
RWECODE(E_RX_INVALIDENTRYPOINT,
        "Pipeline has an invalid entry point")
RWECODE(E_RX_INVALIDPRIMTYPE,
        "Unknown primitive type %d")
RWECODE(E_RX_INVALIDRESUMEPIPELINE,
        "RwPipeline2Execute cannot resume a different pipeline to the one previously interrupted")
RWECODE(E_RX_LOCKEDPIPE,
        "Illegal operation on a locked pipeline")
RWECODE(E_RX_NODETOOMANYCLUSTERSOFINTEREST,
        "Node contains more than RXNODEMAXCLUSTERSOFINTEREST clusters of interest; illegal")
RWECODE(E_RX_NODETOOMANYOUTPUTS,
        "Node contains more than RXNODEMAXOUTPUTS outputs; illegal")
RWECODE(E_RX_PIPELINETOOMANYNODES,
        "Maximum nodes per pipeline exceeded! You may increase the limit by changing the value of _rxPipelineMaxNodes BEFORE starting RenderWare")
RWECODE(E_RX_NODE_EXECUTION_FAILED,
        "Node execution failed - %s")
RWECODE(E_RX_NOVERTS,
        "Cannot render Im3D primitive - not enough vertices transformed")
RWECODE(E_RX_PACKETPTRINVALID,
        "Value of input/output interruption packets pointer not as expected")
RWECODE(E_RX_PACKETSCOPYFAILED,
        "Failed to make copies of packets input to RxPipelineExecute()")
RWECODE(E_RX_RUNFROMNOTVALID,
        "RunFrom node not a member of the specified pipeline")
RWECODE(E_RX_RUNTOANDRUNFROM,
        "RwPipeline2Execute cannot accept both RunTo *and* RunFrom")
RWECODE(E_RX_RUNTONOTVALID,
        "RunTo node not a member of the specified pipeline")
RWECODE(E_RX_TOOMANYVERTS,
        "More than 65536 vertices passed to RwIm3DTransform; illegal")
RWECODE(E_RX_UNFINISHEDPIPELINE,
        "RwPipeline2Execute must resume and finish an interrupted pipeline")
RWECODE(E_RX_UNLOCKEDPIPE,
        "Illegal operation on an unlocked pipeline")
RWECODE(E_RX_UNPROCESSED_PACKETS,
        "Unprocessed packets found in finished execution context")
RWECODE(E_RX_UNSATISFIED_REQUIREMENTS,
        "Cannot send packet between pipelines, requirements not satisfied. Cluster '%s' is missing")
RWECODE(E_RW_INSUFFICIENTRASTER, 
		"original texture already remove. RwRaster pointer was broken")
RWECODE(E_RW_INSUFFICIENTDIC, 
		"texture dictonanry not use and already remove")
    rwLASTERROR_errcore = RWFORCEENUMSIZEINT
};
typedef enum RwErrorCodePlugin_errcore RwErrorCodePlugin_errcore;
typedef struct RwError RwError;
struct RwError
{
    RwInt32     pluginID;  /**< Internal Use */
    RwInt32     errorCode; /**< Internal Use */
};
extern RwError *RwErrorSet(RwError *code);
extern RwInt32 _rwerror(RwInt32 code, ...);
#define RWRETURN(value) return(value)
#define RWERROR(errorcode)                              \
MACRO_START                                             \
{                                                       \
    RwError _rwErrorCode;                               \
                                                        \
    _rwErrorCode.pluginID = rwID_COREPLUGIN;            \
    _rwErrorCode.errorCode = _rwerror errorcode;        \
                                                        \
    RwErrorSet(&_rwErrorCode);                          \
}                                                       \
MACRO_STOP
#define RWAPIFUNCTION(name)    /* No op */
#define RWASSERT(condition)    /* No op */
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
#define RWPLUGINOFFSET(_type, _base, _offset)                   \
   ((_type *)((RwUInt8 *)(_base) + (_offset)))
typedef struct RwModuleInfo RwModuleInfo;
struct RwModuleInfo
{
        RwInt32     globalsOffset;
        RwInt32     numInstances;
};
extern void         *RwEngineInstance;
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
    /* RWPUBLICEND
     * headBlock is used mainly for debugging.
     * RWPUBLIC */
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
extern RwInt32 _rxPipelineMaxNodes;
#define RXNODEMAXCLUSTERSOFINTEREST 32
#define RXNODEMAXOUTPUTS            32
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
typedef struct RwReqEntry RwReqEntry;
typedef struct RwScopeTrace RwScopeTrace;
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
    /* RWPUBLICEND
     * nothing more need currently be specified, because a
     * cluster is intimately associated with a C struct!
     * RWPUBLIC     */
    RwChar             *name;                  /**< Name */
    /* RWPUBLICEND
     * as a rule, clusters are created by nodes that "know" the semantics of
     * the cluster, so don't need to be informed of the cluster's stride
     * (nor need a given cluster always be created with the same stride);
     * occasionally, however, esp. when construction-time (vs. pipe
     * execution-time) node customisation APIs are used to make nodes
     * handle additional clusters, they are left creating clusters of
     * which they have no semantic knowledge - hence defaultStride
     * RWPUBLIC     */
    RwUInt32            defaultStride;         /**< Default stride */
    /* RWPUBLICEND
     * [optional] attribute support fields follow; these are used by nodes to
     * convey context-specific flags with a cluster
     * RWPUBLIC     */
    RwUInt32            defaultAttributes;     /**< Default attributes */
    /* RWPUBLICEND
     * this string identifies the "name space" for attribute values
     * RWPUBLIC     */
    const RwChar         *attributeSet;           /**< Attribute set */
};
struct rxReq
{
    RwUInt32             numused;
    RwUInt32             numalloced;
    RwUInt32             unavailslots;
        /* RwReqEntry collection wot we maintain */
    RwReqEntry          *reqentries;
        /* back pointer (to "owning" RxPipelineNode) */
    RxPipelineNode      *node;
};
struct RwReqEntry
{
        /* RxClusterDefinition wot we describe requirements on */
    RxClusterDefinition *clusterDef;
    RxClusterValidityReq required;
    RwUInt32             inputs;
        /* which RwScopeTrace have we been assigned to? RwReqEntrys
         * maintain this value so overlapping RwScopeTraces (which
         * need to be merged) can be identified */
    RwScopeTrace        *st;
        /* link pointer for list maintained by RwScopeTrace:
         * RwScopeTrace.head -> next -> next -> ... */
    RwReqEntry          *next;
        /* back pointer (to "owning" rxReq) */
    rxReq               *req;
    RwUInt32             outbf;
    RwUInt32             assignedslot;
        /* remember that requirements get propagated up the pipeline;
         * to provide informative errors, we want to know where the
         * requirement originated */
    RxPipelineNode      *originatingNode;
};
struct RwScopeTrace
{
        /* linked list of RwReqEntrys starts here! */
    RwReqEntry          *head;
        /* if scope traces are merged... */
    RwScopeTrace        *continuation;
        /* for master list of root-level scope traces */
    RwScopeTrace        *next;
        /* for merging: used to find root if
         * we are a child (= previously merged) trace */
    RwScopeTrace        *parent;
};
struct RxOutputSpec
{
     RwChar             *name;                  /**< Name */
    /* RWPUBLICEND
     * During dependency-chasing, the following fields are used to
     * work out which clusters each node creates, which they invalidate
     * and which they leave unchanged.
     *
     * The node should not usually destroy clusters because it assumes
     * they are no longer relevant - the pipeline execution code makes
     * this determination; however, there are occassions when the actions
     * of a node cause the contents of a cluster to become invalid and in
     * this case it should be destroyed to prevent subsequent nodes
     * receiving invalid data.
     * RWPUBLIC     */
     RxClusterValid     *outputClusters;        /**< States of clusters of interest on output */
    /* RWPUBLICEND
     * If the packet contains clusters which don't appear in the
     * ClustersOfInterest array; what happens to these?
     * RWPUBLIC     */
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
    /* RWPUBLICEND
     * Force the cluster to be present for the current node,
     * whether it's created by prior nodes or required by
     * later nodes or not
     * RWPUBLIC     */
    RxClusterForcePresent forcePresent;         /**< Specifies whether the cluster should be forced present */
    RwUInt32              reserved;             /**< Omit or initialize to zero */
};
struct RxIoSpec
{
    RwUInt32              numClustersOfInterest;/**< Number of Clusters of interest */
    RxClusterRef         *clustersOfInterest;   /**< Clusters of interest array */
    /* RWPUBLICEND
     * runs parallel to the ClustersOfInterest array; it specifies
     * what is required of each cluster on input to the node.
     * RWPUBLIC     */
    RxClusterValidityReq *inputRequirements;    /**< Requirements of the clusters of interest on input to the node */
    RwUInt32              numOutputs;           /**< Number of outputs from the node */
    /* RWPUBLICEND
     * What's true of the clusters of interest (and others)
     * on output from this node.
     * RWPUBLIC     */
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
    /* RWPUBLICEND
     * called after init - opportunity to msg other
     * nodes, which have now been initialised
     * RWPUBLIC     */
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
    /* RWPUBLICEND
     * Standard and user-defined node definitions are non-editable
     * (they are probably globals), but functions such as
     * RxPipelineNodeRequestCluster() and RxPipelineNodeReplaceCluster()
     * create temporary, modified node definitions for RxPipelineNodes
     * and these will need to be freed.
     * RWPUBLIC     */
    RxNodeDefEditable   editable;               /**< Flags whether a node is editable */
    /* RWPUBLICEND
     * How many PipelineNodes (in pipes) have been made from this node
     * ##**##**##NB##**##**##
     * InputPipesCnt will only be modified in RxPipelineLock/Unlock()
     * and only in RxPipelineDestroy IF the pipeline being destroyed
     * is locked. These are references to UNLOCKED pipeline nodes
     * RWPUBLIC     */
    RwInt32             InputPipesCnt;          /**< Count of the unlocked pipelines containing this node */
};
struct RxPipelineCluster
{
    RxClusterDefinition *clusterRef;            /**< Cluster refererence */
    RwUInt32             creationAttributes;    /**< Creation Attributes */
};
struct RxCluster
{
    /* RWPUBLICEND
     * e.g rxCLFLAGS_NULL, rxCLFLAGS_CLUSTERVALID, rxCLFLAGS_EXTERNAL,
     * rxCLFLAGS_EXTERNALMODIFIABLE, rxCLFLAGS_MODIFIED
     * RWPUBLIC     */
    RwUInt16            flags;                  /**< Bitfield of flags e.g. modification permissions */
    RwUInt16            stride;                 /**< Stride in bytes of the cluster's data */
    void               *data;                   /**< The Cluster's data */
    /* RWPUBLICEND
     * For RxClusterGetCursorData(), RxClusterCursorInc(), etc.
     * RWPUBLIC     */
    void               *currentData;            /**< A 'cursor', referencing the current element in the data */
    RwUInt32            numAlloced;             /**< Allocated count */
    RwUInt32            numUsed;                /**< Used count */
    RxPipelineCluster  *clusterRef;             /**< Valid after a cluster has been locked for writing,
                                                 *   otherwise NULL. Analog of \ref RxPipelineNode and
                                                 *   its 'nodeDef' member. */
    /* RWPUBLICEND
     * meaning is context specific;
     * see ClusterRef->clusterRef->attributeSet
     * RWPUBLIC     */
    RwUInt32            attributes;             /**< Attributes */
};
struct RxPacket
{
    /* RWPUBLICEND
     * Following commented out because we can't keep track
     * of changes in Cluster sizes easily (many Packets can
     * reference the same Cluster data potentially).
     * RwUInt32                 Size;  _field Size in bytes of this packet (including the header or just the clusters?)
     * RWPUBLIC     */
    RwUInt16            flags;                  /**< Flags to guide pipeline execution */
    /* RWPUBLICEND
     * The max number of Clusters simultaneously present in the current
     * pipeline and hence the (real) length of the Clusters[] array
     * RWPUBLIC     */
    RwUInt16            numClusters;            /**< Maximum number of clusters simultanesouly present in the current pipeline */
    /* RWPUBLICEND
     * It's cheaper to retain use of the current packet when passing between
     * pipelines, instead of copying everything across into the destination
     * pipeline's embedded packet, so we will eventually use global packets
     * of a common/max cluster array length, and then we will just shuffle
     * clusters in-place real-quick-like.
     * RWPUBLIC     */
    RxPipeline         *pipeline;               /**< The pipeline in which this packet is embedded (the current pipeline) */
    /* RWPUBLICEND
     * for each input of the current node, which cluster slot corresponds?
     * RWPUBLIC     */
    RwUInt32           *inputToClusterSlot;     /**< LUT to locate clusters of interest in the packet */
    /* RWPUBLICEND
     * bit-fields: for each output (0..31) which slots persist to output node?
     * RWPUBLIC     */
    RwUInt32           *slotsContinue;          /**< Bitfields specifying clusters persisting to output node */
    RxPipelineCluster **slotClusterRefs;        /**< Internal Use */
    RxCluster           clusters[1];            /**< Cluster array large enough for widest part of the pipeline */
    /* RWPUBLICEND
     * extra memory after this header continues this array - there are
     * enough RwClusters for the 'widest' part of the pipeline
     * RWPUBLIC     */
};
struct RxPipelineNode
{
    RxNodeDefinition   *nodeDef;            /**< Node definition reference */
    RwUInt32            numOutputs;         /**< Output count */
    RwUInt32           *outputs;            /**< Output array, indexing the pipeline's array of pipeline nodes */
    /* RWPUBLICEND
     * for each cluster slot, which cluster is assigned?
     * Point of interest: this field is guaranteed to be NULL when the
     * containing pipeline is locked, and guaranteed to be non-NULL
     * after a successful Unlock() call on the containing pipe. This
     * behaviour can be used to derive assert()ions to guard against
     * misuse of a node's construction time and run time APIs.
     * RWPUBLIC     */
    RxPipelineCluster **slotClusterRefs;    /**< For packets passing through this node, SlotClusterRefs[n]
                                             * identifies the cluster to be found at index n in the packet's
                                             * Clusters[] array. These assignments are determined at pipeline
                                             * Unlock() time and are good for the life of the pipeline. */
    /* RWPUBLICEND
     * bit-field: for each output node (0..31), which clusters persist?
     * The first entry is the AND of all subsequent entries,
     * used to optimize EMBEDDEDPACKETBETWEENNODESMACRO().
     * RWPUBLIC     */
    RwUInt32           *slotsContinue;      /**< Cluster persistence table for the node's outputs */
    void               *privateData;        /**< Pointer to the pipeline Node's private data */
    /* RWPUBLICEND
     * for each cluster of interest of this node, which cluster slot corresponds?
     * RWPUBLIC     */
    RwUInt32           *inputToClusterSlot; /**< LUT to locate clusters of interest in packets passing through this pipeline Node */
    /* RWPUBLICEND
     * Max number of bytes in output packets until packets are split
     * (obsolete but not implemented again yet)
     * RwUInt32          dispatchThreshold;  _field Maximum byte count until packets are split
     *
     * topSortData points at 'topSortData' when the pipe's locked (it's used in
     * topological sorting during \ref RxLockedPipeUnlock). When unlocked, it's NULL.
     * RWPUBLIC     */
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
    /* RWPUBLICEND */
    /*
     * This struct is extensible so we can change effective node parameters
     * at a later date without breaking all nodes in existence. Hurrah.
     * Access macros in p2macros.h/c
     */
    /* RWPUBLIC */
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
    /* RWPUBLICEND
     * Pipelines must be locked before creation/modification
     * RWPUBLIC     */
    RwBool                     locked;                /**< Flags whether the pipeline is locked for editing */
    RwUInt32                   numNodes;              /**< Node count */
    /* RWPUBLICEND
     * An array of PipelineNodes in Simon's 'One True Order'
     * RWPUBLIC     */
    RxPipelineNode            *nodes;                 /**< Topologically sorted array of pipeline nodes */
    /* RWPUBLICEND
     * Number of slots allocated for clusters, equals the maximum number
     * of clusters active simultaneously within this pipeline.
     * RWPUBLIC     */
    RwUInt32                   packetNumClusterSlots; /**< Number of slots allocated for clusters */
    rxEmbeddedPacketState      embeddedPacketState;   /**< The state of this pipeline's embedded packet */
    RxPacket                  *embeddedPacket;        /**< This pipeline's embedded packet */

    RwUInt32                   numInputRequirements;  /**< Input requirements count */
    RxPipelineRequiresCluster *inputRequirements;     /**< Input requirements array */

    /* RWPUBLICEND
     * PipelineNode.[SlotClusterIDs|SlotsContinue|Input2ClusterSlot]
     * are suballocated from this block
     * RWPUBLIC     */
    void                      *superBlock;            /**< Internally used block of memory */
    RwUInt32                   superBlockSize;        /**< Internally used block of memory */

    /* RWPUBLICEND
     * only required between Lock()/Unlock()
     * RWPUBLIC     */
    RwUInt32                   entryPoint;            /**< The index of the node which is the entry point of this pipeline */
    RwUInt32                   pluginId;              /**< If required, the Id of the plugin owning this pipeline, or 0 */
    RwUInt32                   pluginData;            /**< Rights callback extra data */
};
extern RxPipeline *
RxPipelineExecute(RxPipeline  * pipeline,
                  void        * data,
                  RwBool       heapReset);
typedef struct _RxObjSpace3DVertex RxObjSpace3DVertex;
struct _RxObjSpace3DVertex
{
        RwV3d          objVertex;              /* Object space vertex */
        union
        {
            RwRGBA     preLitColor;            /* Vertex pre-lighting values */
            RwRGBA     color;                  /* Immediate mode vertex color */
        }
        c;
        RwV3d          objNormal;              /* Object space normal */
        RwReal         u, v;                   /* Vertex tex coords */
};
typedef RxObjSpace3DVertex RxObjSpace3DLitVertex;
typedef RwIm2DVertex RxScrSpace2DVertex;
typedef RxObjSpace3DLitVertex RwIm3DVertex;
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
typedef struct RxCamSpace3DVertex RxCamSpace3DVertex;
struct RxCamSpace3DVertex
{
    /* Position in sheared camera-space (pre-projection) */
    RwV3d           cameraVertex; /**< \ref RwV3d camera-space position of the vertex */
    /* Clip flags on this vertex */
    RwUInt8         clipFlags;    /**< Clip flags for the vertex generated during transformation into camera-space, see \ref RwClipFlag */
    RwUInt8         pad[3];       /**< Alignment padding */
    /* Lit color */
    RwRGBAReal      col;          /**< Accumulated \ref RwReal light values (initialized to zero or prelight colors) */
    /* Only used by the clipper */
    RwReal          u;            /**< Texture U coordinate */
    RwReal          v;            /**< Texture V coordinate */
};
typedef struct RxMeshStateVector RxMeshStateVector;
struct RxMeshStateVector
{
    RwInt32             Flags;              /**< \ref RxGeometryFlag Flags from the source geometry */
    /* RWPUBLICEND
     * SourceObject is the object which owns the *current* pipeline - in
     * immediate mode it points to a rwIm3DPool struct (an internal structure) and in
     * atomic/worldsector object or material pipelines it points to
     * a RpMaterial (yes I know atomics/worldsectors own instancing
     * pipelines but their pointers can be got from the "(void *)data"
     * passed into every node - a material's pointer can't and we may
     * need to get RpMaterial plugin data)
     * RWPUBLIC */
    void               *SourceObject;       /**< A void pointer. In immediate mode it points to an
                                             * internal structure and in atomic/world-sector object
                                             * or material pipelines it points to an \ref RpMaterial. */
    RwMatrix            Obj2World;          /**< \ref RwMatrix to transform from object-space to world-space */
    RwMatrix            Obj2Cam;            /**< \ref RwMatrix to transform from object-space to camera-space */
    RwSurfaceProperties SurfaceProperties;  /**< \ref RwSurfaceProperties */
    /* We can't necessarily reference an RpMaterial in here (i.e with Im3D),
     * because RpMaterials are defined in RpWorld not RwCore */
    RwTexture          *Texture;            /**< A pointer to a \ref RwTexture */
    RwRGBA              MatCol;             /**< \ref RwRGBA material color */
    RxPipeline         *Pipeline;           /**< A pointer to the material pipeline where appropriate */
    /* rwPRIMTYPETRILIST/TRIFAN/TRISTRIP/LINELIST/POLYLINE */
    RwPrimitiveType     PrimType;           /**< \ref RwPrimitiveType primitive type */
    /* Interpretation based on PrimType */
    RwUInt32            NumElements;        /**< \ref RwUInt32 number of elements (triangles, lines...) */
    RwUInt32            NumVertices;        /**< \ref RwUInt32 number of vertices */
    RwInt32             ClipFlagsOr;        /**< Boolean OR of the \ref RwClipFlag clip flags of all vertices in the mesh */
    RwInt32             ClipFlagsAnd;       /**< Boolean AND of the \ref RwClipFlag clip flags of all vertices in the mesh */
    void               *SourceMesh;         /**< A void pointer to the source \ref RpMesh */
    void               *DataObject;         /**< Mirrors the void data pointer of \ref RxPipelineExecute */
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
enum RwIm3DTransformFlags
{
    rwIM3D_VERTEXUV      = 1,   /**<Texture coordinates in source vertices should be used */
    rwIM3D_ALLOPAQUE     = 2,   /**<All source vertices are opaque (alpha is 255) */
    rwIM3D_NOCLIP        = 4,   /**<No clipping should be performed on the geometry (the
                                 * app may know it is all onscreen or within the guard band clipping
                                 * region for the current hardware, so clipping can be skipped) */
    rwIM3D_VERTEXXYZ     = 8,   /**<Vertex positions */
    rwIM3D_VERTEXRGBA    = 16,  /**<Vertex color */

    rwIM3DTRANSFORMFLAGSFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwIm3DTransformFlags RwIm3DTransformFlags;
typedef struct rwIm3DPool            rwIm3DPool;
typedef struct rwImmediGlobals       rwImmediGlobals;
typedef struct rwIm3DRenderPipelines rwIm3DRenderPipelines;
typedef struct _rwIm3DPoolStash       _rwIm3DPoolStash;
struct _rwIm3DPoolStash
{
    RwUInt32                 flags;    /* rwIM3D_VERTEXUV, rwIM3D_ALLOPAQUE, rwIM3D_NOCLIP etc */
    RwMatrix                *ltm;      /* world-space frame of the vertices, if they have one */
    RwUInt32                 numVerts;
    RxObjSpace3DVertex      *objVerts;
    RxCamSpace3DVertex      *camVerts;
    RxScrSpace2DVertex      *devVerts;
    RxMeshStateVector       *meshState;
    RxRenderStateVector     *renderState;
    RxPipeline              *pipeline;
    RwPrimitiveType          primType;
    RxVertexIndex           *indices;
    RwUInt32                 numIndices;
};
struct rwIm3DPool
{
    RwUInt16         numElements; /* could become RwUInt32 nowadays */
    RwUInt16         pad;         /* alignment padding */
    void            *elements;    /* the original array of verts (or whatever...) - referenced not copied */
    RwInt32          stride;      /* the stride of the element */
    _rwIm3DPoolStash  stash;
};
struct rwIm3DRenderPipelines
{
    /* One Im3D render pipeline pointer per rwPRIMTYPE
     * (several pointers may point to the same pipeline though) */
    RxPipeline *triList;
    RxPipeline *triFan;
    RxPipeline *triStrip;
    RxPipeline *lineList;
    RxPipeline *polyLine;
    RxPipeline *pointList;
};
struct rwImmediGlobals
{
    RxPipeline           *im3DTransformPipeline;
    rwIm3DRenderPipelines im3DRenderPipelines;

    /* Platforms that have their own non-generic pipelines
     * (OPENGL, D3D8, SKY2, KAMUI2, DOLPHIN) put them here: */
    RxPipeline           *platformIm3DTransformPipeline;
    rwIm3DRenderPipelines platformIm3DRenderPipelines;


    rwIm3DPool   curPool;              /* The current cache of transformed vertices */
};
extern void  *RwIm3DTransform(RwIm3DVertex *pVerts, RwUInt32 numVerts,
                              RwMatrix *ltm, RwUInt32 flags);
#define RWIMMEDIGLOBAL(var)                             \
    (RWPLUGINOFFSET(rwImmediGlobals,                    \
                    RwEngineInstance,                   \
                    _rwIm3DModule.globalsOffset)->var)
extern RwModuleInfo _rwIm3DModule;

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                           Immediate mode
  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/**
 * \ingroup rwim3d
 * \ref RwIm3DTransform is used to transform the given vertices into the
 * current camera in preparation for rendering. Following transformation there
 * exists an array of camera space vertices, together with an array of screen
 * space (device-dependent) vertices for those which lie in the camera's
 * frustum.
 *
 * This function executes the immediate mode transform pipeline, which is
 * accessible through \ref RwIm3DGetTransformPipeline. This transforms
 * the vertices into world space and caches them ready for rendering by
 * subsequent calls to RwIm3D render functions. These vertices are cached
 * and can be used by any number of RwIm3D rendering functions until \ref RwIm3DEnd
 * is called, at which point the vertices are discarded.
 *
 * Note that because RwIm3D behaves (from the point of view of the API and
 * the application) exactly as it did with the old RenderWare Graphics pipeline,
 * there are some restrictions due to the details of the implementation using the
 * new flexible pipeline code. In-between a \ref RwIm3DTransform and \ref RwIm3DEnd
 * pair, no other pipeline execution, that is, anything which calls
 * \ref RxPipelineExecute -  can take place. This is because the caching
 * of the vertices requires the global heap - see \ref RxHeapGetGlobalHeap - to
 * remain uncleared. Executing other pipelines would clear the heap. There
 * will be a successor to RwIm3D which will work more cleanly with the new
 * PowerPipe functionality and which will not have these restrictions, but
 * that is not yet available.
 *
 * This function takes an optional matrix which is concatenated onto the
 * world-to-camera matrix before vertices are transformed. This is useful if
 * the vertices are in object space, in which case the argument should be the
 * local transformation matrix.
 *
 * \note The \ref RwIm3DTransformFlags flags \ref rwIM3D_VERTEXXYZ and
 *       \ref rwIM3D_VERTEXRGBA are added automatically by this function, thus
 *       requiring at least positional and color information in the \e pVerts data.
 *
 * \param pVerts  A pointer to the \ref RwIm3DVertex array to transform
 * \param numVerts  An \ref RwInt32 value equal to the number of vertices to transform
 * \param ltm  A pointer to the object-to-world matrix, or
 *       NULL if the vertices are provided in world space
 * \param flags  flags of type \ref RwIm3DTransformFlags.
 *
 * \return Returns the vertex array if successful, otherwise NULL.
 *
 * \see RwIm3DEnd
 * \see RwIm3DRenderIndexedPrimitive
 * \see RwIm3DRenderLine
 * \see RwIm3DRenderPrimitive
 * \see RwIm3DRenderTriangle
 * \see RwIm3DGetRenderPipeline
 * \see \ref RwIm3DGetRenderPipelineplatform
 * \see RwIm3DGetTransformPipeline
 * \see \ref RwIm3DGetTransformPipelineplatform
 * \see RwIm3DSetRenderPipeline
 * \see RwIm3DSetTransformPipeline
 *
 */
// Its data (PLAN M5): literals, constants and tables where the image has them.
void               *
RwIm3DTransform(RwIm3DVertex * pVerts,
                RwUInt32 numVerts, RwMatrix * ltm, RwUInt32 flags)
{
    /* Packages up the vertices/numvertices/flags/matrix  in a struct
     * and then calls RxPipelineExecute() with this struct (potentially
     * several times) at this point during subsequent calls to
     * RwIm3DRender[Indexed]Primitive().
     *
     * Note that the global heap will not be cleared during this time (this
     * function stores a list of packets containing the transformed data to
     * be cached) and thus no other pipelines may be executed until
     * RwIm3DEnd() has been called. Subsequent non-RwIm3DRender* invocations
     * of RxPipelineExecute() will clear the heap (it's also cleared on
     * shutdown). */
    RxPipeline         *result;

    RWAPIFUNCTION(RWSTRING("RwIm3DTransform"));


    /* It's OK to have zero vertices */

    RWASSERT(0 < _rwIm3DModule.numInstances);
    RWASSERT(NULL != pVerts);

    if (numVerts > 65536L)
    {
        RWERROR((E_RX_TOOMANYVERTS));
    }
    else
    {
        RWIMMEDIGLOBAL(curPool).numElements = (RwUInt16) numVerts;
        RWIMMEDIGLOBAL(curPool).elements = pVerts;
        /* Eventually stride may switch on the
         * basis of incoming object type */
        RWIMMEDIGLOBAL(curPool).stride = sizeof(RxObjSpace3DLitVertex);
        /* Flags and transformation matrix put in the stash since they
         * are accessed by the render pipeline (the latter with H/W T&L)
         * and that generally needs the flags (not so the source vertices) */
        RWIMMEDIGLOBAL(curPool).stash.ltm = ltm;
        RWIMMEDIGLOBAL(curPool).stash.flags =
            (flags | rwIM3D_VERTEXXYZ | rwIM3D_VERTEXRGBA);

        /* Execute the pipe - packets get stashed in RXPIPELINEGLOBALs */
        result =
            RxPipelineExecute(RWIMMEDIGLOBAL
                              (im3DTransformPipeline),
                              &RWIMMEDIGLOBAL(curPool), TRUE);

        if (result != NULL)
        {
            /*
             * NOTE: The app needs to do an RwIm3DEnd
             * when it's through doing 3D IM
             */
            RWRETURN(pVerts);
        }

    }

    /* Pipeline execution failed for some reason */
    RwDebugSendMessage(rwDEBUGMESSAGE, "RwIm3DTransform",
                       "Pipeline execution failed");


    RWRETURN(NULL);
}
