#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall CoTaskMemAlloc(int);
/*005d41ea*/ extern int wcscpy();
/*005d4238*/ extern int __cdecl wcslen(undefined4 p0);
#define NAME_PTR_OFFSET (0x14)
#define WCHAR_SIZE (2)
#define HRESULT_OUT_OF_MEMORY (0x8007000e)
/* FUN_005d0370 */

/* Copies the wide string at entry+NAME_PTR_OFFSET into a new CoTaskMem block (inferred: a name
   field of a record) and stores the block in *out_name; the caller frees it.
   entry: pointer to the record (inferred). out_name: receives the new buffer.
   Returns 0 (S_OK) on success, HRESULT_OUT_OF_MEMORY if the allocation fails. */
undefined4 __stdcall FUN_005d0370(char *entry, undefined4 *out_name)

{
  int name_len;
  LPVOID name_copy;

  /* length in wide chars; buffer holds those chars plus the terminating NUL */

  name_len = wcslen(*(undefined4 *)(entry + NAME_PTR_OFFSET));
  name_copy = CoTaskMemAlloc(name_len * WCHAR_SIZE + WCHAR_SIZE);
  *out_name = name_copy;
  if (name_copy != (LPVOID)0x0) {
    wcscpy(name_copy,*(undefined4 *)(entry + NAME_PTR_OFFSET));
    return 0;
  }
  return HRESULT_OUT_OF_MEMORY;
}


