// expect: clean class 8
/*004c7150*/ extern int lua_gettop();
int negtest_fn(void *L)
{
  return lua_gettop(L);
}
