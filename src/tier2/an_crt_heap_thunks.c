/* libcmt heap entry points that the exe keeps outside the split members (B3, own CRT layer).
   operator_new (new.obj, 0x4b3c0c): `operator new(size)` = _nh_malloc with the new-handler flag set.
   _free_004b45fd (0x4b3bca): a 5 byte `jmp free` thunk the original link placed in front of the heap code; tier 1 callers
   (video_dialog_free) name it, so it stays an entry point and forwards to free. */
extern void *__cdecl _nh_malloc(unsigned int size, int call_new_handler);
extern void __cdecl free(void *block);

/* operator new: allocate, calling the installed new handler (and retrying) while the heap is exhausted */
/* TIER2: unvalidated 2026-11-04 authority=diffemu */
void *__cdecl operator_new(unsigned int size)
{
    return _nh_malloc(size, 1);
}

/* the free thunk: same argument, same (void) result */
/* TIER2: unvalidated 2026-11-04 authority=diffemu */
void __cdecl _free_004b45fd(void *block)
{
    free(block);
}
