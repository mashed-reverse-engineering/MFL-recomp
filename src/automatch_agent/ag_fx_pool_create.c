#include "mfl_tier2.h"
/*00497820*/ extern int __cdecl pool_create(undefined4 p0);
/*007519b8*/ extern undefined4 g_particles_7519b8;


/* fx_pool_create */

/* "g_particles_7519b8 = pool_create(0x500) and fills the free list" */

void fx_pool_create(void)

{
  int iVar1;
  
  iVar1 = 0;
  g_particles_7519b8 = pool_create(0x500);
  do {
    pool_push(g_particles_7519b8,iVar1);
    iVar1 = iVar1 + 1;
  } while (iVar1 < 0x500);
  return;
}
