#include "mfl_tier2.h"
/*00480260*/ extern int __cdecl text_pages_find_string(char *p0);
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7260*/ extern char * lua_tostring(void * p0, int p1);
/*004c72d0*/ extern void __cdecl lua_pushnumber(void *L, double n);

/* size of the local buffer that holds the copied Lua string argument */
#define NAME_BUF_SIZE (128)

/* lua_Physics_Scenario_Get_Function */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* Lua function Physics_Scenario_Get_Function (registered by the script engine) */
/*
 * Takes the string named by the last Lua argument (inferred), copies it into a local
 * buffer and pushes text_pages_find_string(name) as a number (the text id, inferred).
 * L: the Lua state. Returns 1 (one result pushed).
 * BUG: the copy loop has no bounds check, so a name of NAME_BUF_SIZE or more chars overflows name_buf.
 */

undefined4 lua_Physics_Scenario_Get_Function(undefined4 L)

{
  char ch;
  undefined4 arg_count;
  char *src;
  int text_id;
  char *dst;
  char name_buf [NAME_BUF_SIZE];

  arg_count = lua_gettop(L);
  src = (char *)lua_tostring(L,arg_count);
  dst = name_buf;
  /* copy the string, including its terminating NUL, into name_buf (no bounds check) */
  do {
    ch = *src;
    src = src + 1;
    *dst = ch;
    dst = dst + 1;
  } while (ch != '\0');
  text_id = text_pages_find_string(name_buf);
  lua_pushnumber(L,(double)text_id);
  return 1;
}


