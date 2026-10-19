#include "mfl_tier2.h"
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *p0);
/*00486f10*/ extern undefined __cdecl particle_system_setup_ptank(uint *p0, uint p1, uint p2, struct RwTexture *p3);
/*004c3b70*/ extern int stream_read_file();
/*004c3d80*/ extern int texdict_load_file(char * p0, int p1, int p2);
/*004c3e40*/ extern int FUN_004c3e40(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*004c6730*/ extern int piz_find_entry_size_004c6a70();
/*004d5e10*/ extern struct RwTexDictionary * RwTexDictionarySetCurrent(struct RwTexDictionary *p0);
/*006a8bc0*/ extern struct Powerup g_powerups[25];
/*006a9390*/ extern undefined g_powerup_glow;
/*006a9398*/ extern undefined g_powerups_init_6a9398;
/*006a93d4*/ extern undefined4 g_powerup_icons_txd;

#define PUGLOW_PTANK_TYPE (0x817) /* particle_system_setup_ptank p1 (inferred) */
#define PUGLOW_PTANK_COUNT (0x19) /* particle_system_setup_ptank p2 (inferred; same as the 25 powerup slots) */
#define READ_BUFFER_SIZE (0x800) /* bytes of stack buffer for IconCube.dff (inferred) */
#define FLOAT_ONE_BITS (0x3f800000) /* IEEE-754 bit pattern of 1.0f */
/* powerups_init */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/*
 * Start-up setup for the powerup pickups. Takes no parameters, returns nothing.
 * Sets up the "puglow" glow particle system, loads PowerUpIcons.txd into
 * g_powerup_icons_txd, reads IconCube.dff into a stack buffer, then creates
 * the clump for each of the 25 powerup slots in g_powerups from that buffer.
 * (was: "loads PowerUpIcons.txd and IconCube.dff once per slot, creates the glow particle system")
 */

void powerups_init(void)

{
  RwTexture *texture_dict;
  undefined4 result;
  undefined4 *cursor;
  undefined4 *next_cursor;
  undefined1 read_buffer [READ_BUFFER_SIZE];
  
  texture_dict = (RwTexture *)find_texture_dict_b("puglow");
  particle_system_setup_ptank((uint *)&g_powerup_glow,PUGLOW_PTANK_TYPE,PUGLOW_PTANK_COUNT,texture_dict);
  g_powerup_icons_txd = texdict_load_file("PowerUpIcons.txd",0,0);
  RwTexDictionarySetCurrent((RwTexDictionary *)0x0);
  mem_zero(read_buffer,READ_BUFFER_SIZE);
  thunk_FUN_004c6a60();
  /* size of IconCube.dff, then read the file into read_buffer */
  result = piz_find_entry_size_004c6a70("IconCube.dff");
  stream_read_file("IconCube.dff",read_buffer,result);
  /* walk the 25 slots backwards, 0x50 bytes (struct Powerup) per pass:
     the clump from FUN_004c3e40 goes to slot offset 0, and 1.0f / 0 / 1.0f / 0
     go to slot offsets 8..20, which the dossier lists as unnamed padding (inferred) */
  cursor = (undefined4 *)&g_powerups_init_6a9398;
  do {
    next_cursor = cursor + -0x14;
    result = FUN_004c3e40(read_buffer,READ_BUFFER_SIZE,0,0);
    cursor[-0x16] = result;
    *next_cursor = FLOAT_ONE_BITS;
    cursor[-0x13] = 0;
    cursor[-0x12] = FLOAT_ONE_BITS;
    cursor[-0x11] = 0;
    cursor = next_cursor;
  } while (next_cursor != (undefined4 *)((char *)&g_powerups[0] + 8));
  return;
}


