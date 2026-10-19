#include "mfl_tier2.h"
/*00406be0*/ extern int ktc_new_copter();
/*004b2b48*/ extern int _ftol();
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7220*/ extern double __cdecl lua_tonumber(void *L, int index);
/*004c7260*/ extern char * lua_tostring(void * p0, int p1);
/*004c72d0*/ extern void __cdecl lua_pushnumber(void *L, double n);

#define ARG_SLOT_BYTES (4) /* size of one int argument slot in the stack array */
#define HIGH_DWORD_SHIFT (32) /* bit shift that moves the low dword into the high dword */
/* lua_KTC_NewCopter */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* Lua function KTC_NewCopter (registered by the script engine) */

/*
 * Lua binding: KTC_NewCopter(name, arg2, arg3, ...).
 * lua_state: the Lua state the script engine passes in.
 * Arg 1 is a string copied into a 64-byte local buffer with no length check (inferred: overflows if longer).
 * Args 2..N are converted to int and stored in argument order in a 20-byte stack array (5 slots);
 * no bound on N is checked (inferred: more than 6 Lua args overflows it).
 * Calls ktc_new_copter(name, args, num_args) and pushes its int result as the single Lua return value.
 * Returns 1 (number of Lua results).
 */
undefined4 lua_KTC_NewCopter(undefined4 lua_state)

{
  float arg_value;
  int num_args;
  char ch;
  int arg_count;
  char *name_src;
  undefined4 arg_word;
  char *name_dst;
  uint ebx_high;
  int remaining;
  undefined4 *arg_slot;
  longlong ebx_reg_pair;
  union {
    char gs_buf[88];
    struct {
      int iStack_5c;
      undefined1 auStack_58 [20];
      char acStack_44 [64];
    } m;
  } frame;
  
  arg_count = lua_gettop((void *)lua_state);
  num_args = arg_count + -1;
  name_src = (char *)lua_tostring((void *)lua_state,arg_count - num_args);
  name_dst = frame.m.acStack_44;
  /* Copy the name string (arg 1) into the local buffer, including its NUL terminator */
  do {
    ch = *name_src;
    name_src = name_src + 1;
    *name_dst = ch;
    name_dst = name_dst + 1;
  } while (ch != '\0');
  /*
   * Convert args 2..N to int. Walks from the last Lua arg to the first, filling slots from the end,
   * so the array ends up in Lua argument order.
   * ebx_high / ebx_reg_pair: register-pair artifact (inferred, check disassembly). Only the high dword
   * is read; the low dword is always overwritten with 0x48cc0a and never read.
   */
  if (0 < num_args) {
    ebx_reg_pair = (ulonglong)ebx_high << HIGH_DWORD_SHIFT;
    arg_slot = (undefined4 *)(frame.m.auStack_58 + num_args * ARG_SLOT_BYTES + -ARG_SLOT_BYTES);
    remaining = num_args;
    do {
      arg_word = (undefined4)((ulonglong)ebx_reg_pair >> 0x20);
      arg_value = lua_tonumber((void *)lua_state,arg_count);
      ebx_reg_pair = CONCAT44(arg_word,0x48cc0a);
      arg_word = ((int)(arg_value));
      *arg_slot = arg_word;
      arg_slot = arg_slot + -1;
      arg_count = arg_count + -1;
      remaining = remaining + -1;
    } while (remaining != 0);
  }
  /* Call the copter creator with the name, the argument array and the count; push its result */
  frame.m.iStack_5c = ktc_new_copter(frame.m.acStack_44,frame.m.auStack_58,num_args);
  lua_pushnumber((void *)lua_state,(double)frame.m.iStack_5c);
  return 1;
}


