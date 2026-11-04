/* RenderWare Audio headers from codfh-win32 by L2KSolkov (https://github.com/L2KSolkov/codfh-win32, src/rwa/rwaint.h), used with credit.
 * P7c: the codfh revision of 2026-11-04, kept beside the older rwa.h / rwaint.h that the installed tier 1 files use. */
/*
 * codfh-match: RenderWare Audio internals shared by the library units, named by
 * offset; see rwa.h.
 */
#ifndef RWAINT_H
#define RWAINT_H

#include "rwa2.h"

typedef struct rwaLink rwaLink;
struct rwaLink
{
    rwaLink        *next;
    rwaLink        *prev;
};


/* The emitter's sound state (0x64 bytes at +0x28). */
typedef struct rwaSound rwaSound;
struct rwaSound
{
    rwaSound       *next;       /* 0x28 */
    rwaSound       *prev;       /* 0x2C */
    RwaObjHandle   *handle;     /* 0x30 */
    RwUInt32        flags;      /* 0x34 */
    RwUInt32        unk38;
    volatile RwReal unk3C;      /* 22050 */
    RwUInt32        unk40;      /* 1.0 */
    RwUInt32        unk44;
    RwUInt32        unk48;
    RwUInt32        unk4C;
    void           *unk50;
    RwUInt32        unk54;
    RwaV3d          unk58;
    RwaV3d          unk64;
    RwUInt32        unk70;      /* 1.0 */
    RwUInt32        unk74;      /* 10000 */
    RwUInt32        unk78;      /* 0.5 */
    RwUInt32        unk7C;      /* 0.5 */
    RwUInt32        unk80;
    RwUInt32        unk84;
    RwUInt32        unk88;
};

typedef struct rwaEmitter rwaEmitter;
struct rwaEmitter
{
    RwUInt8         unk00[0x28];
    rwaSound        sound;
};

typedef struct rwaWave rwaWave;
struct rwaWave
{
    RwUInt8         unk00[0x2C];
    RwUInt32        unk2C;
};

typedef struct rwaInstance rwaInstance;

/* An object definition (Xbox layout; the PS2 header's RwaObjDef differs). */
typedef struct rwaObjDef rwaObjDef;
struct rwaObjDef
{
    RwaUniqueID     uniqueID;
    RwUInt32        flags0C;    /* 0x40: instance free list, 0x80: pool */
    rwaLink         children;   /* 0x10 */
    rwaLink         sibling;    /* 0x18 */
    rwaLink         waveDefs;   /* 0x20 */
    RwBool        (*create)(rwaInstance *obj, rwaInstance *parent, void *params,
                            void *a, void *b);              /* 0x28 */
    /* RwaObjExecuteFunc in rwacore_ps2.h; installed callbacks return their object. */
    rwaInstance * (*unk2C)(rwaInstance *obj);
    rwaInstance * (*unk30)(rwaInstance *obj);
    void          (*unlink)(rwaInstance *obj, void *a, void *b);   /* 0x34 */
    void          (*unregister)(struct rwaObjDef *def); /* 0x38 */
    void          (*query)(void *obj, void *params);    /* 0x3C */
    /* 0x40: packed state size and log2 alignment. The names stateSize and
     * stateAlign are in rwacore_ps2.h; the Xbox packing is established by
     * 23F300's masks and 23DF00's matched bitfield initialization. */
    union
    {
        RwUInt32 flags;
        struct
        {
            RwUInt32 stateSize : 28;
            RwUInt32 stateAlign : 4;
        } unk40;
    };
    RwUInt16        paramsSize; /* 0x44: creation parameters */
    RwUInt16        maxInstances;   /* 0x46 */
    void           *instances;  /* 0x48 */
    void          **interfaces; /* 0x4C */
    void           *unk50;
    void           *unk54;
    RwUInt32        hint;       /* 0x58 */
};

/* An object instance (0x28 bytes, RwaObj in the PS2 header). Its state,
 * an extra word (flag 2) and its parameter tables (flag 1) follow it. */
struct rwaInstance
{
    rwaObjDef      *def;
    rwaLink         link;       /* 0x04: all instances */
    rwaLink         subObjs;    /* 0x0C */
    rwaLink         subObjLink; /* 0x14: in the parent's subObjs */
    RwUInt32        flags;      /* 0x1C */
    RwUInt8        *state;      /* 0x20 */
    rwaInstance    *parent;     /* 0x24 */
};

/* An instance search's parameters. */
typedef struct rwaInstSearch rwaInstSearch;
struct rwaInstSearch
{
    rwaObjDef      *def;
    RwUInt32        found;
};

typedef struct rwaInstFind rwaInstFind;
struct rwaInstFind
{
    rwaInstance    *found;
    rwaObjDef      *def;
};

/* An interface's parameter accessor (8 bytes). */
typedef struct rwaParamFunc rwaParamFunc;
struct rwaParamFunc
{
    RwBool        (*func)(rwaInstance *obj, RwUInt32 param, void *data);
    RwUInt16        param;
};

/* An object interface: its set and get accessor tables. */
typedef struct rwaObjInterface rwaObjInterface;
struct rwaObjInterface
{
    struct RwaObjInterfaceDef *def;
    rwaParamFunc   *setters;
    rwaParamFunc   *getters;
    RwUInt32        refCount;   /* 0x0C */
    RwUInt32        flags;      /* 0x10: 1 caller's storage */
    rwaLink         link;       /* 0x14: in the parameter tables' list */
};

/* An object handle (0x18 bytes; the PS2 header's has an object link). */
struct RwaObjHandle
{
    rwaInstance    *obj;
    rwaObjInterface *objInterface;
    rwaLink         link;       /* 0x08: all handles */
    RwUInt32        flags;      /* 0x10: 1 caller's, 2 counted */
    RwUInt32        refCount;   /* 0x14 */
};

/* Handle creation parameters. */
typedef struct rwaHandleParams rwaHandleParams;
struct rwaHandleParams
{
    RwUInt32        flags;
    RwUInt32        unk04;
    RwUInt32        unk08;
    RwUInt32        unk0C;
    RwUInt32        unk10;
    RwaObjHandle   *handle;     /* 0x14: caller's storage */
};

/* Handle release parameters. */
typedef struct rwaReleaseParams rwaReleaseParams;
struct rwaReleaseParams
{
    void           *a;
    void           *b;
    RwaObjHandle   *handle;
    rwaObjInterface *objInterface;
};

#define rwaObjDefGetStateSize(_def) ((_def)->flags & 0x0FFFFFFF)

/* The log2 table (_rwMsbBit[v] = 1 + the index of v's top bit) and the
 * lowest set bit of a size (sub_242150). */
extern const RwInt8 _rwMsbBit[];
extern RwUInt8 sub_242150(RwUInt32 value);

/* The power of two of a size's lowest set bit, at least 1; each use calls
 * sub_242150 again. */
#define rwaPow2(_n)                                                         \
    ((RwUInt16)(1 << sub_242150(_n)) ? (RwUInt16)(1 << sub_242150(_n)) : 1)

#define rwaLog2_16(_v)                                                      \
    (((_v) & 0xFF00) ?                                                      \
     _rwMsbBit[(RwInt32)(_v) >> 8] + 7 :                                  \
     _rwMsbBit[(_v)] - 1)

#define rwaLog2(_x)                                                         \
    (((_x) & 0xFFFF0000) ?                                                  \
     rwaLog2_16((RwInt32)(_x) >> 16) + 16 :                                 \
     rwaLog2_16(_x))

/* A definition's state alignment, as log2 in the flags' top four bits. */
#define rwaObjDefSetStateAlignment(_def, _align)                            \
    ((_def)->flags |= rwaLog2(_align) << 28)

/* The instance's extra word (flag 2), after its state. */
#define rwaInstGetExtra(_obj)                                               \
    ((_obj)->state ?                                                        \
     (RwUInt32 *)((_obj)->state + rwaObjDefGetStateSize((_obj)->def)) :     \
     (RwUInt32 *)((_obj) + 1))

/* An input's connection list (0x0C bytes); outputs have a bare rwaLink. */
typedef struct rwaInput rwaInput;
struct rwaInput
{
    rwaLink         conns;
    void           *unk08;
};

/* An instance's parameter tables, after its state. */
typedef struct rwaInstParams rwaInstParams;
struct rwaInstParams
{
    rwaInput       *inputs;
    rwaLink        *outputs;
};

/* A parameter connection (0x1C bytes): in its output's list at 0x0C and
 * its input's at 0x14. */
typedef struct rwaConnection rwaConnection;
struct rwaConnection
{
    RwUInt8         unk00[0x0C];
    rwaLink         outLink;
    rwaLink         inLink;
};

/* A stream's sample format. */
typedef struct rwaStreamFormat rwaStreamFormat;
struct rwaStreamFormat
{
    RwUInt32        rate;
    RwUInt8         unk04[0x09];
    RwUInt8         channels;   /* 0x0D */
    RwUInt8         unk0E[0x0A];
    RwUInt32        flags;      /* 0x18: 4 mono */
};

/* A stream track (0x28 bytes). */
typedef struct rwaStreamTrack rwaStreamTrack;
struct rwaStreamTrack
{
    RwUInt8         unk00[0x0C];
    RwUInt32        unk0C;
    RwUInt32        unk10;
    rwaStreamFormat *format;    /* 0x14 */
    RwUInt16        unk18;
    RwUInt16        unk1A;
    RwUInt16        unk1C;
    RwUInt16        pad1E;
    RwUInt32        unk20;
    RwUInt32        unk24;
};

/* A stream marker (0x20 bytes). */
typedef struct rwaStreamMarker rwaStreamMarker;
struct rwaStreamMarker
{
    const RwaUUID  *uuid;
    const RwChar   *name;       /* 0x04 */
    RwUInt8         unk08[0x04];
    RwUInt32       *unk0C;
    RwUInt32        noCues;     /* 0x10 */
    struct rwaMarkerCue *cues;  /* 0x14 */
    RwUInt32        unk18;
    RwUInt32        unk1C;
};

/* A marker's cue (0x14 bytes). */
typedef struct rwaMarkerCue rwaMarkerCue;
struct rwaMarkerCue
{
    RwaUniqueID     uniqueID;
    RwUInt32        unk0C;
    RwUInt32        unk10;
};

/* A stream file's description. */
typedef struct rwaStreamInfo rwaStreamInfo;
struct rwaStreamInfo
{
    RwUInt8         unk00[0x0C];
    RwUInt32        unk0C;
    rwaStreamMarker *markers;   /* 0x10 */
    RwUInt32        noTracks;   /* 0x14 */
    rwaStreamTrack *tracks;     /* 0x18 */
    RwUInt32        unk1C;
    RwUInt32        unk20;
    RwUInt32        unk24;
    RwUInt32        unk28;
};

/* A stream position (0x10 bytes): the value (a marker, its index or a
 * position), its kind, the position and the position's unit. */
typedef struct rwaStreamPosition rwaStreamPosition;
struct rwaStreamPosition
{
    RwUInt32        value;
    RwInt32         type;       /* 1, 2: marker lookups, 3: index */
    RwUInt32        pos;        /* RwReal seconds when unit is 1 */
    RwInt32         unit;
};

/* A stream region (0x14 bytes). */
typedef struct rwaStreamRegion rwaStreamRegion;
struct rwaStreamRegion
{
    rwaStreamMarker *marker;
    RwUInt32        unk04;
    RwUInt32        unk08;
    RwUInt32        unk0C;
    RwUInt32        flags;      /* 0x10 */
};

typedef struct rwaQueueItem rwaQueueItem;
typedef struct rwaDecoder rwaDecoder;

/* A play queue item's completion callback. */
typedef struct rwaQueueCallback rwaQueueCallback;
struct rwaQueueCallback
{
    void          (*func)(rwaQueueItem *item, rwaDecoder *obj);
    rwaDecoder     *data;
};

/* A play queue item (0x14 bytes). */
struct rwaQueueItem
{
    RwUInt32        flags;      /* 8: report the position */
    RwUInt8        *unk04;      /* its data */
    RwUInt32        unk08;
    rwaStreamPosition *position;
    rwaQueueCallback *callback;
};

/* A play queue: a ring of items (the first, how many, the ring's size). */
typedef struct rwaQueue rwaQueue;
struct rwaQueue
{
    RwUInt8         head;
    RwUInt8         count;
    RwUInt8         unk02;
    RwUInt8         size;
    rwaQueueItem   *items;      /* 0x04 */
};

/* The state every stream node shares (its object's +0x28). */
typedef struct rwaStreamNodeState rwaStreamNodeState;
struct rwaStreamNodeState
{
    void           *iface;      /* the stream node interface */
    rwaLink         link;       /* 0x04: in its container's node list */
    RwInt32         unk0C;
    struct rwaDecoder *next;    /* 0x10 */
    RwUInt32        unk14;
    RwUInt32        unk18;
    RwInt32         unk1C;
    RwInt32         mode;       /* 0x20: 1, 2, 3 */
    RwUInt32        numCreated; /* 0x24 */
    RwInt32         status;     /* 0x28 */
    rwaQueueCallback *callback; /* 0x2C */
};

/* A stream cache read's completion callback. */
typedef void (*rwaCacheCallBack)(void *file, RwUInt32 bytes, RwInt32 status, RwInt32 event,
                                 void *data);

/* A stream cache read request (0x3C bytes). */
typedef struct rwaCacheRequest rwaCacheRequest;
struct rwaCacheRequest
{
    rwaLink         link;       /* in the queue */
    RwInt32         state;      /* 1 idle, 2 queued, 3 active, 4 reading, 5 seeking */
    RwUInt32        offset;     /* 0x0C */
    RwUInt32        size;
    RwUInt32        remaining;  /* 0x14 */
    RwUInt8        *dest;
    void           *file;       /* 0x1C */
    RwUInt32        unk20;
    RwUInt32        unk24;
    RwUInt32        readOffset; /* 0x28 */
    RwUInt32        readSize;
    RwUInt8        *readBuffer; /* 0x30 */
    rwaCacheCallBack callback;
    void           *data;       /* 0x38 */
};

/* A stream cache file. */
typedef struct rwaCacheFile rwaCacheFile;
struct rwaCacheFile
{
    RwInt32         type;       /* 2: opened through the file system */
    RwUInt32        unk04;
    RwUInt32        unk08;
    void           *file;       /* 0x0C */
};

/* A stream cache client (0x58 bytes): its file, request and callback. */
typedef struct rwaCacheClient rwaCacheClient;
struct rwaCacheClient
{
    rwaCacheFile   *holder;
    rwaCacheRequest request;    /* 0x04 */
    RwInt32         mode;       /* 0x40: 1 open, 2 header, 3 read */
    rwaCacheCallBack callback;  /* 0x44 */
    void           *data;
    RwUInt32        unk4C;
    RwUInt32       *header;     /* 0x50: three words */
    RwBool          seeking;    /* 0x54 */
};

/* A stream source's read cache (0x4C bytes at its node state's +0x8C). */
typedef struct rwaReadRequest rwaReadRequest;
struct rwaReadRequest
{
    RwUInt32        e;
    void           *dest;       /* 0x04 */
    RwUInt32        size;       /* 0x08 */
    RwUInt32        offset;     /* 0x0C */
    RwInt32         kind;       /* 0x10 */
};

typedef struct rwaReadCache rwaReadCache;
struct rwaReadCache
{
    rwaReadRequest *requests;   /* a ring of them */
    RwUInt16        size;       /* 0x04 */
    RwUInt16        unk06;      /* the next written */
    RwUInt16        unk08;      /* the next read */
    RwUInt16        unk0A;      /* full */
    void           *semaphore;  /* 0x0C */
    RwUInt8         unk10[0x3C];
};

/* A stream source's node state (0x1AC bytes at +0x5C): the stream file
 * read through the stream cache into two rings of chunks (the playing
 * chunks and the look-ahead), and its play queue. */
typedef struct rwaDecoderState rwaDecoderState;
struct rwaDecoderState
{
    RwUInt32        unk00;
    rwaCacheFile   *file;       /* 0x04 */
    RwUInt8         unk08[0x20];
    rwaCacheClient  client;     /* 0x28 */
    RwUInt32        header;     /* 0x80: 2061, 2062, 2063 */
    RwUInt32        unk84;
    RwUInt32        unk88;
    rwaReadCache    cache;      /* 0x8C */
    RwUInt32        unkD8;
    volatile RwUInt32 unkDC;    /* the next read: destination */
    volatile RwUInt32 unkE0;    /* size */
    volatile RwUInt32 unkE4;    /* the next seek */
    volatile RwUInt32 requests; /* 0xE8: 1, 4 */
    RwInt32         loops;      /* 0xEC: -1 forever */
    RwUInt32        unkF0;
    RwUInt32        unkF4;
    RwUInt32        unkF8;
    RwUInt32        unkFC;
    RwUInt32        unk100;
    void         *(*alloc)(RwUInt32 size, void *data);  /* 0x104 */
    void          (*free)(void *mem, void *data);       /* 0x108 */
    void           *allocData;  /* 0x10C */
    void          (*notify)(struct rwaDecoderState *state, RwInt32 code, void *data);   /* 0x110 */
    void           *notifyData; /* 0x114 */
    rwaStreamInfo  *info;       /* 0x118 */
    RwUInt32        infoSize;   /* 0x11C: the file's header (0x14 bytes) */
    RwUInt32        unk120;
    RwUInt32        uuidSize;   /* 0x124 */
    RwUInt32        unk128;
    RwUInt32        nameSize;   /* 0x12C */
    RwaUniqueID     id;         /* 0x130: the stream wanted */
    rwaStreamMarker *current;   /* 0x13C */
    rwaStreamMarker *marker;    /* 0x140 */
    RwUInt8        *buffer;     /* 0x144: the playing chunks */
    rwaStreamRegion *regions;   /* 0x148 */
    RwUInt32        unk14C;
    volatile RwUInt32 region;   /* 0x150 */
    volatile RwUInt32 unk154;
    volatile RwUInt32 unk158;
    volatile RwUInt32 unk15C;
    RwUInt32        bufferSize; /* 0x160 */
    RwUInt32        noItems;    /* 0x164 */
    RwUInt8        *aheadBuffer;    /* 0x168: the look-ahead chunks */
    rwaStreamRegion *entries;   /* 0x16C */
    RwUInt32        aheadSize;  /* 0x170 */
    /* Xbox 0x247660 accesses and the 0x249240 reset require volatile
     * ordering. The original declaration is unknown. */
    volatile RwUInt32 unk174;
    volatile RwUInt32 entry;    /* 0x178 */
    RwUInt32        noEntries;  /* 0x17C */
    RwUInt32        unk180;
    RwUInt32        unk184;
    volatile RwUInt32 flags;    /* 0x188 */
    void           *semaphore;  /* 0x18C */
    RwUInt32        unk190;
    rwaStreamPosition *positions;   /* 0x194 */
    rwaQueueItem   *items;      /* 0x198 */
    rwaQueue        queue;      /* 0x19C */
    rwaQueueCallback callback;  /* 0x1A4 */
};

/* A stream source object (0x208 bytes): an instance with its node state. */
struct rwaDecoder
{
    rwaInstance     inst;
    void           *unk28;
    rwaLink         link;       /* 0x2C: in its graph's node list */
    RwReal          unk34;
    rwaDecoder     *next;       /* 0x38: the next node of a chain */
    RwUInt32        unk3C;
    RwUInt8         unk40[0x08];
    RwInt32         mode;       /* 0x48 */
    RwUInt32        unk4C;      /* 0x4C: its track */
    RwInt32         status;     /* 0x50 */
    RwUInt8         unk54[0x04];
    void           *objInterface;   /* 0x58 */
    rwaDecoderState state;      /* 0x5C */
};

/* A position request: the position and whether to flush first. */
typedef struct rwaStreamSeek rwaStreamSeek;
struct rwaStreamSeek
{
    rwaStreamPosition *position;
    RwBool          flush;
};

/* A stream reader: the queue fill callback and its object. */
typedef struct rwaStreamReader rwaStreamReader;
struct rwaStreamReader
{
    RwUInt32        unk00;
    rwaQueueItem *(*read)(rwaDecoder *obj);
    rwaDecoder     *data;
};

/* A stream source's connection to a reader. */
typedef struct rwaStreamConnect rwaStreamConnect;
struct rwaStreamConnect
{
    RwUInt32        unk00;
    rwaStreamReader *reader;
};

/* A stream source's creation parameters (0x44 bytes). */
typedef struct rwaStreamSourceParams rwaStreamSourceParams;
struct rwaStreamSourceParams
{
    /* Typed view of the pointer parameters used by sub_249160; the word
     * view is retained for existing parameter transport. */
    union
    {
        RwUInt32 values[15];
        struct
        {
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
    };
    void          (*callback)(struct rwaDecoderState *state, RwBool failed, rwaDecoder *obj);
    rwaDecoder     *data;
};

/* A stream source object's creation parameters (0x58 bytes). */
typedef struct rwaStreamNodeParams rwaStreamNodeParams;
struct rwaStreamNodeParams
{
    RwUInt32        unk00;
    RwUInt32        unk04;
    RwUInt32        unk08;
    RwUInt32        unk0C;
    rwaQueueCallback *callback; /* 0x10 */
    RwUInt32        unk14;
    RwUInt32        unk18;
    RwUInt32        source[15]; /* 0x1C */
};

/* A stream object's player (0x5C bytes at +0xEC; the stream container,
 * unit_2452A0.c): its decoders, sources and output node, and the counts of
 * their state changes. */
typedef struct rwaStreamPlayer rwaStreamPlayer;
struct rwaStreamPlayer
{
    volatile RwInt32 numPlaying;    /* 0x00 */
    volatile RwInt32 numPending;    /* 0x04 */
    RwInt32         state;      /* 0x08 */
    rwaStreamInfo  *info;       /* 0x0C */
    RwUInt32        unk10;
    rwaDecoder     *decoder;    /* 0x14: the output node */
    rwaLink         sources;    /* 0x18: decoders' link at 0x2C */
    rwaLink         nodes;      /* 0x20: decoders' link at 0x2C */
    rwaQueueCallback callback;  /* 0x28 */
    RwUInt32        unk30;
    RwReal          unk34;      /* 0x34: the playback rate */
    RwInt32         mode;       /* 0x38: 1, 2, 3 */
    volatile RwInt32 numBusy;   /* 0x3C */
    RwUInt32        unk40;
    RwUInt32        unk44;      /* 0x44: 32 */
    RwUInt32        unk48;
    RwUInt32        unk4C;
    void           *unk50;      /* 0x50: creation argument */
    void           *unk54;      /* 0x54: stop argument */
    void           *unk58;      /* 0x58: creation and stop argument */
};

/* A stream source (0x10 bytes): a name (type 1) or a UUID (type 2). */
typedef struct rwaStreamSource rwaStreamSource;
struct rwaStreamSource
{
    void           *data;
    RwInt32         type;
    RwUInt32        unk08;
    RwUInt32        unk0C;
};

/* A stream object's open parameters. */
typedef struct rwaStreamOpenParams rwaStreamOpenParams;
struct rwaStreamOpenParams
{
    rwaStreamSource *source;
    RwUInt32        unk04;
};

/* A stream play request's settings (0x1C bytes). */
typedef struct rwaStreamSettings rwaStreamSettings;
struct rwaStreamSettings
{
    RwUInt32        data[7];
};

/* A stream play request (0x24 bytes). */
typedef struct rwaStreamPlay rwaStreamPlay;
struct rwaStreamPlay
{
    RwUInt32        unk00;
    RwUInt32        unk04;
    rwaStreamSettings *settings;    /* 0x08 */
    RwUInt32        unk0C[6];
};

/* A stream object's position query: a position and the track count. */
typedef struct rwaStreamQuery rwaStreamQuery;
struct rwaStreamQuery
{
    RwUInt32        value;
    RwInt32         type;
    RwUInt32        pos;
    RwInt32         unit;
    RwUInt32        unk10;
};

/* A stream cue request. */
typedef struct rwaStreamCue rwaStreamCue;
struct rwaStreamCue
{
    RwUInt32        unk00;
    RwUInt32        unk04;
    RwUInt32        unk08;
    RwUInt32        unk0C;
};

/* A stream object (0xDA0 bytes): an instance with its source, player and
 * play queue. */
typedef struct rwaStream rwaStream;
struct rwaStream
{
    rwaInstance     inst;
    rwaStreamOpenParams open;   /* 0x28 */
    rwaStreamSource sourceCopy; /* 0x30 */
    RwChar          name[0x80]; /* 0x40 */
    rwaStreamSettings settings; /* 0xC0 */
    RwaUUID         uuid;       /* 0xDC */
    rwaStreamPlayer player;     /* 0xEC */
    RwUInt8         unk148[0x0C];
    RwUInt8         queue[0x18];    /* 0x154 */
    RwUInt8         unk16C[0xC00];
    RwUInt32        unkD6C;
    RwUInt32        unkD70;
    RwUInt32        unkD74;
    rwaStreamPlay   play;       /* 0xD78 */
    RwUInt32        flags;      /* 0xD9C: 1 playing */
};

/* A library thread: its handle, entry function and argument, priority and
 * stack size. */
typedef struct rwaThread rwaThread;
struct rwaThread
{
    void           *handle;
    void        **(*func)(void *data);
    void           *data;
    RwInt32         priority;
    RwUInt32        stackSize;
};

/* A parameter type (0x28 bytes; the PS2 header's RwaParamType with a flags
 * word: 1 caller's storage, 2 caller's range storage). */
typedef struct RwaParamType RwaParamType;
struct RwaParamType
{
    RwaUniqueID     uniqueID;
    void           *rangeMin;   /* 0x0C */
    void           *rangeMax;   /* 0x10 */
    RwUInt32        type;       /* 0x14: 0x1000 pointer, 0x2000 reference,
                                 * 0x4000 derived */
    RwaParamType   *paramType;  /* 0x18: parent */
    RwUInt32        flags;      /* 0x1C */
    rwaLink         link;       /* 0x20 */
};

/* A handle to a parameter type, by UUID until resolved. */
typedef struct RwaParamTypeHandle RwaParamTypeHandle;
struct RwaParamTypeHandle
{
    union
    {
        const RwaUUID  *paramTypeUUID;
        RwaParamType   *paramType;
    } u;
    RwBool          isUUID;
};

/* A parameter type definition table entry (0x20 bytes). */
typedef struct _rwaParamTypeDef _rwaParamTypeDef;
struct _rwaParamTypeDef
{
    const RwChar   *name;
    RwUInt32        type;
    RwUInt32        size;
    RwUInt8        *range;
    const RwaUUID  *typeUUID;
    const RwaUUID  *sourceTypeUUID;
    RwaParamType   *paramType;  /* caller's storage */
    void           *rangeStorage;
};

/* A free list of parameter values of one size. */
typedef struct rwaValueFreeList rwaValueFreeList;
struct rwaValueFreeList
{
    void           *freeList;
    RwUInt32        size;
};

/* A wave format (0x1C bytes; the PS2 header's RwaWaveFormat with extra
 * format data). */
typedef struct RwaWaveFormat RwaWaveFormat;
struct RwaWaveFormat
{
    RwUInt32        sampleRate;
    const RwaUUID  *dataType;
    RwUInt32        length;
    RwUInt8         bitDepth;   /* 0x0C */
    RwUInt8         noChannels;
    RwUInt8         unk0E;
    RwUInt8         unk0F;
    void           *extra;      /* 0x10 */
    RwUInt32        extraSize;
    RwUInt8         flags;      /* 0x18: 1, 2 caller's extra, 4 */
    RwUInt8         unk19;
    RwUInt8         pad[2];
};

/* Clears a wave format. */
#define rwaWaveFormatInit(_format)                                          \
    (_format)->sampleRate = 0;                                              \
    (_format)->dataType = NULL;                                             \
    (_format)->length = 0;                                                  \
    (_format)->bitDepth = 0;                                                \
    (_format)->noChannels = 0;                                              \
    (_format)->flags = 0;                                                   \
    (_format)->unk19 = 0;                                                   \
    (_format)->extra = NULL;                                                \
    (_format)->extraSize = 0

/* A wave definition's supported format. */
typedef struct rwaWaveFormatDesc rwaWaveFormatDesc;
struct rwaWaveFormatDesc
{
    const RwaUUID  *dataType;
    RwUInt8         bitDepth;
    RwUInt8         noChannels;
    RwUInt8         pad[2];
    RwUInt32        minRate;
    RwUInt32        maxRate;
    RwUInt8         flags;      /* 0x10 */
};

typedef struct RwaWave RwaWave;

/* A wave definition (0x6C bytes). */
typedef struct RwaWaveDef RwaWaveDef;
struct RwaWaveDef
{
    RwaUniqueID     uniqueID;
    RwBool        (*create)(RwaWave *wave, void *params);  /* 0x0C */
    void          (*destroy)(RwaWave *wave);
    void           *unk14;
    void           *unk18;
    RwBool        (*busy)(void); /* 0x1C: RwaWaveDefUploadBusyFunc, reference rwacore_ps2.h */
    void           *unk20;
    void           *unk24;
    void           *unk28;
    void           *unk2C;
    void          (*unregister)(RwaWaveDef *waveDef);       /* 0x30 */
    RwUInt32        noFormats;  /* 0x34 */
    rwaWaveFormatDesc **formats;
    RwUInt32        format;     /* 0x3C */
    rwaObjDef      *objDef;     /* 0x40 */
    RwUInt32        dataSize;   /* 0x44 */
    RwUInt32        unk48;
    void           *instances;  /* 0x4C: a free list */
    rwaLink         link;       /* 0x50: in the object definition's list */
    RwUInt32        flags;      /* 0x58: 1 formats allocated, 2 free list */
    RwUInt8         unk5C[0x10];
};

/* A wave (0x5C bytes). */
struct RwaWave
{
    RwaUniqueID     uniqueID;
    RwaWaveDef     *waveDef;    /* 0x0C */
    RwaWaveFormat   format;     /* 0x10 */
    RwaWaveFormat   deviceFormat;   /* 0x2C */
    RwUInt32        unk48;
    RwUInt8        *buffer;     /* 0x4C: the memory definition's data */
    void           *data;       /* 0x50 */
    RwUInt32        flags;      /* 0x54: 1 free list, 2 mono, 4 caller's,
                                 * 8 data allocated, 0x10 */
    void           *state;      /* 0x58 */
};

/* A wave definition search by UUID. */
typedef struct rwaWaveDefFind rwaWaveDefFind;
struct rwaWaveDefFind
{
    const RwaUUID  *uuid;
    RwaWaveDef     *found;
};

/* A node of the library's doubly linked lists (the list head is a node). */
typedef struct RwaLLNode RwaLLNode;
struct RwaLLNode
{
    RwaLLNode      *prev;
    RwaLLNode      *next;
    void           *data;
};

/* A free list (0x24 bytes; the PS2 header's RwaFreeList with a bitmap
 * size). Each block is a link, a bitmap of used entries and the aligned
 * entries. */
typedef struct RwaFreeList RwaFreeList;
struct RwaFreeList
{
    RwUInt32        entrySize;
    RwUInt32        entriesPerBlock;
    RwUInt32        bitmapSize; /* 0x08 */
    RwUInt32        alignment;
    rwaLink         blocks;     /* 0x10 */
    RwUInt32        flags;      /* 0x18: 1 caller's storage, 2 free empty blocks */
    rwaLink         link;       /* 0x1C: all free lists */
};

/* A free list block: its link, then a bitmap of used entries. */
typedef struct rwaFreeListBlock rwaFreeListBlock;
struct rwaFreeListBlock
{
    rwaLink         link;
    RwUInt8         bitmap[1];
};

/* A pool chunk's header (0x0C bytes), before its data. */
typedef struct rwaPoolChunk rwaPoolChunk;
struct rwaPoolChunk
{
    rwaPoolChunk   *next;
    rwaPoolChunk   *prev;
    RwUInt32        size;
};

/* A pool block: its link, first chunk and end, then the chunks. */
typedef struct rwaPoolBlock rwaPoolBlock;
struct rwaPoolBlock
{
    rwaLink         link;
    rwaPoolChunk   *chunks;
    rwaPoolChunk   *end;
};

/* A pool of variably sized allocations (0x1C bytes). */
typedef struct rwaPool rwaPool;
struct rwaPool
{
    RwUInt32        flags;      /* 1 caller's storage */
    RwUInt32        blockSize;
    RwUInt32        alignment;
    rwaLink         blocks;     /* 0x0C */
    rwaLink         link;       /* 0x14: all pools */
};

/* A symbol table of parameter types (the PS2 header's RwaSymbolTable). */
typedef struct RwaSymbolTable RwaSymbolTable;
struct RwaSymbolTable
{
    RwInt32         noParamTypes;
    rwaLink         list;
};

typedef struct rwaVoiceState rwaVoiceState;
struct rwaVoiceState
{
    RwUInt32        unk00, unk04, unk08, unk0C, unk10, unk14, unk18, unk1C;
    RwUInt32        unk20, unk24, unk28, unk2C, unk30, unk34, unk38, unk3C;
    RwUInt32        unk40;
    RwUInt32        unk44, unk48, unk4C;
    RwUInt32        unk50;
    RwUInt32        unk54;
};

typedef struct rwaVoiceParams rwaVoiceParams;
struct rwaVoiceParams
{
    RwUInt32        unk00;
    RwUInt32        unk04;
};

/* A wave header as stored (0x2C bytes), converted in place. */
typedef struct rwaWaveHeader rwaWaveHeader;
struct rwaWaveHeader
{
    RwUInt32        unk00;
    RwUInt8         unk04, unk05, unk06, unk07;
    RwUInt32        unk08;
    RwUInt32        unk0C;
    RwUInt32        unk10;
    RwUInt32        unk14;
    RwaUUID         unk18;
    RwUInt32        unk28;
};

typedef struct rwaObjClass rwaObjClass;
typedef struct rwaObj rwaObj;

struct rwaObjClass
{
    RwUInt8         unk00[0x20];
    RwBool        (*unk20)(rwaObj *obj);
    void          (*unk24)(rwaObj *obj);
    void           *unk28;
    void          (*unk2C)(rwaObj *obj, void *data, RwInt32 flags);
};

struct rwaObj
{
    RwUInt8         unk00[0x0C];
    rwaObjClass    *objClass;   /* 0x0C */
    RwUInt8         unk10[0x18];
    void           *unk28;
};

/* A wave dictionary as loaded: offsets until fixed up. */
typedef struct rwaDictWave rwaDictWave;
struct rwaDictWave
{
    RwUInt32        unk00;
    RwUInt32        unk04;
    RwUInt8         unk08[0x0C];
    RwUInt32        unk14;
    RwUInt8         unk18[0x08];
    RwUInt32        unk20;
    RwUInt8         unk24[0x0C];
    RwUInt32        unk30;
    RwUInt8         unk34[0x08];
    RwUInt32        unk3C;
    RwUInt8         unk40[0x10];
    RwUInt32        unk50;
};

typedef struct rwaDictNode rwaDictNode;
struct rwaDictNode
{
    RwUInt32        unk00;
    RwUInt32        unk04;      /* next */
    RwUInt32        unk08;      /* wave */
};

typedef struct rwaDict rwaDict;
struct rwaDict
{
    RwUInt32        unk00;
    RwUInt32        unk04;
    RwUInt32        unk08;
    RwUInt32        unk0C;      /* last node */
    RwUInt32        unk10;      /* first node */
    RwUInt32        unk14;
};

typedef struct rwaStreamObj rwaStreamObj;
struct rwaStreamObj
{
    RwUInt8         unk00[0x14];
    const RwaUUID  *format;     /* 0x14 */
    RwUInt32        size;       /* 0x18 */
};

typedef struct rwaReadParams rwaReadParams;
struct rwaReadParams
{
    void           *file;
    void           *buffer;
    RwUInt32        bufferSize;
};

/* A wave dictionary in memory (its first 0x18 bytes come from the file). */
typedef struct rwaWaveDict rwaWaveDict;
struct rwaWaveDict
{
    RwUInt32        unk00;
    RwUInt32        unk04;
    RwUInt32        unk08;
    rwaLink         waves;      /* 0x0C */
    RwUInt32        unk14;
    RwUInt8         unk18[2];
    RwUInt8         flags;      /* 0x1A */
    RwUInt8         type;       /* 0x1B */
    rwaLink         link;       /* 0x1C */
    void           *data;       /* 0x24 */
    RwUInt32        unk28;
    RwUInt32        unk2C;
};

/* The argument block a dictionary's destruction passes on (0x5C bytes). */
typedef struct rwaDictRelease rwaDictRelease;
struct rwaDictRelease
{
    RwUInt8         unk00[0x29];
    RwUInt8         unk29;
    RwUInt8         unk2A[0x0A];
    RwUInt32        unk34;
    RwUInt8         unk38[0x14];
    RwUInt32        unk4C;
    RwUInt32        unk50;
    RwUInt8         unk54[0x08];
};

/* A node of a dictionary's wave list (the list head is the dictionary's
 * waves; nodes chain through next). */
typedef struct rwaWaveNode rwaWaveNode;
struct rwaWaveNode
{
    rwaWaveNode    *unk00;
    rwaWaveNode    *next;
    void           *item;
};

typedef struct rwaWaveItem rwaWaveItem;
struct rwaWaveItem
{
    RwUInt8         unk00[0x0C];
    RwUInt32        id;         /* 0x0C */
};

/* A 3D orientation as passed to the listener: right, up and at rows on
 * 16-byte strides (an RwMatrix's layout). */
typedef struct rwaOrientation rwaOrientation;
struct rwaOrientation
{
    RwaV3d          right;
    RwUInt32        pad0;
    RwaV3d          up;
    RwUInt32        pad1;
    RwaV3d          at;
};

/* Listener state begins at object +0x28: sub_23CD40 addresses its fields
 * through this base. Offsets below are relative to the whole object. */
typedef struct rwaListenerState rwaListenerState;
struct rwaListenerState
{
    RwaV3d          pos;        /* 0x28 */
    RwaV3d          vel;        /* 0x34 */
    RwaV3d          up;         /* 0x40 */
    RwaV3d          at;         /* 0x4C */
    RwaV3d          right;      /* 0x58 */
    RwUInt32        unk64;      /* 10 */
    RwUInt32        unk68;      /* 1 */
    RwUInt32        unk6C;      /* 1 */
    RwUInt32        dirty;      /* 0x70 */
};
typedef struct rwaListener rwaListener;
struct rwaListener
{
    RwUInt8         unk00[0x28];
    union
    {
        rwaListenerState state;
        struct
        {
            RwaV3d          pos;        /* 0x28 */
            RwaV3d          vel;        /* 0x34 */
            RwaV3d          up;         /* 0x40 */
            RwaV3d          at;         /* 0x4C */
            RwaV3d          right;      /* 0x58 */
            RwUInt32        unk64;      /* 10 */
            RwUInt32        unk68;      /* 1 */
            RwUInt32        unk6C;      /* 1 */
            RwUInt32        dirty;      /* 0x70 */
        };
    };
};

/* A parameter definition (0x18 bytes): unique id, type handle, handler. */
typedef struct rwaParamDef rwaParamDef;
struct rwaParamDef
{
    const RwaUUID  *uuid;
    const void     *name;
    RwUInt32        idFlags;
    void           *type;       /* 0x0C */
    RwUInt32        typeIndex;  /* 0x10 */
    void           *handler;    /* 0x14 */
};

/* An object definition's parameter tables (at its +0x4C). */
typedef struct rwaParamTables rwaParamTables;
struct rwaParamTables
{
    void           *unk00;
    rwaLink         interfaces; /* 0x04: interface instances */
    RwUInt16        noInputs;   /* 0x0C */
    RwUInt16        noOutputs;  /* 0x0E */
    rwaParamDef    *inputs;     /* 0x10 */
    rwaParamDef    *outputs;    /* 0x14 */
};

/* An interface definition's parameter ID (0x14 bytes). */
typedef struct _rwaParamID _rwaParamID;
struct _rwaParamID
{
    RwaUniqueID     uniqueID;
    RwaParamTypeHandle type;    /* 0x0C */
};

/* An object interface definition (0x28 bytes). */
typedef struct RwaObjInterfaceDef RwaObjInterfaceDef;
struct RwaObjInterfaceDef
{
    RwaUniqueID     uniqueID;
    _rwaParamID    *outputs;    /* 0x0C */
    _rwaParamID    *inputs;     /* 0x10 */
    RwUInt16        noInputs;   /* 0x14 */
    RwUInt16        noOutputs;
    RwUInt32        flags;      /* 0x18: 1 caller's parameter IDs,
                                 * 2 caller's storage */
    void          (*unregister)(RwaObjInterfaceDef *def);  /* 0x1C */
    rwaLink         link;       /* 0x20: all interface definitions */
};

/* The interface search's argument (sub_23DC60). */
typedef struct rwaIfaceSearch rwaIfaceSearch;
struct rwaIfaceSearch
{
    void           *iface;
    void           *list;
};

/* A module of the core: open and close (8 bytes, in tables). */
typedef struct rwaModule rwaModule;
struct rwaModule
{
    RwBool        (*open)(void);
    void          (*close)(void);
};

typedef RwBool (*rwaModuleCallBack)(rwaModule *module, RwUInt32 index);

/* A voice's wave as the voice manager reads it (flags 2: 3D). */
typedef struct rwaVoiceWave rwaVoiceWave;
struct rwaVoiceWave
{
    RwUInt8         unk00[0x54];
    RwUInt32        flags;
};

/* RenderWare's string functions (RwEngineInstance's stringFuncs; the
 * engine instance is the static globals array under RWGLOBALSIZE). */
typedef struct rwaStringFunctions rwaStringFunctions;
struct rwaStringFunctions
{
    void           *vecSprintf;
    void           *vecVsprintf;
    void           *vecStrcpy;
    void           *vecStrncpy;
    void           *vecStrcat;
    void           *vecStrncat;
    void           *vecStrrchr;
    void           *vecStrchr;
    void           *vecStrstr;
    RwInt32       (*vecStrcmp)(const RwChar *string1, const RwChar *string2);
    void           *vecStrncmp;
    RwInt32       (*vecStricmp)(const RwChar *string1, const RwChar *string2);
    RwUInt32      (*vecStrlen)(const RwChar *string);
    void           *vecStrupr;
    void           *vecStrlwr;
    void           *vecStrtok;
    void           *vecSscanf;
};

/* RenderWare's memory functions (RwEngineInstance's memoryFuncs), which
 * rwamemory.c calls and sets. */
typedef struct rwaMemoryFunctions rwaMemoryFunctions;
struct rwaMemoryFunctions
{
    void         *(*rwmalloc)(RwUInt32 size, RwUInt32 hint);
    void          (*rwfree)(void *mem);
    void         *(*rwrealloc)(void *mem, RwUInt32 size, RwUInt32 hint);
    void         *(*rwcalloc)(RwUInt32 count, RwUInt32 size, RwUInt32 hint);
};

typedef struct rwaEngineGlobals rwaEngineGlobals;
struct rwaEngineGlobals
{
    RwUInt8         unk00[0xF0];
    rwaStringFunctions stringFuncs; /* 0xF0 */
    rwaMemoryFunctions memoryFuncs; /* 0x134 */
};

extern rwaEngineGlobals ourGlobals;
#define rwstrcmp(_a, _b)    (ourGlobals.stringFuncs.vecStrcmp((_a), (_b)))
#define rwstricmp(_a, _b)   (ourGlobals.stringFuncs.vecStricmp((_a), (_b)))
#define rwstrlen(_s)        (ourGlobals.stringFuncs.vecStrlen((_s)))

/* A codec definition's formats (an object definition's +0x54): file
 * extensions (decoders found by name), input formats or input/output
 * pairs (converters), and wave formats. */
typedef struct rwaCodecInfo rwaCodecInfo;
struct rwaCodecInfo
{
    RwUInt32        unk00;
    void          **entries;    /* 0x04: extensions, formats or format pairs */
    RwUInt32        numEntries; /* 0x08 */
    rwaWaveFormatDesc **formats;    /* 0x0C */
    RwUInt32        numFormats; /* 0x10 */
};

/* A stream container's source description (0x24 bytes): a file name
 * (type 1), a format (type 2) or the default decoder (type 3). */
typedef struct rwaContainerSource rwaContainerSource;
struct rwaContainerSource
{
    const RwChar   *name;
    RwInt32         type;       /* 0x04 */
    RwaWaveFormat  *format;     /* 0x08 */
    RwUInt32        unk0C;
    RwUInt32        unk10;
    RwUInt32        unk14;
    RwUInt32        unk18;
    RwUInt32        unk1C;
    RwUInt32        unk20;
};

/* A step of a stream container's node graph (0x54 bytes): a node made from
 * def for the track index, converting from one format to another, and the
 * step after it. */
typedef struct rwaConversion rwaConversion;
struct rwaConversion
{
    rwaDecoder     *node;
    rwaObjDef      *def;       /* 0x04 */
    void           *key;       /* 0x08 */
    RwaWaveFormat   from;       /* 0x0C */
    RwaWaveFormat   to;         /* 0x28 */
    rwaConversion  *next;       /* 0x44 */
    RwUInt32        index;      /* 0x48 */
    rwaLink         link;       /* 0x4C */
};

#define rwaConversionFromLink(_link) ((rwaConversion *)((RwUInt8 *)(_link) - 0x4C))

/* A stream container's node set: its entries and nodes (unit_2452A0.c). */
typedef struct rwaNodeSet rwaNodeSet;
struct rwaNodeSet
{
    RwUInt32        unk00;
    RwUInt32        numEntries; /* 0x04 */
    RwUInt32        numNodes;   /* 0x08 */
};

/* A node set entry (0x10 bytes): a track's (bit 31: changed) channel's
 * value of one kind (0 to 3). */
struct rwaNodeSetEntry
{
    RwUInt32        track;
    RwUInt32        channel;    /* 0x04 */
    RwUInt32        kind;       /* 0x08 */
    RwReal          value;      /* 0x0C */
};


/* A conversion node's creation parameters (0x60 bytes). */
typedef struct rwaConversionParams rwaConversionParams;
struct rwaConversionParams
{
    RwUInt32        unk00;
    RwUInt32        unk04;
    rwaStreamInfo  *info;       /* 0x08 */
    RwUInt32        unk0C[4];
    RwaWaveFormat   from;       /* 0x1C */
    RwaWaveFormat   to;         /* 0x38 */
    RwUInt32        unk54;
    RwUInt32        unk58;
    RwUInt32        unk5C;
};

/* A stream container's play request: its settings (+0x04 the start). */
struct rwaPlaySettings
{
    RwUInt32        unk00;
    RwUInt32        unk04;
};

/* A node callback of the stream container's walks. */
typedef void (*rwaContainerNodeFunc)(rwaDecoder *node, void *data);

typedef struct rwaPlaySettings rwaPlaySettings;
typedef struct rwaNodeSetEntry rwaNodeSetEntry;

/* A stream container's command (0x18 bytes, then the node set's entries):
 * what changed (flags) and the new values. */
typedef struct rwaContainerCommand rwaContainerCommand;
struct rwaContainerCommand
{
    RwUInt32        flags;      /* 4 restart, 8 play, 0x10 entries, 0x20 rate,
                                 * 0x40 0x14, 0x80 mode */
    RwInt32         mode;       /* 0x04 */
    const struct rwaContainerSource *source;    /* 0x08 */
    rwaPlaySettings *settings;  /* 0x0C */
    RwReal          rate;       /* 0x10 */
    RwUInt32        unk14;
    struct rwaNodeSetEntry entries[0xC0];   /* 0x18 */
};

/* A stream container object: its player, node set and command, and the
 * states it last reported. */
typedef struct rwaStreamContainer rwaStreamContainer;
struct rwaStreamContainer
{
    rwaStreamPlayer player;
    rwaNodeSet      set;        /* 0x5C */
    rwaContainerCommand command;    /* 0x68 */
    RwInt32         states[3];  /* 0xC80 */
};

#define rwaDecoderFromLink(_link) ((rwaDecoder *)((RwUInt8 *)(_link) - 0x2C))

/* An allocation bitmap (0x18 bytes; unit_2FD5D0.c): one bit per unit, and
 * the memory the units cover (the streamed waves' at 0x51DE60: 144-byte
 * units). */
typedef struct RwaAllocBitmap RwaAllocBitmap;
struct RwaAllocBitmap
{
    RwUInt8        *bits;
    RwUInt32        numBytes;   /* 0x04 */
    RwUInt8         shift;      /* 0x08: a handle's length is above it */
    RwUInt8         flags;      /* 0x09: 1 from the free list, 2 bits allocated */
    RwUInt8         pad0A[2];
    void           *base;       /* 0x0C */
    RwUInt32        size;       /* 0x10 */
    RwUInt32        unitSize;   /* 0x14 */
};

/* A wave format converter (unit_249490.c): the formats it converts between
 * (flags 1: it changes the sample rate) and its conversion. */
typedef struct rwaConverterInfo rwaConverterInfo;
struct rwaConverterInfo
{
    const rwaWaveFormatDesc *from;
    const rwaWaveFormatDesc *to;
    RwUInt8         flags;      /* 0x08 */
};

typedef struct rwaConverter rwaConverter;
struct rwaConverter
{
    const rwaConverterInfo *info;
    RwUInt32      (*convert)(const RwaWaveFormat *from, const void *src, RwUInt32 srcSize,
                             RwaWaveFormat *to, void *dest, RwUInt32 *used);
};

#define rwaNUMCONVERTERS 51

/* A chain of converters found between two formats (0x4C bytes). */
typedef struct rwaConversionPath rwaConversionPath;
struct rwaConversionPath
{
    RwaWaveFormat   from;
    RwaWaveFormat   to;         /* 0x1C */
    RwUInt32        count;      /* 0x38 */
    const rwaConverter *steps[4];   /* 0x3C */
};

/* The streamed waves' memory as the Xbox streamed wave definition's object
 * describes it (its state). */
typedef struct rwaStreamedMemory rwaStreamedMemory;
struct rwaStreamedMemory
{
    RwUInt8         unk00[0x48];
    void           *base;       /* 0x48 */
    RwUInt32        size;       /* 0x4C */
};

/* A RenderWare file system file, as far as RW Audio reads it. */
typedef struct rwaRtFile rwaRtFile;
struct rwaRtFile
{
    RwUInt8         unk00[0x20];
    RwInt32         status;     /* 0x20: 2 ready */
};

/* The Xbox output's I3DL2 listener settings (dsound.h's DSI3DL2LISTENER). */
typedef struct rwaI3DL2Listener rwaI3DL2Listener;
struct rwaI3DL2Listener
{
    RwInt32         room;
    RwInt32         roomHF;
    RwReal          roomRolloffFactor;
    RwReal          decayTime;
    RwReal          decayHFRatio;
    RwInt32         reflections;
    RwReal          reflectionsDelay;
    RwInt32         reverb;
    RwReal          reverbDelay;
    RwReal          diffusion;
    RwReal          density;
    RwReal          hfReference;
};

/* A mix bin and its level (dsound.h's DSMIXBINVOLUMEPAIR); the creation
 * parameters give gains, converted to levels. */
typedef struct rwaMixBinLevel rwaMixBinLevel;
struct rwaMixBinLevel
{
    RwUInt32        mixBin;
    RwInt32         level;
};

typedef struct rwaMixBinGain rwaMixBinGain;
struct rwaMixBinGain
{
    RwUInt32        mixBin;
    RwReal          gain;
};

typedef struct rwaMixBinGains rwaMixBinGains;
struct rwaMixBinGains
{
    RwUInt32        count;
    rwaMixBinGain   entries[1];
};

/* An effect data transfer (IDirectSound_SetEffectData's arguments). */
typedef struct rwaEffectData rwaEffectData;
struct rwaEffectData
{
    RwUInt32        index;
    RwUInt32        offset;
    void           *data;
    RwUInt32        size;
};

/* The Xbox output's creation parameters (0x3C). */
typedef struct rwaXboxOutputParams rwaXboxOutputParams;
struct rwaXboxOutputParams
{
    RwUInt32        unk00;
    const RwChar   *effectsPath;    /* 0x04 */
    RwUInt32        unk08;
    RwUInt32        unk0C;
    void           *memory;         /* 0x10: the application's */
    RwUInt32        memorySize;     /* 0x14 */
    RwUInt32        speakerMode;    /* 0x18 */
    RwBool          i3dl2;          /* 0x1C */
    rwaMixBinGains *mixBins[5];     /* 0x20 */
    RwReal          unk34;
    RwReal          unk38;
};

/* The Xbox output's state (after its instance). */
typedef struct rwaXboxOutputState rwaXboxOutputState;
struct rwaXboxOutputState
{
    RwUInt16        unk00;
    RwUInt16        unk02;
    RwReal          unk04;
    rwaInstance    *listener;       /* 0x08 */
    rwaI3DL2Listener i3dl2;         /* 0x0C */
    RwUInt32        speakerMode;    /* 0x3C */
    volatile RwUInt32 flags;        /* 0x40 */
    void           *dsound;         /* 0x44 */
    void           *memory;         /* 0x48 */
    RwUInt32        memorySize;     /* 0x4C */
    RwInt32         unk50;
    RwInt32         unk54;
    RwUInt32        unk58;
    RwUInt32        unk5C;
    void           *effects;        /* 0x60 */
};

typedef struct rwaXboxOutput rwaXboxOutput;
struct rwaXboxOutput
{
    rwaInstance     inst;
    rwaXboxOutputState state;       /* 0x28 */
};

/* The Xbox listener's state: what changed is applied by the output. */
typedef struct rwaXboxListenerState rwaXboxListenerState;
struct rwaXboxListenerState
{
    RwaV3d          pos;
    RwaV3d          vel;            /* 0x0C */
    RwaV3d          top;            /* 0x18 */
    RwaV3d          front;          /* 0x24 */
    RwUInt32        unk30[3];
    RwReal          rolloff;        /* 0x3C */
    RwReal          doppler;        /* 0x40 */
    RwReal          distance;       /* 0x44 */
    RwUInt32        flags;          /* 0x48 */
};

/* A mix bin list (dsound.h's DSMIXBINS). */
typedef struct rwaMixBins rwaMixBins;
struct rwaMixBins
{
    RwUInt32        count;
    rwaMixBinLevel *levels;
};

/* A buffer's 3D parameters (dsound.h's DS3DBUFFER, 0x4C). */
typedef struct rwaDS3DBuffer rwaDS3DBuffer;
struct rwaDS3DBuffer
{
    RwUInt32        size;
    RwaV3d          position;
    RwaV3d          velocity;
    RwUInt32        insideConeAngle;
    RwUInt32        outsideConeAngle;
    RwaV3d          coneOrientation;
    RwInt32         coneOutsideVolume;
    RwReal          minDistance;
    RwReal          maxDistance;
    RwUInt32        mode;
    RwReal          distanceFactor;
    RwReal          rolloffFactor;
    RwReal          dopplerFactor;
};

/* A buffer's I3DL2 source settings (dsound.h's DSI3DL2BUFFER). */
typedef struct rwaI3DL2Source rwaI3DL2Source;
struct rwaI3DL2Source
{
    RwInt32         direct;
    RwInt32         directHF;
    RwInt32         room;
    RwInt32         roomHF;
    RwReal          roomRolloffFactor;
    RwInt32         obstruction;
    RwReal          obstructionLFRatio;
    RwInt32         occlusion;
    RwReal          occlusionLFRatio;
};

/* A sound buffer's description (dsound.h's DSBUFFERDESC). */
typedef struct rwaBufferDesc rwaBufferDesc;
struct rwaBufferDesc
{
    RwUInt32        size;
    RwUInt32        flags;
    RwUInt32        bufferBytes;
    const void     *format;
    rwaMixBins     *mixBins;
    RwUInt32        inputMixBin;
};

/* mmreg.h's WAVEFORMATEX and the Xbox ADPCM format (packed). */
#pragma pack(push, 1)
typedef struct rwaWaveFormatEx rwaWaveFormatEx;
struct rwaWaveFormatEx
{
    RwUInt16        formatTag;
    RwUInt16        channels;
    RwUInt32        samplesPerSec;
    RwUInt32        avgBytesPerSec;
    RwUInt16        blockAlign;
    RwUInt16        bitsPerSample;
    RwUInt16        size;
};

typedef struct rwaAdpcmWaveFormat rwaAdpcmWaveFormat;
struct rwaAdpcmWaveFormat
{
    rwaWaveFormatEx wfx;
    RwUInt16        samplesPerBlock;
};
#pragma pack(pop)

/* The Xbox voice's creation parameters (0x14). */
typedef struct rwaXboxVoiceParams rwaXboxVoiceParams;
struct rwaXboxVoiceParams
{
    RwUInt32        flags;
    struct rwaXboxVoice *target;    /* 0x04: the voice it mixes into */
    RwUInt32        id;             /* 0x08: its mix bin, as a target */
    rwaMixBinGains *mixBins;        /* 0x0C */
    RwReal          headroomGain;   /* 0x10 */
};

/* The Xbox voice's state (0x16C, after its instance): a DirectSound buffer
 * playing a wave from the streamed waves' memory, or (flags 0x68000) a
 * buffer other voices mix into. */
typedef struct rwaXboxVoiceState rwaXboxVoiceState;
struct rwaXboxVoiceState
{
    RwUInt32        flags;
    RwReal          frequency;      /* 0x04 */
    RwReal          volume;         /* 0x08 */
    RwReal          pan;            /* 0x0C */
    RwReal          gain;           /* 0x10: from the listener */
    rwaI3DL2Listener i3dl2;         /* 0x14 */
    RwInt32         direct;         /* 0x44 */
    RwInt32         obstruction;    /* 0x48 */
    RwReal          obstructionLFRatio;
    RwInt32         occlusion;      /* 0x50 */
    RwReal          occlusionLFRatio;
    RwaWave        *wave;           /* 0x58 */
    RwUInt32        position;       /* 0x5C: in frames */
    RwUInt32        current;        /* 0x60: in bytes */
    RwUInt32        start;          /* 0x64 */
    rwaXboxOutputState *output;     /* 0x68 */
    void           *buffer;         /* 0x6C */
    rwaDS3DBuffer   ds3d;           /* 0x70 */
    RwUInt32        noMixBins;      /* 0xBC */
    rwaMixBinLevel  mixBins[8];     /* 0xC0 */
    RwUInt32        noLevels;       /* 0x100 */
    rwaMixBinLevel  levels[8];      /* 0x104 */
    RwaWaveFormat   format;         /* 0x144 */
    struct rwaXboxVoice *target;    /* 0x160 */
    RwUInt32        id;             /* 0x164 */
    RwInt32         headroom;       /* 0x168 */
};

typedef struct rwaXboxVoice rwaXboxVoice;
struct rwaXboxVoice
{
    rwaInstance     inst;
    rwaXboxVoiceState state;        /* 0x28 */
};

/* Xbox voice flags above 0x10000 (not addresses). */
#define rwaXBVOICE3DBINS     0x100000   /* its bins replace the 3D ones; not an address */
#define rwaXBVOICE2DBINS     0x080000   /* its bins replace the 2D ones; not an address */
#define rwaXBVOICEI3DL2      0x200000   /* it feeds the I3DL2 bin; not an address */
#define rwaXBVOICEHEADROOM   0x400000   /* its own headroom; not an address */
#define rwaXBVOICEMIXTARGET  0x068000   /* others mix into it; not an address */
#define rwaDSBCAPS_FXIN      0x080000   /* dsound.h; not an address */

/* A 3D source as the listener's gains take it. */
typedef struct rwa3DSource rwa3DSource;
struct rwa3DSource
{
    RwaV3d          position;
    RwaV3d          velocity;
    RwReal          minDistance;
    RwReal          maxDistance;
};

/* A voice search by its mix bin. */
typedef struct rwaXboxVoiceFind rwaXboxVoiceFind;
struct rwaXboxVoiceFind
{
    RwUInt32        id;
    rwaXboxVoice   *found;
};

/* An Xbox media object (xmo.h's XMediaObject, and the WMA file decoder's
 * additions): its calls are __stdcall through the vtable. */
typedef struct rwaXMediaPacket rwaXMediaPacket;
struct rwaXMediaPacket
{
    void           *buffer;
    RwUInt32        size;
    RwUInt32       *completed;      /* 0x08 */
    RwUInt32       *status;         /* 0x0C */
    void           *event;
    void           *timestamp;
};

typedef struct rwaXMediaObject rwaXMediaObject;
typedef struct rwaXMediaObjectVtbl rwaXMediaObjectVtbl;
struct rwaXMediaObjectVtbl
{
    RwUInt32 (__stdcall *addRef)(rwaXMediaObject *obj);
    RwUInt32 (__stdcall *release)(rwaXMediaObject *obj);
    RwInt32 (__stdcall *getInfo)(rwaXMediaObject *obj, void *info);
    RwInt32 (__stdcall *getStatus)(rwaXMediaObject *obj, RwUInt32 *status);
    RwInt32 (__stdcall *process)(rwaXMediaObject *obj, rwaXMediaPacket *input,
                                 rwaXMediaPacket *output);
    RwInt32 (__stdcall *discontinuity)(rwaXMediaObject *obj);
    RwInt32 (__stdcall *flush)(rwaXMediaObject *obj);
    RwInt32 (__stdcall *seek)(rwaXMediaObject *obj, RwInt32 offset, RwUInt32 origin,
                              RwUInt32 *position);
    RwInt32 (__stdcall *getLength)(rwaXMediaObject *obj, RwUInt32 *length);
    RwInt32 (__stdcall *doWork)(rwaXMediaObject *obj);
    RwInt32 (__stdcall *getFileHeader)(rwaXMediaObject *obj, void *header);
    RwInt32 (__stdcall *getFileContentDescription)(rwaXMediaObject *obj, void *desc);
    RwInt32 (__stdcall *seekToTime)(rwaXMediaObject *obj, RwUInt32 time, RwUInt32 *actual);
};

struct rwaXMediaObject
{
    const rwaXMediaObjectVtbl *vtbl;
};

/* The WMA decoder's file header and content description (wmaxmo.h). */
typedef struct rwaWmaFileHeader rwaWmaFileHeader;
struct rwaWmaFileHeader
{
    RwUInt32        version;
    RwUInt32        sampleRate;
    RwUInt32        noChannels;
    RwUInt32        duration;       /* 0x0C: in ms */
    RwUInt32        bitrate;
};

#pragma pack(push, 1)
typedef struct rwaWmaContentDesc rwaWmaContentDesc;
struct rwaWmaContentDesc
{
    RwUInt16        titleLength;
    RwUInt16        authorLength;
    RwUInt16        copyrightLength;
    RwUInt16        descriptionLength;
    RwUInt16        ratingLength;
    RwUInt16       *title;          /* 0x0A */
    RwUInt16       *author;
    RwUInt16       *copyright;
    RwUInt16       *description;
    RwUInt16       *rating;
};
#pragma pack(pop)

/* A WMA source's decoded packet (0x28): the media packet, its status and
 * size, whether it is filled (1) or the stream's last (2), its position. */
typedef struct rwaWmaPacket rwaWmaPacket;
struct rwaWmaPacket
{
    rwaXMediaPacket header;
    RwUInt32        status;         /* 0x18 */
    RwUInt32        completed;      /* 0x1C */
    RwInt32         state;          /* 0x20 */
    RwUInt32        position;       /* 0x24 */
};

/* A WMA source's queue item position (with the packet it plays). */
typedef struct rwaWmaPosition rwaWmaPosition;
struct rwaWmaPosition
{
    rwaStreamPosition position;
    rwaWmaPacket   *packet;         /* 0x10 */
};

/* The WMA stream source's creation parameters (0x5C). */
typedef struct rwaWmaParams rwaWmaParams;
struct rwaWmaParams
{
    RwUInt32        unk00;
    RwUInt32        unk04;
    RwUInt32        unk08;
    RwUInt32        unk0C;
    rwaQueueCallback *callback;     /* 0x10 */
    RwUInt32        unk14;
    RwUInt32        unk18;
    RwUInt32        unk1C;
    RwUInt32        unk20;
    RwUInt32        unk24;
    RwUInt32        unk28;
    const void     *file;           /* 0x2C: its name or handle */
    RwInt32         fileType;       /* 0x30: 1 name, 2 handle */
    RwUInt32        unk34;
    RwUInt32        unk38;
    RwUInt32        minPackets;     /* 0x3C */
    RwUInt32        noPackets;      /* 0x40 */
    RwUInt32        lookahead;      /* 0x44 */
    RwUInt32        unk48;
    RwUInt32        unk4C;
    RwUInt32        unk50;
    RwUInt32        unk54;
    RwUInt32        yieldRate;      /* 0x58 */
};

/* The WMA stream source's state (0x2A0): a stream node decoding a WMA file
 * into a ring of packets on the WMA thread, which the play queue reads. */
typedef struct rwaWmaState rwaWmaState;
struct rwaWmaState
{
    rwaStreamNodeState node;
    void           *objInterface;   /* 0x30 */
    rwaStreamInfo   info;           /* 0x34 */
    rwaStreamTrack  track;          /* 0x60 */
    RwaWaveFormat   format;         /* 0x88 */
    rwaStreamMarker marker;         /* 0xA4 */
    rwaXMediaObject *decoder;       /* 0xC4 */
    rwaQueue        queue;          /* 0xC8 */
    rwaQueueItem   *items;          /* 0xD0 */
    RwUInt32        minPackets;     /* 0xD4 */
    RwUInt32        noPackets;      /* 0xD8 */
    RwUInt32        packetSize;     /* 0xDC */
    rwaQueueCallback callback;      /* 0xE0 */
    RwUInt32        flags;          /* 0xE8: 1 loop, 2 seek, 4 flushed */
    RwChar          title[128];     /* 0xEC */
    RwChar          author[128];    /* 0x16C */
    RwChar          description[128];   /* 0x1EC */
    RwUInt32        seekTime;       /* 0x26C */
    RwUInt32        position;       /* 0x270 */
    RwUInt32        length;         /* 0x274 */
    RwUInt32        duration;       /* 0x278 */
    RwUInt32        bitrate;        /* 0x27C */
    void           *semaphore;      /* 0x280 */
    rwaWmaPacket   *packets;        /* 0x284 */
    RwUInt32        write;          /* 0x288 */
    RwUInt32        read;           /* 0x28C */
    rwaLink         link;           /* 0x290: in the WMA thread's list */
    RwUInt8        *buffers;        /* 0x298 */
    RwUInt32        pad29C;
};

typedef struct rwaWmaSource rwaWmaSource;
struct rwaWmaSource
{
    rwaInstance     inst;
    rwaWmaState     state;          /* 0x28 */
};

#define rwaWmaFromLink(_link) ((rwaWmaSource *)((RwUInt8 *)(_link) - 0x2B8))

/* A node's memory functions (the stream splitter's parameters). */
typedef struct rwaNodeAllocator rwaNodeAllocator;
struct rwaNodeAllocator
{
    void         *(*alloc)(RwUInt32 size, void *data);
    void          (*free)(void *mem, void *data);
    void           *data;
};

/* The stream splitter's creation parameters (0x20): a stream node's, the
 * stream's description and the memory functions. */
typedef struct rwaSplitterParams rwaSplitterParams;
struct rwaSplitterParams
{
    RwUInt32        unk00;
    RwUInt32        unk04;
    rwaStreamInfo  *info;           /* 0x08 */
    RwUInt32        noBuffers;      /* 0x0C */
    rwaQueueCallback *callback;     /* 0x10 */
    RwUInt32        unk14;
    RwUInt32        unk18;
    rwaNodeAllocator *allocator;    /* 0x1C */
};

typedef struct rwaSplitterState rwaSplitterState;

/* A splitter track's queue callback data. */
typedef struct rwaSplitterTrack rwaSplitterTrack;
struct rwaSplitterTrack
{
    rwaSplitterState *owner;
    RwUInt32        index;
};

/* The stream splitter's state (0x88): the items its reader gives, handed
 * to each track as a track item (offset into the interleaved data); an
 * item is given back once every track has played it. */
struct rwaSplitterState
{
    rwaStreamNodeState node;
    void           *objInterface;   /* 0x30 */
    rwaNodeAllocator *allocator;    /* 0x34 */
    rwaStreamInfo   info;           /* 0x38 */
    RwUInt32        noBuffers;      /* 0x64 */
    RwBool          starved;        /* 0x68 */
    RwUInt32       *masks;          /* 0x6C: the tracks yet to play each */
    rwaQueueItem  **items;          /* 0x70 */
    RwUInt32       *readIndex;      /* 0x74: per track */
    RwUInt32       *writeIndex;     /* 0x78: per track */
    rwaQueueItem   *trackItems;     /* 0x7C */
    rwaQueueCallback *callbacks;    /* 0x80 */
    rwaSplitterTrack *tracks;       /* 0x84 */
};

typedef struct rwaSplitter rwaSplitter;
struct rwaSplitter
{
    rwaInstance     inst;
    rwaSplitterState state;         /* 0x28 */
};

/* A stream voice's playback entry (0x14 bytes): where a queue item's data
 * plays and its frames. */
typedef struct rwaStreamVoiceEntry rwaStreamVoiceEntry;
struct rwaStreamVoiceEntry
{
    rwaStreamPosition position;
    RwUInt32        frames;     /* 0x10 */
};

/* A stream voice's creation parameters (0x30 bytes; their head is the
 * stream node's). */
typedef struct rwaStreamVoiceParams rwaStreamVoiceParams;
struct rwaStreamVoiceParams
{
    RwUInt32        unk00;
    RwUInt32        unk04;
    RwUInt32        unk08;
    RwUInt32        noEntries;  /* 0x0C */
    rwaQueueCallback *callback; /* 0x10 */
    RwUInt32        unk14;
    RwUInt32        unk18;
    rwaStreamTrack *track;      /* 0x1C */
    rwaNodeAllocator *allocator;    /* 0x20 */
    void          (*done)(rwaDecoder *obj, void *data);    /* 0x24 */
    void           *doneData;   /* 0x28 */
    RwBool          mono;       /* 0x2C */
};

/* A channel's level as the stream voice's parameter takes it. */
typedef struct rwaChannelLevel rwaChannelLevel;
struct rwaChannelLevel
{
    RwInt32         channel;
    RwInt32         index;      /* 0x04: below 2 */
    RwReal          level;      /* 0x08 */
};

/* A list of object handles (the stream voice's muted voices). */
typedef struct rwaHandleList rwaHandleList;
struct rwaHandleList
{
    RwUInt32        count;
    RwaObjHandle  **handles;
};

/* The allocation callback and its data a stream voice hands on to its
 * embedded voice's creation and destruction, with the voice's owner. */
typedef struct rwaStreamVoiceAlloc rwaStreamVoiceAlloc;
struct rwaStreamVoiceAlloc
{
    void           *func;
    void           *data;
    struct rwaStreamVoiceState *state;
};

/* A stream voice's state (0x298 bytes): a stream node that plays its queue
 * items through an embedded Xbox voice looping over a streamed wave. */
typedef struct rwaStreamVoiceState rwaStreamVoiceState;
struct rwaStreamVoiceState
{
    void           *iface;      /* the stream node's head */
    rwaLink         link;       /* 0x04 */
    RwInt32         unk0C;
    rwaStreamReader reader;     /* 0x10 */
    RwInt32         unk1C;
    RwInt32         mode;       /* 0x20: 1, 2, 3 */
    RwUInt32        numCreated; /* 0x24 */
    RwInt32         status;     /* 0x28 */
    rwaQueueCallback *callback; /* 0x2C */
    void           *objInterface;   /* 0x30 */
    rwaXboxVoice    voice;      /* 0x34 */
    RwUInt32        unk1C8;
    RwaWave         wave;       /* 0x1CC */
    rwaStreamPosition position; /* 0x228 */
    rwaStreamVoiceEntry *entries;   /* 0x238 */
    RwUInt32        entrySize;  /* 0x23C: bytes */
    RwUInt32        entryFrames;    /* 0x240 */
    RwUInt32        totalFrames;    /* 0x244 */
    RwUInt32        filled;     /* 0x248 */
    RwUInt32        noEntries;  /* 0x24C */
    rwaQueueItem   *item;       /* 0x250 */
    RwUInt32        written;    /* 0x254 */
    rwaQueueItem    silence;    /* 0x258 */
    RwUInt32        lastPosition;   /* 0x26C */
    RwUInt32        lastEntry;  /* 0x270 */
    RwUInt32        unk274;
    rwaNodeAllocator allocator; /* 0x278 */
    RwUInt32        refill;     /* 0x284 */
    void          (*done)(rwaDecoder *obj, void *data);    /* 0x288 */
    void           *doneData;   /* 0x28C */
    RwUInt8         noChannels; /* 0x290 */
    RwUInt8         flags;      /* 0x291: 1 an entry being uploaded */
    RwUInt8         unk292[0x06];
};

#endif /* RWAINT_H */
