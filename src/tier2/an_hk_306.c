#include "mfl_tier2.h"
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7220*/ extern double __cdecl lua_tonumber(void *L, int index);
/*0089df68*/ extern undefined4 g_gfx_89df68;
/*0089df94*/ extern undefined4 g_gfx_89df94;
#define OBJ_BLOCK_STRIDE (0x944)
#define OBJ_SUBCELL_STRIDE (0x117)
#define OBJ_VALUE_SLOT (3)
#define OBJ_COUNT_SLOT (0xb)
/* lua_Physics_Scenario_Trigger_Add_Objects */

/* Lua function Physics_Scenario_Trigger_Add_Objects (registered by the script engine) */

/* Lua binding: Physics_Scenario_Trigger_Add_Objects(x, value, extra) (inferred).
   Reads three numbers from the top of the Lua stack: x (top-2), value (top-1), extra (top); extra is read and ignored.
   Stores value into the object table at x and bumps that cell's count. Returns 0.
   lua_state: Lua state passed by the script engine.
   Note: arg_count holds the stack top first, then the block index; x_pos holds x first, then the sub-cell (inferred). */
undefined4 lua_Physics_Scenario_Trigger_Add_Objects(char *lua_state)

{
  int arg_count;
  uint x_pos;
  undefined4 value;

  arg_count = lua_gettop(lua_state);
  x_pos = (int)lua_tonumber(lua_state,arg_count + -2);
  value = (int)lua_tonumber(lua_state,arg_count + -1);
  lua_tonumber(lua_state,arg_count);
  /* split x into block index (x / 8, rounding toward zero for negative x) and sub-cell (x & 7) */
  arg_count = (int)(x_pos + ((int)x_pos >> 0x1f & 7U)) >> 3;
  x_pos = x_pos & 7;
  /* store value into the cell's value slot */
  (&g_gfx_89df68)
  [arg_count * OBJ_BLOCK_STRIDE + x_pos * OBJ_SUBCELL_STRIDE + (&g_gfx_89df94)[arg_count * OBJ_BLOCK_STRIDE + x_pos * OBJ_SUBCELL_STRIDE] + OBJ_VALUE_SLOT] =
       value;
  /* increment the cell's count */
  (&g_gfx_89df68)[arg_count * OBJ_BLOCK_STRIDE + x_pos * OBJ_SUBCELL_STRIDE + OBJ_COUNT_SLOT] =
       (&g_gfx_89df68)[arg_count * OBJ_BLOCK_STRIDE + x_pos * OBJ_SUBCELL_STRIDE + OBJ_COUNT_SLOT] + 1;
  return 0;
}


