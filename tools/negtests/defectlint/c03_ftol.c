// expect: class 3
/* _ftol() without its ST0 argument (stopclock_set_digit_count_c, usercall_candidates/b002) */
extern int _ftol();
int negtest_fn(float f)
{
  int iVar1;
  iVar1 = _ftol();
  return iVar1;
}
