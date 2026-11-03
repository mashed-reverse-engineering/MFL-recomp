// expect: class 11
/* an int object compared with a float literal (car_apply_forces in_contact) */
int negtest_fn(int *flag)
{
  if (*flag != 0.0f) {
    return 1;
  }
  return 0;
}
