/* The native-data stream plugin callbacks of baworobj (RenderWare 3.7 names, recomp/aliases.txt): writeGeometryNative (MFL.exe 004f4320),
 * readGeometryNative (004f4340), sizeGeometryNative (004f4360), writeWorldSectorNative (004f4370), readWorldSectorNative (004f4390),
 * sizeWorldSectorNative (004f43b0).  Each forwards to the D3D9 driver's _rpGeometryNative* / _rpWorldSectorNative* function.  Written
 * from the MFL.exe disassembly (lane P W8); RenderWare SDK types from rw37_slim.h.  Compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"

extern RwStream *_rpGeometryNativeWrite(RwStream *stream, const void *geometry);
extern RwStream *_rpGeometryNativeRead(RwStream *stream, void *geometry);
extern RwInt32 _rpGeometryNativeSize(const void *geometry);
extern RwStream *_rpWorldSectorNativeWrite(RwStream *stream, const void *sector);
extern RwStream *_rpWorldSectorNativeRead(RwStream *stream, void *sector);
extern RwInt32 _rpWorldSectorNativeSize(const void *sector);

/* TIER2: unvalidated authority=diffemu */
RwStream * writeGeometryNative(RwStream *stream, RwInt32 binaryLength __RWUNUSED__, const void *object,
                               RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__)
{
    return (_rpGeometryNativeWrite(stream, object));
}

/* TIER2: unvalidated authority=diffemu */
RwStream * readGeometryNative(RwStream *stream, RwInt32 binaryLength __RWUNUSED__, void *object,
                              RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__)
{
    return (_rpGeometryNativeRead(stream, object) ? stream : (RwStream *)NULL);
}

/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwInt32 sizeGeometryNative(const void *object, RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__)
{
    return (_rpGeometryNativeSize(object));
}

/* TIER2: unvalidated authority=diffemu */
RwStream * writeWorldSectorNative(RwStream *stream, RwInt32 binaryLength __RWUNUSED__, const void *object,
                                  RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__)
{
    return (_rpWorldSectorNativeWrite(stream, object));
}

/* TIER2: unvalidated authority=diffemu */
RwStream * readWorldSectorNative(RwStream *stream, RwInt32 binaryLength __RWUNUSED__, void *object,
                                 RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__)
{
    return (_rpWorldSectorNativeRead(stream, object) ? stream : (RwStream *)NULL);
}

/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwInt32 sizeWorldSectorNative(const void *object, RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__)
{
    return (_rpWorldSectorNativeSize(object));
}
