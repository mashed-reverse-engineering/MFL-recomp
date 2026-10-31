#include "mfl_tier2.h"
/* The game's own thin layer over RenderWare: load a chunk from a file / memory stream, and "for all" iterators that
   collect the visited objects into an array (or only count them).  All of it is game code that sits between the RenderWare
   libraries, 0x4c3b70..0x4c42b0. */
typedef struct RpLight *(*RpLightCallBack)(struct RpLight *, void *);
typedef struct RpClump *(*RpClumpCallBack)(struct RpClump *, void *);
typedef struct RwTexture *(*RwTextureCallBack)(struct RwTexture *, void *);
#define RpGeometry_matList_numMaterials(p) (*(RwInt32 *)((char *)(p) + 0x24))
/*004dbe70*/ extern RwUInt32 RwStreamRead(struct RwStream *stream, void *buffer, RwUInt32 size);
/*004dc2a0*/ extern RwBool __cdecl RwStreamClose(struct RwStream *stream, void *data);
/*004dc370*/ extern struct RwStream *RwStreamOpen(enum RwStreamType type, enum RwStreamAccessType access, void *data);
/*004dbfc0*/ extern struct RwStream *RwStreamWrite(struct RwStream *stream, void *buffer, RwUInt32 size);
/*004dc720*/ extern RwBool RwStreamFindChunk(struct RwStream *stream, RwUInt32 type, RwUInt32 *length, RwUInt32 *version);
/*004f7560*/ extern struct RpClump *RpClumpStreamRead(struct RwStream *stream);
/*004d0f70*/ extern struct RwFrame *RwFrameUpdateObjects(struct RwFrame *frame);
/*004f9bd0*/ extern struct RpWorld *__cdecl RpWorldStreamRead(struct RwStream *stream);
/*00555890*/ extern struct RpSpline *__cdecl RpSplineStreamRead(struct RwStream *stream);
/*004df580*/ extern struct RwTexDictionary *__cdecl RwTexDictionaryStreamRead(struct RwStream *stream);
/*0055ff00*/ extern struct RwTexDictionary *__cdecl RtPITexDictionaryStreamRead(struct RwStream *stream);
/*0053e040*/ extern struct RtAnimAnimation *__cdecl RtAnimAnimationStreamRead(struct RwStream *stream);
/*004d1420*/ extern struct RwFrame *RwFrameForAllChildren(struct RwFrame *frame, RwFrameCallBack callback, void *data);
/*004f6810*/ extern struct RpClump *RpClumpForAllAtomics(struct RpClump *clump, RpAtomicCallBack callback, void *data);
/*004f68a0*/ extern struct RpClump *RpClumpForAllLights(struct RpClump *clump, RpLightCallBack callback, void *data);
/*004f8b60*/ extern struct RpGeometry *RpGeometryForAllMaterials(struct RpGeometry *geometry, RpMaterialCallBack callback, void *data);
/*004f5e10*/ extern struct RpWorld *RpWorldForAllWorldSectors(struct RpWorld *world, RpWorldSectorCallBack callback, void *data);
/*004f5dd0*/ extern struct RpWorld *RpWorldForAllMaterials(struct RpWorld *world, RpMaterialCallBack callback, void *data);
/*004f5d90*/ extern struct RpWorld *RpWorldForAllClumps(struct RpWorld *world, RpClumpCallBack callback, void *data);
/*004d5b50*/ extern struct RwTexDictionary *RwTexDictionaryForAllTextures(struct RwTexDictionary *dict, RwTextureCallBack callback, void *data);

#define RW_CHUNK_WORLD 0x0b
#define RW_CHUNK_SPLINE 0x0c
#define RW_CHUNK_CLUMP 0x10
#define RW_CHUNK_TEXDICT 0x16
#define RW_CHUNK_ANIM 0x1b
#define RW_CHUNK_TEXDICT_PI 0x23        /* platform independent texture dictionary */

/* A "for all" callback target: the visited objects go to array[*count] when there is an array; they are always counted. */
struct MflRwItemCollector {
  void **array;
  int *count;
};
/* Memory stream data: start address and length. */
struct MflRwMemoryRange {
  void *start;
  RwUInt32 length;
};
/* already C elsewhere (cluster / tier 2 members): the callbacks handed to the "for all" functions and the first-geometry lookup */
/*004c4040*/ extern void *rw_callback_store_first(void *item, void **first);
/*004c4200*/ extern void *rw_callback_collect(void *item, struct MflRwItemCollector *collector);
/*004c4020*/ extern struct RpGeometry *clump_first_geometry(struct RpClump *clump);

/* Reads `size` bytes of the file `name` into `buffer`; returns the number of bytes read (0 when the file does not open). */
/* TIER2: validated 2026-10-31 authority=diffemu */
RwUInt32 stream_read_file(void *name, void *buffer, RwUInt32 size)
{
  RwUInt32 bytesRead = 0;
  struct RwStream *stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, name);

  if (stream != 0) {
    bytesRead = RwStreamRead(stream, buffer, size);
    RwStreamClose(stream, 0);
  }
  return bytesRead;
}

/* Writes `size` bytes of `buffer` to the file `name`; returns the stream (non-NULL = written). */
/* TIER2: validated 2026-10-31 authority=diffemu */
struct RwStream *stream_write_file(void *name, void *buffer, RwUInt32 size)
{
  struct RwStream *result = 0;
  struct RwStream *stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMWRITE, name);

  if (stream != 0) {
    result = RwStreamWrite(stream, buffer, size);
    RwStreamClose(result, 0);
  }
  return result;
}

/* Loads the first clump chunk of the file `name`. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
struct RpClump *clump_read_from_file(void *name, RwUInt32 *length, RwUInt32 *version)
{
  struct RpClump *clump = 0;
  struct RwStream *stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, name);

  if (stream == 0) {
    return 0;
  }
  if (RwStreamFindChunk(stream, RW_CHUNK_CLUMP, length, version)) {
    clump = RpClumpStreamRead(stream);
    if (clump != 0) {
      RwFrameUpdateObjects(RpClump_object_parent(clump));
    }
  }
  RwStreamClose(stream, 0);
  return clump;
}

/* Loads the first world chunk of the file `name`. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
struct RpWorld *world_read_from_file(void *name, RwUInt32 *length, RwUInt32 *version)
{
  struct RpWorld *world = 0;
  struct RwStream *stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, name);

  if (stream == 0) {
    return 0;
  }
  if (RwStreamFindChunk(stream, RW_CHUNK_WORLD, length, version)) {
    world = RpWorldStreamRead(stream);
  }
  RwStreamClose(stream, 0);
  return world;
}

/* Loads the first spline chunk of the file `name`. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
struct RpSpline *spline_read_from_file(void *name, RwUInt32 *length, RwUInt32 *version)
{
  struct RpSpline *spline = 0;
  struct RwStream *stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, name);

  if (stream == 0) {
    return 0;
  }
  if (RwStreamFindChunk(stream, RW_CHUNK_SPLINE, length, version)) {
    spline = RpSplineStreamRead(stream);
  }
  RwStreamClose(stream, 0);
  return spline;
}

/* Loads the first texture dictionary chunk of the file `name`. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
struct RwTexDictionary *texdict_read_from_file(void *name, RwUInt32 *length, RwUInt32 *version)
{
  struct RwTexDictionary *dict = 0;
  struct RwStream *stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, name);

  if (stream == 0) {
    return 0;
  }
  if (RwStreamFindChunk(stream, RW_CHUNK_TEXDICT, length, version)) {
    dict = RwTexDictionaryStreamRead(stream);
  }
  RwStreamClose(stream, 0);
  return dict;
}

/* Loads the first platform independent texture dictionary chunk of the file `name`. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
struct RwTexDictionary *texdict_load_file(void *name, RwUInt32 *length, RwUInt32 *version)
{
  struct RwTexDictionary *dict = 0;
  struct RwStream *stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, name);

  if (stream == 0) {
    return 0;
  }
  if (RwStreamFindChunk(stream, RW_CHUNK_TEXDICT_PI, length, version)) {
    dict = RtPITexDictionaryStreamRead(stream);
  }
  RwStreamClose(stream, 0);
  return dict;
}

/* Loads the first animation chunk of the file `name`. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
struct RtAnimAnimation *anim_read_from_file(void *name, RwUInt32 *length, RwUInt32 *version)
{
  struct RtAnimAnimation *anim = 0;
  struct RwStream *stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, name);

  if (stream == 0) {
    return 0;
  }
  if (RwStreamFindChunk(stream, RW_CHUNK_ANIM, length, version)) {
    anim = RtAnimAnimationStreamRead(stream);
  }
  RwStreamClose(stream, 0);
  return anim;
}

/* Loads the first clump chunk of a memory image (start, length passed by value). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
struct RpClump *clump_read_from_memory(void *start, RwUInt32 length, RwUInt32 *chunkLength, RwUInt32 *version)
{
  struct MflRwMemoryRange range;
  struct RpClump *clump = 0;
  struct RwStream *stream;

  range.start = start;
  range.length = length;
  stream = RwStreamOpen(rwSTREAMMEMORY, rwSTREAMREAD, &range);
  if (stream == 0) {
    return 0;
  }
  if (RwStreamFindChunk(stream, RW_CHUNK_CLUMP, chunkLength, version)) {
    clump = RpClumpStreamRead(stream);
  }
  RwStreamClose(stream, &range);
  return clump;
}

/* Loads the first platform independent texture dictionary chunk of a memory image. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
struct RwTexDictionary *texdict_read_from_memory(void *start, RwUInt32 length, RwUInt32 *chunkLength, RwUInt32 *version)
{
  struct MflRwMemoryRange range;
  struct RwTexDictionary *dict = 0;
  struct RwStream *stream;

  range.start = start;
  range.length = length;
  stream = RwStreamOpen(rwSTREAMMEMORY, rwSTREAMREAD, &range);
  if (stream == 0) {
    return 0;
  }
  if (RwStreamFindChunk(stream, RW_CHUNK_TEXDICT_PI, chunkLength, version)) {
    dict = RtPITexDictionaryStreamRead(stream);
  }
  RwStreamClose(stream, &range);
  return dict;
}

/* Collects the child frames of `frame` into `array` (may be NULL = count only); returns how many there are. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int frame_collect_children(struct RwFrame *frame, void **array)
{
  int count = 0;
  struct MflRwItemCollector collector;

  collector.array = array;
  collector.count = &count;
  RwFrameForAllChildren(frame, (RwFrameCallBack)rw_callback_collect, &collector);
  return count;
}

/* The first atomic of the clump (NULL when there is none or `clump` is NULL). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
struct RpAtomic *clump_first_atomic(struct RpClump *clump)
{
  struct RpAtomic *first = 0;

  if (clump != 0) {
    RpClumpForAllAtomics(clump, (RpAtomicCallBack)rw_callback_store_first, &first);
  }
  return first;
}

/* Collects the atomics of the clump into `array` (may be NULL = count only); returns how many there are. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int clump_get_atomics(struct RpClump *clump, struct RpAtomic **array)
{
  int count = 0;
  struct MflRwItemCollector collector;

  collector.array = (void **)array;
  collector.count = &count;
  RpClumpForAllAtomics(clump, (RpAtomicCallBack)rw_callback_collect, &collector);
  return count;
}

/* Collects the lights of the clump into `array` (may be NULL = count only); returns how many there are. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int clump_get_lights(struct RpClump *clump, struct RpLight **array)
{
  int count = 0;
  struct MflRwItemCollector collector;

  collector.array = (void **)array;
  collector.count = &count;
  RpClumpForAllLights(clump, (RpLightCallBack)rw_callback_collect, &collector);
  return count;
}

/* First material of the first atomic of the clump (NULL when there is none). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
struct RpMaterial *clump_first_material(struct RpClump *clump)
{
  struct RpMaterial *first = 0;

  if (clump != 0) {
    struct RpGeometry *geometry = clump_first_geometry(clump);

    if (geometry != 0) {
      RpGeometryForAllMaterials(geometry, (RpMaterialCallBack)rw_callback_store_first, &first);
    }
  }
  return first;
}

/* Copies the material pointers of the geometry into `out` (when not NULL); returns the material count. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int geometry_copy_materials(struct RpGeometry *geometry, struct RpMaterial **out)
{
  int count = RpGeometry_matList_numMaterials(geometry);
  int i;

  if (out != 0) {
    for (i = 0; i < count; i++) {
      out[i] = RpGeometry_matList_materials(geometry)[i];
    }
  }
  return count;
}

/* First material of the atomic's geometry (NULL when there is none). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
struct RpMaterial *atomic_first_material(struct RpAtomic *atomic)
{
  struct RpMaterial *first = 0;

  if (atomic != 0 && RpAtomic_geometry(atomic) != 0) {
    RpGeometryForAllMaterials(RpAtomic_geometry(atomic), (RpMaterialCallBack)rw_callback_store_first, &first);
  }
  return first;
}

/* Copies the materials of the atomic's geometry into `out`; returns the count. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int atomic_copy_materials(struct RpAtomic *atomic, struct RpMaterial **out)
{
  return geometry_copy_materials(RpAtomic_geometry(atomic), out);
}

/* Number of materials of the atomic's geometry. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int atomic_count_materials(struct RpAtomic *atomic)
{
  return atomic_copy_materials(atomic, 0);
}

/* Collects the world sectors of the world (NULL world = 0). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int world_collect_sectors(struct RpWorld *world, void **array)
{
  int count = 0;
  struct MflRwItemCollector collector;

  collector.array = array;
  collector.count = &count;
  if (world != 0) {
    RpWorldForAllWorldSectors(world, (RpWorldSectorCallBack)rw_callback_collect, &collector);
  }
  return count;
}

/* Collects the materials of the world (NULL world = 0). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int world_collect_materials(struct RpWorld *world, void **array)
{
  int count = 0;
  struct MflRwItemCollector collector;

  collector.array = array;
  collector.count = &count;
  if (world != 0) {
    RpWorldForAllMaterials(world, (RpMaterialCallBack)rw_callback_collect, &collector);
  }
  return count;
}

/* Collects the clumps of the world (NULL world = 0). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int world_collect_clumps(struct RpWorld *world, void **array)
{
  int count = 0;
  struct MflRwItemCollector collector;

  collector.array = array;
  collector.count = &count;
  if (world != 0) {
    RpWorldForAllClumps(world, (RpClumpCallBack)rw_callback_collect, &collector);
  }
  return count;
}

/* First texture of the dictionary (NULL when empty or `dict` is NULL). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
struct RwTexture *texdict_first_texture(struct RwTexDictionary *dict)
{
  struct RwTexture *first = 0;

  if (dict != 0) {
    RwTexDictionaryForAllTextures(dict, (RwTextureCallBack)rw_callback_store_first, &first);
  }
  return first;
}

/* Collects the textures of the dictionary (NULL dict = 0). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int texdict_collect_textures(struct RwTexDictionary *dict, void **array)
{
  int count = 0;
  struct MflRwItemCollector collector;

  collector.array = array;
  collector.count = &count;
  if (dict != 0) {
    RwTexDictionaryForAllTextures(dict, (RwTextureCallBack)rw_callback_collect, &collector);
  }
  return count;
}
