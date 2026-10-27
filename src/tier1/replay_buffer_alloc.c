#include "mfl_tier2.h"
/* compile: /GS */
/*00494460*/ extern int physics_buffer_size(int a, int b);
/*004a8420*/ extern int debug_printf(char *fmt, ...);
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *fmt, ...);
/*006037d0*/ extern int g_replay_6037d0[];
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*008d8b08*/ extern void *g_replay_buffer;
/*008d8b0c*/ extern unsigned int g_replay_buffer_size;
typedef int (__cdecl *RwSprintfFn)(char *, char *, ...);
#define rwsprintf RW_GLOBAL(0xc4, RwSprintfFn) /* stringFuncs.vecSprintf */
extern void *__cdecl memset(void *, int, unsigned int);
#pragma intrinsic(memset)
/* replay_buffer_alloc */
/* "allocates the ghost buffer; its size (0x4178C) must equal the sum of the track times * 7.5 frames" */
int replay_buffer_alloc(void)
{
  int *entry;
  char path[64];

  g_replay_buffer_size = 0;
  if (g_replay_6037d0[0] != -1) {
    entry = g_replay_6037d0;
    do {
      g_replay_buffer_size = g_replay_buffer_size + physics_buffer_size(entry[1], 0xf);
      entry += 2;
    } while (entry[0] != -1);
  }
  if (g_replay_buffer_size != 0x4178c) {
    debug_printf("GHOST Size ERROR %d != %d\n", g_replay_buffer_size, 0x4178c);
    for (;;) {
    }
  }
  FID_conflict__wprintf("Replay buffer will be %d bytes in size\n", 0x4178c);
  g_replay_buffer = RwMalloc(g_replay_buffer_size, 0);
  if (g_replay_buffer != 0) {
    memset(g_replay_buffer, 0, g_replay_buffer_size);
    rwsprintf(path, "%sAll%d.rep", "U:\\toast\\", 0x10b);
  }
  return 1;
}
