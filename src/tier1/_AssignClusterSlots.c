/* compile: /G5 */
/* _AssignClusterSlots (MFL.exe 0x004f2c80): RenderWare Graphics 3.7 (Criterion Software), codfh name, P7b near miss
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rw/src/pipe/p2/p2dep.c, function _AssignClusterSlots), a byte-identical decompilation of the RenderWare
 * build of an Xbox game; the Win32 branches it kept are RenderWare 3.7.2 source. Used with credit; split into one file per
 * function, the declarations it needs copied from the codfh headers named in the comments (PC build: no _XBOX, no RWDEBUG).
 * Changes: no renames; data definitions of the unit are extern declarations here.
 * P7 delta classes for MFL's PC build: RWALIGN is empty (no __declspec(align(16))), RwGlobals has no RW 3.6 fileFuncs table,
 * Rt2dGlobals has no codfh `unknown` field; P7b: RenderWare library version 3.7.0.2 (stream range 0x35000..0x37002), static same-unit
 * callees the code calls or takes the address of are extern (bound to the MFL function by name / alias), not copied.
 * Byte-identical to MFL.exe 0x004f2c80 (tools/cmpfn.py /O2 /G5, relocations masked). */
#include <float.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <math.h>
typedef int  RwInt32;
typedef unsigned int RwUInt32;
typedef unsigned short RwUInt16;
typedef char RwChar;
typedef RwInt32 RwBool;
#define RWFORCEENUMSIZEINT ((RwInt32)((~((RwUInt32)0))>>1))
#define FALSE 0
#define TRUE !FALSE
#define _RWSTRING(x) x
#define RWSTRING(x) _RWSTRING(x)
#define RWRETURN(value) return(value)
#define RWFUNCTION(name)       /* No op */
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
#define E_RX_DEP_SUCCESS              0

/*****************************************************************************/
RwUInt32
_AssignClusterSlots(RxPipeline * pipeline, RwScopeTrace ** stheadref)
{

    RwUInt32            numslotsused = 0U;
    RwScopeTrace       *st = *stheadref;

    RWFUNCTION(RWSTRING("_AssignClusterSlots"));

    /* now iterate through scope traces, assigning cluster slots */

    while (st != NULL)         /* outer loop moves through master list */
    {
        RwScopeTrace       *st2;
        RwUInt32            unavailslots = 0U;
        RwUInt32            assignedslot;

        st2 = st;
        do
        {                      /* inner loop moves through continuations (created by merging) */
            RwReqEntry         *reqentry = st2->head;

            while (reqentry != NULL)
            {
                unavailslots |= reqentry->req->unavailslots; /* this bit-field ends up with a 1 for any slot
                                                              * used somewhere
                                                              */

                reqentry = reqentry->next;
            }

            st2 = st2->continuation;
        }
        while (st2 != NULL);

        assignedslot = 0;
        while (unavailslots & 1) /* once we exhaust all slots, will always pick slot 32! */
        {
            unavailslots >>= 1;
            ++assignedslot;
        }

        if (assignedslot >= numslotsused)
        {
            numslotsused = assignedslot + 1;
        }

        /* & again to update all req->unavailslots w/ chosen slot */

        st2 = st;
        do
        {                      /* inner loop moves through continuations (created by merging) */
            RwReqEntry         *reqentry = st2->head;

            while (reqentry != NULL)
            {
                reqentry->assignedslot = assignedslot;
                reqentry->req->unavailslots |= 1U << assignedslot;

                reqentry = reqentry->next;
            }

            st2 = st2->continuation;
        }
        while (st2 != NULL);

        st = st->next;
    }

    pipeline->packetNumClusterSlots = numslotsused;

    RWRETURN(E_RX_DEP_SUCCESS);
}
