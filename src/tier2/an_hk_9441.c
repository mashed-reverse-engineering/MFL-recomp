#include "mfl_tier2.h"
/*00494480*/ extern struct ReplayBuffer *__cdecl physics_buffer_create(float length, int resolution, int flag);
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *fmt, ...);
/*004dbe70*/ extern RwUInt32 RwStreamRead(struct RwStream *stream, void *buffer, RwUInt32 length);
/*004dc2a0*/ extern RwBool __cdecl RwStreamClose(struct RwStream *stream, void *data);
/*004dc370*/ extern struct RwStream * RwStreamOpen(enum RwStreamType type, enum RwStreamAccessType access, void *data);
/*005e06ac*/ extern char s_loaded_replay_of_length__f__res___005e06aa_2[];
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/* memory range of the replay stream (RwMemory) */
typedef struct ReplayChunk {
  int type;                   /* 0x10b = replay buffer */
  int size;
  int id;
} ReplayChunk;
typedef struct ReplayMemory {
  ReplayChunk *data;
  int length;
} ReplayMemory;
typedef struct ReplaySlot {
  char data[0x1c];
  struct ReplaySlot *next;
  int pad;
} ReplaySlot;                 /* 0x24 bytes */
typedef struct ReplayBuffer {
  int id;                     /* 0x10b */
  char pad0[0x10];
  ReplaySlot *slots;          /* +0x14 slot array (count + 2 entries) */
  ReplaySlot *head;
  ReplaySlot *cursor;
  ReplaySlot *tail;
  char pad1[0x148];
  int count;                  /* +0x16c */
  char pad2[0x2c];
} ReplayBuffer;               /* 0x19c bytes */
/* replay_buffer_create_from_stream */
/* "finds the 0x10b chunk of the given id in the replay stream and builds the physics buffer from it" */
/* TIER2: unvalidated 2026-10-21 authority=diffemu */
ReplayBuffer * __cdecl replay_buffer_create_from_stream(ReplayMemory *source, int id)
{
  ReplayMemory mem;
  int header[0x67];
  ReplayChunk *chunk;
  int found;
  ReplayBuffer *buffer;
  ReplaySlot *first;
  struct RwStream *stream;
  float length;
  int resolution;
  int i;

  mem.data = source->data;
  mem.length = source->length;
  buffer = 0;
  found = 0;
  do {
    if (mem.data->id == id && mem.data->type == 0x10b) {
      found = 100;
    }
    else {
      chunk = mem.data;
      mem.data = (ReplayChunk *)((char *)mem.data + chunk->size);
      mem.length = mem.length - chunk->size;
    }
    found = found + 1;
  } while (found < 0xd);
  if (found < 100) {
    return 0;
  }
  stream = RwStreamOpen(rwSTREAMMEMORY, rwSTREAMREAD, &mem);
  if (stream != 0) {
    RwStreamRead(stream, header, 0x19c);
    length = *(float *)&header[3];
    resolution = header[4];
    FID_conflict__wprintf(s_loaded_replay_of_length__f__res___005e06aa_2, (double)length, resolution);
    RwStreamClose(stream, &mem);
  }
  stream = RwStreamOpen(rwSTREAMMEMORY, rwSTREAMREAD, &mem);
  if (stream != 0) {
    buffer = physics_buffer_create(length, resolution, 0);
    first = buffer->slots;
    RwStreamRead(stream, buffer, 0x19c);
    if (buffer->id != 0x10b) {
      RwFree(buffer);
      RwStreamClose(stream, &mem);
      return 0;
    }
    buffer->slots = first;
    buffer->head = first;
    buffer->cursor = first;
    RwStreamRead(stream, first, (buffer->count + 2) * 0x24);
    buffer->head = buffer->slots;
    buffer->cursor = buffer->slots;
    buffer->slots->next = buffer->slots + 1;
    for (i = 0; i < buffer->count; i++) {
      buffer->cursor->next = buffer->cursor + 1;
      buffer->cursor = buffer->cursor->next;
    }
    buffer->tail = buffer->cursor;
    buffer->cursor->next = buffer->head;
    buffer->cursor = buffer->head;
    RwStreamClose(stream, &mem);
  }
  return buffer;
}
