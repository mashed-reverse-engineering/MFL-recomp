/* RenderWare Audio (RWA) internals for the MFL.exe lib/rwa sources.  RwaLLNode, RwaParamType, RwaParamTypeHandle and
 * RwaType are taken from the Piglet's Big Game decompilation (https://github.com/tgsm/pbg, include/rwa/core, CC0 1.0);
 * many thanks to tgsm and the pbg contributors.
 * Every other struct here has only the members the lib/rwa sources use, at the offsets the byte-identical functions
 * show in MFL.exe; the bytes between them are padding (padNN: the offset in hex).  The rwaASSERT lines pin the sizes
 * and offsets. */
#ifndef RWAINT_H
#define RWAINT_H

#include "rwa.h"

typedef RwLLLink rwaLink;

typedef struct rwaSound rwaSound;
typedef struct rwaEmitter rwaEmitter;
typedef struct rwaWave rwaWave;
typedef struct rwaObjDef rwaObjDef;
typedef struct rwaInstance rwaInstance;
typedef struct rwaInstSearch rwaInstSearch;
typedef struct rwaInstFind rwaInstFind;
typedef struct rwaParamFunc rwaParamFunc;
typedef struct rwaObjInterface rwaObjInterface;
typedef struct rwaHandleParams rwaHandleParams;
typedef struct rwaReleaseParams rwaReleaseParams;
typedef struct rwaStreamFormat rwaStreamFormat;
typedef struct rwaStreamTrack rwaStreamTrack;
typedef struct rwaStreamMarker rwaStreamMarker;
typedef struct rwaStreamInfo rwaStreamInfo;
typedef struct rwaStreamPosition rwaStreamPosition;
typedef struct rwaStreamRegion rwaStreamRegion;
typedef struct rwaQueueCallback rwaQueueCallback;
typedef struct rwaQueueItem rwaQueueItem;
typedef struct rwaQueue rwaQueue;
typedef struct rwaStreamNodeState rwaStreamNodeState;
typedef struct rwaCacheRequest rwaCacheRequest;
typedef struct rwaCacheFile rwaCacheFile;
typedef struct rwaCacheClient rwaCacheClient;
typedef struct rwaReadRequest rwaReadRequest;
typedef struct rwaReadCache rwaReadCache;
typedef struct rwaDecoderState rwaDecoderState;
typedef struct rwaDecoder rwaDecoder;
typedef struct rwaStreamReader rwaStreamReader;
typedef struct rwaStreamConnect rwaStreamConnect;
typedef struct rwaStreamSourceParams rwaStreamSourceParams;
typedef struct rwaStreamNodeParams rwaStreamNodeParams;
typedef struct rwaStreamPlayer rwaStreamPlayer;
typedef struct rwaStream rwaStream;
typedef struct rwaThread rwaThread;
typedef struct _rwaParamTypeDef _rwaParamTypeDef;
typedef struct rwaValueFreeList rwaValueFreeList;
typedef struct RwaWaveFormat RwaWaveFormat;
typedef struct rwaWaveFormatDesc rwaWaveFormatDesc;
typedef struct RwaWaveDef RwaWaveDef;
typedef struct RwaWave RwaWave;
typedef struct rwaWaveDefFind rwaWaveDefFind;
typedef struct RwaFreeList RwaFreeList;
typedef struct rwaFreeListBlock rwaFreeListBlock;
typedef struct rwaPoolChunk rwaPoolChunk;
typedef struct rwaPoolBlock rwaPoolBlock;
typedef struct rwaPool rwaPool;
typedef struct RwaSymbolTable RwaSymbolTable;
typedef struct rwaWaveDict rwaWaveDict;
typedef struct rwaWaveNode rwaWaveNode;
typedef struct rwaWaveItem rwaWaveItem;
typedef struct rwaOrientation rwaOrientation;
typedef struct rwaListenerState rwaListenerState;
typedef struct rwaListener rwaListener;
typedef struct rwaParamDef rwaParamDef;
typedef struct rwaParamTables rwaParamTables;
typedef struct _rwaParamID _rwaParamID;
typedef struct RwaObjInterfaceDef RwaObjInterfaceDef;
typedef struct rwaIfaceSearch rwaIfaceSearch;
typedef struct rwaModule rwaModule;
typedef struct rwaVoiceWave rwaVoiceWave;
typedef struct rwaCodecInfo rwaCodecInfo;
typedef struct rwaContainerSource rwaContainerSource;
typedef struct rwaConversion rwaConversion;
typedef struct rwaNodeSet rwaNodeSet;
typedef struct rwaNodeSetEntry rwaNodeSetEntry;
typedef struct rwaConversionParams rwaConversionParams;
typedef struct rwaPlaySettings rwaPlaySettings;
typedef struct rwaContainerCommand rwaContainerCommand;
typedef struct rwaStreamContainer rwaStreamContainer;
typedef struct RwaAllocBitmap RwaAllocBitmap;
typedef struct rwaConverterInfo rwaConverterInfo;
typedef struct rwaConverter rwaConverter;
typedef struct rwaConversionPath rwaConversionPath;
typedef struct rwaNodeAllocator rwaNodeAllocator;
typedef struct rwaSplitterParams rwaSplitterParams;
typedef struct rwaSplitterTrack rwaSplitterTrack;
typedef struct rwaSplitterState rwaSplitterState;
typedef struct rwaSplitter rwaSplitter;
typedef struct rwaStreamVoiceParams rwaStreamVoiceParams;
typedef struct rwaHandleList rwaHandleList;
typedef struct rwaStreamVoiceState rwaStreamVoiceState;

typedef enum RwaType {
    rwaTYPE_UINT8 = (1 << 0),
    rwaTYPE_INT8 = (1 << 1),
    rwaTYPE_UINT16 = (1 << 2),
    rwaTYPE_INT16 = (1 << 3),
    rwaTYPE_UINT32 = (1 << 4),
    rwaTYPE_INT32 = (1 << 5),
    rwaTYPE_UINT64 = (1 << 6),
    rwaTYPE_INT64 = (1 << 7),
    rwaTYPE_UINT128 = (1 << 8),
    rwaTYPE_INT128 = (1 << 9),
    rwaTYPE_FLOAT = (1 << 10),
    rwaTYPE_DOUBLE = (1 << 11),
    rwaTYPE_POINTER = (1 << 12),
    rwaTYPE_ARRAY = (1 << 13),
    rwaTYPE_ALIAS = (1 << 14),
} RwaType;

typedef void (*rwaCacheCallBack)(void *file, RwUInt32 bytes, RwInt32 status, RwInt32 event, void *data);
typedef void (*rwaContainerNodeFunc)(rwaDecoder *node, void *data);
typedef RwBool (*rwaModuleCallBack)(rwaModule *module, RwUInt32 index);
extern const RwInt8 _rwMsbBit[];
extern RwUInt8 sub_242150(RwUInt32 value);

struct rwaSound
{
    rwaSound *next;    /* 0x00 */
    rwaSound *prev;    /* 0x04 */
    RwaObjHandle *handle;    /* 0x08 */
    RwUInt32 flags;    /* 0x0C */
    RwUInt32 unk38;    /* 0x10 */
    volatile RwReal unk3C;    /* 0x14 */
    RwUInt32 unk40;    /* 0x18 */
    RwUInt32 unk44;    /* 0x1C */
    RwUInt32 unk48;    /* 0x20 */
    RwUInt32 unk4C;    /* 0x24 */
    void *unk50;    /* 0x28 */
    RwUInt32 unk54;    /* 0x2C */
    RwaV3d unk58;    /* 0x30 */
    RwaV3d unk64;    /* 0x3C */
    RwUInt32 unk70;    /* 0x48 */
    RwUInt32 unk74;    /* 0x4C */
    RwUInt32 unk78;    /* 0x50 */
    RwUInt32 unk7C;    /* 0x54 */
    RwUInt32 unk80;    /* 0x58 */
    RwUInt32 unk84;    /* 0x5C */
    RwUInt32 unk88;    /* 0x60 */
};
rwaASSERT(rwaSound__sizeof, sizeof(rwaSound) == 0x64);
rwaASSERT(rwaSound_prev, offsetof(rwaSound, prev) == 0x4);
rwaASSERT(rwaSound_handle, offsetof(rwaSound, handle) == 0x8);
rwaASSERT(rwaSound_flags, offsetof(rwaSound, flags) == 0xC);
rwaASSERT(rwaSound_unk38, offsetof(rwaSound, unk38) == 0x10);
rwaASSERT(rwaSound_unk3C, offsetof(rwaSound, unk3C) == 0x14);
rwaASSERT(rwaSound_unk40, offsetof(rwaSound, unk40) == 0x18);
rwaASSERT(rwaSound_unk44, offsetof(rwaSound, unk44) == 0x1C);
rwaASSERT(rwaSound_unk48, offsetof(rwaSound, unk48) == 0x20);
rwaASSERT(rwaSound_unk4C, offsetof(rwaSound, unk4C) == 0x24);
rwaASSERT(rwaSound_unk50, offsetof(rwaSound, unk50) == 0x28);
rwaASSERT(rwaSound_unk54, offsetof(rwaSound, unk54) == 0x2C);
rwaASSERT(rwaSound_unk58, offsetof(rwaSound, unk58) == 0x30);
rwaASSERT(rwaSound_unk64, offsetof(rwaSound, unk64) == 0x3C);
rwaASSERT(rwaSound_unk70, offsetof(rwaSound, unk70) == 0x48);
rwaASSERT(rwaSound_unk74, offsetof(rwaSound, unk74) == 0x4C);
rwaASSERT(rwaSound_unk78, offsetof(rwaSound, unk78) == 0x50);
rwaASSERT(rwaSound_unk7C, offsetof(rwaSound, unk7C) == 0x54);
rwaASSERT(rwaSound_unk80, offsetof(rwaSound, unk80) == 0x58);
rwaASSERT(rwaSound_unk84, offsetof(rwaSound, unk84) == 0x5C);
rwaASSERT(rwaSound_unk88, offsetof(rwaSound, unk88) == 0x60);

struct rwaEmitter
{
    RwUInt32 pad00[10];
    rwaSound sound;    /* 0x28 */
};
rwaASSERT(rwaEmitter__sizeof, sizeof(rwaEmitter) == 0x8C);
rwaASSERT(rwaEmitter_sound, offsetof(rwaEmitter, sound) == 0x28);

struct rwaWave
{
    RwUInt32 pad00[11];
    RwUInt32 unk2C;    /* 0x2C */
};
rwaASSERT(rwaWave__sizeof, sizeof(rwaWave) == 0x30);
rwaASSERT(rwaWave_unk2C, offsetof(rwaWave, unk2C) == 0x2C);

struct rwaObjDef
{
    RwUInt32 pad00[3];
    RwUInt32 flags0C;    /* 0x0C */
    rwaLink children;    /* 0x10 */
    rwaLink sibling;    /* 0x18 */
    rwaLink waveDefs;    /* 0x20 */
    RwBool (*create)(rwaInstance *obj, rwaInstance *parent, void *params, void *a, void *b);    /* 0x28 */
    rwaInstance * (*unk2C)(rwaInstance *obj);    /* 0x2C */
    rwaInstance * (*unk30)(rwaInstance *obj);    /* 0x30 */
    void (*unlink)(rwaInstance *obj, void *a, void *b);    /* 0x34 */
    void (*unregister)(struct rwaObjDef *def);    /* 0x38 */
    void (*query)(void *obj, void *params);    /* 0x3C */
    union {
        RwUInt32 flags;
        struct {
            RwUInt32 stateSize : 28;
            RwUInt32 stateAlign : 4;
        } unk40;
    };    /* 0x40 */
    RwUInt16 paramsSize;    /* 0x44 */
    RwUInt16 maxInstances;    /* 0x46 */
    void *instances;    /* 0x48 */
    void **interfaces;    /* 0x4C */
    void *unk50;    /* 0x50 */
    void *unk54;    /* 0x54 */
    RwUInt32 hint;    /* 0x58 */
};
rwaASSERT(rwaObjDef__sizeof, sizeof(rwaObjDef) == 0x5C);
rwaASSERT(rwaObjDef_flags0C, offsetof(rwaObjDef, flags0C) == 0xC);
rwaASSERT(rwaObjDef_children, offsetof(rwaObjDef, children) == 0x10);
rwaASSERT(rwaObjDef_sibling, offsetof(rwaObjDef, sibling) == 0x18);
rwaASSERT(rwaObjDef_waveDefs, offsetof(rwaObjDef, waveDefs) == 0x20);
rwaASSERT(rwaObjDef_create, offsetof(rwaObjDef, create) == 0x28);
rwaASSERT(rwaObjDef_unk2C, offsetof(rwaObjDef, unk2C) == 0x2C);
rwaASSERT(rwaObjDef_unk30, offsetof(rwaObjDef, unk30) == 0x30);
rwaASSERT(rwaObjDef_unlink, offsetof(rwaObjDef, unlink) == 0x34);
rwaASSERT(rwaObjDef_unregister, offsetof(rwaObjDef, unregister) == 0x38);
rwaASSERT(rwaObjDef_query, offsetof(rwaObjDef, query) == 0x3C);
rwaASSERT(rwaObjDef_paramsSize, offsetof(rwaObjDef, paramsSize) == 0x44);
rwaASSERT(rwaObjDef_maxInstances, offsetof(rwaObjDef, maxInstances) == 0x46);
rwaASSERT(rwaObjDef_instances, offsetof(rwaObjDef, instances) == 0x48);
rwaASSERT(rwaObjDef_interfaces, offsetof(rwaObjDef, interfaces) == 0x4C);
rwaASSERT(rwaObjDef_unk50, offsetof(rwaObjDef, unk50) == 0x50);
rwaASSERT(rwaObjDef_unk54, offsetof(rwaObjDef, unk54) == 0x54);
rwaASSERT(rwaObjDef_hint, offsetof(rwaObjDef, hint) == 0x58);

struct rwaInstance
{
    rwaObjDef *def;    /* 0x00 */
    RwUInt32 pad04[2];
    rwaLink subObjs;    /* 0x0C */
    rwaLink subObjLink;    /* 0x14 */
    RwUInt32 flags;    /* 0x1C */
    RwUInt8 *state;    /* 0x20 */
    rwaInstance *parent;    /* 0x24 */
};
rwaASSERT(rwaInstance__sizeof, sizeof(rwaInstance) == 0x28);
rwaASSERT(rwaInstance_subObjs, offsetof(rwaInstance, subObjs) == 0xC);
rwaASSERT(rwaInstance_subObjLink, offsetof(rwaInstance, subObjLink) == 0x14);
rwaASSERT(rwaInstance_flags, offsetof(rwaInstance, flags) == 0x1C);
rwaASSERT(rwaInstance_state, offsetof(rwaInstance, state) == 0x20);
rwaASSERT(rwaInstance_parent, offsetof(rwaInstance, parent) == 0x24);

struct rwaInstSearch
{
    rwaObjDef *def;    /* 0x00 */
    RwUInt32 found;    /* 0x04 */
};
rwaASSERT(rwaInstSearch__sizeof, sizeof(rwaInstSearch) == 0x8);
rwaASSERT(rwaInstSearch_found, offsetof(rwaInstSearch, found) == 0x4);

struct rwaInstFind
{
    rwaInstance *found;    /* 0x00 */
    rwaObjDef *def;    /* 0x04 */
};
rwaASSERT(rwaInstFind__sizeof, sizeof(rwaInstFind) == 0x8);
rwaASSERT(rwaInstFind_def, offsetof(rwaInstFind, def) == 0x4);

struct rwaParamFunc
{
    RwBool (*func)(rwaInstance *obj, RwUInt32 param, void *data);    /* 0x00 */
    RwUInt16 param;    /* 0x04 */
    RwUInt8 pad06[2];
};
rwaASSERT(rwaParamFunc__sizeof, sizeof(rwaParamFunc) == 0x8);
rwaASSERT(rwaParamFunc_param, offsetof(rwaParamFunc, param) == 0x4);

struct rwaObjInterface
{
    RwUInt32 pad00;
    rwaParamFunc *setters;    /* 0x04 */
    rwaParamFunc *getters;    /* 0x08 */
    RwUInt32 refCount;    /* 0x0C */
    RwUInt32 flags;    /* 0x10 */
    rwaLink link;    /* 0x14 */
};
rwaASSERT(rwaObjInterface__sizeof, sizeof(rwaObjInterface) == 0x1C);
rwaASSERT(rwaObjInterface_setters, offsetof(rwaObjInterface, setters) == 0x4);
rwaASSERT(rwaObjInterface_getters, offsetof(rwaObjInterface, getters) == 0x8);
rwaASSERT(rwaObjInterface_refCount, offsetof(rwaObjInterface, refCount) == 0xC);
rwaASSERT(rwaObjInterface_flags, offsetof(rwaObjInterface, flags) == 0x10);
rwaASSERT(rwaObjInterface_link, offsetof(rwaObjInterface, link) == 0x14);

struct RwaObjHandle
{
    rwaInstance *obj;    /* 0x00 */
    rwaObjInterface *objInterface;    /* 0x04 */
    RwUInt32 pad08[4];
};
rwaASSERT(RwaObjHandle__sizeof, sizeof(RwaObjHandle) == 0x18);
rwaASSERT(RwaObjHandle_objInterface, offsetof(RwaObjHandle, objInterface) == 0x4);

struct rwaHandleParams
{
    RwUInt32 pad00[6];
};
rwaASSERT(rwaHandleParams__sizeof, sizeof(rwaHandleParams) == 0x18);

struct rwaReleaseParams
{
    RwUInt32 pad00[4];
};
rwaASSERT(rwaReleaseParams__sizeof, sizeof(rwaReleaseParams) == 0x10);

struct rwaStreamFormat
{
    RwUInt8 pad00[13];
    RwUInt8 channels;    /* 0x0D */
    RwUInt8 pad0E[14];
};
rwaASSERT(rwaStreamFormat__sizeof, sizeof(rwaStreamFormat) == 0x1C);
rwaASSERT(rwaStreamFormat_channels, offsetof(rwaStreamFormat, channels) == 0xD);

struct rwaStreamTrack
{
    RwUInt32 pad00[5];
    rwaStreamFormat *format;    /* 0x14 */
    RwUInt32 pad18[2];
    RwUInt32 unk20;    /* 0x20 */
    RwUInt32 pad24;
};
rwaASSERT(rwaStreamTrack__sizeof, sizeof(rwaStreamTrack) == 0x28);
rwaASSERT(rwaStreamTrack_format, offsetof(rwaStreamTrack, format) == 0x14);
rwaASSERT(rwaStreamTrack_unk20, offsetof(rwaStreamTrack, unk20) == 0x20);

struct rwaStreamMarker
{
    const RwaUUID *uuid;    /* 0x00 */
    const RwChar *name;    /* 0x04 */
    RwUInt32 pad08[6];
};
rwaASSERT(rwaStreamMarker__sizeof, sizeof(rwaStreamMarker) == 0x20);
rwaASSERT(rwaStreamMarker_name, offsetof(rwaStreamMarker, name) == 0x4);

struct rwaStreamInfo
{
    RwUInt32 pad00[4];
    rwaStreamMarker *markers;    /* 0x10 */
    RwUInt32 noTracks;    /* 0x14 */
    rwaStreamTrack *tracks;    /* 0x18 */
    RwUInt32 pad1C;
    RwUInt32 unk20;    /* 0x20 */
    RwUInt32 pad24[2];
};
rwaASSERT(rwaStreamInfo__sizeof, sizeof(rwaStreamInfo) == 0x2C);
rwaASSERT(rwaStreamInfo_markers, offsetof(rwaStreamInfo, markers) == 0x10);
rwaASSERT(rwaStreamInfo_noTracks, offsetof(rwaStreamInfo, noTracks) == 0x14);
rwaASSERT(rwaStreamInfo_tracks, offsetof(rwaStreamInfo, tracks) == 0x18);
rwaASSERT(rwaStreamInfo_unk20, offsetof(rwaStreamInfo, unk20) == 0x20);

struct rwaStreamPosition
{
    RwUInt32 value;    /* 0x00 */
    RwInt32 type;    /* 0x04 */
    RwUInt32 pos;    /* 0x08 */
    RwInt32 unit;    /* 0x0C */
};
rwaASSERT(rwaStreamPosition__sizeof, sizeof(rwaStreamPosition) == 0x10);
rwaASSERT(rwaStreamPosition_type, offsetof(rwaStreamPosition, type) == 0x4);
rwaASSERT(rwaStreamPosition_pos, offsetof(rwaStreamPosition, pos) == 0x8);
rwaASSERT(rwaStreamPosition_unit, offsetof(rwaStreamPosition, unit) == 0xC);

struct rwaStreamRegion
{
    rwaStreamMarker *marker;    /* 0x00 */
    RwUInt32 pad04[2];
    RwUInt32 unk0C;    /* 0x0C */
    RwUInt32 flags;    /* 0x10 */
};
rwaASSERT(rwaStreamRegion__sizeof, sizeof(rwaStreamRegion) == 0x14);
rwaASSERT(rwaStreamRegion_unk0C, offsetof(rwaStreamRegion, unk0C) == 0xC);
rwaASSERT(rwaStreamRegion_flags, offsetof(rwaStreamRegion, flags) == 0x10);

struct rwaQueueCallback
{
    void (*func)(rwaQueueItem *item, rwaDecoder *obj);    /* 0x00 */
    rwaDecoder *data;    /* 0x04 */
};
rwaASSERT(rwaQueueCallback__sizeof, sizeof(rwaQueueCallback) == 0x8);
rwaASSERT(rwaQueueCallback_data, offsetof(rwaQueueCallback, data) == 0x4);

struct rwaQueueItem
{
    RwUInt32 flags;    /* 0x00 */
    RwUInt8 *unk04;    /* 0x04 */
    RwUInt32 unk08;    /* 0x08 */
    rwaStreamPosition *position;    /* 0x0C */
    rwaQueueCallback *callback;    /* 0x10 */
};
rwaASSERT(rwaQueueItem__sizeof, sizeof(rwaQueueItem) == 0x14);
rwaASSERT(rwaQueueItem_unk04, offsetof(rwaQueueItem, unk04) == 0x4);
rwaASSERT(rwaQueueItem_unk08, offsetof(rwaQueueItem, unk08) == 0x8);
rwaASSERT(rwaQueueItem_position, offsetof(rwaQueueItem, position) == 0xC);
rwaASSERT(rwaQueueItem_callback, offsetof(rwaQueueItem, callback) == 0x10);

struct rwaQueue
{
    RwUInt8 head;    /* 0x00 */
    RwUInt8 count;    /* 0x01 */
    RwUInt8 unk02;    /* 0x02 */
    RwUInt8 size;    /* 0x03 */
    rwaQueueItem *items;    /* 0x04 */
};
rwaASSERT(rwaQueue__sizeof, sizeof(rwaQueue) == 0x8);
rwaASSERT(rwaQueue_count, offsetof(rwaQueue, count) == 0x1);
rwaASSERT(rwaQueue_unk02, offsetof(rwaQueue, unk02) == 0x2);
rwaASSERT(rwaQueue_size, offsetof(rwaQueue, size) == 0x3);
rwaASSERT(rwaQueue_items, offsetof(rwaQueue, items) == 0x4);

struct rwaStreamNodeState
{
    void *iface;    /* 0x00 */
    RwUInt32 pad04[2];
    RwInt32 unk0C;    /* 0x0C */
    struct rwaDecoder *next;    /* 0x10 */
    RwUInt32 unk14;    /* 0x14 */
    RwUInt32 unk18;    /* 0x18 */
    RwInt32 unk1C;    /* 0x1C */
    RwInt32 mode;    /* 0x20 */
    RwUInt32 numCreated;    /* 0x24 */
    RwInt32 status;    /* 0x28 */
    rwaQueueCallback *callback;    /* 0x2C */
};
rwaASSERT(rwaStreamNodeState__sizeof, sizeof(rwaStreamNodeState) == 0x30);
rwaASSERT(rwaStreamNodeState_unk0C, offsetof(rwaStreamNodeState, unk0C) == 0xC);
rwaASSERT(rwaStreamNodeState_next, offsetof(rwaStreamNodeState, next) == 0x10);
rwaASSERT(rwaStreamNodeState_unk14, offsetof(rwaStreamNodeState, unk14) == 0x14);
rwaASSERT(rwaStreamNodeState_unk18, offsetof(rwaStreamNodeState, unk18) == 0x18);
rwaASSERT(rwaStreamNodeState_unk1C, offsetof(rwaStreamNodeState, unk1C) == 0x1C);
rwaASSERT(rwaStreamNodeState_mode, offsetof(rwaStreamNodeState, mode) == 0x20);
rwaASSERT(rwaStreamNodeState_numCreated, offsetof(rwaStreamNodeState, numCreated) == 0x24);
rwaASSERT(rwaStreamNodeState_status, offsetof(rwaStreamNodeState, status) == 0x28);
rwaASSERT(rwaStreamNodeState_callback, offsetof(rwaStreamNodeState, callback) == 0x2C);

struct rwaCacheRequest
{
    rwaLink link;    /* 0x00 */
    RwInt32 state;    /* 0x08 */
    RwUInt32 offset;    /* 0x0C */
    RwUInt32 size;    /* 0x10 */
    RwUInt32 remaining;    /* 0x14 */
    RwUInt8 *dest;    /* 0x18 */
    void *file;    /* 0x1C */
    RwUInt32 pad20[5];
    rwaCacheCallBack callback;    /* 0x34 */
    void *data;    /* 0x38 */
};
rwaASSERT(rwaCacheRequest__sizeof, sizeof(rwaCacheRequest) == 0x3C);
rwaASSERT(rwaCacheRequest_state, offsetof(rwaCacheRequest, state) == 0x8);
rwaASSERT(rwaCacheRequest_offset, offsetof(rwaCacheRequest, offset) == 0xC);
rwaASSERT(rwaCacheRequest_size, offsetof(rwaCacheRequest, size) == 0x10);
rwaASSERT(rwaCacheRequest_remaining, offsetof(rwaCacheRequest, remaining) == 0x14);
rwaASSERT(rwaCacheRequest_dest, offsetof(rwaCacheRequest, dest) == 0x18);
rwaASSERT(rwaCacheRequest_file, offsetof(rwaCacheRequest, file) == 0x1C);
rwaASSERT(rwaCacheRequest_callback, offsetof(rwaCacheRequest, callback) == 0x34);
rwaASSERT(rwaCacheRequest_data, offsetof(rwaCacheRequest, data) == 0x38);

struct rwaCacheFile
{
    RwInt32 type;    /* 0x00 */
    RwUInt32 pad04[2];
    void *file;    /* 0x0C */
};
rwaASSERT(rwaCacheFile__sizeof, sizeof(rwaCacheFile) == 0x10);
rwaASSERT(rwaCacheFile_file, offsetof(rwaCacheFile, file) == 0xC);

struct rwaCacheClient
{
    rwaCacheFile *holder;    /* 0x00 */
    rwaCacheRequest request;    /* 0x04 */
    RwInt32 mode;    /* 0x40 */
    rwaCacheCallBack callback;    /* 0x44 */
    void *data;    /* 0x48 */
    RwUInt32 unk4C;    /* 0x4C */
    RwUInt32 *header;    /* 0x50 */
    RwBool seeking;    /* 0x54 */
};
rwaASSERT(rwaCacheClient__sizeof, sizeof(rwaCacheClient) == 0x58);
rwaASSERT(rwaCacheClient_request, offsetof(rwaCacheClient, request) == 0x4);
rwaASSERT(rwaCacheClient_mode, offsetof(rwaCacheClient, mode) == 0x40);
rwaASSERT(rwaCacheClient_callback, offsetof(rwaCacheClient, callback) == 0x44);
rwaASSERT(rwaCacheClient_data, offsetof(rwaCacheClient, data) == 0x48);
rwaASSERT(rwaCacheClient_unk4C, offsetof(rwaCacheClient, unk4C) == 0x4C);
rwaASSERT(rwaCacheClient_header, offsetof(rwaCacheClient, header) == 0x50);
rwaASSERT(rwaCacheClient_seeking, offsetof(rwaCacheClient, seeking) == 0x54);

struct rwaReadRequest
{
    RwUInt32 pad00[5];
};
rwaASSERT(rwaReadRequest__sizeof, sizeof(rwaReadRequest) == 0x14);

struct rwaReadCache
{
    rwaReadRequest *requests;    /* 0x00 */
    RwUInt16 size;    /* 0x04 */
    RwUInt16 unk06;    /* 0x06 */
    RwUInt16 unk08;    /* 0x08 */
    RwUInt16 unk0A;    /* 0x0A */
    void *semaphore;    /* 0x0C */
    RwUInt32 pad10[15];
};
rwaASSERT(rwaReadCache__sizeof, sizeof(rwaReadCache) == 0x4C);
rwaASSERT(rwaReadCache_size, offsetof(rwaReadCache, size) == 0x4);
rwaASSERT(rwaReadCache_unk06, offsetof(rwaReadCache, unk06) == 0x6);
rwaASSERT(rwaReadCache_unk08, offsetof(rwaReadCache, unk08) == 0x8);
rwaASSERT(rwaReadCache_unk0A, offsetof(rwaReadCache, unk0A) == 0xA);
rwaASSERT(rwaReadCache_semaphore, offsetof(rwaReadCache, semaphore) == 0xC);

struct rwaDecoderState
{
    RwUInt32 pad00[10];
    rwaCacheClient client;    /* 0x28 */
    RwUInt32 pad80[3];
    rwaReadCache cache;    /* 0x8C */
    RwUInt32 unkD8;    /* 0xD8 */
    volatile RwUInt32 unkDC;    /* 0xDC */
    volatile RwUInt32 unkE0;    /* 0xE0 */
    volatile RwUInt32 unkE4;    /* 0xE4 */
    volatile RwUInt32 requests;    /* 0xE8 */
    RwInt32 loops;    /* 0xEC */
    RwUInt32 padF0[3];
    RwUInt32 unkFC;    /* 0xFC */
    RwUInt32 pad100[6];
    rwaStreamInfo *info;    /* 0x118 */
    RwUInt32 pad11C[11];
    rwaStreamRegion *regions;    /* 0x148 */
    RwUInt32 unk14C;    /* 0x14C */
    volatile RwUInt32 region;    /* 0x150 */
    volatile RwUInt32 unk154;    /* 0x154 */
    RwUInt32 pad158;
    volatile RwUInt32 unk15C;    /* 0x15C */
    RwUInt32 pad160;
    RwUInt32 noItems;    /* 0x164 */
    RwUInt8 *aheadBuffer;    /* 0x168 */
    rwaStreamRegion *entries;    /* 0x16C */
    RwUInt32 pad170;
    volatile RwUInt32 unk174;    /* 0x174 */
    volatile RwUInt32 entry;    /* 0x178 */
    RwUInt32 pad17C[3];
    volatile RwUInt32 flags;    /* 0x188 */
    RwUInt32 pad18C[2];
    rwaStreamPosition *positions;    /* 0x194 */
    rwaQueueItem *items;    /* 0x198 */
    rwaQueue queue;    /* 0x19C */
    rwaQueueCallback callback;    /* 0x1A4 */
};
rwaASSERT(rwaDecoderState__sizeof, sizeof(rwaDecoderState) == 0x1AC);
rwaASSERT(rwaDecoderState_client, offsetof(rwaDecoderState, client) == 0x28);
rwaASSERT(rwaDecoderState_cache, offsetof(rwaDecoderState, cache) == 0x8C);
rwaASSERT(rwaDecoderState_unkD8, offsetof(rwaDecoderState, unkD8) == 0xD8);
rwaASSERT(rwaDecoderState_unkDC, offsetof(rwaDecoderState, unkDC) == 0xDC);
rwaASSERT(rwaDecoderState_unkE0, offsetof(rwaDecoderState, unkE0) == 0xE0);
rwaASSERT(rwaDecoderState_unkE4, offsetof(rwaDecoderState, unkE4) == 0xE4);
rwaASSERT(rwaDecoderState_requests, offsetof(rwaDecoderState, requests) == 0xE8);
rwaASSERT(rwaDecoderState_loops, offsetof(rwaDecoderState, loops) == 0xEC);
rwaASSERT(rwaDecoderState_unkFC, offsetof(rwaDecoderState, unkFC) == 0xFC);
rwaASSERT(rwaDecoderState_info, offsetof(rwaDecoderState, info) == 0x118);
rwaASSERT(rwaDecoderState_regions, offsetof(rwaDecoderState, regions) == 0x148);
rwaASSERT(rwaDecoderState_unk14C, offsetof(rwaDecoderState, unk14C) == 0x14C);
rwaASSERT(rwaDecoderState_region, offsetof(rwaDecoderState, region) == 0x150);
rwaASSERT(rwaDecoderState_unk154, offsetof(rwaDecoderState, unk154) == 0x154);
rwaASSERT(rwaDecoderState_unk15C, offsetof(rwaDecoderState, unk15C) == 0x15C);
rwaASSERT(rwaDecoderState_noItems, offsetof(rwaDecoderState, noItems) == 0x164);
rwaASSERT(rwaDecoderState_aheadBuffer, offsetof(rwaDecoderState, aheadBuffer) == 0x168);
rwaASSERT(rwaDecoderState_entries, offsetof(rwaDecoderState, entries) == 0x16C);
rwaASSERT(rwaDecoderState_unk174, offsetof(rwaDecoderState, unk174) == 0x174);
rwaASSERT(rwaDecoderState_entry, offsetof(rwaDecoderState, entry) == 0x178);
rwaASSERT(rwaDecoderState_flags, offsetof(rwaDecoderState, flags) == 0x188);
rwaASSERT(rwaDecoderState_positions, offsetof(rwaDecoderState, positions) == 0x194);
rwaASSERT(rwaDecoderState_items, offsetof(rwaDecoderState, items) == 0x198);
rwaASSERT(rwaDecoderState_queue, offsetof(rwaDecoderState, queue) == 0x19C);
rwaASSERT(rwaDecoderState_callback, offsetof(rwaDecoderState, callback) == 0x1A4);

struct rwaDecoder
{
    rwaInstance inst;    /* 0x00 */
    void *unk28;    /* 0x28 */
    rwaLink link;    /* 0x2C */
    RwUInt32 pad34;
    rwaDecoder *next;    /* 0x38 */
    RwUInt32 pad3C[3];
    RwInt32 mode;    /* 0x48 */
    RwUInt32 pad4C;
    RwInt32 status;    /* 0x50 */
    RwUInt32 pad54;
    void *objInterface;    /* 0x58 */
    rwaDecoderState state;    /* 0x5C */
};
rwaASSERT(rwaDecoder__sizeof, sizeof(rwaDecoder) == 0x208);
rwaASSERT(rwaDecoder_unk28, offsetof(rwaDecoder, unk28) == 0x28);
rwaASSERT(rwaDecoder_link, offsetof(rwaDecoder, link) == 0x2C);
rwaASSERT(rwaDecoder_next, offsetof(rwaDecoder, next) == 0x38);
rwaASSERT(rwaDecoder_mode, offsetof(rwaDecoder, mode) == 0x48);
rwaASSERT(rwaDecoder_status, offsetof(rwaDecoder, status) == 0x50);
rwaASSERT(rwaDecoder_objInterface, offsetof(rwaDecoder, objInterface) == 0x58);
rwaASSERT(rwaDecoder_state, offsetof(rwaDecoder, state) == 0x5C);

struct rwaStreamReader
{
    RwUInt32 unk00;    /* 0x00 */
    rwaQueueItem *(*read)(rwaDecoder *obj);    /* 0x04 */
    rwaDecoder *data;    /* 0x08 */
};
rwaASSERT(rwaStreamReader__sizeof, sizeof(rwaStreamReader) == 0xC);
rwaASSERT(rwaStreamReader_read, offsetof(rwaStreamReader, read) == 0x4);
rwaASSERT(rwaStreamReader_data, offsetof(rwaStreamReader, data) == 0x8);

struct rwaStreamConnect
{
    RwUInt32 unk00;    /* 0x00 */
    rwaStreamReader *reader;    /* 0x04 */
};
rwaASSERT(rwaStreamConnect__sizeof, sizeof(rwaStreamConnect) == 0x8);
rwaASSERT(rwaStreamConnect_reader, offsetof(rwaStreamConnect, reader) == 0x4);

struct rwaStreamSourceParams
{
    union {
        RwUInt32 values[15];
        struct {
            RwUInt32 unk00;
            RwUInt32 unk04;
            void *unk08;
            RwUInt32 unk0C;
            RwChar *unk10;
            RwUInt32 unk14;
            RwUInt32 unk18;
            const RwChar *unk1C;
            RwUInt32 unk20;
            RwUInt32 unk24;
            RwUInt32 unk28;
            RwUInt8 *unk2C;
            RwUInt32 unk30;
            RwUInt32 unk34;
            RwUInt8 *unk38;
        };
    };    /* 0x00 */
    void (*callback)(struct rwaDecoderState *state, RwBool failed, rwaDecoder *obj);    /* 0x3C */
    rwaDecoder *data;    /* 0x40 */
};
rwaASSERT(rwaStreamSourceParams__sizeof, sizeof(rwaStreamSourceParams) == 0x44);
rwaASSERT(rwaStreamSourceParams_callback, offsetof(rwaStreamSourceParams, callback) == 0x3C);
rwaASSERT(rwaStreamSourceParams_data, offsetof(rwaStreamSourceParams, data) == 0x40);

struct rwaStreamNodeParams
{
    RwUInt32 unk00;    /* 0x00 */
    RwUInt32 unk04;    /* 0x04 */
    RwUInt32 unk08;    /* 0x08 */
    RwUInt32 unk0C;    /* 0x0C */
    rwaQueueCallback *callback;    /* 0x10 */
    RwUInt32 unk14;    /* 0x14 */
    RwUInt32 unk18;    /* 0x18 */
    RwUInt32 source[15];    /* 0x1C */
};
rwaASSERT(rwaStreamNodeParams__sizeof, sizeof(rwaStreamNodeParams) == 0x58);
rwaASSERT(rwaStreamNodeParams_unk04, offsetof(rwaStreamNodeParams, unk04) == 0x4);
rwaASSERT(rwaStreamNodeParams_unk08, offsetof(rwaStreamNodeParams, unk08) == 0x8);
rwaASSERT(rwaStreamNodeParams_unk0C, offsetof(rwaStreamNodeParams, unk0C) == 0xC);
rwaASSERT(rwaStreamNodeParams_callback, offsetof(rwaStreamNodeParams, callback) == 0x10);
rwaASSERT(rwaStreamNodeParams_unk14, offsetof(rwaStreamNodeParams, unk14) == 0x14);
rwaASSERT(rwaStreamNodeParams_unk18, offsetof(rwaStreamNodeParams, unk18) == 0x18);
rwaASSERT(rwaStreamNodeParams_source, offsetof(rwaStreamNodeParams, source) == 0x1C);

struct rwaStreamPlayer
{
    volatile RwInt32 numPlaying;    /* 0x00 */
    volatile RwInt32 numPending;    /* 0x04 */
    RwInt32 state;    /* 0x08 */
    rwaStreamInfo *info;    /* 0x0C */
    RwUInt32 unk10;    /* 0x10 */
    rwaDecoder *decoder;    /* 0x14 */
    rwaLink sources;    /* 0x18 */
    rwaLink nodes;    /* 0x20 */
    rwaQueueCallback callback;    /* 0x28 */
    RwUInt32 unk30;    /* 0x30 */
    RwReal unk34;    /* 0x34 */
    RwInt32 mode;    /* 0x38 */
    volatile RwInt32 numBusy;    /* 0x3C */
    RwUInt32 unk40;    /* 0x40 */
    RwUInt32 unk44;    /* 0x44 */
    RwUInt32 unk48;    /* 0x48 */
    RwUInt32 unk4C;    /* 0x4C */
    void *unk50;    /* 0x50 */
    void *unk54;    /* 0x54 */
    void *unk58;    /* 0x58 */
};
rwaASSERT(rwaStreamPlayer__sizeof, sizeof(rwaStreamPlayer) == 0x5C);
rwaASSERT(rwaStreamPlayer_numPending, offsetof(rwaStreamPlayer, numPending) == 0x4);
rwaASSERT(rwaStreamPlayer_state, offsetof(rwaStreamPlayer, state) == 0x8);
rwaASSERT(rwaStreamPlayer_info, offsetof(rwaStreamPlayer, info) == 0xC);
rwaASSERT(rwaStreamPlayer_unk10, offsetof(rwaStreamPlayer, unk10) == 0x10);
rwaASSERT(rwaStreamPlayer_decoder, offsetof(rwaStreamPlayer, decoder) == 0x14);
rwaASSERT(rwaStreamPlayer_sources, offsetof(rwaStreamPlayer, sources) == 0x18);
rwaASSERT(rwaStreamPlayer_nodes, offsetof(rwaStreamPlayer, nodes) == 0x20);
rwaASSERT(rwaStreamPlayer_callback, offsetof(rwaStreamPlayer, callback) == 0x28);
rwaASSERT(rwaStreamPlayer_unk30, offsetof(rwaStreamPlayer, unk30) == 0x30);
rwaASSERT(rwaStreamPlayer_unk34, offsetof(rwaStreamPlayer, unk34) == 0x34);
rwaASSERT(rwaStreamPlayer_mode, offsetof(rwaStreamPlayer, mode) == 0x38);
rwaASSERT(rwaStreamPlayer_numBusy, offsetof(rwaStreamPlayer, numBusy) == 0x3C);
rwaASSERT(rwaStreamPlayer_unk40, offsetof(rwaStreamPlayer, unk40) == 0x40);
rwaASSERT(rwaStreamPlayer_unk44, offsetof(rwaStreamPlayer, unk44) == 0x44);
rwaASSERT(rwaStreamPlayer_unk48, offsetof(rwaStreamPlayer, unk48) == 0x48);
rwaASSERT(rwaStreamPlayer_unk4C, offsetof(rwaStreamPlayer, unk4C) == 0x4C);
rwaASSERT(rwaStreamPlayer_unk50, offsetof(rwaStreamPlayer, unk50) == 0x50);
rwaASSERT(rwaStreamPlayer_unk54, offsetof(rwaStreamPlayer, unk54) == 0x54);
rwaASSERT(rwaStreamPlayer_unk58, offsetof(rwaStreamPlayer, unk58) == 0x58);

struct rwaStream
{
    RwUInt32 pad00[859];
    RwUInt32 unkD6C;    /* 0xD6C */
    RwUInt32 unkD70;    /* 0xD70 */
    RwUInt32 unkD74;    /* 0xD74 */
    RwUInt32 padD78[10];
};
rwaASSERT(rwaStream__sizeof, sizeof(rwaStream) == 0xDA0);
rwaASSERT(rwaStream_unkD6C, offsetof(rwaStream, unkD6C) == 0xD6C);
rwaASSERT(rwaStream_unkD70, offsetof(rwaStream, unkD70) == 0xD70);
rwaASSERT(rwaStream_unkD74, offsetof(rwaStream, unkD74) == 0xD74);

struct rwaThread
{
    void *handle;    /* 0x00 */
    void **(*func)(void *data);    /* 0x04 */
    void *data;    /* 0x08 */
    RwInt32 priority;    /* 0x0C */
    RwUInt32 stackSize;    /* 0x10 */
};
rwaASSERT(rwaThread__sizeof, sizeof(rwaThread) == 0x14);
rwaASSERT(rwaThread_func, offsetof(rwaThread, func) == 0x4);
rwaASSERT(rwaThread_data, offsetof(rwaThread, data) == 0x8);
rwaASSERT(rwaThread_priority, offsetof(rwaThread, priority) == 0xC);
rwaASSERT(rwaThread_stackSize, offsetof(rwaThread, stackSize) == 0x10);

typedef struct RwaParamType {
    RwaUniqueID uniqueID;
    void* rangeMin;
    void* rangeMax;
    RwaType type;
    struct RwaParamType* paramType;
    RwUInt32 flags;
    RwLLLink link;
} RwaParamType; // size: 0x28

typedef struct RwaParamTypeHandle {
    union {
        RwaUUID* paramTypeUUID;
        RwaParamType* paramType;
        RwaParamType* paramTypePtr;
    } u;
    RwBool isUUID;
} RwaParamTypeHandle; // size: 0x8

struct _rwaParamTypeDef
{
    RwUInt32 pad00[6];
    RwaParamType *paramType;    /* 0x18 */
    RwUInt32 pad1C;
};
rwaASSERT(_rwaParamTypeDef__sizeof, sizeof(_rwaParamTypeDef) == 0x20);
rwaASSERT(_rwaParamTypeDef_paramType, offsetof(_rwaParamTypeDef, paramType) == 0x18);

struct rwaValueFreeList
{
    void *freeList;    /* 0x00 */
    RwUInt32 size;    /* 0x04 */
};
rwaASSERT(rwaValueFreeList__sizeof, sizeof(rwaValueFreeList) == 0x8);
rwaASSERT(rwaValueFreeList_size, offsetof(rwaValueFreeList, size) == 0x4);

struct RwaWaveFormat
{
    RwUInt32 sampleRate;    /* 0x00 */
    const RwaUUID *dataType;    /* 0x04 */
    RwUInt32 length;    /* 0x08 */
    RwUInt8 bitDepth;    /* 0x0C */
    RwUInt8 noChannels;    /* 0x0D */
    RwUInt8 pad0E[2];
    void *extra;    /* 0x10 */
    RwUInt32 extraSize;    /* 0x14 */
    RwUInt8 flags;    /* 0x18 */
    RwUInt8 unk19;    /* 0x19 */
    RwUInt8 pad1A[2];
};
rwaASSERT(RwaWaveFormat__sizeof, sizeof(RwaWaveFormat) == 0x1C);
rwaASSERT(RwaWaveFormat_dataType, offsetof(RwaWaveFormat, dataType) == 0x4);
rwaASSERT(RwaWaveFormat_length, offsetof(RwaWaveFormat, length) == 0x8);
rwaASSERT(RwaWaveFormat_bitDepth, offsetof(RwaWaveFormat, bitDepth) == 0xC);
rwaASSERT(RwaWaveFormat_noChannels, offsetof(RwaWaveFormat, noChannels) == 0xD);
rwaASSERT(RwaWaveFormat_extra, offsetof(RwaWaveFormat, extra) == 0x10);
rwaASSERT(RwaWaveFormat_extraSize, offsetof(RwaWaveFormat, extraSize) == 0x14);
rwaASSERT(RwaWaveFormat_flags, offsetof(RwaWaveFormat, flags) == 0x18);
rwaASSERT(RwaWaveFormat_unk19, offsetof(RwaWaveFormat, unk19) == 0x19);

struct rwaWaveFormatDesc
{
    const RwaUUID *dataType;    /* 0x00 */
    RwUInt8 bitDepth;    /* 0x04 */
    RwUInt8 noChannels;    /* 0x05 */
    RwUInt8 pad06[2];
    RwUInt32 minRate;    /* 0x08 */
    RwUInt32 maxRate;    /* 0x0C */
    RwUInt8 flags;    /* 0x10 */
    RwUInt8 pad11[3];
};
rwaASSERT(rwaWaveFormatDesc__sizeof, sizeof(rwaWaveFormatDesc) == 0x14);
rwaASSERT(rwaWaveFormatDesc_bitDepth, offsetof(rwaWaveFormatDesc, bitDepth) == 0x4);
rwaASSERT(rwaWaveFormatDesc_noChannels, offsetof(rwaWaveFormatDesc, noChannels) == 0x5);
rwaASSERT(rwaWaveFormatDesc_minRate, offsetof(rwaWaveFormatDesc, minRate) == 0x8);
rwaASSERT(rwaWaveFormatDesc_maxRate, offsetof(rwaWaveFormatDesc, maxRate) == 0xC);
rwaASSERT(rwaWaveFormatDesc_flags, offsetof(rwaWaveFormatDesc, flags) == 0x10);

struct RwaWaveDef
{
    RwaUniqueID uniqueID;    /* 0x00 */
    RwBool (*create)(RwaWave *wave, void *params);    /* 0x0C */
    void (*destroy)(RwaWave *wave);    /* 0x10 */
    void *unk14;    /* 0x14 */
    void *unk18;    /* 0x18 */
    RwBool (*busy)(void);    /* 0x1C */
    RwUInt32 pad20[19];
};
rwaASSERT(RwaWaveDef__sizeof, sizeof(RwaWaveDef) == 0x6C);
rwaASSERT(RwaWaveDef_create, offsetof(RwaWaveDef, create) == 0xC);
rwaASSERT(RwaWaveDef_destroy, offsetof(RwaWaveDef, destroy) == 0x10);
rwaASSERT(RwaWaveDef_unk14, offsetof(RwaWaveDef, unk14) == 0x14);
rwaASSERT(RwaWaveDef_unk18, offsetof(RwaWaveDef, unk18) == 0x18);
rwaASSERT(RwaWaveDef_busy, offsetof(RwaWaveDef, busy) == 0x1C);

struct RwaWave
{
    RwUInt32 pad00[3];
    RwaWaveDef *waveDef;    /* 0x0C */
    RwUInt32 pad10[15];
    RwUInt8 *buffer;    /* 0x4C */
    RwUInt32 pad50[3];
};
rwaASSERT(RwaWave__sizeof, sizeof(RwaWave) == 0x5C);
rwaASSERT(RwaWave_waveDef, offsetof(RwaWave, waveDef) == 0xC);
rwaASSERT(RwaWave_buffer, offsetof(RwaWave, buffer) == 0x4C);

struct rwaWaveDefFind
{
    const RwaUUID *uuid;    /* 0x00 */
    RwaWaveDef *found;    /* 0x04 */
};
rwaASSERT(rwaWaveDefFind__sizeof, sizeof(rwaWaveDefFind) == 0x8);
rwaASSERT(rwaWaveDefFind_found, offsetof(rwaWaveDefFind, found) == 0x4);

typedef struct RwaLLNode {
    struct RwaLLNode* prev;
    struct RwaLLNode* next;
    void* data;
} RwaLLNode; // size: 0xC

struct RwaFreeList
{
    RwUInt32 entrySize;    /* 0x00 */
    RwUInt32 entriesPerBlock;    /* 0x04 */
    RwUInt32 bitmapSize;    /* 0x08 */
    RwUInt32 alignment;    /* 0x0C */
    rwaLink blocks;    /* 0x10 */
    RwUInt32 flags;    /* 0x18 */
    rwaLink link;    /* 0x1C */
};
rwaASSERT(RwaFreeList__sizeof, sizeof(RwaFreeList) == 0x24);
rwaASSERT(RwaFreeList_entriesPerBlock, offsetof(RwaFreeList, entriesPerBlock) == 0x4);
rwaASSERT(RwaFreeList_bitmapSize, offsetof(RwaFreeList, bitmapSize) == 0x8);
rwaASSERT(RwaFreeList_alignment, offsetof(RwaFreeList, alignment) == 0xC);
rwaASSERT(RwaFreeList_blocks, offsetof(RwaFreeList, blocks) == 0x10);
rwaASSERT(RwaFreeList_flags, offsetof(RwaFreeList, flags) == 0x18);
rwaASSERT(RwaFreeList_link, offsetof(RwaFreeList, link) == 0x1C);

struct rwaFreeListBlock
{
    rwaLink link;    /* 0x00 */
    RwUInt8 bitmap[1];    /* 0x08 */
    RwUInt8 pad09[3];
};
rwaASSERT(rwaFreeListBlock__sizeof, sizeof(rwaFreeListBlock) == 0xC);
rwaASSERT(rwaFreeListBlock_bitmap, offsetof(rwaFreeListBlock, bitmap) == 0x8);

struct rwaPoolChunk
{
    rwaPoolChunk *next;    /* 0x00 */
    rwaPoolChunk *prev;    /* 0x04 */
    RwUInt32 size;    /* 0x08 */
};
rwaASSERT(rwaPoolChunk__sizeof, sizeof(rwaPoolChunk) == 0xC);
rwaASSERT(rwaPoolChunk_prev, offsetof(rwaPoolChunk, prev) == 0x4);
rwaASSERT(rwaPoolChunk_size, offsetof(rwaPoolChunk, size) == 0x8);

struct rwaPoolBlock
{
    RwUInt32 pad00[2];
    rwaPoolChunk *chunks;    /* 0x08 */
    rwaPoolChunk *end;    /* 0x0C */
};
rwaASSERT(rwaPoolBlock__sizeof, sizeof(rwaPoolBlock) == 0x10);
rwaASSERT(rwaPoolBlock_chunks, offsetof(rwaPoolBlock, chunks) == 0x8);
rwaASSERT(rwaPoolBlock_end, offsetof(rwaPoolBlock, end) == 0xC);

struct rwaPool
{
    RwUInt32 pad00[7];
};
rwaASSERT(rwaPool__sizeof, sizeof(rwaPool) == 0x1C);

struct RwaSymbolTable
{
    RwInt32 noParamTypes;    /* 0x00 */
    rwaLink list;    /* 0x04 */
};
rwaASSERT(RwaSymbolTable__sizeof, sizeof(RwaSymbolTable) == 0xC);
rwaASSERT(RwaSymbolTable_list, offsetof(RwaSymbolTable, list) == 0x4);

struct rwaWaveDict
{
    RwUInt32 pad00[3];
    rwaLink waves;    /* 0x0C */
    RwUInt32 pad14[7];
};
rwaASSERT(rwaWaveDict__sizeof, sizeof(rwaWaveDict) == 0x30);
rwaASSERT(rwaWaveDict_waves, offsetof(rwaWaveDict, waves) == 0xC);

struct rwaWaveNode
{
    RwUInt32 pad00;
    rwaWaveNode *next;    /* 0x04 */
    void *item;    /* 0x08 */
};
rwaASSERT(rwaWaveNode__sizeof, sizeof(rwaWaveNode) == 0xC);
rwaASSERT(rwaWaveNode_next, offsetof(rwaWaveNode, next) == 0x4);
rwaASSERT(rwaWaveNode_item, offsetof(rwaWaveNode, item) == 0x8);

struct rwaWaveItem
{
    RwUInt32 pad00[3];
    RwUInt32 id;    /* 0x0C */
};
rwaASSERT(rwaWaveItem__sizeof, sizeof(rwaWaveItem) == 0x10);
rwaASSERT(rwaWaveItem_id, offsetof(rwaWaveItem, id) == 0xC);

struct rwaOrientation
{
    RwaV3d right;    /* 0x00 */
    RwUInt32 pad0C;
    RwaV3d up;    /* 0x10 */
    RwUInt32 pad1C;
    RwaV3d at;    /* 0x20 */
};
rwaASSERT(rwaOrientation__sizeof, sizeof(rwaOrientation) == 0x2C);
rwaASSERT(rwaOrientation_up, offsetof(rwaOrientation, up) == 0x10);
rwaASSERT(rwaOrientation_at, offsetof(rwaOrientation, at) == 0x20);

struct rwaListenerState
{
    RwUInt32 pad00[19];
};
rwaASSERT(rwaListenerState__sizeof, sizeof(rwaListenerState) == 0x4C);

struct rwaListener
{
    RwUInt32 pad00[10];
    union {
        rwaListenerState state;
        struct {
            RwaV3d pos;
            RwaV3d vel;
            RwaV3d up;
            RwaV3d at;
            RwaV3d right;
            RwUInt32 unk64;
            RwUInt32 unk68;
            RwUInt32 unk6C;
            RwUInt32 dirty;
        };
    };    /* 0x28 */
};
rwaASSERT(rwaListener__sizeof, sizeof(rwaListener) == 0x74);

struct rwaParamDef
{
    RwUInt32 pad00[3];
    void *type;    /* 0x0C */
    RwUInt32 typeIndex;    /* 0x10 */
    void *handler;    /* 0x14 */
};
rwaASSERT(rwaParamDef__sizeof, sizeof(rwaParamDef) == 0x18);
rwaASSERT(rwaParamDef_type, offsetof(rwaParamDef, type) == 0xC);
rwaASSERT(rwaParamDef_typeIndex, offsetof(rwaParamDef, typeIndex) == 0x10);
rwaASSERT(rwaParamDef_handler, offsetof(rwaParamDef, handler) == 0x14);

struct rwaParamTables
{
    void *unk00;    /* 0x00 */
    rwaLink interfaces;    /* 0x04 */
    RwUInt16 noInputs;    /* 0x0C */
    RwUInt16 noOutputs;    /* 0x0E */
    rwaParamDef *inputs;    /* 0x10 */
    rwaParamDef *outputs;    /* 0x14 */
};
rwaASSERT(rwaParamTables__sizeof, sizeof(rwaParamTables) == 0x18);
rwaASSERT(rwaParamTables_interfaces, offsetof(rwaParamTables, interfaces) == 0x4);
rwaASSERT(rwaParamTables_noInputs, offsetof(rwaParamTables, noInputs) == 0xC);
rwaASSERT(rwaParamTables_noOutputs, offsetof(rwaParamTables, noOutputs) == 0xE);
rwaASSERT(rwaParamTables_inputs, offsetof(rwaParamTables, inputs) == 0x10);
rwaASSERT(rwaParamTables_outputs, offsetof(rwaParamTables, outputs) == 0x14);

struct _rwaParamID
{
    RwaUniqueID uniqueID;    /* 0x00 */
    RwaParamTypeHandle type;    /* 0x0C */
};
rwaASSERT(_rwaParamID__sizeof, sizeof(_rwaParamID) == 0x14);
rwaASSERT(_rwaParamID_type, offsetof(_rwaParamID, type) == 0xC);

struct RwaObjInterfaceDef
{
    RwaUniqueID uniqueID;    /* 0x00 */
    _rwaParamID *outputs;    /* 0x0C */
    _rwaParamID *inputs;    /* 0x10 */
    RwUInt16 noInputs;    /* 0x14 */
    RwUInt16 noOutputs;    /* 0x16 */
    RwUInt32 flags;    /* 0x18 */
    void (*unregister)(RwaObjInterfaceDef *def);    /* 0x1C */
    rwaLink link;    /* 0x20 */
};
rwaASSERT(RwaObjInterfaceDef__sizeof, sizeof(RwaObjInterfaceDef) == 0x28);
rwaASSERT(RwaObjInterfaceDef_outputs, offsetof(RwaObjInterfaceDef, outputs) == 0xC);
rwaASSERT(RwaObjInterfaceDef_inputs, offsetof(RwaObjInterfaceDef, inputs) == 0x10);
rwaASSERT(RwaObjInterfaceDef_noInputs, offsetof(RwaObjInterfaceDef, noInputs) == 0x14);
rwaASSERT(RwaObjInterfaceDef_noOutputs, offsetof(RwaObjInterfaceDef, noOutputs) == 0x16);
rwaASSERT(RwaObjInterfaceDef_flags, offsetof(RwaObjInterfaceDef, flags) == 0x18);
rwaASSERT(RwaObjInterfaceDef_unregister, offsetof(RwaObjInterfaceDef, unregister) == 0x1C);
rwaASSERT(RwaObjInterfaceDef_link, offsetof(RwaObjInterfaceDef, link) == 0x20);

struct rwaIfaceSearch
{
    void *iface;    /* 0x00 */
    void *list;    /* 0x04 */
};
rwaASSERT(rwaIfaceSearch__sizeof, sizeof(rwaIfaceSearch) == 0x8);
rwaASSERT(rwaIfaceSearch_list, offsetof(rwaIfaceSearch, list) == 0x4);

struct rwaModule
{
    RwBool (*open)(void);    /* 0x00 */
    void (*close)(void);    /* 0x04 */
};
rwaASSERT(rwaModule__sizeof, sizeof(rwaModule) == 0x8);
rwaASSERT(rwaModule_close, offsetof(rwaModule, close) == 0x4);

struct rwaVoiceWave
{
    RwUInt32 pad00[21];
    RwUInt32 flags;    /* 0x54 */
};
rwaASSERT(rwaVoiceWave__sizeof, sizeof(rwaVoiceWave) == 0x58);
rwaASSERT(rwaVoiceWave_flags, offsetof(rwaVoiceWave, flags) == 0x54);

struct rwaCodecInfo
{
    RwUInt32 pad00[5];
};
rwaASSERT(rwaCodecInfo__sizeof, sizeof(rwaCodecInfo) == 0x14);

struct rwaContainerSource
{
    RwUInt32 pad00[9];
};
rwaASSERT(rwaContainerSource__sizeof, sizeof(rwaContainerSource) == 0x24);

struct rwaConversion
{
    RwUInt32 pad00[21];
};
rwaASSERT(rwaConversion__sizeof, sizeof(rwaConversion) == 0x54);

struct rwaNodeSet
{
    RwUInt32 unk00;    /* 0x00 */
    RwUInt32 numEntries;    /* 0x04 */
    RwUInt32 numNodes;    /* 0x08 */
};
rwaASSERT(rwaNodeSet__sizeof, sizeof(rwaNodeSet) == 0xC);
rwaASSERT(rwaNodeSet_numEntries, offsetof(rwaNodeSet, numEntries) == 0x4);
rwaASSERT(rwaNodeSet_numNodes, offsetof(rwaNodeSet, numNodes) == 0x8);

struct rwaNodeSetEntry
{
    RwUInt32 track;    /* 0x00 */
    RwUInt32 channel;    /* 0x04 */
    RwUInt32 kind;    /* 0x08 */
    RwReal value;    /* 0x0C */
};
rwaASSERT(rwaNodeSetEntry__sizeof, sizeof(rwaNodeSetEntry) == 0x10);
rwaASSERT(rwaNodeSetEntry_channel, offsetof(rwaNodeSetEntry, channel) == 0x4);
rwaASSERT(rwaNodeSetEntry_kind, offsetof(rwaNodeSetEntry, kind) == 0x8);
rwaASSERT(rwaNodeSetEntry_value, offsetof(rwaNodeSetEntry, value) == 0xC);

struct rwaConversionParams
{
    RwUInt32 pad00[2];
    rwaStreamInfo *info;    /* 0x08 */
    RwUInt32 pad0C[4];
    RwaWaveFormat from;    /* 0x1C */
    RwaWaveFormat to;    /* 0x38 */
    RwUInt32 unk54;    /* 0x54 */
    RwUInt32 unk58;    /* 0x58 */
    RwUInt32 pad5C;
};
rwaASSERT(rwaConversionParams__sizeof, sizeof(rwaConversionParams) == 0x60);
rwaASSERT(rwaConversionParams_info, offsetof(rwaConversionParams, info) == 0x8);
rwaASSERT(rwaConversionParams_from, offsetof(rwaConversionParams, from) == 0x1C);
rwaASSERT(rwaConversionParams_to, offsetof(rwaConversionParams, to) == 0x38);
rwaASSERT(rwaConversionParams_unk54, offsetof(rwaConversionParams, unk54) == 0x54);
rwaASSERT(rwaConversionParams_unk58, offsetof(rwaConversionParams, unk58) == 0x58);

struct rwaPlaySettings
{
    RwUInt32 pad00[2];
};
rwaASSERT(rwaPlaySettings__sizeof, sizeof(rwaPlaySettings) == 0x8);

struct rwaContainerCommand
{
    RwUInt32 flags;    /* 0x00 */
    RwInt32 mode;    /* 0x04 */
    const struct rwaContainerSource *source;    /* 0x08 */
    rwaPlaySettings *settings;    /* 0x0C */
    RwReal rate;    /* 0x10 */
    RwUInt32 unk14;    /* 0x14 */
    struct rwaNodeSetEntry entries[0xC0];    /* 0x18 */
};
rwaASSERT(rwaContainerCommand__sizeof, sizeof(rwaContainerCommand) == 0xC18);
rwaASSERT(rwaContainerCommand_mode, offsetof(rwaContainerCommand, mode) == 0x4);
rwaASSERT(rwaContainerCommand_source, offsetof(rwaContainerCommand, source) == 0x8);
rwaASSERT(rwaContainerCommand_settings, offsetof(rwaContainerCommand, settings) == 0xC);
rwaASSERT(rwaContainerCommand_rate, offsetof(rwaContainerCommand, rate) == 0x10);
rwaASSERT(rwaContainerCommand_unk14, offsetof(rwaContainerCommand, unk14) == 0x14);
rwaASSERT(rwaContainerCommand_entries, offsetof(rwaContainerCommand, entries) == 0x18);

struct rwaStreamContainer
{
    rwaStreamPlayer player;    /* 0x00 */
    rwaNodeSet set;    /* 0x5C */
    rwaContainerCommand command;    /* 0x68 */
    RwInt32 states[3];    /* 0xC80 */
};
rwaASSERT(rwaStreamContainer__sizeof, sizeof(rwaStreamContainer) == 0xC8C);
rwaASSERT(rwaStreamContainer_set, offsetof(rwaStreamContainer, set) == 0x5C);
rwaASSERT(rwaStreamContainer_command, offsetof(rwaStreamContainer, command) == 0x68);
rwaASSERT(rwaStreamContainer_states, offsetof(rwaStreamContainer, states) == 0xC80);

struct RwaAllocBitmap
{
    RwUInt8 *bits;    /* 0x00 */
    RwUInt8 pad04[5];
    RwUInt8 flags;    /* 0x09 */
    RwUInt8 pad0A[14];
};
rwaASSERT(RwaAllocBitmap__sizeof, sizeof(RwaAllocBitmap) == 0x18);
rwaASSERT(RwaAllocBitmap_flags, offsetof(RwaAllocBitmap, flags) == 0x9);

struct rwaConverterInfo
{
    const rwaWaveFormatDesc *from;    /* 0x00 */
    const rwaWaveFormatDesc *to;    /* 0x04 */
    RwUInt8 flags;    /* 0x08 */
    RwUInt8 pad09[3];
};
rwaASSERT(rwaConverterInfo__sizeof, sizeof(rwaConverterInfo) == 0xC);
rwaASSERT(rwaConverterInfo_to, offsetof(rwaConverterInfo, to) == 0x4);
rwaASSERT(rwaConverterInfo_flags, offsetof(rwaConverterInfo, flags) == 0x8);

struct rwaConverter
{
    const rwaConverterInfo *info;    /* 0x00 */
    RwUInt32 pad04;
};
rwaASSERT(rwaConverter__sizeof, sizeof(rwaConverter) == 0x8);

struct rwaConversionPath
{
    RwaWaveFormat from;    /* 0x00 */
    RwaWaveFormat to;    /* 0x1C */
    RwUInt32 count;    /* 0x38 */
    const rwaConverter *steps[4];    /* 0x3C */
};
rwaASSERT(rwaConversionPath__sizeof, sizeof(rwaConversionPath) == 0x4C);
rwaASSERT(rwaConversionPath_to, offsetof(rwaConversionPath, to) == 0x1C);
rwaASSERT(rwaConversionPath_count, offsetof(rwaConversionPath, count) == 0x38);
rwaASSERT(rwaConversionPath_steps, offsetof(rwaConversionPath, steps) == 0x3C);

struct rwaNodeAllocator
{
    void *(*alloc)(RwUInt32 size, void *data);    /* 0x00 */
    void (*free)(void *mem, void *data);    /* 0x04 */
    void *data;    /* 0x08 */
};
rwaASSERT(rwaNodeAllocator__sizeof, sizeof(rwaNodeAllocator) == 0xC);
rwaASSERT(rwaNodeAllocator_free, offsetof(rwaNodeAllocator, free) == 0x4);
rwaASSERT(rwaNodeAllocator_data, offsetof(rwaNodeAllocator, data) == 0x8);

struct rwaSplitterParams
{
    RwUInt32 unk00;    /* 0x00 */
    RwUInt32 unk04;    /* 0x04 */
    rwaStreamInfo *info;    /* 0x08 */
    RwUInt32 noBuffers;    /* 0x0C */
    RwUInt32 pad10[3];
    rwaNodeAllocator *allocator;    /* 0x1C */
};
rwaASSERT(rwaSplitterParams__sizeof, sizeof(rwaSplitterParams) == 0x20);
rwaASSERT(rwaSplitterParams_unk04, offsetof(rwaSplitterParams, unk04) == 0x4);
rwaASSERT(rwaSplitterParams_info, offsetof(rwaSplitterParams, info) == 0x8);
rwaASSERT(rwaSplitterParams_noBuffers, offsetof(rwaSplitterParams, noBuffers) == 0xC);
rwaASSERT(rwaSplitterParams_allocator, offsetof(rwaSplitterParams, allocator) == 0x1C);

struct rwaSplitterTrack
{
    rwaSplitterState *owner;    /* 0x00 */
    RwUInt32 index;    /* 0x04 */
};
rwaASSERT(rwaSplitterTrack__sizeof, sizeof(rwaSplitterTrack) == 0x8);
rwaASSERT(rwaSplitterTrack_index, offsetof(rwaSplitterTrack, index) == 0x4);

struct rwaSplitterState
{
    rwaStreamNodeState node;    /* 0x00 */
    void *objInterface;    /* 0x30 */
    rwaNodeAllocator *allocator;    /* 0x34 */
    rwaStreamInfo info;    /* 0x38 */
    RwUInt32 noBuffers;    /* 0x64 */
    RwBool starved;    /* 0x68 */
    RwUInt32 *masks;    /* 0x6C */
    rwaQueueItem **items;    /* 0x70 */
    RwUInt32 *readIndex;    /* 0x74 */
    RwUInt32 *writeIndex;    /* 0x78 */
    rwaQueueItem *trackItems;    /* 0x7C */
    rwaQueueCallback *callbacks;    /* 0x80 */
    rwaSplitterTrack *tracks;    /* 0x84 */
};
rwaASSERT(rwaSplitterState__sizeof, sizeof(rwaSplitterState) == 0x88);
rwaASSERT(rwaSplitterState_objInterface, offsetof(rwaSplitterState, objInterface) == 0x30);
rwaASSERT(rwaSplitterState_allocator, offsetof(rwaSplitterState, allocator) == 0x34);
rwaASSERT(rwaSplitterState_info, offsetof(rwaSplitterState, info) == 0x38);
rwaASSERT(rwaSplitterState_noBuffers, offsetof(rwaSplitterState, noBuffers) == 0x64);
rwaASSERT(rwaSplitterState_starved, offsetof(rwaSplitterState, starved) == 0x68);
rwaASSERT(rwaSplitterState_masks, offsetof(rwaSplitterState, masks) == 0x6C);
rwaASSERT(rwaSplitterState_items, offsetof(rwaSplitterState, items) == 0x70);
rwaASSERT(rwaSplitterState_readIndex, offsetof(rwaSplitterState, readIndex) == 0x74);
rwaASSERT(rwaSplitterState_writeIndex, offsetof(rwaSplitterState, writeIndex) == 0x78);
rwaASSERT(rwaSplitterState_trackItems, offsetof(rwaSplitterState, trackItems) == 0x7C);
rwaASSERT(rwaSplitterState_callbacks, offsetof(rwaSplitterState, callbacks) == 0x80);
rwaASSERT(rwaSplitterState_tracks, offsetof(rwaSplitterState, tracks) == 0x84);

struct rwaSplitter
{
    rwaInstance inst;    /* 0x00 */
    rwaSplitterState state;    /* 0x28 */
};
rwaASSERT(rwaSplitter__sizeof, sizeof(rwaSplitter) == 0xB0);
rwaASSERT(rwaSplitter_state, offsetof(rwaSplitter, state) == 0x28);

struct rwaStreamVoiceParams
{
    RwUInt32 unk00;    /* 0x00 */
    RwUInt32 unk04;    /* 0x04 */
    RwUInt32 pad08;
    RwUInt32 noEntries;    /* 0x0C */
    RwUInt32 pad10[3];
    rwaStreamTrack *track;    /* 0x1C */
    rwaNodeAllocator *allocator;    /* 0x20 */
    void (*done)(rwaDecoder *obj, void *data);    /* 0x24 */
    void *doneData;    /* 0x28 */
    RwBool mono;    /* 0x2C */
};
rwaASSERT(rwaStreamVoiceParams__sizeof, sizeof(rwaStreamVoiceParams) == 0x30);
rwaASSERT(rwaStreamVoiceParams_unk04, offsetof(rwaStreamVoiceParams, unk04) == 0x4);
rwaASSERT(rwaStreamVoiceParams_noEntries, offsetof(rwaStreamVoiceParams, noEntries) == 0xC);
rwaASSERT(rwaStreamVoiceParams_track, offsetof(rwaStreamVoiceParams, track) == 0x1C);
rwaASSERT(rwaStreamVoiceParams_allocator, offsetof(rwaStreamVoiceParams, allocator) == 0x20);
rwaASSERT(rwaStreamVoiceParams_done, offsetof(rwaStreamVoiceParams, done) == 0x24);
rwaASSERT(rwaStreamVoiceParams_doneData, offsetof(rwaStreamVoiceParams, doneData) == 0x28);
rwaASSERT(rwaStreamVoiceParams_mono, offsetof(rwaStreamVoiceParams, mono) == 0x2C);

struct rwaHandleList
{
    RwUInt32 pad00[2];
};
rwaASSERT(rwaHandleList__sizeof, sizeof(rwaHandleList) == 0x8);

struct rwaStreamVoiceState
{
    RwUInt32 pad00[166];
};
rwaASSERT(rwaStreamVoiceState__sizeof, sizeof(rwaStreamVoiceState) == 0x298);

#define rwaDecoderFromLink(_link) ((rwaDecoder *)((RwUInt8 *)(_link) - 0x2C))
#define rwaLog2(_x) \
    (((_x) & 0xFFFF0000) ? \
     rwaLog2_16((RwInt32)(_x) >> 16) + 16 : \
     rwaLog2_16(_x))
#define rwaLog2_16(_v) \
    (((_v) & 0xFF00) ? \
     _rwMsbBit[(RwInt32)(_v) >> 8] + 7 : \
     _rwMsbBit[(_v)] - 1)
#define rwaNUMCONVERTERS 51
#define rwaObjDefSetStateAlignment(_def, _align) \
    ((_def)->flags |= rwaLog2(_align) << 28)
#define rwaPow2(_n) \
    ((RwUInt16)(1 << sub_242150(_n)) ? (RwUInt16)(1 << sub_242150(_n)) : 1)
#define rwaWaveFormatInit(_format) \
    (_format)->sampleRate = 0; \
    (_format)->dataType = NULL; \
    (_format)->length = 0; \
    (_format)->bitDepth = 0; \
    (_format)->noChannels = 0; \
    (_format)->flags = 0; \
    (_format)->unk19 = 0; \
    (_format)->extra = NULL; \
    (_format)->extraSize = 0

#endif /* RWAINT_H */
