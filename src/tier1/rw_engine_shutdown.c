#include "mfl_tier2.h"
/*004a2d00*/ extern int rw_engine_stop_close_term();
/* Shuts down the RenderWare engine: calls rw_engine_stop_close_term() (inferred: stops and closes the engine)
 * and discards its int result. Takes no parameters, returns nothing. */
void rw_engine_shutdown(void)
{
  rw_engine_stop_close_term();
}
