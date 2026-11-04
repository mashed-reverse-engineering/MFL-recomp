/* RenderWare Audio headers from codfh-win32 by L2KSolkov (https://github.com/L2KSolkov/codfh-win32, src/rwa/rwa.h), used with credit.
 * P7c: the codfh revision of 2026-11-04, kept beside the older rwa.h / rwaint.h that the installed tier 1 files use. */
/*
 * codfh-match: RenderWare Audio, rebuilt from the image alone. No source,
 * headers or symbols for it exist; names are the IDA database's, and types
 * and fields are named by their offsets until something names them.
 */
#ifndef RWA_H
#define RWA_H

typedef unsigned char   RwUInt8;
typedef signed char     RwInt8;
typedef unsigned short  RwUInt16;
typedef short           RwInt16;
typedef unsigned int    RwUInt32;
typedef int             RwInt32;
typedef float           RwReal;
typedef int             RwBool;
typedef char            RwChar;
typedef unsigned __int64 RwUInt64;

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef NULL
#define NULL ((void *)0)
#endif

typedef struct RwaV3d RwaV3d;
struct RwaV3d
{
    RwReal  x, y, z;
};

/* A 16-byte identifier (parameter and type ids are pointers to these). */
typedef struct RwaUUID RwaUUID;
struct RwaUUID
{
    RwUInt32        time_low;
    RwUInt16        time_mid;
    RwUInt16        time_hi_and_version;
    RwUInt8         node[8];
};

/* A unique ID: a UUID and a name, each static or copied (flags). */
#define rwaUNIQUEIDFLAGS_COPIEDUUID 0x0001
#define rwaUNIQUEIDFLAGS_COPIEDNAME 0x0002

typedef struct RwaUniqueID RwaUniqueID;
struct RwaUniqueID
{
    const RwaUUID  *uuid;
    const RwChar   *name;
    RwUInt32        flags;
};

/* RenderWare memory hints: the event duration (rwMEMHINTDUR_EVENT) with
 * the audio library's allocation ids, named by value. */
#define rwaMEMHINTDUR_FUNCTION  0x00010000
#define rwaMEMHINTDUR_EVENT     0x00030000
#define rwaMEMHINTDUR_GLOBAL    0x00040000
#define rwaID_800               0x800
#define rwaID_802               0x802
#define rwaID_803               0x803
#define rwaID_804               0x804
#define rwaID_805               0x805
#define rwaID_806               0x806
#define rwaID_807               0x807
#define rwaID_808               0x808
#define rwaID_80A               0x80A
#define rwaID_809               0x809
#define rwaID_80B               0x80B
#define rwaID_80E               0x80E
#define rwaID_801               0x801
#define rwaID_80C               0x80C
#define rwaID_80D               0x80D
#define rwaID_80F               0x80F
#define rwaID_810               0x810

/* An object handle in the audio graph. */
typedef struct RwaObjHandle RwaObjHandle;

#endif /* RWA_H */
