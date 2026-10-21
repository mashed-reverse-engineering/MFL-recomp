// cluster-entries: voice_queue_update
/* Installed cluster voice_queue_update (tools/cluster_install.py from output/cluster_pilot_uc3/voice_queue_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00468690*/ extern void voice_queue_assign_slot(int queue, int first);
/*00468790*/ extern int voice_queue_slot_alloc(int queue);
/*00468a90*/ extern int sound_allowed_for_pair(int queue, int count, int index);
/*004c45c0*/ extern int FUN_004c45c0(int low, int high);
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];

/* A queued voice request (g_sound_8be7e0[queue] points at one): the two cars involved, how many team pairs it covers and the first sound of the range. */
typedef struct VoiceQueueRequest {
  int id;
  int car_a;                 /* +4: car whose team chooses the sound set */
  int car_b;                 /* +8 */
  char pad_c[0xc];
  int pair_count;            /* +0x18 */
  int first;                 /* +0x1c first sound of the range */
  int total;                 /* +0x20 number of sounds */
} VoiceQueueRequest;
/* An entry of the voice slot table of a queue (0x2c bytes). */
typedef struct QueueVoiceSlot {
  char pad_0[4];
  int car_a;                 /* +4 */
  char pad_8[8];
  int counter;               /* +0x10 round robin counter */
  int car_b;                 /* +0x14 */
  char pad_18[0xc];
  int handle;                /* +0x24 */
  int id;                    /* +0x28 */
} QueueVoiceSlot;
extern VoiceQueueRequest *g_sound_8be7e0[4];
extern QueueVoiceSlot *g_sound_stream_slots[4];

/* Starts the voice of queue `queue`: the sound range is chosen from the teams of the two cars (none when they are the same team or neutral; the range
   holds one set of `count` sounds per other team group 0,1,2,5), a start sound is picked (random for the 13 / 25 / 35 sound ranges), the slot table of the
   queue is advanced round robin to the next free sound allowed for the pair, and the slot receives the cars, the id and a freshly allocated handle. */
void voice_queue_update(int queue)
{
  VoiceQueueRequest *request;
  int count;
  int first;
  int start;
  int best;
  int cur;
  int sel;
  int i;
  int team_a, team_b, rank;
  QueueVoiceSlot *slot;

  request = g_sound_8be7e0[queue];
  if (request->pair_count <= 0) {
    first = request->first;
    count = request->total;
  } else {
    count = request->total / request->pair_count;
    team_a = g_player_slots[request->car_a].team;
    if ((unsigned int)team_a > 5) {
      first = queue;
    } else {
      if (team_a == 3 || team_a == 4) {
        return;
      }
      team_b = g_player_slots[request->car_b].team;
      if ((unsigned int)team_b > 5) {
        first = queue;
      } else {
        if (team_b == 3 || team_b == 4 || team_b == team_a) {
          return;
        }
        rank = 0;
        for (i = 0; i < 6; i++) {
          if (i == 3 || i == 4 || i == team_a) {
            continue;
          }
          if (i == team_b) {
            break;
          }
          rank++;
        }
        first = request->first + count * rank;
      }
    }
  }
  if (count == 0xd) {
    start = FUN_004c45c0(0, 6);
    start = start + FUN_004c45c0(0, 6);
    start = start + FUN_004c45c0(0, 1);
  } else if (count == 0x19) {
    start = FUN_004c45c0(0, 0xc);
    start = start + FUN_004c45c0(0, 0xc);
    start = start + FUN_004c45c0(0, 1);
  } else if (count == 0x23) {
    start = FUN_004c45c0(0, 0x11);
    start = start + FUN_004c45c0(0, 0x11);
    start = start + FUN_004c45c0(0, 1);
  } else {
    start = queue;
  }
  if (first == -1) {
    return;
  }
  voice_queue_assign_slot(queue, first);
  if (g_sound_stream_slots[queue] == 0) {
    return;
  }
  best = -1;
  if (count > 0) {
    cur = start;
    for (i = count; i != 0; i--) {
      sel = cur;
      if (cur >= count) {
        sel = cur - count;
      }
      if (sound_allowed_for_pair(queue, count, sel) == 0) {
        g_sound_stream_slots[queue][sel].counter = 1;
      }
      slot = &g_sound_stream_slots[queue][sel];
      if (slot->counter == 0) {
        if (best == -1) {
          g_sound_stream_slots[queue][sel].counter = 1;
          best = sel;
        }
      } else {
        slot->counter = slot->counter + 1;
        if (slot->counter >= count) {
          slot->counter = 0;
        }
      }
      cur++;
    }
    if (best != -1) {
      g_sound_stream_slots[queue] = g_sound_stream_slots[queue] + best;
      g_sound_stream_slots[queue]->car_b = g_sound_8be7e0[queue]->car_b;
      g_sound_stream_slots[queue]->car_a = g_sound_8be7e0[queue]->car_a;
    }
  }
  g_sound_stream_slots[queue]->id = g_sound_8be7e0[queue]->id;
  g_sound_stream_slots[queue]->handle = voice_queue_slot_alloc(queue);
}

