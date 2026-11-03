// expect: clean class 18
extern int g_neg_out;
__declspec(naked) int neg_wrapper_r(int a, int *out)
{
  __asm {
    ret
  }
}
void negtest_fn(void)
{
  volatile int v;
  v = -1;
  neg_wrapper_r(1, (int *)&v);
  g_neg_out = v;
}
