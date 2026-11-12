/* rwa_ds_buffer_destroy_005cd2f0 (0x005cd2f0): _rwaDSBufferDestroy, release the DirectSound interfaces of a buffer record */
#include "mfl_tier2.h"

/* COM IUnknown::Release, vtable slot 2 */
typedef int (__stdcall *ComReleaseFn)(void *self);
#define COM_RELEASE(p) ((*(ComReleaseFn **)(p))[2](p))

/* The record holds three interface pointers: { buffer, 3D buffer, notify } (+0, +4, +8); released in reverse order.
   The result is the last Release's count, or 0 when the buffer pointer is NULL. */
/* TIER2: unvalidated 2026-11-10 authority=static */
int rwa_ds_buffer_destroy_005cd2f0(void **rec)
{
  if (rec[2] != 0)
    COM_RELEASE(rec[2]);
  if (rec[1] != 0)
    COM_RELEASE(rec[1]);
  if (rec[0] != 0)
    return COM_RELEASE(rec[0]);
  return 0;
}
