/* RenderWare Audio (RWA) types for the MFL.exe lib/rwa sources: the basic RenderWare types, the 3D vector, the UUID and
 * the unique ID.  RwLLLink, RwV3d, RwaUUID and the basic types are taken from the Piglet's Big Game decompilation
 * (https://github.com/tgsm/pbg, include/rwsdk and include/rwa, CC0 1.0); many thanks to tgsm and the pbg contributors.
 * The sources that include this header come from codfh-win32 by L2KSolkov (each file header credits it).  RwaUniqueID has the layout of pbg's (a
 * UUID pointer, a name pointer and flags) without its unions, because the sources name the members directly. */
#ifndef RWA_H
#define RWA_H

#include <stddef.h>

typedef unsigned char RwUInt8;
typedef char RwInt8;
typedef unsigned short RwUInt16;
typedef short RwInt16;
typedef unsigned int RwUInt32;
typedef int RwInt32;
typedef float RwReal;
typedef int RwBool;
typedef char RwChar;

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef NULL
#define NULL ((void *)0)
#endif

/* compile time layout check: the array size is -1 when the condition fails */
#define rwaASSERT(_name, _cond) typedef char rwaAssert_##_name[(_cond) ? 1 : -1]

typedef struct RwLLLink {
    struct RwLLLink* next;
    struct RwLLLink* prev;
} RwLLLink;

typedef struct RwV3d {
    RwReal x;
    RwReal y;
    RwReal z;
} RwV3d;
typedef RwV3d RwaV3d;

typedef struct RwaUUID {
    RwUInt32 time_low;
    RwUInt16 time_mid;
    RwUInt16 time_hi_and_version;
    RwUInt8 node[8];
} RwaUUID;

typedef struct RwaUniqueID RwaUniqueID;
struct RwaUniqueID
{
    const RwaUUID *uuid;
    const RwChar *name;
    RwUInt32 flags;
};
rwaASSERT(RwaUniqueID__sizeof, sizeof(RwaUniqueID) == 0xC);

/* memory hints passed to the RenderWare allocator: the duration and the RWA allocation ids */
#define rwaMEMHINTDUR_FUNCTION 0x00010000
#define rwaMEMHINTDUR_EVENT    0x00030000
#define rwaMEMHINTDUR_GLOBAL   0x00040000
#define rwaID_800              0x800
#define rwaID_803              0x803
#define rwaID_804              0x804
#define rwaID_806              0x806
#define rwaID_809              0x809
#define rwaID_80A              0x80A
#define rwaID_80B              0x80B
#define rwaID_80D              0x80D
#define rwaID_80E              0x80E
#define rwaID_80F              0x80F
#define rwaID_810              0x810

typedef struct RwaObjHandle RwaObjHandle;

#endif /* RWA_H */
