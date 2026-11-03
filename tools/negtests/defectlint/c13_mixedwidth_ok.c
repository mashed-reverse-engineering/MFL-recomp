// expect: clean class 13
void negtest_fn(float *pfVar3)
{
  *(int *)&pfVar3[2] = 0;
  if (*(int *)&pfVar3[2] == 0) {
    pfVar3[3] = 1.0f;
  }
}
