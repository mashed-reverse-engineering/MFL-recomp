/* mfl_tier2.h: the types of the machine generated sources (tier 2: src/tier2, clusters: src/clusters).
   Curated: plain C types, the game's own structs, and the library types (RenderWare, Windows, C runtime) as incomplete
   structs (handles), except a few plain math / vertex records the code uses by value.  RenderWare object fields the code
   touches are reached through offset accessors (below), as rwrs.h does: no RenderWare struct bodies.
   Every object file compiled with this header is identical to the one compiled with the complete, uncurated type headers.
   Compiler: cl 13.00.9466 (Visual C++ .NET 2002), C mode.  Generated once by a script, then maintained by hand. */
#ifndef MFL_TIER2_H
#define MFL_TIER2_H

/* ---- integer types: <stdint.h> names (cl 13 has no stdint.h) and the decompiler spellings used in the sources ---- */
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;

typedef uint8_t undefined;      /* "undefinedN": N bytes of unknown type (an unsigned integer of that size) */
typedef uint8_t undefined1;
typedef uint16_t undefined2;
typedef uint32_t undefined3;    /* 3 byte values only appear in casts of the CONCAT31 kind below */
typedef uint32_t undefined4;
typedef uint64_t undefined6;
typedef uint64_t undefined8;
typedef int int3;
typedef unsigned int uint3;
typedef uint8_t byte;
typedef uint16_t word;
typedef uint32_t dword;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;    /* long, not int: the same code, but a distinct C type */
typedef __int64 longlong;
typedef unsigned __int64 ulonglong;
typedef unsigned char bool;
#define true 1
#define false 0

/* float10: the x87 80 bit extended value of an FPU register.  cl 13 has no such type (long double is double), so float10 is
   double; the compiler keeps intermediate results in ST registers at full precision anyway, which is what the original did. */
typedef double float10;
typedef void code();            /* an untyped code address: called as ((code *)p)() */
#define NAN (*(float *)"\x00\x00\xc0\x7f")    /* the quiet NaN constant, as the float bit pattern */

/* partial register / byte composition helpers: CONCATxy(a, b) = a (x bytes) above b (y bytes); SUBxy(v, i) = y bytes of the x byte v at byte i */
#define CONCAT11(a, b) ((unsigned short)(((unsigned short)(unsigned char)(a) << 8) | (unsigned char)(b)))
#define CONCAT12(a, b) ((unsigned int)(((unsigned int)(unsigned char)(a) << 16) | (unsigned short)(b)))
#define CONCAT13(a, b) ((unsigned int)(((unsigned int)(unsigned char)(a) << 24) | ((unsigned int)(b) & 0xffffff)))
#define CONCAT21(a, b) ((unsigned int)(((unsigned int)(unsigned short)(a) << 8) | (unsigned char)(b)))
#define CONCAT22(a, b) ((unsigned int)(((unsigned int)(unsigned short)(a) << 16) | (unsigned short)(b)))
#define CONCAT31(a, b) ((unsigned int)(((unsigned int)(a) << 8) | (unsigned char)(b)))
#define CONCAT44(a, b) ((unsigned __int64)(((unsigned __int64)(unsigned int)(a) << 32) | (unsigned int)(b)))
#define SUB41(x, i) ((unsigned char)((unsigned int)(x) >> (8 * (i))))
#define SUB42(x, i) ((unsigned short)((unsigned int)(x) >> (8 * (i))))
#define SUB84(x, i) ((unsigned int)((unsigned __int64)(x) >> (8 * (i))))

/* ---- library types: RenderWare (Rw*, Rp*, Rt*, Rx*), Windows, C runtime, in dependency order.  Structs without a body are
   incomplete on purpose (handles); the one line structs are the plain records used by value (vectors, matrix, vertices,
   RECT, LARGE_INTEGER); enums carry the library's values; callbacks are the function pointer types of library calls ---- */
typedef unsigned __int64 GUID;
typedef short wchar_t;
typedef ulong DWORD;
typedef uint UINT;
typedef long LONG;
typedef void *HANDLE;
typedef struct _iobuf _iobuf;
typedef struct _LARGE_INTEGER _LARGE_INTEGER;

struct _LARGE_INTEGER {
    DWORD LowPart;
    LONG HighPart;
};

typedef char CHAR;
typedef int RwInt32;
typedef uchar RwUInt8;
typedef float RwReal;
typedef uint RwUInt32;
typedef struct RwRGBA { unsigned char red, green, blue, alpha; } RwRGBA;
typedef RwInt32 RwBool;
typedef char RwChar;
typedef struct RwV3d { float x, y, z; } RwV3d;
typedef struct RwBBox { RwV3d sup, inf; } RwBBox;
typedef struct _RTL_CRITICAL_SECTION _RTL_CRITICAL_SECTION;
typedef ushort WORD;
typedef struct RwMatrixTag { RwV3d right; unsigned int flags; RwV3d up; unsigned int pad1; RwV3d at; unsigned int pad2; RwV3d pos; unsigned int pad3; } RwMatrixTag;
typedef struct RwMatrixTag RwMatrix;
typedef struct RxObjSpace3DVertex { RwV3d objVertex; RwV3d objNormal; unsigned int color; float u, v; } RxObjSpace3DVertex;
typedef struct RwD3D9Vertex { float x, y, z, rhw; unsigned int emissiveColor; float u, v; } RwD3D9Vertex;
typedef struct RwD3D9Vertex RwIm2DVertex;
typedef struct RxPipeline RxPipeline;

typedef enum RwPrimitiveType {
    rwPRIMITIVETYPEFORCEENUMSIZEINT=-1,
    rwPRIMTYPENAPRIMTYPE=0,
    rwPRIMTYPELINELIST=1,
    rwPRIMTYPEPOLYLINE=2,
    rwPRIMTYPETRILIST=3,
    rwPRIMTYPETRISTRIP=4,
    rwPRIMTYPETRIFAN=5,
    rwPRIMTYPEPOINTLIST=6
} RwPrimitiveType;

typedef ushort RwUInt16;
typedef RwUInt16 RxVertexIndex;
typedef struct RwRGBAReal { float red, green, blue, alpha; } RwRGBAReal;
typedef struct RwTexture RwTexture;

typedef enum RwShadeMode {
    rwSHADEMODEFORCEENUMSIZEINT=-1,
    rwSHADEMODENASHADEMODE=0,
    rwSHADEMODEFLAT=1,
    rwSHADEMODEGOURAUD=2
} RwShadeMode;

typedef enum RwBlendFunction {
    rwBLENDFUNCTIONFORCEENUMSIZEINT=-1,
    rwBLENDNABLEND=0,
    rwBLENDZERO=1,
    rwBLENDONE=2,
    rwBLENDSRCCOLOR=3,
    rwBLENDINVSRCCOLOR=4,
    rwBLENDSRCALPHA=5,
    rwBLENDINVSRCALPHA=6,
    rwBLENDDESTALPHA=7,
    rwBLENDINVDESTALPHA=8,
    rwBLENDDESTCOLOR=9,
    rwBLENDINVDESTCOLOR=10,
    rwBLENDSRCALPHASAT=11
} RwBlendFunction;

typedef struct RwRaster RwRaster;

typedef enum RwTextureAddressMode {
    rwTEXTUREADDRESSMODEFORCEENUMSIZEINT=-1,
    rwTEXTUREADDRESSNATEXTUREADDRESS=0,
    rwTEXTUREADDRESSWRAP=1,
    rwTEXTUREADDRESSMIRROR=2,
    rwTEXTUREADDRESSCLAMP=3,
    rwTEXTUREADDRESSBORDER=4
} RwTextureAddressMode;

typedef enum RwTextureFilterMode {
    rwTEXTUREFILTERMODEFORCEENUMSIZEINT=-1,
    rwFILTERNAFILTERMODE=0,
    rwFILTERNEAREST=1,
    rwFILTERLINEAR=2,
    rwFILTERMIPNEAREST=3,
    rwFILTERMIPLINEAR=4,
    rwFILTERLINEARMIPNEAREST=5,
    rwFILTERLINEARMIPLINEAR=6
} RwTextureFilterMode;

typedef struct RxPipelineNode RxPipelineNode;
typedef struct RwTexDictionary RwTexDictionary;
typedef struct RxNodeDefinition RxNodeDefinition;
typedef struct RwObject RwObject;
typedef struct RpMaterial RpMaterial;
typedef struct _SECURITY_ATTRIBUTES _SECURITY_ATTRIBUTES;
typedef void *LPVOID;
typedef wchar_t WCHAR;
typedef WORD ATOM;
#define NULL 0
#define TRUE 1
typedef struct HWND__ HWND__;
typedef struct HWND__ *HWND;
typedef uint UINT_PTR;
typedef UINT_PTR WPARAM;
typedef long LONG_PTR;
typedef LONG_PTR LPARAM;
typedef struct _iobuf FILE;
typedef struct HBRUSH__ HBRUSH__;
typedef struct HBRUSH__ *HBRUSH;
typedef struct HICON__ HICON__;
typedef struct HICON__ *HICON;
typedef HICON HCURSOR;
typedef struct HINSTANCE__ HINSTANCE__;
typedef struct HINSTANCE__ *HINSTANCE;
typedef struct HMENU__ HMENU__;
typedef struct HMENU__ *HMENU;
typedef HINSTANCE HMODULE;
typedef GUID IID;
typedef struct IUnknown IUnknown;
typedef struct _LARGE_INTEGER LARGE_INTEGER;
typedef struct _RTL_CRITICAL_SECTION *PRTL_CRITICAL_SECTION;
typedef PRTL_CRITICAL_SECTION LPCRITICAL_SECTION;
typedef char *LPCSTR;
typedef struct tagRECT tagRECT;

struct tagRECT {
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
};

typedef struct _SECURITY_ATTRIBUTES *LPSECURITY_ATTRIBUTES;
typedef struct IUnknown *LPUNKNOWN;
typedef WCHAR *LPWSTR;
typedef LONG_PTR LRESULT;
typedef struct RpAtomic RpAtomic;
typedef struct RwResEntry RwResEntry;
typedef struct RpGeometry RpGeometry;
typedef struct RwSphere { RwV3d center; float radius; } RwSphere;
typedef struct RpClump RpClump;
typedef struct RpTriangle { unsigned short vertIndex[3]; unsigned short matIndex; } RpTriangle;
typedef struct RwTexCoords { float u, v; } RwTexCoords;
typedef struct RpMorphTarget RpMorphTarget;
typedef RpAtomic * (*RpAtomicCallBack)(struct RpAtomic *, void *);
typedef struct RtAnimAnimation RtAnimAnimation;
typedef struct RwStream RwStream;

typedef enum RwStreamType {
    rwSTREAMTYPEFORCEENUMSIZEINT=-1,
    rwNASTREAM=0,
    rwSTREAMFILE=1,
    rwSTREAMFILENAME=2,
    rwSTREAMMEMORY=3,
    rwSTREAMCUSTOM=4
} RwStreamType;

typedef enum RwStreamAccessType {
    rwSTREAMACCESSTYPEFORCEENUMSIZEINT=-1,
    rwNASTREAMACCESS=0,
    rwSTREAMREAD=1,
    rwSTREAMWRITE=2,
    rwSTREAMAPPEND=3
} RwStreamAccessType;

typedef struct RpHAnimHierarchy RpHAnimHierarchy;
typedef struct RwFrame RwFrame;
typedef RwFrame * (*RwFrameCallBack)(struct RwFrame *, void *);
typedef struct RtAnimInterpolator RtAnimInterpolator;
typedef struct RtQuat { RwV3d imag; float real; } RtQuat;

typedef enum RpInterpolatorFlag {
    rpINTERPOLATORFLAGFORCEENUMSIZEINT=-1,
    rpINTERPOLATORDIRTYINSTANCE=1,
    rpINTERPOLATORDIRTYSPHERE=2,
    rpINTERPOLATORNOFRAMEDIRTY=4
} RpInterpolatorFlag;

typedef struct RpLight RpLight;
typedef struct RpWorldSector RpWorldSector;
typedef RpMaterial * (*RpMaterialCallBack)(struct RpMaterial *, void *);

typedef enum RpMatFXMaterialFlags {
    rpMATFXFORCEENUMSIZEINT=-1,
    rpMATFXEFFECTNULL=0,
    rpMATFXEFFECTBUMPMAP=1,
    rpMATFXEFFECTENVMAP=2,
    rpMATFXEFFECTBUMPENVMAP=3,
    rpMATFXEFFECTDUAL=4,
    rpMATFXEFFECTUVTRANSFORM=5,
    rpMATFXEFFECTDUALUVTRANSFORM=6,
    rpMATFXEFFECTMAX=7,
    rpMATFXNUMEFFECTS=8
} RpMatFXMaterialFlags;

typedef struct RpSpline RpSpline;
typedef struct RpWorld RpWorld;
typedef RpWorldSector * (*RpWorldSectorCallBack)(struct RpWorldSector *, void *);
typedef struct RtDict RtDict;
typedef struct RtDictSchema RtDictSchema;
typedef struct RtFileSystem RtFileSystem;
typedef struct RwCamera RwCamera;

typedef enum RwCameraProjection {
    rwCAMERAPROJECTIONFORCEENUMSIZEINT=-1,
    rwNACAMERAPROJECTION=0,
    rwPERSPECTIVE=1,
    rwPARALLEL=2
} RwCameraProjection;

typedef struct RwV2d { float x, y; } RwV2d;

typedef enum RwCameraClearMode {
    rwCAMERACLEARMODEFORCEENUMSIZEINT=-1,
    rwCAMERACLEARIMAGE=1,
    rwCAMERACLEARZ=2,
    rwCAMERACLEARSTENCIL=4
} RwCameraClearMode;

typedef enum RwCullMode {
    rwCULLMODEFORCEENUMSIZEINT=-1,
    rwCULLMODENACULLMODE=0,
    rwCULLMODECULLNONE=1,
    rwCULLMODECULLBACK=2,
    rwCULLMODECULLFRONT=3
} RwCullMode;

typedef struct RwDevice RwDevice;

typedef enum RwRenderState {
    rwRENDERSTATEFORCEENUMSIZEINT=-1,
    rwRENDERSTATENARENDERSTATE=0,
    rwRENDERSTATETEXTURERASTER=1,
    rwRENDERSTATETEXTUREADDRESS=2,
    rwRENDERSTATETEXTUREADDRESSU=3,
    rwRENDERSTATETEXTUREADDRESSV=4,
    rwRENDERSTATETEXTUREPERSPECTIVE=5,
    rwRENDERSTATEZTESTENABLE=6,
    rwRENDERSTATESHADEMODE=7,
    rwRENDERSTATEZWRITEENABLE=8,
    rwRENDERSTATETEXTUREFILTER=9,
    rwRENDERSTATESRCBLEND=10,
    rwRENDERSTATEDESTBLEND=11,
    rwRENDERSTATEVERTEXALPHAENABLE=12,
    rwRENDERSTATEBORDERCOLOR=13,
    rwRENDERSTATEFOGENABLE=14,
    rwRENDERSTATEFOGCOLOR=15,
    rwRENDERSTATEFOGTYPE=16,
    rwRENDERSTATEFOGDENSITY=17,
    rwRENDERSTATECULLMODE=20,
    rwRENDERSTATESTENCILENABLE=21,
    rwRENDERSTATESTENCILFAIL=22,
    rwRENDERSTATESTENCILZFAIL=23,
    rwRENDERSTATESTENCILPASS=24,
    rwRENDERSTATESTENCILFUNCTION=25,
    rwRENDERSTATESTENCILFUNCTIONREF=26,
    rwRENDERSTATESTENCILFUNCTIONMASK=27,
    rwRENDERSTATESTENCILFUNCTIONWRITEMASK=28,
    rwRENDERSTATEALPHATESTFUNCTION=29,
    rwRENDERSTATEALPHATESTFUNCTIONREF=30
} RwRenderState;

typedef RwBool (*RwRenderStateSetFunction)(enum RwRenderState, void *);
typedef RwBool (*RwRenderStateGetFunction)(enum RwRenderState, void *);
typedef RwBool (*RwIm2DRenderLineFunction)(RwIm2DVertex *, RwInt32, RwInt32, RwInt32);
typedef RwBool (*RwIm2DRenderTriangleFunction)(RwIm2DVertex *, RwInt32, RwInt32, RwInt32, RwInt32);
typedef RwBool (*RwIm2DRenderPrimitiveFunction)(enum RwPrimitiveType, RwIm2DVertex *, RwInt32);
typedef RxVertexIndex RwImVertexIndex;
typedef RwBool (*RwIm2DRenderIndexedPrimitiveFunction)(enum RwPrimitiveType, RwIm2DVertex *, RwInt32, RwImVertexIndex *, RwInt32);
typedef struct RwEngineOpenParams { void *displayID; } RwEngineOpenParams;

typedef enum RwFrustumTestResult {
    rwFRUSTUMTESTRESULTFORCEENUMSIZEINT=-1,
    rwSPHEREOUTSIDE=0,
    rwSPHEREBOUNDARY=1,
    rwSPHEREINSIDE=2
} RwFrustumTestResult;

typedef struct RwStringFunctions RwStringFunctions;
typedef struct RwMemoryFunctions RwMemoryFunctions;
typedef uint size_t;
typedef RwChar * (*vecStrcpyFunc)(RwChar *, RwChar *);
typedef RwChar * (*vecStrcatFunc)(RwChar *, RwChar *);
typedef int (*vecStrcmpFunc)(RwChar *, RwChar *);
typedef struct RwGlobalsRelease RwGlobalsRelease;

typedef enum RwIm3DTransformFlags {
    rwIM3DTRANSFORMFLAGSFORCEENUMSIZEINT=-1,
    rwIM3D_VERTEXUV=1,
    rwIM3D_ALLOPAQUE=2,
    rwIM3D_NOCLIP=4,
    rwIM3D_VERTEXXYZ=8,
    rwIM3D_VERTEXRGBA=16
} RwIm3DTransformFlags;

typedef struct RxObjSpace3DVertex RxObjSpace3DLitVertex;
typedef RxObjSpace3DLitVertex RwIm3DVertex;
typedef struct RwImage RwImage;
typedef RwImage * (*RwImageCallBackRead)(RwChar *);
typedef RwImage * (*RwImageCallBackWrite)(struct RwImage *, RwChar *);

typedef enum RwMatrixFlag {
    rwMATRIXFLAGFORCEENUMSIZEINT=-1,
    rwMATRIXINTERNALIDENTITY=131072
} RwMatrixFlag;

typedef enum RwMatrixType {
    rwMATRIXTYPEFORCEENUMSIZEINT=-1,
    rwMATRIXTYPENORMAL=1,
    rwMATRIXTYPEORTHOGONAL=2,
    rwMATRIXTYPEMASK=3,
    rwMATRIXTYPEORTHONORMAL=3
} RwMatrixType;

typedef enum RwOpCombineType {
    rwOPCOMBINETYPEFORCEENUMSIZEINT=-1,
    rwCOMBINEREPLACE=0,
    rwCOMBINEPRECONCAT=1,
    rwCOMBINEPOSTCONCAT=2
} RwOpCombineType;

typedef RwInt32 (*RwPluginDataChunkGetSizeCallBack)(void *, RwInt32, RwInt32);
typedef RwStream * (*RwPluginDataChunkReadCallBack)(struct RwStream *, RwInt32, void *, RwInt32, RwInt32);
typedef RwStream * (*RwPluginDataChunkWriteCallBack)(struct RwStream *, RwInt32, void *, RwInt32, RwInt32);
typedef void * (*RwPluginObjectConstructor)(void *, RwInt32, RwInt32);
typedef void * (*RwPluginObjectCopy)(void *, void *, RwInt32, RwInt32);
typedef void * (*RwPluginObjectDestructor)(void *, RwInt32, RwInt32);

typedef enum RwRasterFlipMode {
    rwRASTERFLIPMODEFORCEENUMSIZEINT=-1,
    rwRASTERFLIPDONTWAIT=0,
    rwRASTERFLIPWAITVSYNC=1
} RwRasterFlipMode;

typedef struct RwRect { int x, y, w, h; } RwRect;
typedef struct RwVideoMode { int width, height, depth; enum RwVideoModeFlag flags; int refRate, format; } RwVideoMode;

typedef enum RwVideoModeFlag {
    rwVIDEOMODEFLAGFORCEENUMSIZEINT=-1,
    rwVIDEOMODEEXCLUSIVE=1,
    rwVIDEOMODEINTERLACE=2,
    rwVIDEOMODEFFINTERLACE=4,
    rwVIDEOMODE_PS2_FSAASHRINKBLIT=256,
    rwVIDEOMODE_XBOX_WIDESCREEN=256,
    rwVIDEOMODE_PS2_FSAAREADCIRCUIT=512,
    rwVIDEOMODE_XBOX_PROGRESSIVE=512,
    rwVIDEOMODE_XBOX_FIELD=1024,
    rwVIDEOMODE_XBOX_10X11PIXELASPECT=2048
} RwVideoModeFlag;

typedef void (*RxD3D9AllInOneRenderCallBack)(struct RwResEntry *, void *, RwUInt8, RwUInt32);
typedef struct RxPipeline RxLockedPipe;
typedef struct tagWNDCLASSA tagWNDCLASSA;
typedef LRESULT (*WNDPROC)(HWND, UINT, WPARAM, LPARAM);

struct tagWNDCLASSA {
    UINT style;
    WNDPROC lpfnWndProc;
    int cbClsExtra;
    int cbWndExtra;
    HINSTANCE hInstance;
    HICON hIcon;
    HCURSOR hCursor;
    HBRUSH hbrBackground;
    LPCSTR lpszMenuName;
    LPCSTR lpszClassName;
};

typedef struct tagWNDCLASSA WNDCLASSA;



/* RenderWare engine globals: the API macros of the RenderWare headers that read RwEngineInstance (RWSRCGLOBAL), here by byte offset
   into the engine instance (as rwrs.h does), so the instance itself stays an incomplete type.  Offsets of the release build. */
typedef void *(*RwMallocFunc)(size_t, RwUInt32);
typedef void (*RwFreeFunc)(void *);
#define RW_GLOBAL(off, type) (*(type *)((char *)RwEngineInstance + (off)))
#define RwCameraGetCurrentCamera() RW_GLOBAL(0x00, void *)                                   /* curCamera */
#define RwIm2DGetNearScreenZ() RW_GLOBAL(0x18, RwReal)                                       /* dOpenDevice.zBufferNear */
#define RwRenderStateSet RW_GLOBAL(0x20, RwRenderStateSetFunction)                          /* dOpenDevice.fpRenderStateSet */
#define RwRenderStateGet RW_GLOBAL(0x24, RwRenderStateGetFunction)
#define RwIm2DRenderLine RW_GLOBAL(0x28, RwIm2DRenderLineFunction)
#define RwIm2DRenderTriangle RW_GLOBAL(0x2c, RwIm2DRenderTriangleFunction)
#define RwIm2DRenderPrimitive RW_GLOBAL(0x30, RwIm2DRenderPrimitiveFunction)
#define RwIm2DRenderIndexedPrimitive RW_GLOBAL(0x34, RwIm2DRenderIndexedPrimitiveFunction)
typedef int (*vecSprintfFunc)(RwChar *, RwChar *, ...);
typedef size_t (*vecStrlenFunc)(RwChar *);
#define rwsprintf RW_GLOBAL(0xc4, vecSprintfFunc)
#define rwstrlen RW_GLOBAL(0xf4, vecStrlenFunc)
#define rwstrcpy RW_GLOBAL(0xcc, vecStrcpyFunc)                                              /* stringFuncs (+0xc4) */
#define rwstrcat RW_GLOBAL(0xd4, vecStrcatFunc)
#define rwstrcmp RW_GLOBAL(0xe8, vecStrcmpFunc)
#define RwMalloc RW_GLOBAL(0x108, RwMallocFunc)                                             /* memoryFuncs (+0x108) */
#define RwFree RW_GLOBAL(0x10c, RwFreeFunc)

/* RwRasterCreate flags (RenderWare raster type / creation flags) */
#define rwRASTERTYPETEXTURE 0x04
#define rwRASTERDONTALLOCATE 0x80

/* RenderWare object fields the sources read or write: by byte offset (release build layouts), named <struct>_<member path>(object) */
#define RpAtomic_boundingSphere(p) (*(struct RwSphere *)((char *)(p) + 0x1c))
#define RpAtomic_geometry(p) (*(struct RpGeometry * *)((char *)(p) + 0x18))
#define RpAtomic_interpolator_flags(p) (*(RwInt32 *)((char *)(p) + 0x4c))
#define RpAtomic_object_object_parent(p) (*(void * *)((char *)(p) + 0x4))
#define RpAtomic_object_object_type(p) (*(RwUInt8 *)((char *)(p) + 0x0))
#define RpClump_object_parent(p) (*(void * *)((char *)(p) + 0x4))
#define RpGeometry_flags(p) (*(RwUInt32 *)((char *)(p) + 0x8))
#define RpGeometry_matList_materials(p) (*(struct RpMaterial ** *)((char *)(p) + 0x20))
#define RpGeometry_morphTarget(p) (*(struct RpMorphTarget * *)((char *)(p) + 0x5c))
#define RpGeometry_numVertices(p) (*(RwInt32 *)((char *)(p) + 0x14))
#define RpGeometry_preLitLum(p) (*(struct RwRGBA * *)((char *)(p) + 0x30))
#define RpGeometry_texCoords(p) ((struct RwTexCoords * *)((char *)(p) + 0x34))
#define RpGeometry_triangles(p) (*(struct RpTriangle * *)((char *)(p) + 0x2c))
#define RpWorld_matList_materials(p) (*(struct RpMaterial ** *)((char *)(p) + 0x10))
#define RpWorldSector_matListWindowBase(p) (*(RwUInt16 *)((char *)(p) + 0x80))
#define RpWorldSector_numTriangles(p) (*(RwUInt16 *)((char *)(p) + 0x84))
#define RpWorldSector_triangles(p) (*(struct RpTriangle * *)((char *)(p) + 0x4))
#define RpWorldSector_vertices(p) (*(struct RwV3d * *)((char *)(p) + 0x8))
#define RpHAnimHierarchy_currentAnim(p) (*(struct RtAnimInterpolator * *)((char *)(p) + 0x20))
#define RpLight_object_object_flags(p) (*(RwUInt8 *)((char *)(p) + 0x2))
#define RpLight_object_object_parent(p) (*(void * *)((char *)(p) + 0x4))
#define RpMaterial_color(p) (*(struct RwRGBA *)((char *)(p) + 0x4))
#define RpMaterial_surfaceProps_ambient(p) (*(RwReal *)((char *)(p) + 0xc))
#define RpMaterial_surfaceProps_diffuse(p) (*(RwReal *)((char *)(p) + 0x14))
#define RpMaterial_surfaceProps_specular(p) (*(RwReal *)((char *)(p) + 0x10))
#define RpMaterial_texture(p) (*(struct RwTexture * *)((char *)(p) + 0x0))
#define RpMorphTarget_boundingSphere(p) (*(struct RwSphere *)((char *)(p) + 0x4))
#define RpMorphTarget_normals(p) (*(struct RwV3d * *)((char *)(p) + 0x18))
#define RpMorphTarget_verts(p) (*(struct RwV3d * *)((char *)(p) + 0x14))
#define RwCamera_fogPlane(p) (*(RwReal *)((char *)(p) + 0x88))
#define RwCamera_frameBuffer(p) (*(struct RwRaster * *)((char *)(p) + 0x60))
#define RwCamera_object_object_parent(p) (*(void * *)((char *)(p) + 0x4))
#define RwCamera_projectionType(p) (*(enum RwCameraProjection *)((char *)(p) + 0x14))
#define RwCamera_viewWindow(p) (*(struct RwV2d *)((char *)(p) + 0x68))
#define RwCamera_zBuffer(p) (*(struct RwRaster * *)((char *)(p) + 0x64))
#define RwDevice_fpRenderStateSet(p) (*(RwRenderStateSetFunction *)((char *)(p) + 0x10))
#define RwFrame_modelling(p) (*(RwMatrix *)((char *)(p) + 0x10))
#define RwFrame_object_parent(p) (*(void * *)((char *)(p) + 0x4))
#define RwGlobalsRelease_dOpenDevice(p) (*(struct RwDevice *)((char *)(p) + 0x10))
#define RwGlobalsRelease_dOpenDevice_fpRenderStateSet(p) (*(RwRenderStateSetFunction *)((char *)(p) + 0x20))
#define RwGlobalsRelease_dOpenDevice_zBufferNear(p) (*(RwReal *)((char *)(p) + 0x18))
#define RwGlobalsRelease_stringFuncs(p) (*(struct RwStringFunctions *)((char *)(p) + 0xc4))
#define RwObject_parent(p) (*(void * *)((char *)(p) + 0x4))
#define RwRaster_height(p) (*(RwInt32 *)((char *)(p) + 0x10))
#define RwRaster_width(p) (*(RwInt32 *)((char *)(p) + 0xc))
#define RwStringFunctions_vecStrcpy(p) (*(vecStrcpyFunc *)((char *)(p) + 0x8))
#define RwStringFunctions_vecStrcmp(p) (*(vecStrcmpFunc *)((char *)(p) + 0x24))
#define RwTexture_name(p) ((char *)(p) + 0x10)
#define RwTexture_filterAddressing(p) (*(RwUInt32 *)((char *)(p) + 0x50))
#define RxNodeDefinition_name(p) (*(RwChar * *)((char *)(p) + 0x0))

/* ---- named constants: values of game state globals ---- */
/* table sizes shared with mfl_consts.h */
#ifndef MFL_COUNT_CONSTS
#define MFL_COUNT_CONSTS
#define MAX_CARS 16               /* Car records player_car[16] (index checks `>= 0x10`) */
#define MAX_PLAYERS 4             /* player slots / vehicle records / AI bots */
#define WHEELS_PER_CAR 4
#endif

/* bit patterns / ids / voice record offsets repeated in many sources */
#ifndef MFL_SHARED_BITS
#define MFL_SHARED_BITS
#define FLOAT_ONE_BITS (0x3f800000)         /* IEEE-754 bits of 1.0f, stored through an int-typed slot */
#define CAR_ID_BASE (0xff00)                /* collider type / event parameter 0xff00 + n denotes car n */
#define MESSAGE_COLOUR_DARK_RED (0xff460000) /* message_show colour argument (ARGB; inferred) */
#define COLOUR_WHITE_ARGB (0xffffffff)
#define VOICE_DEFAULT_FREQ_BITS (0x46ac4400) /* bits of 22050.0f, default voice frequency */
#define VOICE_OFF_HANDLE (0x30)             /* voice record: sound object / sub voice */
#define VOICE_OFF_FLAGS (0x34)              /* voice record flags: bit 2 set = the voice is active (playing); free voices have it clear (sfx_play_voice_at, sound_voices_set_position_start, sfx_set_voice_volume) */
#define VOICE_OFF_FREQ (0x3c)
#define VOICE_OFF_POS_X (0x58)
#define VOICE_OFF_POS_Y (0x5c)
#define VOICE_OFF_POS_Z (0x60)
#endif

/* ---- data layouts reached through raw offsets (macros expand to the same byte accesses) ---- */
#ifndef MFL_LAYOUT_MACROS
#define MFL_LAYOUT_MACROS
/* g_sound_groups: table of sound group records, SOUND_GROUP_STRIDE bytes each (sfx_play_group_at: +0x80 first voice slot, +0x84 voice count) */
#ifndef SOUND_GROUP_STRIDE
#define SOUND_GROUP_STRIDE (0x90)
#endif
#define SOUND_GROUP_FIRST_VOICE_OF(group) (*(int *)(g_sound_groups + (group) * SOUND_GROUP_STRIDE + 0x80))
/* course data block (the `course` / `world` / `game_state` argument of the course_*_cb0 setup functions, offsets from its start) */
#define COURSE_FX_SOURCE_COUNT(c, set) (*(int *)((char *)(c) + 0xc004 + (set) * 4)) /* number of 0x40 byte fx source records of set 0..2 (arrays at +0xc30, +0x1830, +0x2430; each gets an fx slot) */
#define COURSE_MTS_LIST(c, i) ((c)->mts_matrices + (i) * 0xc00) /* matrix list i of CourseWorld.mts_matrices (0x30 RwMatrix, count in mts_matrix_counts[i]) */
#define COURSE_CLUMP(c, i) (((RpClump **)(c)->clumps)[i]) /* Clump_Filename clump i of a CourseWorld as a pointer (the field is declared uint) */
#define COURSE_TEXDICT(c) (*(RwTexDictionary **)((char *)(c) + 0xc9d0))
#define COURSE_WORLD(c) (*(RpWorld **)((char *)(c) + 0xc9d4))
#define COURSE_FOG_NEAR(c) (*(float *)((char *)(c) + 0xc9f8))     /* an_hk_908 blends these with the *_END values by the lap-data fraction (inferred meaning) */
#define COURSE_FOG_FAR(c) (*(float *)((char *)(c) + 0xc9fc))
#define COURSE_FOG_NEAR_END(c) (*(float *)((char *)(c) + 0xca04))
#define COURSE_FOG_FAR_END(c) (*(float *)((char *)(c) + 0xca08))
/* weapon_rocket record (rocket_fly_straight / rocket_fly_homing / rocket_hit_callback); the +0x5c holder's +4 is the RwFrame of the rocket model */
typedef struct RocketRecord {
    char unknown_00[0xc];
    RwV3d pos;              /* 0x0c */
    float age;              /* 0x18: drives the sine wobble of straight flight */
    RwV3d vel;              /* 0x1c: per frame movement, set from the frame's at-vector * speed */
    float lift;             /* 0x28: added to vel.y (inferred) */
    float accel;            /* 0x2c: straight flight: +1e-4 per frame, capped at 0.005; homing: +1e-4 or -1e-3 per frame, clamped to 0..0.005 */
    float speed;            /* 0x30 */
    float target_speed_a;   /* 0x34: 4.878e-5 * car_get_speed of the target (homing; inferred) */
    float target_speed_b;   /* 0x38: target_speed_a - speed, i.e. a speed difference (homing: below 0.02 -> accel +1e-4, else -1e-3); the name is historical */
    int field_3c;           /* compared with the hit object's id in rocket_hit_callback */
    float pitch_rate;       /* 0x40: homing steering, 0 / +-20 */
    float yaw_rate;         /* 0x44: homing steering, 0 / +-2 */
    float yaw;              /* 0x48 */
    float pitch;            /* 0x4c */
    int field_50;
    int target_car;         /* 0x54: -1 = none (then target_fx_object is used) */
    undefined4 target_fx_object; /* 0x58 */
    char *frame_holder;     /* 0x5c */
} RocketRecord;
/* team panel entry (table g_team_658fd8, TEAM_PANEL_ENTRY_SIZE bytes each; team_panel_* functions).  visible[k] and part[k] belong to the same panel part k;
   part[k] is the RpAtomic-like holder whose vtable slot 0x48 draws it (team_panel_entry_render). */
typedef struct TeamPanelEntry {
    int player[3];          /* 0x00: player slot indices of the team (team_panel_reset appends them) */
    int player_count;       /* 0x0c */
    int visible[40];        /* 0x10: per part, rewritten every frame by team_panel_entry_update */
    char *part[40];         /* 0xb0: per part objects (team_panel_load_parts fills the table by the part slot of each atomic) */
    RwV3d origin;           /* 0x150: panel frame translation, copied by team_panel_load_origin from the 12-byte rows at g_team_panel_origin_table ((+-0.45, 0.33, 1.0), a position, not a colour) */
    int clump;              /* 0x15c */
    int frame;              /* 0x160: RwFrame of the clump */
    int team;               /* 0x164: panel index 0 / 1 (team0panel / team1panel); selects the origin row in g_team_panel_origin_table */
    unsigned int flags;     /* 0x168: bit 0x40000 follows g_team_6592b0[player[0]], 0x800000 << (player_count - 1) selects the 1 / 2 / 3 player layout, and bit (6 * i + team of player i) is the team bit of player slot i; team_panel_build_team_mask */
} TeamPanelEntry;
/* effect object record (table g_scene_651250, 0xec bytes each; fx_update_effects, explosion_spawn_fx, the rwp collision callback) */
typedef struct FxSceneObject {
    char unknown_00[0x10];
    int frame_a;          /* 0x10: +4 is the RwFrame */
    char unknown_14[4];
    int owner;            /* 0x18: rwp object handle whose owner object (rwp_owner_object_get) carries the physics body; 0 ends the table; fx_find_object_by_owner matches on it */
    char unknown_1c[4];
    int spin_a;           /* 0x20: +4 is a node whose matrix (+0x10) is turned 1 degree per update about its axis (+0x20) around its pivot (+0x40), while not exploded */
    int spin_b;           /* 0x24: second node turned like spin_a */
    char unknown_28[0x14];
    int frame_c;          /* 0x3c: +4 is an RwFrame (explosion_spawn_fx reads its LTM) */
    int frame_d;          /* 0x40 */
    undefined4 rwp_object; /* 0x44 */
    int exploded;         /* 0x48: 0 until the damage fraction reaches 1, then 1 (explosion_spawn_fx; fx_update_effects only drives the body while 0) */
    float age;            /* 0x4c: += tick seconds */
    float offset_x;       /* 0x50: rwp object position minus frame position */
    float offset_y;
    float offset_z;
    float health;         /* 0x5c: percent, explosion_spawn_fx uses (100 - health) / 100 */
    float damage_last;    /* 0x60: damage fraction 0..1 returned by explosion_spawn_fx on the last update; fx_update_effects uses it as 1 / ((damage_last * 8 + 5) * dt) body response */
    int flag_64;          /* 0x64: set once the damage fraction passes 0.5 (explosion_spawn_fx) */
    float f_68;           /* 0x68: copy of damage_last taken whenever timer_6c reaches 3 s (written only, no reader found) */
    float timer_6c;       /* 0x6c: seconds, restarts at 3 */
    char unknown_70[0x34];
    int flag_a4;          /* 0xa4 */
    char unknown_a8[8];
    int choice_count;     /* 0xb0 */
    int choice[5];        /* 0xb4: random pick of the next entry in the course table (+0xc9b0) */
    int choice_index;     /* 0xc8 */
    int cur;              /* 0xcc: current entry, its +0xc is the duration */
    float timer;          /* 0xd0: time within cur; when it passes cur+0xc a new entry is picked from choice[] */
    float gravity;        /* 0xd4: added to vel.y each step */
    float timer_rate;     /* 0xd8: timer advances by timer_rate * tick seconds */
    RwV3d vel;            /* 0xdc */
    float damping;        /* 0xe8 */
} FxSceneObject;
#endif

/* g_race_phase values (race_set_phase / race_get_phase); 4, 8, 9 named from the handlers that set them (inferred), 10 and 11 are set but not named yet */
#ifndef MFL_RACE_PHASE_ENUM
#define MFL_RACE_PHASE_ENUM
enum RacePhase {
    RACE_PHASE_SETUP = 1,         /* course_race_phase_hook: course marker positions are initialised */
    RACE_PHASE_PRE_LOAD = 2,
    RACE_PHASE_LOADING = 3,       /* race_phase_3_update: camera path plays, then phase 4 (or 6 in training) */
    RACE_PHASE_INTRO = 4,         /* (inferred) set by race_phase_3_update together with the 12000 ms timer; race_phase_4_update only prepares the panels and moves on to the countdown at once */
    RACE_PHASE_COUNTDOWN = 5,     /* the deterministic test harness seeds the RNG here */
    RACE_PHASE_RACING = 6,
    RACE_PHASE_STANDINGS = 7,
    RACE_PHASE_FINISHED = 8,      /* (inferred) set by race_phase_7_update (standings) when no winner camera is started (training or no winner) */
    RACE_PHASE_WINNER_VIEW = 9,   /* (inferred) set by race_phase_7_update when a winner is found (not training): camera on the winner; handler race_phase_9_update */
    RACE_PHASE_WINNER_DONE = 10,  /* (inferred) set by the event handler right after race_phase_9_update; cars stop updating */
    RACE_PHASE_ROUND_END = 11     /* (inferred) race_check_winner_by_game_mode: weapons/powerups off, end-of-round camera */
};
#endif
#ifndef MFL_APP_STATE_ENUM
#define MFL_APP_STATE_ENUM
/* g_app_state (0x0076e7f4, get_app_state()): the main loop state machine of the frontend frame step (update and draw per state).
   Values derived from what each case does; plain int constants. */
enum AppState {
    APP_STATE_NONE = 0,           /* before main_loop_init (the music code treats it as "no game": fades out) */
    APP_STATE_MAIN_LOOP_INIT = 1, /* set by main_loop_init: title / attract menu (menu ids 0, 1, 3, 0x21) */
    APP_STATE_COURSE_LOADED = 2,  /* course_load_time_report, then straight on to the race; renders nothing */
    APP_STATE_RACE = 3,           /* race running (training_mode_update); sound voices are only positioned in this state */
    APP_STATE_MENU = 4,           /* frontend page drawn alone (plus the options menu for menu id 0x20) */
    APP_STATE_FRONTEND = 5,       /* app_enter_frontend: race drawn with the frontend on top (pause / results), waits for ui state 2 */
    APP_STATE_RACE_START = 6,     /* chunked race_start_setup loop, frontend over the race; ends when ui state 0 */
    APP_STATE_RACE_MESSAGE = 7    /* race with a message box up (message_show_*) */
};
#endif

#ifndef MFL_EVENT_ENUMS
#define MFL_EVENT_ENUMS
/* g_event_type (0x0069cbd4, get_event_type()): the career grid column of the selected event (0..0x12).  Names follow the icon
   medal_asset_names picks per event (TimeTrial / cup / MultiPlayer / QuickRace / tournament icons); 9 and 10 are not identified. */
enum EventType {
    EVENT_TIME_TRIAL_0 = 0,        /* "TimeTrial" icon */
    EVENT_TIME_TRIAL_1 = 1,
    EVENT_TIME_TRIAL_2 = 2,
    EVENT_CUP_3 = 3,               /* cup events: "BronzeCup" / "SilverCup" / "GoldCup" and the medal icons, special stages (GameMode 5, 7..10) */
    EVENT_CUP_4 = 4,
    EVENT_CUP_5 = 5,
    EVENT_MULTIPLAYER_6 = 6,       /* "MultiPlayer" icon; battle events with 2 opponents left end the round */
    EVENT_MULTIPLAYER_7 = 7,
    EVENT_MULTIPLAYER_8 = 8,
    EVENT_QUICK_RACE_11 = 0xb,     /* "QuickRace" icon; bot skill from menu_bot_skill_b */
    EVENT_QUICK_RACE_12 = 0xc,
    EVENT_QUICK_RACE_13 = 0xd,
    EVENT_TURBO_TOURNAMENT = 0xe,  /* medal_asset_names: "TurboT" icon */
    EVENT_ICE_TOURNAMENT = 0xf,    /* "IceT" */
    EVENT_NIGHT_TOURNAMENT = 0x10, /* "NightT" */
    EVENT_TRAINING = 0x11,         /* the training lessons (training_lesson_get branches, training prompt, no course overlay) */
    EVENT_QUICK_RACE = 0x12        /* set by quick_race_defaults_set together with GAME_MODE_QUICK_RACE */
};
/* game_mode (0x007ef7b8): the rule set of the race.  Names from the text the game shows for the mode (Font36/ENGLISH.DAT: the
   "special stage" titles of menu_draw_event_select, selected by the career kind code that menu_update maps to the mode) and from the code
   that runs only in the mode. */
enum GameMode {
    GAME_MODE_STANDARD = 0,        /* "Standard Play" (multiplayer game mode choice 0, battle events) */
    GAME_MODE_HOLD_THE_FLAG = 1,   /* choice 1: only the flag holder (g_cars_7ef7bc, the "leader" / fugitive) scores, car_hit_by_leader passes the flag (inferred) */
    GAME_MODE_HARE_AND_HOUNDS = 2, /* choice 2: one special car (g_cars_7ef7c0, grid slot 100) is chased (inferred) */
    GAME_MODE_RACE = 4,            /* "Race Game: 3 lap race against N opponents" (career kinds 2, 3, 0xb) */
    GAME_MODE_KILL_COPTER = 5,     /* special stage "Kill-the-Copter" (copter models are loaded in this mode) */
    GAME_MODE_QUICK_RACE = 6,      /* quick_race_defaults_set */
    GAME_MODE_AIRSTRIKE = 7,       /* special stage "Air Strike", no HUD */
    GAME_MODE_SURVIVAL = 8,        /* special stage "Survival Game" */
    GAME_MODE_CHASE_FUGITIVE = 9,  /* special stage "Chase-the-Fugitive": one human, scripted car, time limit text */
    GAME_MODE_BEAT_THE_BOMB = 10   /* special stage "Beat-the-Bomb": bomb timer text, bomb model (Bomb.dff) */
};
/* menu page ids (g_menu_records[].page_id, menu_page_enter): named after the title / item texts of the page tables at g_menu_page_item_lists
   (text id = Font36/DEFINES.TXT value + 1).  Pages 0x14, 0x16, 0x17, 0x19, 0x1b, 0x26, 0x27 are not identified. */
enum MenuPage {
    MENU_PAGE_PAUSE = 0,           /* retry / options / restart race / quit race */
    MENU_PAGE_MAIN = 1,            /* single player / multiplayer / options / bonus features / exit */
    MENU_PAGE_SINGLE_PLAYER = 2,   /* training / challenge cup / arcade modes / tournaments */
    MENU_PAGE_MULTIPLAYER = 3,     /* standard play / team play */
    MENU_PAGE_TEAM_COLOUR_SELECT = 4,
    MENU_PAGE_RACE_RESULTS = 5,
    MENU_PAGE_CHALLENGE_SELECT_A = 6,
    MENU_PAGE_CHALLENGE_SELECT_B = 7,
    MENU_PAGE_OPTIONS = 8,         /* sound / gamma correction / load-save */
    MENU_PAGE_TEAMS = 9,           /* free play / custom */
    MENU_PAGE_CUSTOMISE_PLAY = 0xa,
    MENU_PAGE_XBOX_LIVE = 0xb,     /* host / join */
    MENU_PAGE_LOBBY_A = 0xc,
    MENU_PAGE_JOIN = 0xd,
    MENU_PAGE_LOBBY_B = 0xe,
    MENU_PAGE_ABILITY_SELECT = 0xf,
    MENU_PAGE_TEAM_SELECT = 0x10,
    MENU_PAGE_CUSTOM_ABILITIES = 0x11, /* ability / airstrike / game length */
    MENU_PAGE_GAME_SETUP_MULTI = 0x12, /* play game / power-ups / game length / extra opponents / vehicle / game mode */
    MENU_PAGE_SOUND = 0x13,        /* music / sfx / insults volume sliders (g_game_state.option_sliders) */
    MENU_PAGE_GAME_SETUP_SINGLE = 0x18, /* play game / power-ups / difficulty / vehicle / opponents / game mode */
    MENU_PAGE_GAME_OPTIONS = 0x1a, /* move screen */
    MENU_PAGE_VIBRATION = 0x1c,
    MENU_PAGE_CONTROLLERS = 0x1d,
    MENU_PAGE_GAMMA = 0x1e,        /* PC gamma correction */
    MENU_PAGE_BONUS_FEATURES = 0x1f, /* flipped / oz / retro / mashed camera */
    MENU_PAGE_AUTOSAVE = 0x20,
    MENU_PAGE_BONUS_CAMERAS = 0x21,
    MENU_PAGE_ARCADE_MODES = 0x22, /* quick race / time attack */
    MENU_PAGE_WIDESCREEN = 0x23,
    MENU_PAGE_SCREEN = 0x24,
    MENU_PAGE_LOAD_SAVE = 0x25
};
#endif

#ifndef MFL_WEAPON_ENUMS
#define MFL_WEAPON_ENUMS
/* weapon type ids (player_equip_weapon / player_get_weapon_id / powerup_random_weapon_type); only the ids that code names. */
enum WeaponType {
    WEAPON_NONE = -1,
    WEAPON_MORTAR = 7,
    WEAPON_MACHINEGUN = 9,
    WEAPON_DEPTHCHARGE = 10,
    WEAPON_MISSILE = 11,
    WEAPON_MINE = 12,
    WEAPON_FLAMETHROWER = 16,
    WEAPON_SHOTGUN = 17,
    WEAPON_FLARE = 18,
    WEAPON_OIL = 19,
    WEAPON_CHAOS = 21,
    WEAPON_AIRSTRIKE = 22
};
/* bits of the vehicle record flag word set by vehicle_set_flag_* (FRONT / BACK inferred, the others only known as bit values) */
enum VehicleFlag {
    VEHICLE_FLAG_ACTIVE = 0x8,     /* vehicle_set_active */
    VEHICLE_FLAG_FRONT = 0x10,
    VEHICLE_FLAG_BACK = 0x20,
    VEHICLE_FLAG_40 = 0x40,
    VEHICLE_FLAG_80 = 0x80,
    VEHICLE_FLAG_400 = 0x400,
    VEHICLE_FLAG_800 = 0x800,
    VEHICLE_FLAG_1000 = 0x1000,
    VEHICLE_FLAG_2000 = 0x2000
};
#endif

/* ---- the game's structs (names / layouts from names/MFL_structs*.txt, the ApplyStructs specs) ---- */

typedef struct AiBotRecord AiBotRecord, *PAiBotRecord;

struct AiBotRecord {
    int base_value; /* "value copied from the table at 0x603A84 (per bot) by ai_bots_randomise" */
    int catchup_cycles; /* "incremented every 10000 catch-up ticks (ai_catchup_bot_state_update)" */
    int catchup_ticks; /* "ticks accumulated while the human player is far ahead" */
    int stage; /* "stage of the AI stage state machine (0..2), ai_bot_stage_state_update" */
    int stage_counter; /* "counter inside the stage (wraps at 3)" */
    char _gap_14[8];
    int target_valid; /* 1 once ai_find_target_b chose a target car, 0 when it is dropped */
    int target_car; /* car index of the chosen target (car_get_forward argument) */
    int leader_far_ms; /* ms the human player has been further than 2.5 away (ai_find_leader_a returns 1 after 72000) */
    union { float leader_distance; undefined4 field_0x28; }; /* player_distance_get to the human player (type 1 slot), read by ai_drive_* (> 4, > 6) */
    int steer_side; /* last steer side written by ai_drive_*: 1 = PlayerInput.left (controls[0]), 2 = right (controls[1]) */
    undefined4 field_0x30;
    int steer_amount; /* last steer amount written by ai_drive_* (inferred) */
    int start_random; /* "random start value from the table at 0x603950 (ai_bots_randomise)" */
    union { int cooldown_timer; undefined4 field_0x3c; }; /* ms countdown (ai_bot_set_cooldown_b4 / _78 set it, ai_bot_update decrements by g_tick_count) */
    union { int cooldown_flag; undefined4 field_0x40; }; /* 1 set by ai_bot_set_cooldown_78, cleared by ai_bot_clear_flag_20, read by ai_bot_get_flag_20 */
    int active; /* "non zero when the bot is active (ai_bot_stage_state_update)" */
    int flag_48; /* "set to 1 while the bot is active" */
    undefined4 field_0x4c;
    undefined4 field_0x50;
    undefined4 field_0x54;
    undefined4 field_0x58;
    undefined4 field_0x5c;
    int weapon_held_ms; /* ms the current weapon has been held (ai_bot_stuck_check adds g_tick_count, ai_drive_* reset it to 0) */
    int fire_delay; /* ms countdown before the AI may fire again (3000 / 6000 / 9000 / 0x2ee set by ai_bot_stuck_check) */
    int must_fire; /* set to 1 when weapon_held_ms > 60000: the AI fires regardless of the distance checks */
    int burst_active; /* weapon 9: 1 while the AI holds fire until weapon_effect_get_slot_value_b becomes non zero */
    int state_70; /* "compared with 10 in ai_bot_course_0x21_target_check" */
    char _gap_74[4];
};

typedef struct AiData AiData, *PAiData;

struct AiData {
    short grid[16384]; /* "[128][128] tile index per 8 unit cell, -1 = none" */
    byte tile_pool[32768]; /* "512 tiles of 8 x 8 danger data bytes, 0xFF = empty" */
    int tiles_used;
    byte routes[6192]; /* "4 sets (race, inside, slow, cheat lines) x 3 slots of AiRoute" */
    byte bot_state[80]; /* "runtime state of the 4 bots, saved by accident" */
};

typedef struct Car Car, *PCar;

typedef struct Wheel Wheel, *PWheel;

typedef struct ContactPoint ContactPoint, *PContactPoint;

struct Wheel {
    int state; /* 2 after car_init_dynamics */
    float pos_x; /* wheel position in car space: +-0.601 (front wheels z +1.083, rear wheels z -1.146) */
    float pos_y; /* 0.397 */
    float pos_z;
    float pos_length; /* length of the position vector (used for the inertia sum) */
    float pos_y_copy; /* copy of pos_y */
    char _gap_18[12];
    float handling_scale; /* g_car_handling_scale from the handling table */
    float const_34; /* 34.0 */
    float contact_distance; /* 10.0 reset every car_wheel_contact_update; distance of the wheel to the track triangle (>= 0.02 counts as lifted, < -0.005 as sunk), copied along with contact_triangle when contacts are shifted */
    int in_contact; /* non-zero while the wheel touches a surface; car_wheel_get_field_30, gates the surface colour effects in the per car update */
    char _gap_34[4];
    undefined4 field_0x38;
    int spin_rate;
    char _gap_40[68];
    int contact_triangle; /* -1 after car_spawn */
    int surface_color; /* RGBA colour of the touched track triangle: selects the surface effect (-0xff0001 stop checks, -0xfcfcfd slow, -0xff01 push, -0xe5cc80 boost); live: 0xffc88080 on the start grid */
    char _gap_8c[12];
    float contact_normal[3]; /* normal of the touched triangle, (0 1 0) after car_reset_state; summed over the wheels in contact by car_wheel_contact_update */
    float load; /* weight carried by the wheel: mass / wheels_in_contact * g plus the clamped acceleration term, 0 without contact (car_apply_forces) */
    char _gap_a8[28];
};

struct ContactPoint {
    int triangle; /* collision triangle currently touched (-1 none); 18 per car: 4 wheels, 8 corners, 6 derived body points */
    char _gap_4[12];
    float lever_arm; /* distance from the centre of mass * 3.6 (car_init_dynamics) */
    char _gap_14[44];
};

struct Car {
    uint index;
    uint active;
    int vehicle_id; /* set by car_set_field_8 (menu vehicle choice) */
    undefined4 field_0xc;
    uint controls_disabled;
    uint rwp_object; /* non-zero while the car is simulated by the physics library (car_enter_rwp_mode) */
    int rwp_timer; /* countdown in ms: 600 set by car_enter_rwp_mode, decremented by cars_step, 0 in car_reset_state */
    char _gap_1c[4];
    uint effect_level[6]; /* 0..100 per effect kind 1..6 (car_set_field_set); decays by dt when effect_hold reaches 0 (cars_update_timers); 0x28 / 0x2C / 0x34 scale throttle and force in car_update_drivetrain / car_apply_forces */
    float effect_hold[6]; /* ms the matching effect_level is held before it decays */
    float mass; /* 1000.0 by default */
    float time_step_const; /* 0.001 */
    float inertia_sum;
    float inertia_inverse;
    union { struct {
        char _gap_60[8];
        undefined4 field_0x68;
        char _gap_6c[36];
    }; float wheel_points[12]; };
    float bounds_points[24]; /* 8 box corners; NOTE the 18 point table read by car_get_bounds_point starts at +0x60 (4 wheel points at +0x60, these 8 corners at +0x90, 6 derived points at +0xF0), 12 bytes per point */
    float body_points[18]; /* 6 derived points (edge midpoints / thirds of the box), filled by car_set_bounds */
    float rwp_body_offset[3];
    float velocity_input[3]; /* velocity added by car_add_velocity / set by car_set_field_144 (the trainer writes it to flip or push a car); verified live: float vector, 0 at rest */
    undefined4 field_0x150;
    float speed_scale; /* set by ai_update_speed_scaling (rubber band factor) */
    undefined4 field_0x158;
    float com_offset[3]; /* centre of mass offset (0 by default) */
    struct Wheel wheels[4]; /* 4 wheel records of 0xC4 bytes (front left / right, rear left / right) */
    float handling_params[6]; /* copied from g_car_init_6292c8..dc */
    int gear; /* index into the per gear speed table of car_update_drivetrain, changes by 1 per shift */
    int gear_shift_timer; /* +-3000 set on a shift, counts down to 0 (car_update_drivetrain) */
    float handling_c4; /* 40000.0 default (g_car_init_6292c4) */
    float handling_c0; /* g_car_init_6292c0 */
    float wheel_extent; /* 1.05 * largest wheel distance in metres */
    float bounds_radius; /* 1.05 * largest body point distance */
    char _gap_4a8[4];
    struct ContactPoint contacts[17]; /* first 17 of the 18 records (stride 0x40; the 18th overlaps the matrices at 0x928) */
    char _gap_8ec[60];
    float matrices[32]; /* 2 x RwMatrix (double buffer) */
    uint matrix_current;
    uint matrix_previous;
    float velocity[3];
    float angular_velocity[3]; /* axis * rate: car_update_drivetrain adds torque * inertia_inverse * dt, RwMatrixRotate axis, zeroed by car_reset_state / car_enter_rwp_mode / car_air_control */
    float up[3];
    float forward[3]; /* unit vector (0 0 1) transformed by the current matrix */
    float wheels_in_contact; /* number of wheels with in_contact == 1 as float (4.0 = all, set by car_wheel_contact_update, reset value 4.0) */
    float speed;
    float angular_speed; /* length of angular_velocity */
    union { int contact_count; undefined4 field_0x9ec; }; /* ground contact points found by the world collision test (inferred): 0 = none (cars_update_all skips car_ground_contact_forces), divisor of the averaged forces in car_ground_contact_forces */
    uint mode; /* 2 halves the velocity every step; 0 and 1 are logged as event 6 */
    uint mode2;
    char _gap_9f8[216];
    union { struct {
        undefined4 field_0xad0;
    }; int velocity_history_index; };
    float velocity_history[6]; /* 2 samples x 3 floats of the velocity, the acceleration is the difference of both * dt (car_apply_forces)",read by car_get_field_af4 (vehicle steering / lean animation) */
    char _gap_aec[8];
    float field_af4[3]; /* read by car_get_field_af4 (vehicle steering / lean animation) */
    float field_b00[3]; /* read by car_get_field_b00 */
    int field_b0c; /* car_get_field_b0c */
    int field_b10; /* counter changed by car_add_field_b10 */
    float force_accum[3]; /* 0xb14: extra force of this step, zeroed at the start of car_update, drive force (car_update_drivetrain) and airborne gravity (car_apply_forces) are added, integrate adds it to the velocity */
    int driven; /* 0xb20: set to 1 by car_update (input, speed > 64), car_update_drivetrain, car_ground_contact_forces and car_apply_forces; cleared at the start of car_update; car_update_drivetrain only zeroes a slow car (speed < 16) while it is 0 */
    char _gap_b24[208];
    int turbo_charge; /* 0..3000: +2 dt while accelerate > 160, -dt every update (race_phase_5_update_sub_47b870) */
    int turbo_state; /* 1 / 2 set when the charge is used up (race_phase_5_update_sub_47b800) */
    int contact_key[32]; /* 32 slot table of the wheel contact cache: value compared with triangle +0x34 by car_wheel_contact_find_index (0x477130), written by car_set_wheel_contact */
    int contact_valid[32]; /* flag per slot of contact_key (0 = slot unused); +0xBFC..+0xCFC */
    union { int handbrake_held; undefined4 field_0xcfc; }; /* 1 while input[4] and input[5] are both pressed (car_integrate_step); car_ground_contact_forces then keeps at least 500 contact force and skips the extra force */
    uint is_bot; /* 0 for the human player, 1 for AI cars (verified in a live race; MashedTrainer PLAYER_BOT) */
};

typedef struct ControllerCfg ControllerCfg, *PControllerCfg;

struct ControllerCfg {
    uint version; /* 6 */
    char name[260];
    uint bindings[13]; /* accelerate brake fire drop menu_select menu_back pause menu_next insult left right up down */
    uint device; /* 0 none 1 gamepad 2 keyboard */
    uint joystick;
    char _gap_144[188];
};

typedef struct CourseCounts CourseCounts, *PCourseCounts;

struct CourseCounts {
    int course_id; /* set by the game before COURSE.LUA runs (the Course_Id Lua function is a no-op) */
    int object_bsp_count;
    int clump_count;
    int mts_count;
    int spline_count;
    int general_anim_count;
    int ai_bsp_count; /* "AI_Bsp_Filename sets it to 1" */
    int collision_bsp_count; /* "Collision_Bsp_Filename sets it to 1" */
    int hoppy_bsp_count;
    char _gap_24[8];
    byte clump_mts_pairs[64]; /* "16 x {u8 clump, u8 mts, u16 0xFFFF}" */
    int clump_mts_pair_count;
    char _gap_70[96];
    int clump_anim_count;
    uint clump_exclude_from_world[128]; /* Clump_Exclude_From_World flags */
    uint clump_fade[640]; /* "ClumpFade records, 128 x 5 dwords: enable, a, b, ?, float" */
    int powerup_file_count;
    float ambient_rgba[4]; /* Ambient_RGB (alpha 1.0) */
    byte fog_color[4]; /* Setup_Fog r g b (a = 255) */
    float fog_range[2]; /* Setup_Fog first two arguments */
    byte fog_color_modified[4]; /* Modify_Fog r g b */
    float fog_modify_a[2]; /* Modify_Fog */
    float fog_modify_b;
    float vehicle_shininess_range[2];
    float shine_scale;
    float shadow_scale;
    int uva_count;
    int copter_count;
    char _gap_d1c[4];
};

typedef struct CourseEntry CourseEntry, *PCourseEntry;

struct CourseEntry {
    char name[16]; /* "training superg arctic rouabout sands sandsnow warzone forest egypt storm storm_R dump highway neustein city NT1 NT1_S AngelS Dem DemR HighwayN SuperG_N KEYS KEYS01 Egypt_R neust_R forest_R Tierra_N" */
    uint course_id; /* "also used as course id + 1000 when mirrored" */
    uint callbacks[13]; /* "cb0 = post load (world), cb12 (+0x44) = world setup before creation; 0x50B5E0 = nothing to do" */
};

typedef struct CourseFiles CourseFiles, *PCourseFiles;

struct CourseFiles {
    char texture_dictionary[64]; /* Texture_Dictionary_Filename */
    char sky_files[256]; /* "Sky_Filename, 4 x 0x40" */
    char world_bsp[64]; /* World_Bsp_Filename */
    char collision_bsp[64]; /* Collision_Bsp_Filename */
    char _gap_1c0[192];
    char hoppy_bsp[64]; /* Hoppy_Bsp_Filename */
    char _gap_2c0[192];
    char object_bsp_files[1024]; /* "Object_Bsp_Filename, 16 x 0x40" */
    char ai_bsp[64]; /* AI_Bsp_Filename */
    char _gap_7c0[192];
    char ai_data[64]; /* AI_Data_Filename */
    char _gap_8c0[192];
    char clump_files[8192]; /* "Clump_Filename, 128 x 0x40" */
    char occluder[64]; /* Occluder_Filename */
    char mts_files[1088]; /* "Mts_Filename, 17 x 0x40" */
    char spline_files[1024]; /* "Spline_Filename, 16 x 0x40" */
    char camera_anim[64]; /* Camera_Anim_Filename */
    char general_anim_files[512]; /* "General_Anim_Filename, 8 x 0x40" */
    char powerup_files[512]; /* "Powerup_Filename, 8 x 0x40" */
    char lights[64]; /* Lights_Filename */
    char texture_a_name[32]; /* looked up in the course texture dictionary */
    char texture_b_name[32];
    char uva_files[512]; /* "UVA_Filename, 8 x 0x40" */
    char copter_files[512]; /* "SetCopter, 8 x 0x40" */
    int copter_ids[8]; /* SetCopter second argument */
};

typedef struct CourseLap CourseLap, *PCourseLap;

struct CourseLap {
    char _gap_0[49624];
    undefined4 field_0xc1d8;
    char _gap_c1dc[38884];
    int lap_lines[512]; /* "Lap_Line: segment number; -1 = Lap_Line_End, 0x100 = Lap_Line_Change" */
    char _gap_161c0[128];
    int ai_polygon_count; /* set by ai_polygons_build (AI polygon records of 0x4C bytes start at +0xC1CC) */
    int lap_variations;
    int safe_start_count;
    int split_sectors[16]; /* "Split_Sector(index, value)" */
    char _gap_1628c[116];
};

typedef struct CourseWorld CourseWorld, *PCourseWorld;

struct CourseWorld {
    byte mts_matrices[49152]; /* "16 matrix lists of 0x30 RwMatrix (0xC00 bytes each) loaded from Mts_Filename" */
    int mts_matrix_counts[16];
    uint object_bsp_worlds[16]; /* Object_Bsp_Filename worlds */
    uint ai_worlds[4]; /* AI_Bsp_Filename world(s) */
    uint collision_bsps[4]; /* "invisible collision worlds (entries 1..3 default to entry 0)" */
    uint hoppy_bsps[4];
    uint splines[16]; /* Spline_Filename resources */
    uint sky_clumps[4]; /* Sky_Filename clumps */
    char _gap_c100[16];
    uint clumps[128]; /* Clump_Filename clumps */
    uint sky_atomics[4];
    char _gap_c320[528];
    uint object_bsp_loaded[16];
    char _gap_c570[64];
    uint clump_loaded[128];
    char _gap_c7b0[512];
    uint general_anims[8];
    void *texture_dictionary;
    void *world; /* RpWorld of World_Bsp_Filename */
    uint occluder_clump;
    uint lights_clump;
    uint ambient_light;
    char *directional_light;
    uint pvs_enabled;
    uint pvs_data;
    uint camera_anim;
    float fog_a[3];
    float fog_b[3];
    char _gap_ca0c[4];
    uint texture_a;
    uint texture_b;
    uint uva_dicts[8];
    int flag_outdoor; /* "0 for event types 0 6 8 0xC 0xE 0xF 0x10 0x19 0x1A 0x24, else 1" */
    char _gap_ca3c[20];
};

typedef struct ExplosionEntry ExplosionEntry, *PExplosionEntry;

struct ExplosionEntry {
    float damage; /* multiplied by the distance falloff */
    float push_horizontal;
    float push_vertical;
    int frames; /* total number of frames the explosion stays active */
    int frames_left; /* decremented each frame while a type 0x19 trigger sphere is registered */
    float position_radius[4]; /* x y z and the radius */
};

typedef struct GameState GameState, *PGameState;

struct GameState {
    uint progress[270]; /* [15 rows][18 columns] 0 locked 1 open 2 done */
    int records[75]; /* [15][5] -1 empty */
    char _gap_564[600];
    uint best_minutes[45];
    uint best_seconds[45];
    float best_fraction[45];
    byte enabled[270]; /* [15][18] 1 = selectable */
    char _gap_ae6[26];
    uint option_sliders[5];
    char _gap_b14[24];
    uint option_flags[5];
    char _gap_b40[20];
    uint mirror_mode; /* course id + 1000 */
    byte player_bytes[12];
};

typedef struct MachinegunState MachinegunState, *PMachinegunState;

struct MachinegunState {
    char *slot; /* owning WeaponSlot */
    float cooldown; /* 0.0585 s after a shot */
    void *clump; /* GatlingGun.dff */
    uint active;
    int ammo; /* 50 on equip (HUD gauge = ammo * constant) */
    uint fired;
    float spin;
    undefined4 field_0x1c;
    undefined4 field_0x20;
    undefined4 field_0x24;
    undefined4 field_0x28;
    char _gap_2c[24];
    undefined4 field_0x44;
};

typedef struct MenuItemAnim MenuItemAnim, *PMenuItemAnim;

struct MenuItemAnim {
    int item_id; /* "menu item id (0xFF...0000 style codes, -0x1000000 = frame slot, 0 = unused)" */
    int state; /* "0 appearing, 1 idle, 2 disappearing, 0x1000 finished" */
    char _gap_8[4];
    uint colour; /* "RGBA bytes (set to 0xFFFFFFFF in menu_page_enter)" */
    int anim_counter; /* "animation counter (0x1FF = finished, 400 limit while appearing)" */
    float scale; /* "text scale (0.6 default)" */
    float x; /* "x position (64.0 default)" */
    float y; /* "y position (48.0 default; menu_item_y_position + row * 0x1e)" */
    char _gap_20[4];
    int param; /* "item parameter (0 default)" */
    int ref_a; /* "slot 0 only: result of menu_item_list_find_index_a for the first lookup" */
    int ref_b; /* "slot 0 only: result of the second lookup" */
    char _gap_30[4];
};

typedef struct MenuRecord MenuRecord, *PMenuRecord;

struct MenuRecord {
    char *item_list; /* "pointer to the item list of the page (entries end with a 0xFFxxxxxx terminator)" */
    int page_id; /* "frontend page id (0x13 options, 0x21 race menu ...)" */
    int selected; /* "selected entry, -1 none" */
    int entry_enabled[12]; /* "per entry enabled flag, set by menu_page_update_entry_states" */
    int entry_count; /* "number of entries" */
};

typedef struct PlayerInput PlayerInput, *PPlayerInput;

struct PlayerInput {
    byte left; /* "strength 0..255 of the left direction (mirrored mode swaps left and right)" */
    byte right;
    byte up;
    byte down;
    byte accelerate;
    byte brake;
    undefined1 field_0x6;
    byte fire; /* "fire power-up (menu: next stat)" */
    byte drop; /* drop power-up */
    byte pause;
    byte menu_select;
    byte menu_back;
    byte direction_flags[4]; /* "0xFF while left / right / up / down is held" */
    char _gap_10[1];
    byte menu_next;
    char _gap_12[1];
    byte fire_copy;
    float analog_x; /* "gamepad stick (negated y in +0x18); for the keyboard derived from the direction bytes" */
    float analog_y;
    char _gap_1c[4];
    uint rumble_request; /* set to 1 together with the three values below (player_rumble / input_player_rumble_set); the input backend consumes it */
    union { struct {
        undefined4 field_0x24;
    }; uint rumble_enabled; };
    char _gap_28[4];
    uint rumble_duration; /* third argument of player_rumble (2, 10, 0x1e) */
    uint rumble_motor; /* second argument of player_rumble (1..3) */
    uint rumble_strength; /* fourth argument of player_rumble (0x80 / 0xff) */
    char _gap_38[7];
    byte insult;
    char _gap_40[12];
};

typedef struct PlayerSlot PlayerSlot, *PPlayerSlot;

struct PlayerSlot {
    int controller; /* "index of the controller / PlayerInput record, -1 = no player in this slot" */
    int state; /* "-1 / 0 / 1 / 2: set to -1 for unused slots when an event starts (0 = human?)" */
    int team; /* "team / colour index (1 << team is the mask used by the HUD panels)" */
    int extra;
};

typedef struct PlayerStatRecord PlayerStatRecord, *PPlayerStatRecord;

struct PlayerStatRecord {
    int place; /* "finishing place (0 based in the race table, 1 based in the result table)" */
    char _gap_4[4];
    int turbo_skill_score; /* "'Turbo Skills': +200 per turbo skill (spin or long reverse) in player_stats_update" */
    int turbo_skill_kind; /* "last skill: 1 = steering spin, 2 = reversing" */
    int turbo_skill_timer; /* "3000 ticks display timer of the last skill" */
    char _gap_14[136];
    int contact_ticks; /* "counts ticks in contact with another car" */
    int aggression; /* "'Aggression': rams where the other car is a human player (event 0x3FF03)" */
    int kills; /* "'Kills': credited to the last hitter when a car dies within 4500 ticks of the hit" */
    int unforced_errors; /* "'Unforced Errors': deaths without a recent hitter" */
    int death_handled; /* "set once the death of the car has been scored" */
    char _gap_b0[64];
    int wall_hits; /* "'Cleanest Drive': number of wall hits (event 3), lower is better (default 100000 in the ranking)" */
    int crowns; /* "'Crowns' (kind 0; ai_stats_log prints it as number of crowns); no writer in the application code, 0 in event type 4" */
    int pants; /* "number of pants (kind 1, the NOT USED text); never written" */
    int turbo_used; /* "'Turbo Skills' (kind 2): turbo charge consumed, added by player_stat_add_turbo_used when the turbo charge is spent" */
    float turn_start_angle; /* "heading when a steering spin started" */
    int turn_state; /* "0 idle, 1 turning left, 2 turning right" */
    int reverse_ticks; /* "ticks spent braking while moving backwards (skill at 9001 ticks)" */
    int last_collision_time[4]; /* "g_race_clock_b of the last car to car collision with each other player" */
    float collision_speed[4]; /* "relative speed of the last collision with each other player" */
    int last_hit_by; /* "1000 + id of the car that hit this car last" */
    int last_hit_time; /* "g_race_clock_b of that hit" */
    int other_is_human; /* "set by the collision handler when the other car is a human player" */
};

typedef struct Powerup Powerup, *PPowerup;

struct Powerup {
    char *clump; /* RpClump of IconCube.dff */
    char _gap_4[20];
    float respawn_time; /* seconds (default 120.0) */
    float respawn_timer;
    uint state; /* 0 unused 1 available 2 taken (respawning) 3 disabled */
    int type;
    uint special; /* set for the random (0x15) slot */
    float position[3];
    float spawn_position[3];
    char _gap_44[12];
};

typedef struct RwpObjectDesc RwpObjectDesc, *PRwpObjectDesc;

struct RwpObjectDesc {
    int shape; /* "1 box, 2 cylinder, 3 sphere, 4 capsule (0 unused)" */
    char label[64]; /* RWP_Object label */
    char dff[64]; /* RWP_Object clump file */
    char mts[64]; /* RWP_Object matrix list file */
    float box_dimensions[3]; /* RWP_Object_Box */
    float cylinder[2]; /* RWP_Object_Cylinder */
    float capsule[2]; /* RWP_Object_Capsule */
    float sphere_radius; /* RWP_Object_Sphere */
    float property_d; /* RWP_Object_Properties 4th value */
    float centre_of_mass_offset[3]; /* RWP_Object_CentreOfMassOffset */
    float property_b; /* RWP_Object_Properties 2nd value */
    float property_c; /* RWP_Object_Properties 3rd value */
    float property_a; /* RWP_Object_Properties 1st value */
    int material; /* RWP_Object_Properties last value (e.g. RUBBER) */
    int base_label; /* RWP_Object_BaseLabel */
    float shatter_velocity; /* RWP_Object_ShatterVel (default 8.0) */
    undefined4 field_0x10c;
};

/* One 0x48 byte particle record: returned by fx_trail_particle_spawn (already zeroed), or filled in by the caller and handed to
   fx_trail_particle_spawn_interpolated. */
typedef struct FxTrailParticle {
    RwV3d pos;                 /* 0x00 */
    unsigned char colour[4];   /* 0x0c r, g, b, a */
    float size_start;          /* 0x10 */
    float size_end;            /* 0x14 */
    RwV3d velocity;            /* 0x18 */
    RwV3d accel;               /* 0x24 */
    float life_scale;          /* 0x30 */
    float field_34;
    float scale;               /* 0x38 (inferred) */
    float field_3c;
    unsigned int flags;        /* 0x40 */
    unsigned char flag_44;     /* 0x44 */
    unsigned char kind;        /* 0x45 */
    unsigned char pad[2];
} FxTrailParticle;

/* 3D audio listener state (inferred from its accessors gap_5b9a40..gap_5b9cc0 in the sound library: position / velocity / orientation / distance, doppler and
   rolloff factors, i.e. the DirectSound3D listener parameters).  Each setter ORs its LISTENER3D_DIRTY_* bit into `dirty`. */
typedef struct Listener3D {
    char unknown_00[0x28];
    RwV3d position;        /* 0x28 */
    RwV3d velocity;        /* 0x34 */
    RwV3d front;           /* 0x40 */
    RwV3d top;             /* 0x4c */
    RwV3d side;            /* 0x58: cross product of front and top, recomputed by the orientation setters */
    float distance_factor; /* 0x64 */
    float doppler_factor;  /* 0x68 */
    float rolloff_factor;  /* 0x6c */
    uint dirty;            /* 0x70 */
} Listener3D;
enum {
    LISTENER3D_DIRTY_POSITION = 0x01, LISTENER3D_DIRTY_VELOCITY = 0x02, LISTENER3D_DIRTY_TOP = 0x04, LISTENER3D_DIRTY_FRONT = 0x08,
    LISTENER3D_DIRTY_DISTANCE = 0x10, LISTENER3D_DIRTY_DOPPLER = 0x20, LISTENER3D_DIRTY_ROLLOFF = 0x40
};

typedef struct Trigger Trigger, *PTrigger;

struct Trigger {
    float position_radius[4]; /* x y z radius */
    uint type; /* 7 powerup 0xB rocket 0x19 explosion */
    uint owner; /* object the callback works on */
    uint flags;
    uint callback; /* (trigger, other) called for every overlap */
    ushort index;
    char _gap_22[2];
    uint cells[16]; /* grid cells the trigger was entered into */
    uint cell_count;
    undefined4 field_0x68;
    char _gap_6c[32];
};

typedef struct VehicleRecord VehicleRecord, *PVehicleRecord;

struct VehicleRecord {
    int part_frames[140]; /* "part frame handles indexed by part id (vehicle_part_frame(v, part) = [v*0xab + part])" */
    float matrix6[6]; /* "6 floats copied by vehicle_get_matrix" */
    float local_sphere[4]; /* x y z radius copied from the clump bounding sphere (vehicle_strip_clump_to_body_parts), read by vehicle_get_position_148 */
    float world_sphere[4]; /* x y z radius: the centre is the frame position written by vehicle_setup_references, used by vehicle_register_trigger_sphere, hud_marker_get_distance and the shadow camera */
    int object; /* "vehicle object (field +0x80 read by vehicle_get_object_field_80)" */
    char *frame; /* "vehicle frame / clump handle (vehicle_get_frame_16c, matrix at +0x4/+0x10)" */
    int clump_part_count; /* count returned by the clump part scan in vehicle_load (loop bound) */
    float half_scale; /* "vehicle_set_scale: scale * 0.5, sets flag 2" */
    uint envmap_texture; /* RwTexture found by name for RpMatFXMaterialSetEnvMapTexture */
    uint envmap_frame; /* RwFrame handle (g_game_659d60[v]) for RpMatFXMaterialSetEnvMapFrame */
    float envmap_angle; /* environment map rotation, advanced by vehicle_setup_references from the car velocity */
    float size_factor; /* "vehicle_get_float_184" */
    float light_factor; /* 1 - distance of the downward ground probe (1.0 while the course has no probe data); scales the headlight / shadow strength */
    int field_28c; /* "vehicle_get_field_18c" */
    int index; /* slot index 0..3 set by vehicle_load; the car index for car_get_* and (g_tick_number & 3) selects the frame of the ground probe */
    uint flags; /* "bit 1 scale set, 8 active, 0x10/0x20 front/back flags, 0x400 flag_400" */
    int field_298; /* "vehicle_set_field_198 (0 stored as 1)" */
    union { char *render_camera; undefined4 field_0x29c; }; /* RwCamera set by vehicle_render, read by hud_marker_get_distance */
    uint shadow_camera; /* RwCamera of the shadow pass (RwCameraClear in vehicle_shadow_camera_render_setup) */
    uint shadow_frame; /* frame of shadow_camera */
    uint shadow_texture; /* texture g_game_659d74 used for the shadow */
};

typedef struct VideoCfg VideoCfg, *PVideoCfg;

struct VideoCfg {
    uint version; /* 5 on save */
    char subsystem[80]; /* RwSubSystemInfo name */
    char _gap_54[176];
    char mode_name[64];
    char _gap_144[64];
    uint subsystem_index;
    uint mode_index;
    uint width;
    uint height;
    uint depth;
    uint fullscreen;
    uint no_multitexture; /* checkbox checked when 0 */
    uint no_dynamic_shadows; /* checkbox checked when 0 */
    uint trilinear; /* checkbox checked when not 0 */
    uint no_env_effects; /* checkbox checked when 0 */
    char _gap_1ac[84];
};

typedef struct WeaponSlot WeaponSlot, *PWeaponSlot;

struct WeaponSlot {
    char _gap_0[32];
    float aim_vector[3]; /* passed to the fire callbacks (shot direction / velocity) */
    char _gap_2c[4];
    float muzzle_position[3]; /* sound / effect position */
    char _gap_3c[84];
    undefined4 field_0x90;
    undefined4 field_0x94;
    undefined4 field_0x98;
    char _gap_9c[12];
    char *descriptor; /* pointer into weapon_descriptors (0 = no weapon) */
    char *state; /* pointer to the weapon's per player state */
    uint player; /* player index */
};

typedef struct VoiceSlot VoiceSlot;

typedef struct FxSlot FxSlot;

typedef struct EventLogArgs EventLogArgs;

typedef struct TrailSlot TrailSlot;

typedef struct EventLogData EventLogData;

typedef struct SpringNode SpringNode;

struct VoiceSlot {
    uint voice; /* pointer to a record (+off accessed) */
    float volume; /* float by usage */
};

struct FxSlot {
    void *clump;
    float life;
    uint phase;
    uint active;
    uint fx_system;
    float pos_x;
    float pos_y;
    float pos_z;
};

struct EventLogArgs {
    uint arg0;
    uint arg1;
    uint arg2;
    uint w0c;
    uint w10;
    uint w14;
    uint w18;
    uint w1c;
};

struct TrailSlot {
    uint w00; /* pointer to a record (+off accessed) */
    uint active;
    float w08; /* float by usage */
    float w0c; /* float by usage */
    float w10; /* float by usage */
    float w14; /* float by usage */
    float w18; /* float by usage */
    float w1c; /* float by usage */
    uint w20;
    uint object_a;
    uint object_b;
};

struct EventLogData {
    float w00; /* float by usage */
    float w04; /* float by usage */
    uint w08;
    uint w0c;
    uint w10;
    uint w14;
    uint w18;
    char _gap_1c[8];
    uint w24;
    uint w28;
    uint w2c;
    uint w30;
    uint w34;
    uint w38;
    char _gap_3c[4];
};

struct SpringNode {
    float w00; /* float by usage */
    float w04; /* float by usage */
    float w08; /* float by usage */
    float w0c; /* float by usage */
    float w10; /* float by usage */
    float w14; /* float by usage */
    float w18; /* float by usage */
    float w1c; /* float by usage */
    uint w20;
    uint w24;
    uint w28;
    float w2c; /* float by usage */
    float w30; /* float by usage */
    float w34; /* float by usage */
    uint w38;
    float w3c; /* float by usage */
    float w40; /* float by usage */
};

#endif
