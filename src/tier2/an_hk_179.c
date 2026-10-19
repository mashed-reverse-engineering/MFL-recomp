#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall CoTaskMemFree(int);
#define BLOCK_SUBBUFFER_OFFSET (0x44) /* offset of the owned sub-buffer pointer (inferred) */
/* FUN_005cdcc0
 * Frees a block allocated with CoTaskMemAlloc (inferred): first frees the sub-buffer
 * pointer stored at BLOCK_SUBBUFFER_OFFSET if it is non-NULL, then frees the block itself.
 * block: the COM-allocated block to release (inferred). Returns nothing.
 */

void FUN_005cdcc0(LPVOID block)

{
  /* release the owned sub-buffer before the block that points to it */
  if (*(LPVOID *)((LONG_PTR)block + BLOCK_SUBBUFFER_OFFSET) != (LPVOID)0x0) {
    CoTaskMemFree(*(LPVOID *)((LONG_PTR)block + BLOCK_SUBBUFFER_OFFSET));
  }
  CoTaskMemFree(block);
  return;
}


