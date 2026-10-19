#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall CoTaskMemFree(int);
/*004b29a0*/ extern int __stdcall FUN_004b29a0(int a1);
/* FUN_004b2a30 */

/* Frees a block allocated with CoTaskMemAlloc (inferred from the CoTaskMemFree call).
 * FUN_004b29a0 is called on it first (inferred: releases resources owned by the block).
 * Does nothing when mem_block is NULL. */
void __stdcall FUN_004b2a30(LPVOID mem_block)

{
  if (mem_block != (LPVOID)0x0) {
    FUN_004b29a0(mem_block);
    CoTaskMemFree(mem_block);
  }
  return;
}


