// expect: clean class 12
extern float float_1_0;
extern float k_f_0;
float negtest_fn(float param_3)
{
  float fVar6;
  fVar6 = param_3;
  if (param_3 > float_1_0) fVar6 = float_1_0;
  else if (param_3 < k_f_0) fVar6 = k_f_0;
  return fVar6;
}
