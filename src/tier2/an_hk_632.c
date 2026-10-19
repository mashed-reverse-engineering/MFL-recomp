#include "mfl_tier2.h"
extern struct TrailSlot g_trails[6];
/*00488230*/ extern int __cdecl clump_load_logged(void *p0, RwChar *p1);
/*004c3b70*/ extern int stream_read_file();
/*004c3e40*/ extern int FUN_004c3e40(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*004c3f60*/ extern RpAtomic * clump_first_atomic(RpClump * p0);
/*004c5440*/ extern int __cdecl FUN_004c5440(int a1, int a2, int a3);
/*004c56c0*/ extern void FUN_004c56c0(undefined4 p0, undefined4 p1);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*004c6730*/ extern int piz_find_entry_size_004c6a70();
/*00653148*/ extern undefined4 g_trail_653148;
/*0065314c*/ extern char g_car_65314c[];
#define MODEL_BUFFER_SIZE (0x400)
#define TRAIL_SLOT_WORDS (0xb)
/* load_lazer_model */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* lazer.dff */

/*
 * Loads the laser tower model LaserTower.dff into g_trail_653148 and the
 * lazer beam model lazer.dff into the g_trails[] slots: one clump per slot,
 * built from the file buffer, its first atomic stored in the slot's w00 and
 * the slot's active flag cleared. Takes no parameters, returns nothing.
 * model_buffer colour bytes 0xff,0x00,0x00,0xff read as RGBA red (inferred),
 * passed to FUN_004c56c0 (inferred: sets the clump colour).
 */
void load_lazer_model(void)

{
  undefined4 size_or_clump;
  uint first_atomic;
  uint *trail_cursor;
  struct {
    undefined1 colour_r;
    undefined1 colour_g;
    undefined1 colour_b;
    undefined1 colour_a;
    undefined1 file_data [1024];
  } model_buffer;

  model_buffer.colour_r = 0xff;
  model_buffer.colour_a = 0xff;
  model_buffer.colour_g = 0;
  model_buffer.colour_b = 0;
  mem_zero(model_buffer.file_data,MODEL_BUFFER_SIZE);
  thunk_FUN_004c6a60();
  size_or_clump = piz_find_entry_size_004c6a70("lazer.dff");
  stream_read_file("lazer.dff",model_buffer.file_data,size_or_clump);
  g_trail_653148 = clump_load_logged("LaserTower.dff",(RwChar *)0x0);
  trail_cursor = &g_trails[0].active;
  /* one pass per g_trails slot, until trail_cursor reaches g_car_65314c
     (inferred: the global after g_trails); each pass builds a clump,
     stores its first atomic in w00 and clears active */
  do {
    size_or_clump = FUN_004c3e40(model_buffer.file_data,MODEL_BUFFER_SIZE,0,0);
    FUN_004c5440(size_or_clump,0x40,1);
    FUN_004c56c0(size_or_clump,&model_buffer.colour_r);
    first_atomic = (uint)clump_first_atomic((RpClump *)size_or_clump);
    ((TrailSlot *)(trail_cursor + -1))->w00 = first_atomic;
    *trail_cursor = 0;
    trail_cursor = trail_cursor + TRAIL_SLOT_WORDS;
  } while ((int)trail_cursor < ((int)&g_car_65314c));
  return;
}


