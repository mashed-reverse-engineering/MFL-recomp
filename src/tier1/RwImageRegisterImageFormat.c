/* compile: /G5 */
/* RwImageRegisterImageFormat (MFL.exe 0x004de040): RenderWare Graphics 3.7 (Criterion Software), codfh name, P7b near miss
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rw/src/baimage.c, function RwImageRegisterImageFormat), a byte-identical decompilation of the RenderWare
 * build of an Xbox game; the Win32 branches it kept are RenderWare 3.7.2 source. Used with credit; split into one file per
 * function, the declarations it needs copied from the codfh headers named in the comments (PC build: no _XBOX, no RWDEBUG).
 * Changes: no renames; data definitions of the unit are extern declarations here.
 * P7 delta classes for MFL's PC build: RWALIGN is empty (no __declspec(align(16))), RwGlobals has no RW 3.6 fileFuncs table,
 * Rt2dGlobals has no codfh `unknown` field; P7b: RenderWare library version 3.7.0.2 (stream range 0x35000..0x37002), static same-unit
 * callees the code calls or takes the address of are extern (bound to the MFL function by name / alias), not copied.
 * Byte-identical to MFL.exe 0x004de040 (tools/cmpfn.py /O2 /G5, relocations masked). */
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
typedef int  RwInt32;
typedef unsigned int RwUInt32;
typedef unsigned short RwUInt16;
typedef unsigned char RwUInt8;
typedef char RwChar;
typedef float RwReal;
typedef RwInt32 RwBool;
#define RWFORCEENUMSIZEINT ((RwInt32)((~((RwUInt32)0))>>1))
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
enum RwCriterionCoreID
{
    /* Guard value that should not be used. */
    rwID_NACOREID               = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x00),

    /* The valid plugin IDs */
    /** Vector module pluginID. See \ref rwv2d and \ref rwv3d */
    rwID_VECTORMODULE           = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x01),
    /** Matrix module pluginID. See \ref rwmatrix */
    rwID_MATRIXMODULE           = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x02),
    /** Frame module pluginID. See \ref rwframe */
    rwID_FRAMEMODULE            = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x03),
    /** Stream module pluginID. See \ref rwstream */
    rwID_STREAMMODULE           = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x04),
    /** Camera module pluginID. See \ref rwcamera */
    rwID_CAMERAMODULE           = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x05),
    /** Image module pluginID. See \ref rwimage */
    rwID_IMAGEMODULE            = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x06),
    /** Raster module pluginID. See \ref rwraster */
    rwID_RASTERMODULE           = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x07),
    /** Texture module pluginID. See \ref rwtexture */
    rwID_TEXTUREMODULE          = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x08),
    /** Pipeline module pluginID. See \ref RxPipeline */
    rwID_PIPEMODULE             = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x09),
    /** Immediate module pluginID. See \ref rwim3d */
    rwID_IMMEDIATEMODULE        = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0A),
    /** Resource module pluginID. See \ref rwresources */
    rwID_RESOURCESMODULE        = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0B),
    /** Device module pluginID */
    rwID_DEVICEMODULE           = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0C),
    /** Color module pluginID. See \ref rwrgba */
    rwID_COLORMODULE            = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0D),
    /* Not used */
    rwID_POLYPIPEMODULE         = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0E),
    /** Error module pluginID. See \ref rwerror */
    rwID_ERRORMODULE            = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0F),
    /** Metrics module pluginID. See \ref RwMetrics */
    rwID_METRICSMODULE          = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x10),
    /** Driver module pluginID */
    rwID_DRIVERMODULE           = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x11),
    /** Chunk group module pluginID. See \ref rwchunkgroup */
    rwID_CHUNKGROUPMODULE       = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x12),
    rwCRITERIONCOREIDFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCriterionCoreID RwCriterionCoreID;
#define RWRETURN(value) return(value)
#define RWAPIFUNCTION(name)    /* No op */
#define RWASSERT(condition)    /* No op */
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
#define rwstrncpy   RWSRCGLOBAL(stringFuncs).vecStrncpy
#define rwstrcmp    RWSRCGLOBAL(stringFuncs).vecStrcmp
#define rwstrupr    RWSRCGLOBAL(stringFuncs).vecStrupr
#define rwstrlwr    RWSRCGLOBAL(stringFuncs).vecStrlwr
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
#define RWPLUGINOFFSET(_type, _base, _offset)                   \
   ((_type *)((RwUInt8 *)(_base) + (_offset)))
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
typedef struct RwModuleInfo RwModuleInfo;
struct RwModuleInfo
{
        RwInt32     globalsOffset;
        RwInt32     numInstances;
};
extern void         *RwEngineInstance;
typedef struct RwImage RwImage;
struct RwImage
{
        RwInt32             flags;

        RwInt32             width;  /* Device may have different ideas */
        RwInt32             height; /* internally !! */

        RwInt32             depth;  /* Of referenced image */
        RwInt32             stride;

        RwUInt8            *cpPixels;
        RwRGBA             *palette;
};
typedef RwImage *(*RwImageCallBackRead)(const RwChar * imageName);
typedef RwImage *(*RwImageCallBackWrite)(RwImage *image, const RwChar *imageName);
typedef struct rwImageFormat rwImageFormat;
struct rwImageFormat
{
        RwChar              lcExtension[20]; /* Lower case version */
        RwChar              ucExtension[20]; /* Upper case version */
        RwImageCallBackRead readImage;
        RwImageCallBackWrite writeImage;
        rwImageFormat      *nextFormat;
};
typedef struct rwImageGlobals rwImageGlobals;
struct rwImageGlobals
{
        RwFreeList         *imageFreeList;
        RwChar             *imagePath;
        RwInt32             imagePathSize;

        RwUInt8             gammaTable[256];
        RwUInt8             invGammaTable[256];
        RwReal              gammaVal;

        RwUInt8            *scratchMem;
        RwInt32             scratchMemSize;

        RwFreeList         *imageFormatFreeList;
        rwImageFormat      *imageFormats;
};
extern RwBool       RwImageRegisterImageFormat(const RwChar * extension,
                                               RwImageCallBackRead
                                               imageRead,
                                               RwImageCallBackWrite
                                               imageWrite);
#define RWIMAGEGLOBAL(var)                              \
   (RWPLUGINOFFSET(rwImageGlobals,                      \
                   RwEngineInstance,                    \
                   imageModule.globalsOffset)->var)
extern RwModuleInfo imageModule;

/**
 * \ingroup rwimage
 * \ref RwImageRegisterImageFormat is used to register a new image format
 * with RenderWare.  This will allow loading and saving of the supplied image
 * format if suitable callbacks are supplied.
 *
 * RenderWare Graphics is supplied with a number of Toolkits containing such callbacks.
 * These Toolkits can be used to add support for the following image formats:
 *
 * - Sun Microsystem's Raster, supported by the RtRAS Toolkit;
 * - Portable Network Graphics, supported by the RtPNG Toolkit;
 * - Microsoft Windows Bitmap, supported by the RtBMP Toolkit;
 * - Tagged Image File Format, supported by the RtTIFF Toolkit;
 *
 * See the \e Rasters, \e Images \e & \e Textures chapter in the User Guide for more in-depth coverage of
 * this API.
 *
 * These Toolkits replace the functionality found in previous releases. As a result, the
 * following functions are now considered obsolete and will be deleted in a later
 * release:
 *
 * - RwImageReadBMP()
 * - RwImageReadRAS()
 * - RwImageWriteBMP()
 * - RwImageWriteRAS()
 *
 *
 * You can write your own image file format parsers by providing your own callback functions.
 * The format of the read and write callbacks are:
 *
 * \verbatim
   RwImage * (*RwImageCallBackRead)(const RwChar *imageName);
   RwImage *(*RwImageCallBackWrite)(RwImage *image, const RwChar *imageName);
  \endverbatim
 *
 * An example of writing your own file format parser is provided in the
 * ".../examples/imgformt/" folder.
 *
 * Note that as a default there are no image formats registered with RenderWare.
 * Also, image formats must be registered for reading textures and rasters from
 * image files as these rely on the RwImage functionality.
 *
 * This function must be used after the RenderWare engine has been started.
 *
 * Sample Usage:
 *
 * To enable reading and writing of Sun raster image files together with reading
 * Windows Bitmap files, we must first make sure we are linking against the
 * appropriate libraries, "rtras.lib" and "rtbmp.lib". Next, we include the
 * header files, "rtras.h" and "rtbmp.h".
 *
 * At this point, we can add the following lines to our code after the call to
 * RwEngineStart():
 * \verbatim
   RwImageRegisterImageFormat("ras", RtRASImageRead, RtRASImageWrite);
   RwImageRegisterImageFormat("bmp", RtBMPImageRead,NULL);
   \endverbatim
 *
 * \param extension  Extension type for the image (e.g. bmp, ras)
 * \param imageRead  Pointer to a callback to allow reading of this image type (NULL for
 * no reading possible)
 * \param imageWrite  Pointer to a callback to allow writing of this image type (NULL for
 * no writing possible).
 *
 * \return Returns TRUE on success, or FALSE on failure.
 *
 * \see RwImageRead
 * \see RwImageReadMaskedImage
 * \see RwImageWrite
 * \see RwImageSetPath
 *
 * Related Toolkits: RtPNG, RtRAS, RtBMP, RtTIFF
 */
RwBool
RwImageRegisterImageFormat(const RwChar * extension,
                           RwImageCallBackRead imageRead,
                           RwImageCallBackWrite imageWrite)
{
    rwImageFormat      *newFormat, *imageFormat;
    RwChar              lcExtension[20]; /* Lower case version */

    RWAPIFUNCTION(RWSTRING("RwImageRegisterImageFormat"));
    RWASSERT(extension);
    RWASSERT(imageModule.numInstances);

    /* Need to make sure it's not already there (compare the LC version) */
    lcExtension[0] = '.';
    rwstrncpy(&lcExtension[1], extension, 18);
    lcExtension[19] = 0;
    rwstrlwr(lcExtension);

    /* If we find it already there, override the methods */
    imageFormat = RWIMAGEGLOBAL(imageFormats);
    newFormat = (rwImageFormat *)NULL;
    while (imageFormat && !newFormat)
    {
        if (!rwstrcmp(imageFormat->lcExtension, lcExtension))
        {
            /* Got it, so prepare to replace operations */
            newFormat = imageFormat;
        }
        else
        {
            /* Look at the next one */
            imageFormat = imageFormat->nextFormat;
        }
    }

    /* If we didn't find one, create one and link it in */
    if (!newFormat)
    {
        /* Time to allocate the new element */
        newFormat = (rwImageFormat *)
            RwFreeListAlloc(RWIMAGEGLOBAL(imageFormatFreeList),
                            rwMEMHINTDUR_EVENT | rwID_IMAGEMODULE);

        if (!newFormat)
        {
            /* Allocation failure */
            RWRETURN(FALSE);
        }

        /* Put in the extension */
        /* We already got the lower case version */
        memcpy(newFormat->lcExtension, lcExtension, 20);

        /* Figure out the upper case version */
        newFormat->ucExtension[0] = '.';
        rwstrncpy(&newFormat->ucExtension[1], extension, 18);
        newFormat->ucExtension[19] = 0;
        rwstrupr(newFormat->ucExtension);

        /* And some default I/O functions (none) */
        newFormat->readImage = (RwImageCallBackRead) NULL;
        newFormat->writeImage = (RwImageCallBackWrite) NULL;

        /* And link it in */
        newFormat->nextFormat = RWIMAGEGLOBAL(imageFormats);
        RWIMAGEGLOBAL(imageFormats) = newFormat;
    }

    /* And the different functions for reading/writing (if specified) */
    if (imageRead)
    {
        newFormat->readImage = imageRead;
    }
    if (imageWrite)
    {
        newFormat->writeImage = imageWrite;
    }

    RWRETURN(TRUE);
}
