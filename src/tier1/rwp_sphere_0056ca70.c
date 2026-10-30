/* compile: /G5 */
/* Sets the value of a leaf node, or of every node below a group node (kind 8; the child list is re-read after each recursive call). */
struct RwpNode;
typedef struct { struct RwpNode *items; unsigned count; } RwpNodeList;
typedef struct { short kind; } RwpNodeType;
typedef struct RwpNode { char pad0[0x40]; RwpNodeList *list; char pad1[0x10]; int value; char pad2[0x4]; RwpNodeType *type; } RwpNode;
void rwp_sphere_0056ca70(RwpNode *n, int v)
{
  unsigned i;
  RwpNodeList *l;
  if (n->type->kind == 8) {
    l = n->list;
    for (i = 0; i < l->count; i++) { rwp_sphere_0056ca70(&l->items[i], v); l = n->list; }
  } else n->value = v;
}
