// expect: class 18
/* &local to a naked wrapper defined in the same file: MSVC drops the store / reload (T-P16/T-P17 menu_page_enter) */
extern int g_neg_out;
__declspec(naked) int neg_wrapper_r(int a, int *out)
{
  __asm {
    ret
  }
}
void negtest_fn(void)
{
  int v;
  v = -1;
  neg_wrapper_r(1, &v);
  g_neg_out = v;
}
