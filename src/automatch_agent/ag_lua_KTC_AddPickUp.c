#include "mfl_tier2.h"
/*00406b90*/ extern undefined __cdecl ktc_add_pickup(int p0, float *p1, int p2, float p3);
/*004c7150*/ extern int lua_gettop();
/*004c7220*/ extern double lua_tonumber();


/* lua_KTC_AddPickUp */

/* Lua function KTC_AddPickUp (registered by the script engine) */

undefined4 lua_KTC_AddPickUp(int L)

{
  int n;
  int a;
  int b;
  float v[3];
  float w;

  n = lua_gettop(L);
  a = (int)lua_tonumber(L, n - 5);
  v[0] = (float)lua_tonumber(L, n - 4);
  v[1] = (float)lua_tonumber(L, n - 3);
  v[2] = (float)lua_tonumber(L, n - 2);
  b = (int)lua_tonumber(L, n - 1);
  w = (float)lua_tonumber(L, n);
  ktc_add_pickup(a, v, b, w);
  return 0;
}
