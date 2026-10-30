/* compile: /G5 */
typedef struct RwpEngine { char pad[0x108]; unsigned char *(*alloc)(unsigned size, int flags); } RwpEngine;
extern RwpEngine *RwEngineInstance;
/* Allocates size + align bytes through the engine allocator and returns the next align-aligned address inside the block; the distance back to the block start (1..align, one byte) is stored in the byte before the returned address. The allocator hint is copied to a local first (this selects the frame slot of the pad byte). */
unsigned char *rwp_capsule_00575b20(unsigned size, unsigned align, int flags)
{
  unsigned char *p, *q;
  unsigned char pad;
  int hint = flags;
  p = RwEngineInstance->alloc(size + align, hint);
  if (!p) return 0;
  pad = (unsigned char)align - (unsigned char)((unsigned)p % align);
  if (!pad) pad = (unsigned char)align;
  q = p + pad;
  q[-1] = pad;
  return q;
}
