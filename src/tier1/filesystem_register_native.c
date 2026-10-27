#include "mfl_tier2.h"
/*004a48d0*/ extern int filesystem_register_native_b(void);
int filesystem_register_native(void)
{
  return filesystem_register_native_b();
}
