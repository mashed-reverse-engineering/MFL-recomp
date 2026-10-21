// cluster-entries: FUN_004afee0
/* Installed cluster FUN_004afee0 (tools/cluster_install.py from output/cluster_pilot_uc3/FUN_004afee0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"

/* A 16 byte identifier (passed by value to the methods). */
typedef struct Guid16 { unsigned int d[4]; } Guid16;
typedef struct ComObject { void **vtbl; } ComObject;
/* Second level record (8 bytes). */
typedef struct SubEntry { Guid16 *guid_d; Guid16 *guid_c; } SubEntry;
/* First level record (0x24 bytes): five plain words, a type id, one more word, and a list of SubEntry. */
typedef struct Entry {
  int word[5];
  Guid16 *guid_b;            /* +0x14 */
  int tail;                  /* +0x18 */
  unsigned int sub_count;    /* +0x1c */
  SubEntry *subs;            /* +0x20 */
} Entry;
/* The request: a main id, two words and a table of entries. */
typedef struct Request {
  Guid16 *guid_a;
  int word_1, word_2;
  unsigned int entry_count;  /* +0xc */
  Entry *entries;            /* +0x10 */
} Request;

typedef int (__stdcall *SetupFn)(ComObject *, Guid16);
typedef int (__stdcall *SetupFn2)(ComObject *, Guid16, int, int);
typedef int (__stdcall *EntryFn)(ComObject *, Guid16, int, int, int, int, int, Guid16, int);
typedef int (__stdcall *SubFn)(ComObject *, Guid16, int, Guid16, Guid16);

/* Registers a request with the COM object `obj` (method 7: the main id; with `full` also method 3: id and two words, then for every entry method 5 and for
   every sub entry of it method 6; the first failing result stops the walk).  A "file not found" (0x80070002) result is turned into success; a NULL
   request returns 1 without doing anything. */
int FUN_004afee0_n(Request *request, ComObject *obj, int full)
{
  int result;
  unsigned int i, k;
  Entry *entry;
  SubEntry *sub;

  if (request == 0) {
    return 1;
  }
  result = ((SetupFn)obj->vtbl[7])(obj, *request->guid_a);
  if (full != 0) {
    result = ((SetupFn2)obj->vtbl[3])(obj, *request->guid_a, request->word_1, request->word_2);
    if (result >= 0) {
      for (i = 0; i < request->entry_count; i++) {
        entry = &request->entries[i];
        result = ((EntryFn)obj->vtbl[5])(obj, *request->guid_a, entry->word[0], entry->word[1], entry->word[2], entry->word[3], entry->word[4], *entry->guid_b, entry->tail);
        if (result < 0) {
          break;
        }
        for (k = 0; k < request->entries[i].sub_count; k++) {
          sub = &request->entries[i].subs[k];
          result = ((SubFn)obj->vtbl[6])(obj, *request->guid_a, request->entries[i].word[0], *sub->guid_d, *sub->guid_c);
          if (result < 0) {
            break;
          }
        }
        if (result < 0) {
          break;
        }
      }
    }
  }
  if (result == 0x80070002) {
    result = 0;
  }
  return result;
}

/* entry thunk at the original address of FUN_004afee0: register inputs (s0,s1,s2) -> natural C FUN_004afee0_n; ret 12 */
__declspec(naked) void FUN_004afee0(void)
{
    __asm {
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        call FUN_004afee0_n
        add esp, 12
        ret 12
    }
}
