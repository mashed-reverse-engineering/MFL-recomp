/* compile: /G5 */
#include "mfl_tier2.h"
extern char *RwEngineInstance;
typedef struct RwpListNode { int value; char pad[0xc]; struct RwpListNode *next; } RwpListNode;
typedef struct RwpObjBuffers { char pad[0x10]; void *buffer_a; char pad2[4]; void *buffer_b; char pad3[0x10]; RwpListNode *list; } RwpObjBuffers;
/* Clears the object's list values and frees its two buffers; returns 1. (Rwp library) */
int rwp_obj_0056e550(RwpObjBuffers *o)
{
  RwpListNode *n;
  for (n = o->list; n != 0; n = n->next) n->value = 0;
  o->list = 0;
  RwFree(o->buffer_a);
  if (o->buffer_b != 0) RwFree(o->buffer_b);
  return 1;
}
