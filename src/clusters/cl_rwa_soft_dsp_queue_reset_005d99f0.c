// cluster-entries: rwa_soft_dsp_queue_reset_005d99f0 rwa_soft_dsp_process_005d9ad0
/* Installed cluster rwa_soft_dsp_queue_reset_005d99f0 (tools/cluster_install.py from output/cluster_pilot_au5/rwa_soft_dsp_queue_reset_005d99f0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* Software dsp node (RWA soft mixer): the block-queue reset (register function, state in eax) and the node's process
   callback, its only caller.  Cluster: rwa_soft_dsp_queue_reset_005d99f0 is entered through the generated eax thunk. */
#include "mfl_tier2.h"

/*005ca690*/ extern int _rwaDXAudioVoiceGetPlaying(void *voice);
/*005ca710*/ extern void _rwaDXAudioVoiceSetWavePos(void *voice, RwUInt32 pos);
/*005ca650*/ extern void rwa_dxaudio_voice_set_trigger_005ca650(void *voice, int trigger);
/*005c1d00*/ extern void _rwaStreamNodeProcessComplete(void *node);
/*005c1dc0*/ extern void rwa_unit246fd0_005c1dc0(void *node, int a2, int *value);
/*005c1e60*/ extern void *RwaStreamConnectionGetPacket(void *connection);
/*005bc260*/ extern int rwa_unit2422b0_005bc260(void *wave);      /* wave busy (upload pending) */
/*005bc520*/ extern int FUN_005bc520(void *wave, RwUInt32 offset, const void *data, RwUInt32 bytes);      /* wave write */
/*005bd580*/ extern RwUInt32 FUN_005bd580(void *format, RwUInt32 bytes);
/*005bace0*/ extern void *rwa_obj_005bace0(void *obj, int flag);      /* _rwaObjFinishExecute */

typedef struct RwaBlockDesc { RwUInt32 a, b, pos, state, frames; } RwaBlockDesc;      /* 0x14 bytes, st+0x1d8: one per queued block */
typedef struct RwaPacketCb { void (*fn)(void *packet, void *arg); void *arg; } RwaPacketCb;
typedef struct RwaPacket { RwUInt32 flags; void *data; RwUInt32 bytes; RwUInt32 *desc; RwaPacketCb *done; } RwaPacket;      /* a stream packet */

/* dsp state (node+0x20) */
#define ST_MODE(st)      (*(int *)((st) + 0x20))       /* 1 stop, 2 play, 3 drain */
#define ST_STATE(st)     (*(int *)((st) + 0x28))       /* 4 running, 6 / 7 starved, 8 stopping */
#define ST_VOICE(st)     ((st) + 0x34)                 /* embedded DXAudio voice */
#define ST_PLAYPOS(st)   (*(RwUInt32 *)((st) + 0xbc))  /* voice play position, bytes */
#define ST_WAVE(st)      ((st) + 0x160)
#define ST_WAVEFMT(st)   ((st) + 0x170)
#define ST_PLAYDESC(st)  ((RwUInt32 *)((st) + 0x1c8)) /* 4 words: the block being played */
#define ST_BLOCKS(st)    (*(RwaBlockDesc **)((st) + 0x1d8))
#define ST_BLKBYTES(st)  (*(RwUInt32 *)((st) + 0x1dc))
#define ST_BLKFRAMES(st) (*(RwUInt32 *)((st) + 0x1e0))
#define ST_FREE(st)      (*(RwUInt32 *)((st) + 0x1e8)) /* blocks free to fill */
#define ST_NBLOCKS(st)   (*(RwUInt32 *)((st) + 0x1ec))
#define ST_PACKET(st)    (*(RwaPacket **)((st) + 0x1f0))
#define ST_LASTFILL(st)  (*(RwUInt32 *)((st) + 0x1f4))
#define ST_SILENCE(st)   ((RwaPacket *)((st) + 0x1f8))  /* the built-in silence packet */
#define ST_CURPKT(st)    (*(void **)((st) + 0x204))
#define ST_LASTPOS(st)   (*(RwUInt32 *)((st) + 0x20c))
#define ST_LASTBLK(st)   (*(RwUInt32 *)((st) + 0x210))
#define ST_ENDBLK(st)    (*(RwUInt32 *)((st) + 0x224))  /* block after the last data; 2 * n = none */
#define ST_DONECB(st)    (*(void (**)(void *node, void *arg))((st) + 0x228))
#define ST_DONEARG(st)   (*(void **)((st) + 0x22c))
#define ST_FLAGS(st)     (*(unsigned char *)((st) + 0x231)) /* bit 0: a block upload is pending */

/* all blocks free, no position, no end mark */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_soft_dsp_queue_reset_005d99f0_n(char *st)
{
  ST_FREE(st) = 0;
  ST_LASTPOS(st) = 0;
  ST_LASTBLK(st) = 0;
  ST_ENDBLK(st) = ST_NBLOCKS(st) * 2;
}

/* once per stream update: follow the voice's play position through the block ring (blocks played -> free), detect the
   end / an underrun, run the mode machine, then fill the next free block from the input connection (or silence). */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
char *rwa_soft_dsp_process_005d9ad0(char *node, void *unused)
{
  char *st = *(char **)(node + 0x20);
  RwUInt32 cur = ST_LASTBLK(st);
  int done = 0;
  int one;

  if (_rwaDXAudioVoiceGetPlaying(ST_VOICE(st))) {
    RwUInt32 pos = ST_PLAYPOS(st);
    RwUInt32 frames = ST_BLKFRAMES(st);
    RwUInt32 played, off, k, i;
    int starved;
    RwUInt32 *desc;

    cur = pos / frames;
    played = (cur < ST_LASTBLK(st) ? ST_NBLOCKS(st) : 0) + (cur - ST_LASTBLK(st));
    /* more blocks played than were queued: the original then reads its descriptor from the node pointer's stack slot */
    desc = played > ST_FREE(st) ? (RwUInt32 *)node : &ST_BLOCKS(st)[cur].a;
    off = desc[3] == 2 ? pos - frames * cur : frames - 1;
    ST_FREE(st) = ST_FREE(st) - played;
    ST_PLAYDESC(st)[3] = 2;
    ST_PLAYDESC(st)[2] = desc[2] + off;
    ST_PLAYDESC(st)[1] = desc[1];
    ST_PLAYDESC(st)[0] = desc[0];
    starved = 0;
    if (cur == ST_ENDBLK(st) && off >= ST_BLOCKS(st)[cur].frames - 1)
      done = 1;
    for (k = played, i = cur; k > 0; ) {      /* walk back over the blocks played since the last update */
      k--;
      if (i == 0) i = ST_NBLOCKS(st);
      i--;
      if (i == ST_ENDBLK(st)) {
        done = 1;
        break;
      }
      if (ST_BLOCKS(st)[i].state == 0)
        starved = 1;
    }
    if (done) {
      one = 1;
      rwa_unit246fd0_005c1dc0(node, 0, &one);
      if (ST_PACKET(st) == ST_SILENCE(st))
        ST_PACKET(st) = 0;
    } else if (starved) {
      char *st2;

      ST_STATE(st) = 7;
      st2 = *(char **)(node + 0x20);
      ST_LASTPOS(st2) = 0;
      ST_LASTBLK(st2) = 0;
      rwa_dxaudio_voice_set_trigger_005ca650(ST_VOICE(st2), 0);
    }
    ST_LASTBLK(st) = cur;
    ST_LASTPOS(st) = pos;
  }

  switch (ST_MODE(st)) {
  case 1:
    if (ST_STATE(st) == 8 && _rwaDXAudioVoiceGetPlaying(ST_VOICE(st))) {
      char *st2 = *(char **)(node + 0x20);

      ST_LASTPOS(st2) = ST_PLAYPOS(st2);
      rwa_dxaudio_voice_set_trigger_005ca650(ST_VOICE(st2), 0);
      if (done)
        rwa_soft_dsp_queue_reset_005d99f0_n(st);
    }
    _rwaStreamNodeProcessComplete(node);
    if (ST_DONECB(st))
      ST_DONECB(st)(node, ST_DONEARG(st));
    break;

  case 2:
    if (ST_STATE(st) != 6 && ST_STATE(st) != 7) {
      if (ST_FREE(st) == ST_NBLOCKS(st)) {
        if (!_rwaDXAudioVoiceGetPlaying(ST_VOICE(st))) {
          char *st2 = *(char **)(node + 0x20);

          _rwaDXAudioVoiceSetWavePos(ST_VOICE(st2), ST_LASTPOS(st2));
          rwa_dxaudio_voice_set_trigger_005ca650(ST_VOICE(st2), 1);
        }
        _rwaStreamNodeProcessComplete(node);
      } else
        ST_STATE(st) = 4;
    }
    /* fall through */
  case 3:
    if (ST_STATE(st) == 4) {
      if (_rwaDXAudioVoiceGetPlaying(ST_VOICE(st))) {
        char *st2 = *(char **)(node + 0x20);

        ST_LASTPOS(st2) = ST_PLAYPOS(st2);
        rwa_dxaudio_voice_set_trigger_005ca650(ST_VOICE(st2), 0);
      }
      if (ST_MODE(st) == 3 && ST_FREE(st) == ST_NBLOCKS(st) && !_rwaDXAudioVoiceGetPlaying(ST_VOICE(st)))
        _rwaStreamNodeProcessComplete(node);
    } else if (ST_STATE(st) == 7 && ST_FREE(st) == ST_NBLOCKS(st)) {
      if (ST_MODE(st) == 3)
        ST_STATE(st) = 4;
      else {
        char *st2;

        ST_STATE(st) = 6;
        st2 = *(char **)(node + 0x20);
        ST_LASTPOS(st2) = 0;
        ST_LASTBLK(st2) = 0;
        rwa_dxaudio_voice_set_trigger_005ca650(ST_VOICE(st2), 1);
      }
    }

    if (ST_FLAGS(st) & 1) {      /* the last upload: finished? */
      if (!rwa_unit2422b0_005bc260(ST_WAVE(st)) && ST_PACKET(st)) {
        RwaPacket *p = ST_PACKET(st);

        if (p->flags & 8)
          ST_ENDBLK(st) = ST_LASTFILL(st);
        ST_FREE(st)++;
        if (p->done)
          p->done->fn(p, p->done->arg);
        ST_PACKET(st) = 0;
      }
      ST_FLAGS(st) &= 0xfe;
      if (ST_FLAGS(st) & 1)
        break;
    }
    if (ST_FREE(st) < ST_NBLOCKS(st)) {
      if (!ST_PACKET(st)) {
        if (ST_ENDBLK(st) >= ST_NBLOCKS(st)) {      /* no end mark: take the next input packet */
          ST_PACKET(st) = (RwaPacket *)RwaStreamConnectionGetPacket(st + 0x10);
          if (ST_PACKET(st)) {
            if (ST_PACKET(st)->flags == 0x10) {      /* a flush packet */
              if (ST_LASTFILL(st) == ST_NBLOCKS(st) * 2) {
                one = 1;
                rwa_unit246fd0_005c1dc0(node, 0, &one);
                rwa_soft_dsp_queue_reset_005d99f0_n(st);
              }
              if (ST_PACKET(st)->done)
                ST_PACKET(st)->done->fn(ST_PACKET(st), ST_PACKET(st)->done->arg);
            } else
              ST_PACKET(st)->desc[3] &= 3;
          } else if (ST_STATE(st) != 4 && ST_STATE(st) != 7)
            ST_PACKET(st) = ST_SILENCE(st);
        } else
          ST_PACKET(st) = ST_SILENCE(st);
      }
      if (ST_PACKET(st)) {
        RwUInt32 blk = ST_FREE(st) + cur;

        if (blk >= ST_NBLOCKS(st))
          blk -= ST_NBLOCKS(st);
        if (FUN_005bc520(ST_WAVE(st), ST_BLKBYTES(st) * blk, ST_PACKET(st)->data, ST_BLKBYTES(st))) {
          RwUInt32 *src = ST_PACKET(st)->desc;
          RwaBlockDesc *d = &ST_BLOCKS(st)[blk];

          d->a = src[0];
          d->b = src[1];
          d->pos = src[2];
          d->state = src[3];
          ST_BLOCKS(st)[blk].frames = FUN_005bd580(ST_WAVEFMT(st), ST_PACKET(st)->bytes);
          if (ST_PACKET(st) != ST_SILENCE(st))
            ST_CURPKT(st) = &ST_BLOCKS(st)[blk];
          else
            ST_BLOCKS(st)[blk].state = 0;
          ST_LASTFILL(st) = blk;
          ST_FLAGS(st) |= 1;
        }
      }
    }
    break;
  }
  rwa_obj_005bace0(node, 1);
  return node;
}

/* entry thunk at the original address of rwa_soft_dsp_queue_reset_005d99f0: register inputs (eax) -> natural C rwa_soft_dsp_queue_reset_005d99f0_n; ret 0 */
__declspec(naked) void rwa_soft_dsp_queue_reset_005d99f0(void)
{
    __asm {
        push eax
        push edx
        push eax
        call rwa_soft_dsp_queue_reset_005d99f0_n
        add esp, 4
        pop edx
        pop eax
        ret
    }
}
