#include "mfl_tier2.h"
typedef struct V3 { float x, y, z; } V3;
typedef struct Contact { int pad0; int f4; int pad8; V3 v; } Contact;
/* Sets the contact parameters of the body of an object: every contact gets the given value and the constants 1.0, 1.0, 0.5. */
int rwp_object_set_contact_params(int object, int *value)
{
  char *body = *(char **)(object + 0x18);
  V3 v;
  int i;
  v.x = 1.0f; v.y = 1.0f; v.z = 0.5f;
  for (i = 0; i < *(int *)(body + 0x24); i++) {
    Contact *c = (Contact *)(*(int **)(body + 0x20))[i];
    c->f4 = *value;
    c->v = v;
  }
  return object;
}
