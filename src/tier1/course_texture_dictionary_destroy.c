#include "mfl_tier2.h"
/*00485240*/ extern RwBool RwTexDictionaryDestroy_004d5ac0(struct RwTexDictionary *p0);
/*00664414*/ extern undefined4 g_load_664414;


/* course_texture_dictionary_destroy */

/* "RwTexDictionaryDestroy(0x664414)" */

void course_texture_dictionary_destroy(void)
{
  RwTexDictionaryDestroy_004d5ac0(g_load_664414);
  return;
}
