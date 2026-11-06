/* rwa_obj_disconnect_param_index_005bad90 (0x005bad90): objects (RwaObjDisconnectParamIndex): free every connection of one parameter (kind 1: of an output parameter, else: of an input) */
#include "mfl_tier2.h"

/*005bed80*/ extern void RwaFreeListFree(void *list, void *item);
/*007dae3c*/ extern undefined g_data_7dae3c;      /* free list of the 0x1c byte connection items */

typedef struct RwaLink { struct RwaLink *next, *prev; } RwaLink;
typedef struct RwaConnection {      /* 0x1c bytes: linked into the source's and the destination's parameter lists */
  RwUInt32 w0, w4, w8;
  RwaLink in;                       /* +0x0c */
  RwaLink out;                      /* +0x14 */
} RwaConnection;

#define UNLINK(l) ((l)->prev->next = (l)->next, (l)->next->prev = (l)->prev)

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
char *rwa_obj_disconnect_param_index_005bad90(char *obj, int index, int kind)
{
  char *data = *(char **)(obj + 0x20);
  RwaLink **lists, *p, *end, *next;
  RwaConnection *c;

  if (data == 0)
    lists = (RwaLink **)(obj + ((*(unsigned char *)(obj + 0x1c) & 2) ? 0x2c : 0x28));
  else
    lists = (RwaLink **)((*(RwUInt32 *)(*(char **)obj + 0x40) & 0xfffffff) + data + ((*(unsigned char *)(obj + 0x1c) & 2) ? 4 : 0));
  if (kind == 1) {      /* output parameter: list heads of 3 words, the connections linked by their `out` link */
    p = ((RwaLink **)lists[0])[index * 3];
    end = ((RwaLink **)lists[0])[index * 3 + 1];
    while (p != end) {
      c = (RwaConnection *)((char *)p - 0x14);
      next = p->next;
      UNLINK(&c->in);
      UNLINK(&c->out);
      RwaFreeListFree(&g_data_7dae3c, c);
      p = next;
    }
  } else {      /* input parameter: list heads of 2 words, linked by `in` */
    p = ((RwaLink **)lists[1])[index * 2];
    end = ((RwaLink **)lists[1])[index * 2 + 1];
    while (p != end) {
      c = (RwaConnection *)((char *)p - 0xc);
      next = p->next;
      UNLINK(&c->in);
      UNLINK(&c->out);
      RwaFreeListFree(&g_data_7dae3c, c);
      p = next;
    }
  }
  return obj;
}
