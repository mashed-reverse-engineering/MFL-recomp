// expect: class 4
/* call with more arguments than the prototype has (load_waterfall_fx -> clump_first_atomic, an_ok_080) */
extern int neg_one_param(int a);
void negtest_fn(void)
{
  neg_one_param(1, 2);
}
