/* compile: /G5 */
/* _rwDeviceSystemRequest (MFL.exe 0x004d2e20): RenderWare Graphics 3.7 (Criterion Software), codfh name, P7b near miss
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rw/src/badevice.c, function _rwDeviceSystemRequest), a byte-identical decompilation of the RenderWare
 * build of an Xbox game; the Win32 branches it kept are RenderWare 3.7.2 source. Used with credit; split into one file per
 * function, the declarations it needs copied from the codfh headers named in the comments (PC build: no _XBOX, no RWDEBUG).
 * Changes: no renames; data definitions of the unit are extern declarations here.
 * P7 delta classes for MFL's PC build: RWALIGN is empty (no __declspec(align(16))), RwGlobals has no RW 3.6 fileFuncs table,
 * Rt2dGlobals has no codfh `unknown` field; P7b: RenderWare library version 3.7.0.2 (stream range 0x35000..0x37002), static same-unit
 * callees the code calls or takes the address of are extern (bound to the MFL function by name / alias), not copied.
 * Byte-identical to MFL.exe 0x004d2e20 (tools/cmpfn.py /O2 /G5, relocations masked). */
#include <stdlib.h>
#include <string.h>
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
#define RWFUNCTION(name)       /* No op */
#define RWASSERT(condition)    /* No op */
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
enum RwCoreDeviceSystemFn
{
    rwDEVICESYSTEMOPEN                  = 0x00,
    rwDEVICESYSTEMCLOSE,
    rwDEVICESYSTEMSTART,
    rwDEVICESYSTEMSTOP,
    rwDEVICESYSTEMREGISTER,
    rwDEVICESYSTEMGETNUMMODES,
    rwDEVICESYSTEMGETMODEINFO,
    rwDEVICESYSTEMUSEMODE,
    rwDEVICESYSTEMFOCUS,
    rwDEVICESYSTEMINITPIPELINE,
    rwDEVICESYSTEMGETMODE,
    rwDEVICESYSTEMSTANDARDS,
    rwDEVICESYSTEMGETTEXMEMSIZE,
    rwDEVICESYSTEMGETNUMSUBSYSTEMS,
    rwDEVICESYSTEMGETSUBSYSTEMINFO,
    rwDEVICESYSTEMGETCURRENTSUBSYSTEM,
    rwDEVICESYSTEMSETSUBSYSTEM,
    rwDEVICESYSTEMFINALIZESTART,
    rwDEVICESYSTEMINITIATESTOP,
    rwDEVICESYSTEMGETMAXTEXTURESIZE,
    rwDEVICESYSTEMRXPIPELINEREQUESTPIPE,
    rwDEVICESYSTEMGETMETRICBLOCK,
    rwDEVICESYSTEMGETID,
    rwDEVICESYSTEMDD                    = 0x1000,
    rwCOREDEVICESYSTEMFNFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCoreDeviceSystemFn RwCoreDeviceSystemFn;
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
#define SUBSYSTEMNAME_MAXLEN 80
typedef struct RwSubSystemInfo RwSubSystemInfo;
struct RwSubSystemInfo
{
    RwChar  name[SUBSYSTEMNAME_MAXLEN]; /**< Sub system string */
};
extern RwBool _rwDeviceSystemRequest(RwDevice *d, RwInt32 id, void *out,
                                 void *inOut, RwInt32 numIn);
#define rwstrcpy    RWSRCGLOBAL(stringFuncs).vecStrcpy
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
#define RWSRCGLOBAL(variable) \
   (((RwGlobals *)RwEngineInstance)->variable)
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

		/* codfh-match: the drop's "Jaewon 20050603" debugLogFunc field is not in
		 * the image's RwGlobals (curCamera is its first word, at 0x5200E0). */

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
extern void         *RwEngineInstance;
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
typedef RxObjSpace3DLitVertex RwIm3DVertex;
extern void  *RwIm3DTransform(RwIm3DVertex *pVerts, RwUInt32 numVerts,
                              RwMatrix *ltm, RwUInt32 flags);
extern void *RwEngineInstance;

/****************************************************************************
 _rwDeviceSystemRequest
 On entry   : device pointer, request ID, rqeuest specific parameters
 On exit    : device pointer on success
 */
RwBool
_rwDeviceSystemRequest(RwDevice * device, RwInt32 requestID,
                       void *pOut, void *pInOut, RwInt32 numIn)
{
    RwBool result;

    RWFUNCTION(RWSTRING("_rwDeviceSystemRequest"));
    RWASSERT(device);

    result = (device->fpSystem) (requestID, pOut, pInOut, numIn);

    if (!result)
    {
        /* Catch the cases that we can provide default implementations for */
        switch (requestID)
        {
            case rwDEVICESYSTEMFINALIZESTART:
            case rwDEVICESYSTEMINITIATESTOP:
                /* It ain't the end of the world if these
                 * don't exist in the driver, so catch the error.
                 */
                result = TRUE;
                break;

            case rwDEVICESYSTEMGETNUMSUBSYSTEMS:
                /* Default to 1 sub system */
                *(RwInt32 *) pOut = 1;
                result = TRUE;
                break;

            case rwDEVICESYSTEMGETSUBSYSTEMINFO:
                /* Fill info in as only sub system */
                result = (numIn == 0);
                if (result)
                {
                    RwSubSystemInfo    *subSystemInfo =
                        (RwSubSystemInfo *) pOut;

                    rwstrcpy(subSystemInfo->name,
                             RWSTRING("Only rendering sub system"));
                }
                break;

            case rwDEVICESYSTEMGETCURRENTSUBSYSTEM:
                /* Fill in default as number 0 */
                *(RwInt32 *) pOut = 0;
                result = TRUE;
                break;

            case rwDEVICESYSTEMSETSUBSYSTEM:
                /* If it's sub system 0, everything's cool */
                result = (numIn == 0);
                break;

        }
    }

    if (!result)
    {
        /* Failure */
        RWERROR((E_RW_SYSTEMREQUEST, requestID));
    }

    RWRETURN(result);
}
