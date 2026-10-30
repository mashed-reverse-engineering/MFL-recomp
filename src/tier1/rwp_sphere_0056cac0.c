/* compile: /G5 */
/* Sets the value of a leaf node, or of every node below a group node (kind 8; the child list is re-read after each recursive call). */
struct RwpNode;
typedef struct { struct RwpNode *items; unsigned count; } RwpNodeList;
typedef struct { short kind; } RwpNodeType;
typedef struct RwpNode { char pad0[0x40]; RwpNodeList *list; char pad1[0xc]; int value; char pad2[0x8]; RwpNodeType *type; } RwpNode;
void rwp_sphere_0056cac0(RwpNode *n, int v)
{
  unsigned i;
  RwpNodeList *l;
  if (n->type->kind == 8) {
    l = n->list;
    for (i = 0; i < l->count; i++) { rwp_sphere_0056cac0(&l->items[i], v); l = n->list; }
  } else n->value = v;
}
