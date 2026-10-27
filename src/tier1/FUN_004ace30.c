#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall CoTaskMemAlloc(int);
extern char PTR_g_data_6e0049_005e219c[];
/*004b1090*/ extern int __stdcall FUN_004b1090(void *a1, void *a2);
#define OBJECT_MEM_SIZE (8)
#define HR_E_POINTER (0x80004003)
#define HR_E_OUTOFMEMORY (0x8007000e)
/* FUN_004ace30
 * COM-style creation helper (inferred): allocates an 8-byte object block with
 * CoTaskMemAlloc, stores its address in *out_object, then passes the block and
 * the global PTR_g_data_6e0049_005e219c to FUN_004b1090 (inferred: initialises it).
 * outer_unknown is never read (inferred: aggregation pointer, unused here).
 * Returns 0 (S_OK) on success, HR_E_POINTER if out_object is NULL,
 * HR_E_OUTOFMEMORY if the allocation fails. */
undefined4 __stdcall FUN_004ace30(char *outer_unknown, undefined4 *out_object)
{
  LPVOID object_mem;

  if (out_object == (undefined4 *)0x0) {
    return HR_E_POINTER;
  }
  object_mem = CoTaskMemAlloc(OBJECT_MEM_SIZE);
  *out_object = object_mem;
  if (object_mem == (LPVOID)0x0) {
    return HR_E_OUTOFMEMORY;
  }
  FUN_004b1090(object_mem,&PTR_g_data_6e0049_005e219c);
  return 0;
}
