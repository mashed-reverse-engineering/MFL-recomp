// expect: clean class 1
extern undefined4 g_fx_object_count;
void negtest_fn(void)
{
  if (0 < (int)g_fx_object_count) {
    g_fx_object_count = 0;
  }
}
