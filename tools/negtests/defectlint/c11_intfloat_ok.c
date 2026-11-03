// expect: clean class 11
int negtest_fn(int *flag)
{
  if (*flag != 0) {
    return 1;
  }
  return 0;
}
