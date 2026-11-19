/* rw37_slim.h: the RenderWare Graphics 3.7 types, macros and prototypes the codfh tier 2 rows of this directory use, and nothing else.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, include/rw/sdk @e578672 (rwplcore.h, rwcore.h, rpworld.h and the
 * plugin / toolkit headers), changes: comments removed, conditional compilation resolved for the release D3D9 build of MFL.exe (cl 13.00, no
 * RWDEBUG), reduced to the declarations reachable from the sources that include this file, the D3D9 driver types of MFL.exe in place of the
 * null driver's, no 16 byte alignment, no RwGlobals.fileFuncs, the C runtime / Win32 names declared here instead of system headers, and a
 * compile-time layout assert (RW_LAYOUT_ASSERT, end of file) for every struct with the offsets MFL.exe uses. Generated, then hand-checked.
 * A source includes this file INSTEAD of mfl_tier2.h, never both. */
#ifndef RW37_SLIM_H
#define RW37_SLIM_H
#ifdef __LP64__                 /* mfl_uptr: an unsigned integer as wide as a pointer, for integer <-> pointer casts (as in mfl_tier2.h) */
typedef unsigned long mfl_uptr; /* 64 bit targets (the portability check compiles the sources as LP64) */
#else
typedef unsigned int mfl_uptr;  /* cl 13 / x86: 32 bit, the same code as a cast through unsigned int */
#endif
typedef unsigned int size_t;
typedef char *va_list;
#define NULL ((void *)0)
#define offsetof(s,m) ((size_t)RW_OFFSETOF(s, m))
void *__cdecl memcpy(void *, const void *, size_t);
typedef void code();
#define RW_OFFSETOF(type,member) ((char *)&((type *)0)->member - (char *)0)
#define RW_LAYOUT_ASSERT(name,cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]
typedef int RwInt32;
typedef unsigned int RwUInt32;
typedef short RwInt16;
typedef unsigned short RwUInt16;
typedef unsigned char RwUInt8;
typedef char RwChar;
typedef float RwReal;
typedef RwInt32 RwBool;
#define RWALIGN(type,x) type
#define rwMATRIXALIGNMENT sizeof(RwUInt32)
#define rwFRAMEALIGNMENT sizeof(RwUInt32)
#define RWFORCEENUMSIZEINT ((RwInt32)((~((RwUInt32)0))>>1))
#define __RWUNUSED__ 
#define MACRO_START do
#define MACRO_STOP while(0)
#define FALSE 0
#define TRUE !FALSE
typedef struct RwV2d RwV2d;
struct RwV2d
{
    RwReal x;
    RwReal y;
};
typedef struct RwV3d RwV3d;
struct RwV3d
{
    RwReal x;
    RwReal y;
    RwReal z;
};
typedef struct RwSphere RwSphere;
struct RwSphere
{
    RwV3d center;
    RwReal radius;
};
typedef struct RwLine RwLine;
struct RwLine
{
    RwV3d start;
    RwV3d end;
};
#define rwMAXTEXTURECOORDS 8
typedef struct RwTexCoords RwTexCoords;
struct RwTexCoords
{
    RwReal u;
    RwReal v;
};
typedef struct RwLLLink RwLLLink;
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
typedef struct RwPlane RwPlane;
struct RwPlane
{
    RwV3d normal;
    RwReal distance;
};
#define MAKECHUNKID(vendorID,chunkID) (((vendorID & 0xFFFFFF) << 8) | (chunkID & 0xFF))
enum RwPluginVendor
{
    rwVENDORID_CORE = 0x000000L,
    rwVENDORID_CRITERIONTK = 0x000001L,
    rwVENDORID_REDLINERACER = 0x000002L,
    rwVENDORID_CSLRD = 0x000003L,
    rwVENDORID_CRITERIONINT = 0x000004L,
    rwVENDORID_CRITERIONWORLD = 0x000005L,
    rwVENDORID_BETA = 0x000006L,
    rwVENDORID_CRITERIONRM = 0x000007L,
    rwVENDORID_CRITERIONRWA = 0x000008L,
    rwVENDORID_CRITERIONRWP = 0x000009L,
    rwPLUGINVENDORFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwPluginVendor RwPluginVendor;
enum RwCriterionPluginID
{
    rwID_COREPLUGIN = 0x01,
    rwID_WORLDPLUGIN = 0x02,
    rwID_TOOLPLUGIN = 0x03,
    rwID_TOOL2PLUGIN = 0x04,
    rwCRITERIONPLUGINIDFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCriterionPluginID RwCriterionPluginID;
typedef struct RwObject RwObject;
struct RwObject
{
        RwUInt8 type;
        RwUInt8 subType;
        RwUInt8 flags;
        RwUInt8 privateFlags;
        void *parent;

};
#define rwObjectGetParent(object) (((const RwObject *)(object))->parent)
#define RwMalloc(_s,_h) ((RWSRCGLOBAL(memoryFuncs).rwmalloc)((_s), (_h)))
#define RwFree(_p) ((RWSRCGLOBAL(memoryFuncs).rwfree)((_p)))
enum RwMemoryHintDuration
{
    rwMEMHINTDUR_NADURATION = 0x00000000,

    rwMEMHINTDUR_FUNCTION = 0x00010000,

    rwMEMHINTDUR_FRAME = 0x00020000,

    rwMEMHINTDUR_EVENT = 0x00030000,

    rwMEMHINTDUR_GLOBAL = 0x00040000,
    rwMEMHINTDUR_MASK = 0x00FF0000,
    rwMEMHINTDURFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwMemoryHintDuration RwMemoryHintDuration;
struct RwMemoryFunctions
{

    void *(*rwmalloc)(size_t size, RwUInt32 hint);

    void (*rwfree)(void *mem);

    void *(*rwrealloc)(void *mem, size_t newSize, RwUInt32 hint);

    void *(*rwcalloc)(size_t numObj, size_t sizeObj, RwUInt32 hint);

};
typedef struct RwMemoryFunctions RwMemoryFunctions;
typedef struct RwFreeList RwFreeList;
struct RwFreeList
{
    RwUInt32 entrySize;
    RwUInt32 entriesPerBlock;
    RwUInt32 heapSize;
    RwUInt32 alignment;
    RwLinkList blockList;
    RwUInt32 flags;

    RwLLLink link;

};
typedef void *(*RwMemoryAllocFn) (RwFreeList * fl, RwUInt32 hint);
typedef RwFreeList *(*RwMemoryFreeFn) (RwFreeList * fl, void *pData);
extern RwFreeList*
RwFreeListCreateAndPreallocateSpace(RwInt32 entrySize,
                                    RwInt32 entriesPerBlock,
                                    RwInt32 alignment,
                                    RwInt32 numBlocksToPreallocate,
                                    RwFreeList *inPlaceSpaceForFreeListStruct,
                                    RwUInt32 hint);
extern RwBool RwFreeListDestroy(RwFreeList * freelist);
#define RwFreeListAlloc(_f,_h) RWSRCGLOBAL(memoryAlloc)((_f), (_h))
#define RwFreeListFree(_f,_p) RWSRCGLOBAL(memoryFree)(_f, _p)
typedef struct RwStream RwStream;
typedef int (*vecSprintfFunc)(RwChar *buffer,
                              const RwChar *format,
                              ...) ;
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
                             ...) ;
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
struct RwMatrixTag
{

    RwV3d right;
    RwUInt32 flags;
    RwV3d up;
    RwUInt32 pad1;
    RwV3d at;
    RwUInt32 pad2;
    RwV3d pos;
    RwUInt32 pad3;
};
typedef struct RwMatrixTag RWALIGN(RwMatrix, rwMATRIXALIGNMENT);
typedef struct RwError RwError;
struct RwError
{
    RwInt32 pluginID;
    RwInt32 errorCode;
};
enum RwErrorCodeCommon
{
    E_RW_NOERROR = (int)0x80000000L,
    E_RW_BADENGINESTATE,
    E_RW_BADOPEN,
    E_RW_BADPARAM,
    E_RW_BADVERSION,
    E_RW_DEBUGSTACK,
    E_RW_DEFAULTPIPELINECREATION,
    E_RW_FRAMENOMATRIX,
    E_RW_INVIMAGEDEPTH,
    E_RW_INVIMAGEFORMAT,
    E_RW_INVIMAGEMASK,
    E_RW_INVIMAGESIZE,
    E_RW_INVRASTERDEPTH,
    E_RW_INVRASTERFORMAT,
    E_RW_INVRASTERLOCKREQ,
    E_RW_INVRASTERMIPLEVEL,
    E_RW_INVRASTERSIZE,
    E_RW_INVRASTERUNLOCKREQ,
    E_RW_NOFRAME,
    E_RW_NOMEM,
    E_RW_NOMIPMAPGENERATIONCALLBACK,
    E_RW_NOTSSEENABLEDCPU,
    E_RW_NULLP,
    E_RW_PLUGININIT,
    E_RW_PLUGINNOTINIT,
    E_RW_RANGE,
    E_RW_READ,
    E_RW_REDUNDANT_FUNCTION,
    E_RW_WRITE,
    E_RX_MESHES_RANGES_OVERLAP,
    E_RW_STRING_TRUNCATION,

    E_RW_LASTERROR = RWFORCEENUMSIZEINT
};
typedef enum RwErrorCodeCommon RwErrorCodeCommon;
extern RwError *RwErrorSet(RwError *code);
extern RwInt32 _rwerror(RwInt32 code, ...);
typedef struct RwRGBA RwRGBA;
struct RwRGBA
{
    RwUInt8 red;
    RwUInt8 green;
    RwUInt8 blue;
    RwUInt8 alpha;
};
typedef struct RwD3D9Vertex RwD3D9Vertex;
struct RwD3D9Vertex
{
    RwReal x;
    RwReal y;
    RwReal z;
    RwReal rhw;
    RwUInt32 emissiveColor;
    RwReal u;
    RwReal v;
};
typedef RwD3D9Vertex RwIm2DVertex;
typedef RwUInt16 RxVertexIndex;
typedef RxVertexIndex RwImVertexIndex;
#define RwV3dAddMacro(o,a,b) MACRO_START { (o)->x = (((a)->x) + ( (b)->x)); (o)->y = (((a)->y) + ( (b)->y)); (o)->z = (((a)->z) + ( (b)->z)); } MACRO_STOP
#define RwV3dSubMacro(o,a,b) MACRO_START { (o)->x = (((a)->x) - ( (b)->x)); (o)->y = (((a)->y) - ( (b)->y)); (o)->z = (((a)->z) - ( (b)->z)); } MACRO_STOP
#define RwV3dScaleMacro(o,a,s) MACRO_START { (o)->x = (((a)->x) * ( (s))); (o)->y = (((a)->y) * ( (s))); (o)->z = (((a)->z) * ( (s))); } MACRO_STOP
#define RwV3dDotProductMacro(a,b) ((((( (((a)->x) * ((b)->x))) + ( (((a)->y) * ((b)->y))))) + ( (((a)->z) * ((b)->z)))))
#define RwV3dAdd(o,a,b) RwV3dAddMacro(o, a, b)
#define RwV3dSub(o,a,b) RwV3dSubMacro(o, a, b)
#define RwV3dScale(o,a,s) RwV3dScaleMacro(o, a, s)
#define RwV3dDotProduct(a,b) RwV3dDotProductMacro(a, b)
enum RwRenderState
{
    rwRENDERSTATENARENDERSTATE = 0,

    rwRENDERSTATETEXTURERASTER,

    rwRENDERSTATETEXTUREADDRESS,

    rwRENDERSTATETEXTUREADDRESSU,

    rwRENDERSTATETEXTUREADDRESSV,

    rwRENDERSTATETEXTUREPERSPECTIVE,

    rwRENDERSTATEZTESTENABLE,

    rwRENDERSTATESHADEMODE,

    rwRENDERSTATEZWRITEENABLE,

    rwRENDERSTATETEXTUREFILTER,

    rwRENDERSTATESRCBLEND,

    rwRENDERSTATEDESTBLEND,

    rwRENDERSTATEVERTEXALPHAENABLE,

    rwRENDERSTATEBORDERCOLOR,

    rwRENDERSTATEFOGENABLE,

    rwRENDERSTATEFOGCOLOR,

    rwRENDERSTATEFOGTYPE,

    rwRENDERSTATEFOGDENSITY,

    rwRENDERSTATECULLMODE = 20,

    rwRENDERSTATESTENCILENABLE,

    rwRENDERSTATESTENCILFAIL,

    rwRENDERSTATESTENCILZFAIL,

    rwRENDERSTATESTENCILPASS,

    rwRENDERSTATESTENCILFUNCTION,

    rwRENDERSTATESTENCILFUNCTIONREF,

    rwRENDERSTATESTENCILFUNCTIONMASK,

    rwRENDERSTATESTENCILFUNCTIONWRITEMASK,

    rwRENDERSTATEALPHATESTFUNCTION,

    rwRENDERSTATEALPHATESTFUNCTIONREF,

    rwRENDERSTATEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwRenderState RwRenderState;
enum RwShadeMode
{
    rwSHADEMODENASHADEMODE = 0,
    rwSHADEMODEFLAT,
    rwSHADEMODEGOURAUD,
    rwSHADEMODEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwShadeMode RwShadeMode;
enum RwTextureFilterMode
{
    rwFILTERNAFILTERMODE = 0,
    rwFILTERNEAREST,
    rwFILTERLINEAR,
    rwFILTERMIPNEAREST,
    rwFILTERMIPLINEAR,
    rwFILTERLINEARMIPNEAREST,
    rwFILTERLINEARMIPLINEAR,
    rwTEXTUREFILTERMODEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwTextureFilterMode RwTextureFilterMode;
enum RwFogType
{
    rwFOGTYPENAFOGTYPE = 0,
    rwFOGTYPELINEAR,
    rwFOGTYPEEXPONENTIAL,
    rwFOGTYPEEXPONENTIAL2,
    rwFOGTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwFogType RwFogType;
enum RwBlendFunction
{
    rwBLENDNABLEND = 0,
    rwBLENDZERO,
    rwBLENDONE,
    rwBLENDSRCCOLOR,
    rwBLENDINVSRCCOLOR,
    rwBLENDSRCALPHA,
    rwBLENDINVSRCALPHA,
    rwBLENDDESTALPHA,
    rwBLENDINVDESTALPHA,
    rwBLENDDESTCOLOR,
    rwBLENDINVDESTCOLOR,
    rwBLENDSRCALPHASAT,
    rwBLENDFUNCTIONFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwBlendFunction RwBlendFunction;
enum RwTextureAddressMode
{
    rwTEXTUREADDRESSNATEXTUREADDRESS = 0,
    rwTEXTUREADDRESSWRAP,
    rwTEXTUREADDRESSMIRROR,
    rwTEXTUREADDRESSCLAMP,
    rwTEXTUREADDRESSBORDER,
    rwTEXTUREADDRESSMODEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwTextureAddressMode RwTextureAddressMode;
enum RwPrimitiveType
{
    rwPRIMTYPENAPRIMTYPE = 0,
    rwPRIMTYPELINELIST = 1,

    rwPRIMTYPEPOLYLINE = 2,

    rwPRIMTYPETRILIST = 3,

    rwPRIMTYPETRISTRIP = 4,

    rwPRIMTYPETRIFAN = 5 ,

    rwPRIMTYPEPOINTLIST = 6,

    rwPRIMITIVETYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwPrimitiveType RwPrimitiveType;
#define rwSTANDARDNUMOFSTANDARD 29
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
    RwReal gammaCorrection;
    RwSystemFunc fpSystem;
    RwReal zBufferNear;
    RwReal zBufferFar;

    RwRenderStateSetFunction fpRenderStateSet;
    RwRenderStateGetFunction fpRenderStateGet;

    RwIm2DRenderLineFunction fpIm2DRenderLine;
    RwIm2DRenderTriangleFunction fpIm2DRenderTriangle;
    RwIm2DRenderPrimitiveFunction fpIm2DRenderPrimitive;
    RwIm2DRenderIndexedPrimitiveFunction fpIm2DRenderIndexedPrimitive;

    RwIm3DRenderLineFunction fpIm3DRenderLine;
    RwIm3DRenderTriangleFunction fpIm3DRenderTriangle;
    RwIm3DRenderPrimitiveFunction fpIm3DRenderPrimitive;
    RwIm3DRenderIndexedPrimitiveFunction fpIm3DRenderIndexedPrimitive;
};
typedef struct RwMetrics RwMetrics;
#define RWPLUGINOFFSET(_type,_base,_offset) ((_type *)((RwUInt8 *)(_base) + (_offset)))
#define RWSRCGLOBAL(variable) (((RwGlobals *)RwEngineInstance)->variable)
enum RwEngineStatus
{
    rwENGINESTATUSIDLE = 0,
    rwENGINESTATUSINITED = 1,
    rwENGINESTATUSOPENED = 2,
    rwENGINESTATUSSTARTED = 3,
    rwENGINESTATUSFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwEngineStatus RwEngineStatus;
typedef struct RwGlobals RwGlobals;
struct RwGlobals
{

        void *curCamera;
        void *curWorld;

        RwUInt16 renderFrame;
        RwUInt16 lightFrame;
        RwUInt16 pad[2];

        RwDevice dOpenDevice;

        RwStandardFunc stdFunc[rwSTANDARDNUMOFSTANDARD];

        RwLinkList dirtyFrameList;

        /* no fileFuncs in MFL.exe, see prelude_patches.py P3 */

        RwStringFunctions stringFuncs;

        RwMemoryFunctions memoryFuncs;

        RwMemoryAllocFn memoryAlloc;
        RwMemoryFreeFn memoryFree;

        RwMetrics *metrics;

        RwEngineStatus engineStatus;

        RwUInt32 resArenaInitSize;

};
typedef struct RwModuleInfo RwModuleInfo;
struct RwModuleInfo
{
        RwInt32 globalsOffset;
        RwInt32 numInstances;
};
extern void *RwEngineInstance;
typedef struct RwResEntry RwResEntry;
typedef struct rxHeapFreeBlock rxHeapFreeBlock;
typedef struct rxHeapSuperBlockDescriptor rxHeapSuperBlockDescriptor;
typedef struct RxHeap RxHeap;
typedef struct rxHeapBlockHeader rxHeapBlockHeader;
struct rxHeapFreeBlock
{
    RwUInt32 size;
    rxHeapBlockHeader *ptr;
};
struct rxHeapSuperBlockDescriptor
{
    void *start;
    RwUInt32 size;
    rxHeapSuperBlockDescriptor *next;
};
struct rxHeapBlockHeader
{

    rxHeapBlockHeader *prev, *next;
    RwUInt32 size;
    rxHeapFreeBlock *freeEntry;
    RwUInt32 pad[4];
};
enum RxClusterValidityReq
{
    rxCLREQ_DONTWANT = 0,

    rxCLREQ_REQUIRED = 1,

    rxCLREQ_OPTIONAL = 2,

    rxCLUSTERVALIDITYREQFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef struct RxClusterDefinition RxClusterDefinition;
typedef enum RxClusterValidityReq RxClusterValidityReq;
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
    RwChar *name;
    RwUInt32 defaultStride;
    RwUInt32 defaultAttributes;
    const RwChar *attributeSet;
};
struct RxIoSpec
{
    RwUInt32 numClustersOfInterest;
    RxClusterRef *clustersOfInterest;
    RxClusterValidityReq *inputRequirements;
    RwUInt32 numOutputs;
    RxOutputSpec *outputs;
};
typedef RwBool (*RxNodeBodyFn) (RxPipelineNode * self,
                                const RxPipelineNodeParam *params);
typedef RwBool (*RxNodeInitFn) (RxNodeDefinition * self);
typedef void (*RxNodeTermFn) (RxNodeDefinition * self);
typedef RwBool (*RxPipelineNodeInitFn) (RxPipelineNode * self);
typedef void (*RxPipelineNodeTermFn) (RxPipelineNode * self);
typedef RwBool (*RxPipelineNodeConfigFn) (RxPipelineNode * self,
                                          RxPipeline * pipeline);
typedef RwUInt32 (*RxConfigMsgHandlerFn) (RxPipelineNode * self,
                                          RwUInt32 msg,
                                          RwUInt32 intparam,
                                          void *ptrparam);
struct RxNodeMethods
{
    RxNodeBodyFn nodeBody;
    RxNodeInitFn nodeInit;
    RxNodeTermFn nodeTerm;
    RxPipelineNodeInitFn pipelineNodeInit;
    RxPipelineNodeTermFn pipelineNodeTerm;
    RxPipelineNodeConfigFn pipelineNodeConfig;
    RxConfigMsgHandlerFn configMsgHandler;
};
enum RxNodeDefEditable
{

    rxNODEDEFCONST = FALSE,

    rxNODEDEFEDITABLE = TRUE,
    rxNODEDEFEDITABLEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RxNodeDefEditable RxNodeDefEditable;
struct RxNodeDefinition
{
    RwChar *name;
    RxNodeMethods nodeMethods;
    RxIoSpec io;
    RwUInt32 pipelineNodePrivateDataSize;
    RxNodeDefEditable editable;
    RwInt32 InputPipesCnt;
};
struct RxCluster
{
    RwUInt16 flags;
    RwUInt16 stride;
    void *data;
    void *currentData;
    RwUInt32 numAlloced;
    RwUInt32 numUsed;
    RxPipelineCluster *clusterRef;

    RwUInt32 attributes;
};
struct RxPacket
{
    RwUInt16 flags;
    RwUInt16 numClusters;
    RxPipeline *pipeline;
    RwUInt32 *inputToClusterSlot;
    RwUInt32 *slotsContinue;
    RxPipelineCluster **slotClusterRefs;
    RxCluster clusters[1];
};
struct RxPipelineNode
{
    RxNodeDefinition *nodeDef;
    RwUInt32 numOutputs;
    RwUInt32 *outputs;
    RxPipelineCluster **slotClusterRefs;

    RwUInt32 *slotsContinue;
    void *privateData;
    RwUInt32 *inputToClusterSlot;
    RxPipelineNodeTopSortData *topSortData;
    void *initializationData;

    RwUInt32 initializationDataSize;

};
enum rxEmbeddedPacketState
{
    rxPKST_PACKETLESS = 0,
    rxPKST_UNUSED = 1,
    rxPKST_INUSE = 2,
    rxPKST_PENDING = 3,
    rxEMBEDDEDPACKETSTATEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum rxEmbeddedPacketState rxEmbeddedPacketState;
struct RxPipeline
{
    RwBool locked;
    RwUInt32 numNodes;
    RxPipelineNode *nodes;
    RwUInt32 packetNumClusterSlots;
    rxEmbeddedPacketState embeddedPacketState;
    RxPacket *embeddedPacket;

    RwUInt32 numInputRequirements;
    RxPipelineRequiresCluster *inputRequirements;

    void *superBlock;
    RwUInt32 superBlockSize;

    RwUInt32 entryPoint;
    RwUInt32 pluginId;
    RwUInt32 pluginData;
};
typedef struct RwRaster RwRaster;
typedef struct RxRenderStateVector RxRenderStateVector;
struct RxRenderStateVector
{
    RwUInt32 Flags;
    RwShadeMode ShadeMode;
    RwBlendFunction SrcBlend;
    RwBlendFunction DestBlend;
    RwRaster *TextureRaster;
    RwTextureAddressMode AddressModeU;
    RwTextureAddressMode AddressModeV;
    RwTextureFilterMode FilterMode;
    RwRGBA BorderColor;
    RwFogType FogType;
    RwRGBA FogColor;
};
typedef struct RwTexture RwTexture;
typedef struct RwTexture RwTexture;
typedef struct RwFrame RWALIGN(RwFrame, rwFRAMEALIGNMENT);
typedef struct RwObjectHasFrame RwObjectHasFrame;
typedef RwObjectHasFrame * (*RwObjectHasFrameSyncFunction)(RwObjectHasFrame *object);
struct RwObjectHasFrame
{
    RwObject object;
    RwLLLink lFrame;
    RwObjectHasFrameSyncFunction sync;
};
typedef struct RwBBox RwBBox;
struct RwBBox
{

    RwV3d sup;
    RwV3d inf;
};
enum RwCameraProjection
{
    rwNACAMERAPROJECTION = 0,
    rwPERSPECTIVE = 1,
    rwPARALLEL = 2,
    rwCAMERAPROJECTIONFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCameraProjection RwCameraProjection;
typedef struct RwCamera RWALIGN(RwCamera, rwMATRIXALIGNMENT);
typedef RwCamera *(*RwCameraBeginUpdateFunc) (RwCamera * camera);
typedef RwCamera *(*RwCameraEndUpdateFunc) (RwCamera * camera);
typedef struct RwFrustumPlane RwFrustumPlane;
struct RwFrustumPlane
{
        RwPlane plane;
        RwUInt8 closestX;
        RwUInt8 closestY;
        RwUInt8 closestZ;
        RwUInt8 pad;
};
struct RwCamera
{
        RwObjectHasFrame object;

        RwCameraProjection projectionType;

        RwCameraBeginUpdateFunc beginUpdate;
        RwCameraEndUpdateFunc endUpdate;

        RwMatrix viewMatrix;

        RwRaster *frameBuffer;

        RwRaster *zBuffer;

        RwV2d viewWindow;
        RwV2d recipViewWindow;
        RwV2d viewOffset;
        RwReal nearPlane;
        RwReal farPlane;
        RwReal fogPlane;

        RwReal zScale, zShift;

        RwFrustumPlane frustumPlanes[6];
        RwBBox frustumBoundBox;

        RwV3d frustumCorners[8];
};
struct rwPipeGlobals
{
    RwFreeList *pipesFreeList;
    RxRenderStateVector defaultRenderState;
    RwLinkList allPipelines;

    RwUInt32 maxNodesPerPipe;

    RxPipeline *currentAtomicPipeline;
    RxPipeline *currentWorldSectorPipeline;
    RxPipeline *currentMaterialPipeline;

    RxPipeline *genericAtomicPipeline;
    RxPipeline *genericWorldSectorPipeline;
    RxPipeline *genericMaterialPipeline;

    RxPipeline *platformAtomicPipeline;
    RxPipeline *platformWorldSectorPipeline;
    RxPipeline *platformMaterialPipeline;
};
typedef struct rwPipeGlobals rwPipeGlobals;
#define RXPIPELINEGLOBAL(var) (RWPLUGINOFFSET(rwPipeGlobals, RwEngineInstance, _rxPipelineGlobalsOffset)->var)
extern RwInt32 _rxPipelineGlobalsOffset;
typedef struct RpMaterial RpMaterial;
typedef struct RpMaterialList RpMaterialList;
struct RpMaterialList
{
    RpMaterial **materials;
    RwInt32 numMaterials;
    RwInt32 space;
};
typedef struct RpMeshHeader RpMeshHeader;
struct RpMeshHeader
{
    RwUInt32 flags;
    RwUInt16 numMeshes;
    RwUInt16 serialNum;

    RwUInt32 totalIndicesInMesh;

    RwUInt32 firstMeshOffset;

};
typedef struct RpGeometry RpGeometry;
typedef struct RpMorphTarget RpMorphTarget;
struct RpMorphTarget
{
        RpGeometry *parentGeom;
        RwSphere boundingSphere;
        RwV3d *verts;
        RwV3d *normals;
};
typedef struct RpTriangle RpTriangle;
struct RpTriangle
{
    RwUInt16 vertIndex[3];
    RwUInt16 matIndex;
};
struct RpGeometry
{
    RwObject object;

    RwUInt32 flags;

    RwUInt16 lockedSinceLastInst;
    RwInt16 refCount;

    RwInt32 numTriangles;
    RwInt32 numVertices;
    RwInt32 numMorphTargets;
    RwInt32 numTexCoordSets;

    RpMaterialList matList;

    RpTriangle *triangles;

    RwRGBA *preLitLum;

    RwTexCoords *texCoords[rwMAXTEXTURECOORDS];

    RpMeshHeader *mesh;

    RwResEntry *repEntry;

    RpMorphTarget *morphTarget;

};
typedef struct RpWorldSector RpWorldSector;
typedef struct RpSector RpSector;
typedef struct RpAtomic RpAtomic;
typedef struct RpInterpolator RpInterpolator;
struct RpInterpolator
{
        RwInt32 flags;
        RwInt16 startMorphTarget;
        RwInt16 endMorphTarget;
        RwReal time;
        RwReal recipTime;
        RwReal position;
};
typedef struct RpClump RpClump;
typedef RpClump *(*RpClumpCallBack) (RpClump * clump, void *data);
struct RpClump
{
        RwObject object;

        RwLinkList atomicList;

        RwLinkList lightList;
        RwLinkList cameraList;

        RwLLLink inWorldLink;

        RpClumpCallBack callback;
};
typedef RpAtomic *(*RpAtomicCallBackRender) (RpAtomic * atomic);
struct RpAtomic
{
    RwObjectHasFrame object;

    RwResEntry *repEntry;

    RpGeometry *geometry;

    RwSphere boundingSphere;
    RwSphere worldBoundingSphere;

    RpClump *clump;
    RwLLLink inClumpLink;

    RpAtomicCallBackRender renderCallBack;

    RpInterpolator interpolator;

    RwUInt16 renderFrame;
    RwUInt16 pad;

    RwLinkList llWorldSectorsInAtomic;

    RxPipeline *pipeline;
};
#define RpAtomicGetGeometryMacro(_atomic) ((_atomic)->geometry)
#define RpAtomicSetRenderCallBackMacro(_atomic,_callback) MACRO_START { (_atomic)->renderCallBack = (_callback); if (!(_atomic)->renderCallBack) { (_atomic)->renderCallBack = AtomicDefaultRenderCallBack; } } MACRO_STOP
#define RpAtomicGetFrameMacro(_atomic) ((RwFrame *) rwObjectGetParent(_atomic))
#define RpAtomicGetGeometry(_atomic) RpAtomicGetGeometryMacro(_atomic)
#define RpAtomicSetRenderCallBack(_atomic,_callback) RpAtomicSetRenderCallBackMacro(_atomic, _callback)
#define RpAtomicGetFrame(_atomic) RpAtomicGetFrameMacro(_atomic)
extern RpAtomic *
AtomicDefaultRenderCallBack(RpAtomic * atomic);
enum RpWorldRenderOrder
{
    rpWORLDRENDERNARENDERORDER = 0,
    rpWORLDRENDERFRONT2BACK,
    rpWORLDRENDERBACK2FRONT,
    rpWORLDRENDERORDERFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpWorldRenderOrder RpWorldRenderOrder;
typedef RpWorldSector *(*RpWorldSectorCallBackRender) (RpWorldSector *
                                                       worldSector);
typedef struct RpWorld RpWorld;
struct RpWorld
{
    RwObject object;

    RwUInt32 flags;

    RpWorldRenderOrder renderOrder;

    RpMaterialList matList;

    RpSector *rootSector;

    RwInt32 numTexCoordSets;

    RwInt32 numClumpsInWorld;
    RwLLLink *currentClumpLink;

    RwLinkList clumpList;

    RwLinkList lightList;

    RwLinkList directionalLightList;

    RwV3d worldOrigin;

    RwBBox boundingBox;

    RpWorldSectorCallBackRender renderCallBack;

    RxPipeline *pipeline;
};
#define rwID_LODATMPLUGIN MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x12)
#define RWERROR(errorcode) do { RwError _rwErrorCode; _rwErrorCode.pluginID = rwPLUGIN_ID; _rwErrorCode.errorCode = _rwerror errorcode; RwErrorSet(&_rwErrorCode); } while (0)
#define RWASSERT(condition) 
typedef struct RtAnimAnimation RtAnimAnimation;
typedef void (*RtAnimKeyFrameApplyCallBack) (void *result, void *voidIFrame);
typedef void (*RtAnimKeyFrameBlendCallBack) (void *voidOut, void *voidIn1,
                                             void *voidIn2, RwReal alpha);
typedef void (*RtAnimKeyFrameInterpolateCallBack) (void *voidOut, void *voidIn1,
                                                   void *voidIn2, RwReal time,
                                                   void *customData);
typedef void (*RtAnimKeyFrameAddCallBack) (void *voidOut, void *voidIn1,
                                                    void *voidIn2);
typedef struct RtAnimInterpolatorInfo RtAnimInterpolatorInfo;
struct RtAnimAnimation
{
    RtAnimInterpolatorInfo *interpInfo;

    RwInt32 numFrames;

    RwInt32 flags;

    RwReal duration;

    void *pFrames;

    void *customData;

};
typedef struct RtAnimInterpolator RtAnimInterpolator;
typedef RtAnimInterpolator * (*RtAnimCallBack)
                                        (RtAnimInterpolator *animInstance,
                                        void *data);
struct RtAnimInterpolator
{
    RtAnimAnimation *pCurrentAnim;

    RwReal currentTime;

    void *pNextFrame;

    RtAnimCallBack pAnimCallBack;

    void *pAnimCallBackData;

    RwReal animCallBackTime;

    RtAnimCallBack pAnimLoopCallBack;

    void *pAnimLoopCallBackData;

    RwInt32 maxInterpKeyFrameSize;

    RwInt32 currentInterpKeyFrameSize;

    RwInt32 currentAnimKeyFrameSize;

    RwInt32 numNodes;

    RwBool isSubInterpolator;

    RwInt32 offsetInParent;

    RtAnimInterpolator *parentAnimation;

    RtAnimKeyFrameApplyCallBack keyFrameApplyCB;

    RtAnimKeyFrameBlendCallBack keyFrameBlendCB;

    RtAnimKeyFrameInterpolateCallBack keyFrameInterpolateCB;

    RtAnimKeyFrameAddCallBack keyFrameAddCB;

};
typedef struct RpHAnimAtomicGlobalVars RpHAnimAtomicGlobalVars;
struct RpHAnimAtomicGlobalVars
{
        RwInt32 engineOffset;
        RwFreeList *HAnimFreeList;
};
typedef struct RpHAnimHierarchy RpHAnimHierarchy;
typedef struct RpHAnimNodeInfo RpHAnimNodeInfo;
struct RpHAnimNodeInfo
{
    RwInt32 nodeID;
    RwInt32 nodeIndex;
    RwInt32 flags;
    RwFrame * pFrame;
};
enum RpHAnimHierarchyFlag
{

    rpHANIMHIERARCHYSUBHIERARCHY = 0x01,
    rpHANIMHIERARCHYNOMATRICES = 0x02,

    rpHANIMHIERARCHYUPDATEMODELLINGMATRICES = 0x1000,
    rpHANIMHIERARCHYUPDATELTMS = 0x2000,
    rpHANIMHIERARCHYLOCALSPACEMATRICES = 0x4000,

    rpHANIMHIERARCHYFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpHAnimHierarchyFlag RpHAnimHierarchyFlag;
struct RpHAnimHierarchy
{
    RwInt32 flags;
    RwInt32 numNodes;

    RwMatrix *pMatrixArray;
    void *pMatrixArrayUnaligned;

    RpHAnimNodeInfo *pNodeInfo;
    RwFrame *parentFrame;

    RpHAnimHierarchy *parentHierarchy;
    RwInt32 rootParentOffset;

    RtAnimInterpolator *currentAnim;
};
enum RpMatFXMaterialFlags
{
    rpMATFXEFFECTNULL = 0,
    rpMATFXEFFECTBUMPMAP = 1,
    rpMATFXEFFECTENVMAP = 2,
    rpMATFXEFFECTBUMPENVMAP = 3,
    rpMATFXEFFECTDUAL = 4,
    rpMATFXEFFECTUVTRANSFORM = 5,
    rpMATFXEFFECTDUALUVTRANSFORM = 6,

    rpMATFXEFFECTMAX,
    rpMATFXNUMEFFECTS = rpMATFXEFFECTMAX - 1,

    rpMATFXFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpMatFXMaterialFlags RpMatFXMaterialFlags;
enum RpPTankDataFlags
{
    rpPTANKDFLAGNONE = ((int)0x00000000),
    rpPTANKDFLAGPOSITION = ((int)0x00000001),
    rpPTANKDFLAGCOLOR = ((int)0x00000002),
    rpPTANKDFLAGSIZE = ((int)0x00000004),
    rpPTANKDFLAGMATRIX = ((int)0x00000008),
    rpPTANKDFLAGNORMAL = ((int)0x00000010),
    rpPTANKDFLAG2DROTATE = ((int)0x00000020),
    rpPTANKDFLAGVTXCOLOR = ((int)0x00000040),
    rpPTANKDFLAGVTX2TEXCOORDS = ((int)0x00000080),
    rpPTANKDFLAGVTX4TEXCOORDS = ((int)0x00000100),

    rpPTANKDFLAGCNSMATRIX = ((int)0x00008000),

    rpPTANKDFLAGCNS2DROTATE = ((int)0x00020000),
    rpPTANKDFLAGCNSVTXCOLOR = ((int)0x00040000),
    rpPTANKDFLAGCNSVTX2TEXCOORDS = ((int)0x00080000),
    rpPTANKDFLAGCNSVTX4TEXCOORDS = ((int)0x00100000),

    rpPTANKDFLAGUSECENTER = ((int)0x01000000),

    rpPTANKDFLAGARRAY = ((int)0x10000000),
    rpPTANKDFLAGSTRUCTURE = ((int)0x20000000),

    RPPTANKDATAFLAGSFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpPTankDataFlags RpPTankDataFlags;
typedef struct RpPTankLockStruct RpPTankLockStruct;
struct RpPTankLockStruct{
    RwUInt8 *data;
    RwInt32 stride;
};
typedef struct RpPTankFormatDescriptor RpPTankFormatDescriptor;
struct RpPTankFormatDescriptor
{
    RwInt32 numClusters;
    RwInt32 stride;

    RwInt32 dataFlags;
};
typedef struct RpPTankData RpPTankData;
struct RpPTankData
{
    void *data;

    RpPTankLockStruct clusters[9];

    void *userData;

    RpPTankFormatDescriptor format;

    RwUInt32 srcBlend;
    RwUInt32 dstBlend;
    RwBool vertexAlphaBlend;

    RwV2d cCenter;
    RwV2d cSize;
    RwReal cRotate;
    RwRGBA cColor;
    RwRGBA cVtxColor[4];
    RwTexCoords cUV[4];
    RwMatrix cMatrix;
};
typedef void *(* RpPTankAllocCallBack)(RpPTankData *ptankGlobal,
                                        RwInt32 maxPCount,
                                        RwUInt32 dataFlags,
                                        RwUInt32 platFlags);
typedef RwBool (* RpPTankCreateCallBack)(RpAtomic *atomic,
                                        RpPTankData *ptankGlobal,
                                        RwInt32 maxPCount,
                                        RwUInt32 dataFlags,
                                        RwUInt32 platFlags);
typedef RwBool (* RpPTankInstanceCallBack)(RpAtomic *atomic,
                                            RpPTankData *ptankGlobal,
                                            RwInt32 actPCount,
                                            RwUInt32 instFlags);
typedef RwBool (* RpPTankRenderCallBack)(RpAtomic *atomic,
                                            RpPTankData *ptankGlobal,
                                            RwInt32 actPCount);
typedef struct RpPTankCallBacks RpPTankCallBacks;
struct RpPTankCallBacks
{
    RpPTankAllocCallBack alloc;
    RpPTankCreateCallBack create;
    RpPTankInstanceCallBack instance;
    RpPTankRenderCallBack render;
};
typedef struct RpPTANKInstanceSetupData RpPTANKInstanceSetupData;
typedef void (* RpPTankGENInstancePosCallback)(
    RpPTankLockStruct *dstCluster,
    RwV3d *right,
    RwV3d *up,
    RwInt32 pCount,
    RpPTankData *ptankGlobal);
typedef void (* RpPTankGENInstanceCallback)(
    RpPTankLockStruct *dstCluster,
    RwInt32 pCount,
    RpPTankData *ptankGlobal);
typedef void (* RpPTankGENInstanceSetupCallback)(
    RpPTANKInstanceSetupData *data,
    RpAtomic *atomic,
    RpPTankData *ptankGlobal,
    RwInt32 actPCount,
    RwUInt32 instFlags);
typedef void (* RpPTankGENInstanceEndingCallback)(
    RpPTANKInstanceSetupData *data,
    RpAtomic *atomic,
    RpPTankData *ptankGlobal,
    RwInt32 actPCount,
    RwUInt32 instFlags);
typedef struct RpPTankAtomicExtPrv RpPTankAtomicExtPrv;
struct RpPTankAtomicExtPrv
{
    RwInt32 maxPCount;
    RwInt32 actPCount;

    RwBool isAStructure;
    void *rawdata;

    RpAtomicCallBackRender defaultRenderCB;

    RpPTankCallBacks ptankCallBacks;

    RpPTankGENInstanceSetupCallback insSetupCB;
    RpPTankGENInstancePosCallback insPosCB;
    RpPTankGENInstanceCallback insUVCB;
    RpPTankGENInstanceCallback insColorsCB;
    RpPTankGENInstanceCallback insNormalsCB;
    RpPTankGENInstanceEndingCallback insEndingCB;

    RwUInt32 lockFlags;
    RwUInt32 instFlags;
    RwUInt32 platFlags;

    RpPTankData publicData;
};
extern RwInt32 _rpPTankAtomicDataOffset;
#define RPATOMICPTANKPLUGINDATA(atomic) (*RWPLUGINOFFSET(RpPTankAtomicExtPrv *, (atomic), _rpPTankAtomicDataOffset))
typedef RwRaster RtCharset;
typedef struct RpIntersection RpIntersection;
struct RpCollisionTriangle
{
    RwV3d normal;
    RwV3d point;
    RwInt32 index;
    RwV3d *vertices[3];
};
typedef struct RpCollisionTriangle RpCollisionTriangle;
typedef RpCollisionTriangle *(*RpIntersectionCallBackGeometryTriangle)
    (RpIntersection *intersection, RpCollisionTriangle *collTriangle,
    RwReal distance, void *data);

/* ---- layout asserts: offsets and sizes of MFL.exe; the comment names the evidence ---- */
RW_LAYOUT_ASSERT(rw37_slim_0, RW_OFFSETOF(RpAtomic, boundingSphere) == 0x1c);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_slim_1, RW_OFFSETOF(RpAtomic, geometry) == 0x18);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_slim_2, RW_OFFSETOF(RpAtomic, interpolator.flags) == 0x4c);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_slim_3, RW_OFFSETOF(RpAtomic, object.object.parent) == 0x4);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_slim_4, RW_OFFSETOF(RpAtomic, object.object.type) == 0x0);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_slim_5, RW_OFFSETOF(RpClump, object.parent) == 0x4);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_slim_6, RW_OFFSETOF(RpGeometry, flags) == 0x8);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_slim_7, RW_OFFSETOF(RpGeometry, matList.materials) == 0x20);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_slim_8, RW_OFFSETOF(RpGeometry, morphTarget) == 0x5c);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_slim_9, RW_OFFSETOF(RpGeometry, numVertices) == 0x14);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_slim_10, RW_OFFSETOF(RpGeometry, preLitLum) == 0x30);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_slim_11, RW_OFFSETOF(RpGeometry, texCoords) == 0x34);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_slim_12, RW_OFFSETOF(RpGeometry, triangles) == 0x2c);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_slim_13, RW_OFFSETOF(RpWorld, matList.materials) == 0x10);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_slim_14, RW_OFFSETOF(RpHAnimHierarchy, currentAnim) == 0x20);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_slim_15, RW_OFFSETOF(RpMorphTarget, boundingSphere) == 0x4);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_slim_16, RW_OFFSETOF(RpMorphTarget, normals) == 0x18);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_slim_17, RW_OFFSETOF(RpMorphTarget, verts) == 0x14);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_slim_18, RW_OFFSETOF(RwCamera, fogPlane) == 0x88);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_slim_19, RW_OFFSETOF(RwCamera, frameBuffer) == 0x60);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_slim_20, RW_OFFSETOF(RwCamera, object.object.parent) == 0x4);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_slim_21, RW_OFFSETOF(RwCamera, projectionType) == 0x14);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_slim_22, RW_OFFSETOF(RwCamera, viewWindow) == 0x68);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_slim_23, RW_OFFSETOF(RwCamera, zBuffer) == 0x64);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_slim_24, RW_OFFSETOF(RwDevice, fpRenderStateSet) == 0x10);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_slim_25, RW_OFFSETOF(RwGlobals, dOpenDevice) == 0x10);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_slim_26, RW_OFFSETOF(RwGlobals, dOpenDevice.fpRenderStateSet) == 0x20);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_slim_27, RW_OFFSETOF(RwGlobals, dOpenDevice.zBufferNear) == 0x18);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_slim_28, RW_OFFSETOF(RwGlobals, stringFuncs) == 0xc4);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_slim_29, RW_OFFSETOF(RwObject, parent) == 0x4);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_slim_30, RW_OFFSETOF(RwStringFunctions, vecStrcpy) == 0x8);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_slim_31, RW_OFFSETOF(RwStringFunctions, vecStrcmp) == 0x24);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_slim_32, RW_OFFSETOF(RxNodeDefinition, name) == 0x0);      /* mfl_tier2.h accessor */
RW_LAYOUT_ASSERT(rw37_slim_33, RW_OFFSETOF(RpMorphTarget, boundingSphere) == 4);      /* an_codfh_ClumpAtomicStreamRead.c */
RW_LAYOUT_ASSERT(rw37_slim_34, RW_OFFSETOF(RpAtomic, repEntry) == 0x14);      /* an_codfh_ClumpAtomicStreamRead.c */
RW_LAYOUT_ASSERT(rw37_slim_35, sizeof(RwLLLink) == 8);      /* an_codfh_FreeListCreate.c */
RW_LAYOUT_ASSERT(rw37_slim_36, RW_OFFSETOF(RwFreeList, entriesPerBlock) == 4);      /* an_codfh_FreeListCreate.c */
RW_LAYOUT_ASSERT(rw37_slim_37, RW_OFFSETOF(RwFreeList, heapSize) == 8);      /* an_codfh_FreeListCreate.c */
RW_LAYOUT_ASSERT(rw37_slim_38, RW_OFFSETOF(RwFreeList, alignment) == 0xc);      /* an_codfh_FreeListCreate.c */
RW_LAYOUT_ASSERT(rw37_slim_39, RW_OFFSETOF(RwFreeList, blockList) == 0x10);      /* an_codfh_FreeListCreate.c */
RW_LAYOUT_ASSERT(rw37_slim_40, RW_OFFSETOF(RwFreeList, flags) == 0x18);      /* an_codfh_FreeListCreate.c */
RW_LAYOUT_ASSERT(rw37_slim_41, RW_OFFSETOF(RwFreeList, link) == 0x1c);      /* an_codfh_FreeListCreate.c */
RW_LAYOUT_ASSERT(rw37_slim_42, sizeof(RwFreeList) == 0x24);      /* an_codfh_FreeListCreate.c */
RW_LAYOUT_ASSERT(rw37_slim_43, sizeof(RpMorphTarget) == 0x1c);      /* an_codfh_RpGeometryStreamRead.c */
RW_LAYOUT_ASSERT(rw37_slim_44, RW_OFFSETOF(RpGeometry, flags) == 8);      /* an_codfh_RpGeometryStreamRead.c */
RW_LAYOUT_ASSERT(rw37_slim_45, RW_OFFSETOF(RpGeometry, refCount) == 0xe);      /* an_codfh_RpGeometryStreamRead.c */
RW_LAYOUT_ASSERT(rw37_slim_46, RW_OFFSETOF(RpGeometry, numTriangles) == 0x10);      /* an_codfh_RpGeometryStreamRead.c */
RW_LAYOUT_ASSERT(rw37_slim_47, RW_OFFSETOF(RpGeometry, numTexCoordSets) == 0x1c);      /* an_codfh_RpGeometryStreamRead.c */
RW_LAYOUT_ASSERT(rw37_slim_48, RW_OFFSETOF(RpGeometry, matList) == 0x20);      /* an_codfh_RpGeometryStreamRead.c */
RW_LAYOUT_ASSERT(rw37_slim_49, RW_OFFSETOF(RpGeometry, repEntry) == 0x58);      /* an_codfh_RpGeometryStreamRead.c */
RW_LAYOUT_ASSERT(rw37_slim_50, RW_OFFSETOF(RpWorld, flags) == 8);      /* an_codfh_RpWorldStreamRead.c */
RW_LAYOUT_ASSERT(rw37_slim_51, RW_OFFSETOF(RpWorld, matList) == 0x10);      /* an_codfh_RpWorldStreamRead.c */
RW_LAYOUT_ASSERT(rw37_slim_52, RW_OFFSETOF(RpWorld, rootSector) == 0x1c);      /* an_codfh_RpWorldStreamRead.c */
RW_LAYOUT_ASSERT(rw37_slim_53, RW_OFFSETOF(RpWorld, clumpList) == 0x2c);      /* an_codfh_RpWorldStreamRead.c */
RW_LAYOUT_ASSERT(rw37_slim_54, RW_OFFSETOF(RpWorld, worldOrigin) == 0x44);      /* an_codfh_RpWorldStreamRead.c */
RW_LAYOUT_ASSERT(rw37_slim_55, RW_OFFSETOF(RpWorld, boundingBox) == 0x50);      /* an_codfh_RpWorldStreamRead.c */
RW_LAYOUT_ASSERT(rw37_slim_56, RW_OFFSETOF(RpWorld, pipeline) == 0x6c);      /* an_codfh_RpWorldStreamRead.c */
RW_LAYOUT_ASSERT(rw37_slim_57, sizeof(RtAnimAnimation) == 0x18);      /* an_codfh_RtAnimAnimationStreamRead.c */
RW_LAYOUT_ASSERT(rw37_slim_58, RW_OFFSETOF(RwError, errorCode) == 4);      /* an_codfh_RwErrorSet.c */
RW_LAYOUT_ASSERT(rw37_slim_59, RW_OFFSETOF(RwModuleInfo, numInstances) == 4);      /* an_codfh_RwErrorSet.c */
RW_LAYOUT_ASSERT(rw37_slim_60, RW_OFFSETOF(RwMatrix, flags) == 0xc);      /* an_codfh_RwMatrixStreamRead.c */
RW_LAYOUT_ASSERT(rw37_slim_61, RW_OFFSETOF(RwMatrix, pos) == 0x30);      /* an_codfh_RwMatrixStreamRead.c */
RW_LAYOUT_ASSERT(rw37_slim_62, RW_OFFSETOF(RpWorld, numTexCoordSets) == 0x20);      /* an_codfh_WorldSectorStreamRead.c */
RW_LAYOUT_ASSERT(rw37_slim_63, RW_OFFSETOF(RwGlobals, stdFunc) == 0x48);      /* an_codfh_badevice.c */
RW_LAYOUT_ASSERT(rw37_slim_64, RW_OFFSETOF(RwGlobals, memoryFuncs) == 0x108);      /* an_codfh_badevice.c */
RW_LAYOUT_ASSERT(rw37_slim_65, RW_OFFSETOF(RwGlobals, memoryAlloc) == 0x118);      /* an_codfh_badevice.c */
RW_LAYOUT_ASSERT(rw37_slim_66, RW_OFFSETOF(RwGlobals, engineStatus) == 0x124);      /* an_codfh_badevice.c */
RW_LAYOUT_ASSERT(rw37_slim_67, RW_OFFSETOF(RwGlobals, resArenaInitSize) == 0x128);      /* an_codfh_badevice.c */
RW_LAYOUT_ASSERT(rw37_slim_68, sizeof(RwGlobals) == 0x12c);      /* an_codfh_badevice.c */
RW_LAYOUT_ASSERT(rw37_slim_69, RW_OFFSETOF(RwObject, privateFlags) == 3);      /* an_codfh_baframe_clone.c */
RW_LAYOUT_ASSERT(rw37_slim_70, RW_OFFSETOF(RpGeometry, lockedSinceLastInst) == 0xc);      /* an_codfh_bageomet_life.c */
RW_LAYOUT_ASSERT(rw37_slim_71, RW_OFFSETOF(RpGeometry, mesh) == 0x54);      /* an_codfh_bageomet_life.c */
RW_LAYOUT_ASSERT(rw37_slim_72, RW_OFFSETOF(RpMaterialList, space) == 8);      /* an_codfh_bamatlst_append.c */
RW_LAYOUT_ASSERT(rw37_slim_73, RW_OFFSETOF(RwObjectHasFrame, sync) == 0x10);      /* an_codfh_basync.c */
RW_LAYOUT_ASSERT(rw37_slim_74, sizeof(RwMatrix) == 0x40);      /* an_codfh_basync.c */
RW_LAYOUT_ASSERT(rw37_slim_75, RW_OFFSETOF(RpWorld, renderCallBack) == 0x68);      /* an_codfh_baworld_life.c */
RW_LAYOUT_ASSERT(rw37_slim_76, RW_OFFSETOF(RpPTankData, clusters) == 4);      /* an_codfh_ptank.c */
RW_LAYOUT_ASSERT(rw37_slim_77, RW_OFFSETOF(RpPTankData, format) == 0x50);      /* an_codfh_ptank.c */
RW_LAYOUT_ASSERT(rw37_slim_78, RW_OFFSETOF(RpPTankData, cSize) == 0x70);      /* an_codfh_ptank.c */
RW_LAYOUT_ASSERT(rw37_slim_79, RW_OFFSETOF(RpPTankData, cUV) == 0x90);      /* an_codfh_ptank.c */
RW_LAYOUT_ASSERT(rw37_slim_80, RW_OFFSETOF(RpPTankAtomicExtPrv, insSetupCB) == 0x24);      /* an_codfh_ptank.c */
RW_LAYOUT_ASSERT(rw37_slim_81, RW_OFFSETOF(RpPTankAtomicExtPrv, insEndingCB) == 0x38);      /* an_codfh_ptank.c */
RW_LAYOUT_ASSERT(rw37_slim_82, RW_OFFSETOF(RpPTankAtomicExtPrv, publicData) == 0x48);      /* an_codfh_ptank.c */
RW_LAYOUT_ASSERT(rw37_slim_83, RW_OFFSETOF(RpMaterialList, numMaterials) == 4);      /* an_codfh_rpMaterialListStreamRead.c */
RW_LAYOUT_ASSERT(rw37_slim_84, sizeof(RpMeshHeader) == 0x10);      /* an_codfh_rpMeshRead.c */
RW_LAYOUT_ASSERT(rw37_slim_85, RW_OFFSETOF(RpMeshHeader, serialNum) == 6);      /* an_codfh_rpMeshRead.c */
RW_LAYOUT_ASSERT(rw37_slim_86, sizeof(RpHAnimNodeInfo) == 0x10);      /* an_codfh_rphanim.c */
RW_LAYOUT_ASSERT(rw37_slim_87, RW_OFFSETOF(RpHAnimHierarchy, parentFrame) == 0x14);      /* an_codfh_rphanim.c */
RW_LAYOUT_ASSERT(rw37_slim_88, RW_OFFSETOF(RpAtomic, interpolator) == 0x4c);      /* an_codfh_rpmorph.c */
RW_LAYOUT_ASSERT(rw37_slim_89, RW_OFFSETOF(RpInterpolator, position) == 0x10);      /* an_codfh_rpmorph.c */
RW_LAYOUT_ASSERT(rw37_slim_90, RW_OFFSETOF(RtAnimAnimation, customData) == 0x14);      /* an_codfh_rpuvanim.c */
RW_LAYOUT_ASSERT(rw37_slim_91, sizeof(RwMemoryFunctions) == 0x10);      /* an_codfh_rwMemoryOpen.c */
RW_LAYOUT_ASSERT(rw37_slim_92, RW_OFFSETOF(RwMemoryFunctions, rwfree) == 4);      /* an_codfh_rwMemoryOpen.c */
RW_LAYOUT_ASSERT(rw37_slim_93, RW_OFFSETOF(RwMemoryFunctions, rwrealloc) == 8);      /* an_codfh_rwMemoryOpen.c */
RW_LAYOUT_ASSERT(rw37_slim_94, RW_OFFSETOF(RwMemoryFunctions, rwcalloc) == 0xc);      /* an_codfh_rwMemoryOpen.c */
RW_LAYOUT_ASSERT(rw37_slim_95, RW_OFFSETOF(RwGlobals, stringFuncs.vecStrncpy) == 0xd0);      /* an_codfh_rwStringOpen.c */
RW_LAYOUT_ASSERT(rw37_slim_96, RW_OFFSETOF(RwGlobals, stringFuncs.vecStrlen) == 0xf4);      /* an_codfh_rwStringOpen.c */
RW_LAYOUT_ASSERT(rw37_slim_97, RW_OFFSETOF(RwGlobals, stringFuncs.vecSscanf) == 0x104);      /* an_codfh_rwStringOpen.c */
RW_LAYOUT_ASSERT(rw37_slim_98, RW_OFFSETOF(RwGlobals, dirtyFrameList) == 0xbc);      /* exe: _rwFrameOpen 0x004d0924 `add eax,0xbc` initialises the dirty frame list head */
RW_LAYOUT_ASSERT(rw37_slim_99, sizeof(RwCamera) == 0x184);      /* exe: exe static cameraTKList.sizeOfStruct at 0x0062e534 */
RW_LAYOUT_ASSERT(rw37_slim_100, sizeof(RpWorld) == 0x70);      /* exe: exe static worldTKList.sizeOfStruct at 0x0062ebec */
RW_LAYOUT_ASSERT(rw37_slim_101, sizeof(RpAtomic) == 0x70);      /* exe: exe static atomicTKList.sizeOfStruct at 0x0062ec04 */
RW_LAYOUT_ASSERT(rw37_slim_102, sizeof(RpClump) == 0x2c);      /* exe: exe static clumpTKList.sizeOfStruct at 0x0062ec1c */
RW_LAYOUT_ASSERT(rw37_slim_103, sizeof(RpGeometry) == 0x60);      /* exe: exe static geometryTKList.sizeOfStruct at 0x0062ec70 */
RW_LAYOUT_ASSERT(rw37_slim_104, sizeof(RwV3d) == 12);      /* MFL.exe .rdata 0x5f5a48: rpptank.c datasize[0] = sizeof(RwV3d) = 12 */
RW_LAYOUT_ASSERT(rw37_slim_105, sizeof(RwV2d) == 8);      /* MFL.exe .rdata 0x5f5a54: datasize[3] = sizeof(RwV2d) = 8 */
RW_LAYOUT_ASSERT(rw37_slim_106, sizeof(RwRGBA) == 4);      /* MFL.exe .rdata 0x5f5a58: datasize[4] = sizeof(RwRGBA) = 4 */
RW_LAYOUT_ASSERT(rw37_slim_107, sizeof(RwTexCoords) == 8);      /* MFL.exe .rdata 0x5f5a64: datasize[7] = 2 * sizeof(RwTexCoords) = 16 */
RW_LAYOUT_ASSERT(rw37_slim_108, sizeof(RwMatrix) == 64);      /* MFL.exe .rdata 0x5f5a4c: datasize[1] = sizeof(RwMatrix) = 64 */
RW_LAYOUT_ASSERT(rw37_slim_109, sizeof(RwLinkList) == 8);      /* RwGlobals: dirtyFrameList 0xbc (_rwFrameOpen 0x004d0924) and stringFuncs 0xc4 (mfl_tier2.h accessor) are 8 bytes apart */
RW_LAYOUT_ASSERT(rw37_slim_110, RW_OFFSETOF(RpPTankLockStruct, stride) == 4);      /* MFL.exe 0x545b98 / 0x545b9b (rpPTankAStructAlloc): clusters[0].data at [esi+0x4c], clusters[0].stride = 12 at [esi+0x50]; publicData 0x48 + clusters 4 */
RW_LAYOUT_ASSERT(rw37_slim_111, sizeof(RpPTankLockStruct) == 8);      /* RpPTankData: clusters 4, format 0x50 (an_codfh_ptank.c) = 4 + 9 * 8 + userData 4 */
RW_LAYOUT_ASSERT(rw37_slim_112, RW_OFFSETOF(RpPTankFormatDescriptor, dataFlags) == 8);      /* MFL.exe 0x5459e8-0x545a03 (rpPTankAStructAlloc): dataFlags &= 0xfffffe00 at [esi+0xa0], numClusters = 0 at [esi+0x98]; publicData 0x48 + format 0x50 */
RW_LAYOUT_ASSERT(rw37_slim_113, sizeof(RpPTankCallBacks) == 16);      /* RpPTankAtomicExtPrv: insSetupCB 0x24 (an_codfh_ptank.c), lockFlags = 0 at [esi+0x3c] (MFL.exe 0x545a09): the four callbacks lie at 0x14-0x24 behind defaultRenderCB 0x10 */
RW_LAYOUT_ASSERT(rw37_slim_114, RW_OFFSETOF(RpPTankAtomicExtPrv, lockFlags) == 0x3c);      /* MFL.exe 0x545a09 (rpPTankAStructAlloc): mov [esi+0x3c], eax (lockFlags = 0) */
RW_LAYOUT_ASSERT(rw37_slim_115, sizeof(RxPipelineNode) == 0x28);      /* MFL.exe 0x4e6bb5-0x4e6bca (LockPipelineExpandData): index * 5 * 8, rep movs of 10 words; 0x4e6c43 sub ebx,0x28 */
RW_LAYOUT_ASSERT(rw37_slim_116, RW_OFFSETOF(RxPipelineNode, outputs) == 8);      /* MFL.exe 0x4e6cda (LockPipelineExpandData): mov [ecx+eax+0x8],ebx (nodes[i].outputs) */
RW_LAYOUT_ASSERT(rw37_slim_117, RW_OFFSETOF(RxPipelineNode, slotClusterRefs) == 0xc);      /* MFL.exe 0x4e6bd1 (LockPipelineExpandData): mov [eax+ebx+0xc],0 */
RW_LAYOUT_ASSERT(rw37_slim_118, RW_OFFSETOF(RxPipelineNode, topSortData) == 0x1c);      /* MFL.exe 0x4e6d40 (LockPipelineExpandData): mov [ebx+edi+0x1c],eax */
RW_LAYOUT_ASSERT(rw37_slim_119, RW_OFFSETOF(RxPipelineNode, initializationData) == 0x20);      /* MFL.exe 0x4e6c0d (LockPipelineExpandData): RwMalloc result to [edx+ebx+0x20] */
RW_LAYOUT_ASSERT(rw37_slim_120, RW_OFFSETOF(RxPipelineNode, initializationDataSize) == 0x24);      /* MFL.exe 0x4e6bed (LockPipelineExpandData): mov eax,[eax+ebx+0x24], the RwMalloc size */
RW_LAYOUT_ASSERT(rw37_slim_121, RW_OFFSETOF(RxPipeline, numNodes) == 4);      /* MFL.exe 0x4e6ba7 (LockPipelineExpandData): mov ebx,[edx+0x4]; dec ebx (srcPipe->numNodes - 1) */
RW_LAYOUT_ASSERT(rw37_slim_122, RW_OFFSETOF(RxPipeline, nodes) == 8);      /* MFL.exe 0x4e6bbb / 0x4e6bbe (LockPipelineExpandData): node arrays of both pipelines from [reg+0x8] */
RW_LAYOUT_ASSERT(rw37_slim_123, RW_OFFSETOF(RxPipeline, embeddedPacketState) == 0x10);      /* MFL.exe 0x4f1cfb / 0x4f1d8b (rxEmbeddedPacketBetweenNodes): cmp [edx+0x10],1; mov [edx+0x10],3 (rxPKST_PENDING) */
RW_LAYOUT_ASSERT(rw37_slim_124, RW_OFFSETOF(RxPipeline, embeddedPacket) == 0x14);      /* MFL.exe 0x4f1d0b (rxEmbeddedPacketBetweenNodes): mov ebp,[edx+0x14] */
RW_LAYOUT_ASSERT(rw37_slim_125, RW_OFFSETOF(rwPipeGlobals, maxNodesPerPipe) == 0x38);      /* MFL.exe 0x4e6cae (LockPipelineExpandData): mov eax,[_rxPipelineGlobalsOffset + RwEngineInstance + 0x38] */
RW_LAYOUT_ASSERT(rw37_slim_126, sizeof(RxRenderStateVector) == 0x2c);      /* rwPipeGlobals: maxNodesPerPipe 0x38 (MFL.exe 0x4e6cae) = pipesFreeList 4 + defaultRenderState + allPipelines 8 */
RW_LAYOUT_ASSERT(rw37_slim_127, RW_OFFSETOF(RxNodeDefinition, io) == 0x20);      /* MFL.exe 0x4e734b (NodeCreate): cmp [edx+0x20],0x20 (io.numClustersOfInterest > RXNODEMAXCLUSTERSOFINTEREST) */
RW_LAYOUT_ASSERT(rw37_slim_128, sizeof(RxNodeMethods) == 0x1c);      /* RxNodeDefinition: io at 0x20 (MFL.exe 0x4e734b) behind name 4 + nodeMethods */
RW_LAYOUT_ASSERT(rw37_slim_129, RW_OFFSETOF(RxIoSpec, numOutputs) == 0xc);      /* MFL.exe 0x4e730d (NodeCreate): mov ebx,[eax+0x2c] = nodespec->io.numOutputs, io at 0x20 */
RW_LAYOUT_ASSERT(rw37_slim_130, sizeof(RxCluster) == 0x1c);      /* MFL.exe 0x4f1d1e-0x4f1d27, 0x4f1d6d (rxEmbeddedPacketBetweenNodes): n * 7 * 4; sub esi,0x1c per cluster */
RW_LAYOUT_ASSERT(rw37_slim_131, RW_OFFSETOF(RxCluster, clusterRef) == 0x14);      /* MFL.exe 0x4f1d37 (rxEmbeddedPacketBetweenNodes): cmp [esi+0x10],0 with esi = &cluster.data (4) */
RW_LAYOUT_ASSERT(rw37_slim_132, RW_OFFSETOF(RxPacket, numClusters) == 2);      /* MFL.exe 0x4f1d1a (rxEmbeddedPacketBetweenNodes): mov di,[ebp+0x2] */
RW_LAYOUT_ASSERT(rw37_slim_133, RW_OFFSETOF(RxPacket, slotsContinue) == 0xc);      /* MFL.exe 0x4f1d10 / 0x4f1d82 (rxEmbeddedPacketBetweenNodes): mov ecx,[ebp+0xc]; packet->slotsContinue = nodeTo->slotsContinue */
RW_LAYOUT_ASSERT(rw37_slim_134, RW_OFFSETOF(RxPacket, clusters) == 0x14);      /* MFL.exe 0x4f1d27 (rxEmbeddedPacketBetweenNodes): lea esi,[ebp+n*28-4] = &clusters[n-1].data */
RW_LAYOUT_ASSERT(rw37_slim_135, RW_OFFSETOF(RxClusterDefinition, defaultAttributes) == 8);      /* MFL.exe 0x4f30ab (MyEnumPipelineClustersCallBack): mov ecx,[ecx+0x8] */
RW_LAYOUT_ASSERT(rw37_slim_136, sizeof(rxHeapBlockHeader) == 0x20);      /* MFL.exe 0x4f1730 / 0x4f173f (HeapSuperBlockReset): end header = start + size - 0x20, copied as 8 words */
RW_LAYOUT_ASSERT(rw37_slim_137, RW_OFFSETOF(rxHeapBlockHeader, freeEntry) == 0xc);      /* MFL.exe 0x4f175a (HeapSuperBlockReset): mov [ebx+0xc],eax */
RW_LAYOUT_ASSERT(rw37_slim_138, RW_OFFSETOF(rxHeapSuperBlockDescriptor, size) == 4);      /* MFL.exe 0x4f172d (HeapSuperBlockReset): mov ecx,[ecx+0x4] */
RW_LAYOUT_ASSERT(rw37_slim_139, RW_OFFSETOF(rxHeapFreeBlock, ptr) == 4);      /* MFL.exe 0x4f1764 (HeapSuperBlockReset): mov [eax+0x4],ebx */
RW_LAYOUT_ASSERT(rw37_slim_140, sizeof(RwFrustumPlane) == 20);      /* RwCamera: fogPlane 0x88 (mfl_tier2.h accessor), sizeof 0x184 (cameraTKList): 0x184 - 0x94 - RwBBox 24 - 8 * RwV3d 12 = 6 * 20 */
RW_LAYOUT_ASSERT(rw37_slim_141, sizeof(RwPlane) == 16);      /* RwFrustumPlane 20 (above) = plane + four RwUInt8 */
RW_LAYOUT_ASSERT(rw37_slim_142, sizeof(RwBBox) == 0x18);      /* RpWorld: boundingBox 0x50, renderCallBack 0x68 (an_codfh_RpWorldStreamRead.c, an_codfh_baworld_life.c) */
RW_LAYOUT_ASSERT(rw37_slim_143, sizeof(RwSphere) == 0x10);      /* RpMorphTarget: boundingSphere 4, verts 0x14 (mfl_tier2.h accessors) */
RW_LAYOUT_ASSERT(rw37_slim_144, RW_OFFSETOF(RtAnimInterpolator, maxInterpKeyFrameSize) == 0x20 && RW_OFFSETOF(RtAnimInterpolator, currentInterpKeyFrameSize) == 0x24 && RW_OFFSETOF(RtAnimInterpolator, numNodes) == 0x2c && sizeof(RtAnimInterpolator) == 0x4c);      /* MFL.exe 0x53e4aa / 0x53e4b0 / 0x53e4ad (RtAnimInterpolatorCopy: numNodes, currentInterpKeyFrameSize, frames at +0x4c); 0x543eaa (HAnimCopy: currentAnim->maxInterpKeyFrameSize) */
RW_LAYOUT_ASSERT(rw37_slim_145, RW_OFFSETOF(RpHAnimAtomicGlobalVars, HAnimFreeList) == 4);      /* MFL.exe 0x543c9c engineOffset -> 0x947ac8 (RpHAnimPluginAttach), 0x543ce7 HAnimFreeList -> 0x947acc (HAnimOpen) */
RW_LAYOUT_ASSERT(rw37_slim_146, sizeof(RwLine) == 0x18 && RW_OFFSETOF(RwLine, end) == 0xc);      /* MFL.exe 0x54cc8e push 0x2 (RpAtomicForAllIntersections: RwV3dTransformPoints of 2 points from &line->start); RpIntersection.type 0x18 right behind */
RW_LAYOUT_ASSERT(rw37_slim_147, sizeof(RpTriangle) == 8 && RW_OFFSETOF(RpTriangle, matIndex) == 6);      /* MFL.exe 0x54cbc0 add edi,0x8 / 0x54cb11 / 0x54cb19 / 0x54cb2f (GeometryForAllTriIntsecSphere no-tree loop: vertIndex[0..2] at 0 / 2 / 4) */
RW_LAYOUT_ASSERT(rw37_slim_148, RW_OFFSETOF(RpCollisionTriangle, point) == 0xc && RW_OFFSETOF(RpCollisionTriangle, index) == 0x18 && RW_OFFSETOF(RpCollisionTriangle, vertices) == 0x1c);      /* MFL.exe 0x54cb66..0x54cba2 (GeometryForAllTriIntsecSphere): collTri at esp+0x18, point at +0x24, index +0x30, vertices [esp+0x44] after four pushes = +0x34 */
RW_LAYOUT_ASSERT(rw37_slim_149, sizeof(RwD3D9Vertex) == 0x1c && RW_OFFSETOF(RwD3D9Vertex, emissiveColor) == 0x10 && RW_OFFSETOF(RwD3D9Vertex, u) == 0x14);      /* MFL.exe 0x569087..0x56909b (CharsetProcessString): z / rhw / emissiveColor at vertex+8 / +0xc / +0x10, stride 0x1c; u / v at 0x56901c / 0x56902b */
#endif /* RW37_SLIM_H */
