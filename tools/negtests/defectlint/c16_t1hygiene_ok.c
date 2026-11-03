// expect: clean class 16
// path: recomp/tier1/negtest_t1.c
extern int g_neg_used;
/* Returns the used global. */
int negtest_fn(void)
{
  return g_neg_used;
}
