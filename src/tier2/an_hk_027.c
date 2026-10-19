#include "mfl_tier2.h"
/*004357e0*/ extern int *__cdecl career_record_ptr(int p0, int p1);
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7220*/ extern double __cdecl lua_tonumber(void *L, int index);

/* lua_Set_Track_Powerups */

/* Lua function Set_Track_Powerups (registered by the script engine).
 * Takes 7 Lua arguments: two record keys (inferred: course and slot), then five
 * powerup values (inferred: one per entry of the record). The last five arguments
 * are read from the top of the Lua stack, the first two from 6 and 5 below it.
 * Looks up the career record for the two keys and, if it exists, writes the five
 * powerup values into its first five ints. Returns 0 (no Lua results). */

undefined4 lua_Set_Track_Powerups(void *L)

{
  int arg_count;
  int record_key_a;
  int record_key_b;
  int powerup_0;
  int powerup_1;
  int powerup_2;
  int powerup_3;
  int powerup_4;
  int *record;

  arg_count = lua_gettop(L);
  record_key_a = (int)lua_tonumber(L,arg_count + -6);
  record_key_b = (int)lua_tonumber(L,arg_count + -5);
  powerup_0 = (int)lua_tonumber(L,arg_count + -4);
  powerup_1 = (int)lua_tonumber(L,arg_count + -3);
  powerup_2 = (int)lua_tonumber(L,arg_count + -2);
  powerup_3 = (int)lua_tonumber(L,arg_count + -1);
  powerup_4 = (int)lua_tonumber(L,arg_count);
  record = career_record_ptr(record_key_a,record_key_b);
  /* Write only when the record exists; a missing record is silently skipped. */
  if (record != (int *)0x0) {
    *record = powerup_0;
    record[1] = powerup_1;
    record[2] = powerup_2;
    record[3] = powerup_3;
    record[4] = powerup_4;
  }
  return 0;
}
