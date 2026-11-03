// expect: class 8
/* `()` declaration called with fewer arguments than every original call site pushes (lua_Set_Track_Powerups, an_ok_072) */
/*004c7150*/ extern int lua_gettop();
int negtest_fn(void *L)
{
  return lua_gettop();
}
