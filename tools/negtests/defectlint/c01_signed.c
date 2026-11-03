// expect: class 1
/* unsigned compare of an undefined4 global against a literal (fx_render_all_and_highlight, an_ok_004) */
extern undefined4 g_fx_object_count;
void negtest_fn(void)
{
  if (0 < g_fx_object_count) {
    g_fx_object_count = 0;
  }
}
