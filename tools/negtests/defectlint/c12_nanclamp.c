// expect: class 12
/* Ghidra's clamp idiom: NaN takes the clamp value (camera_path_sample, an_hk_238) */
extern float float_1_0;
extern float k_f_0;
float negtest_fn(float param_3)
{
  float fVar6;
  fVar6 = float_1_0;
  if ((param_3 <= float_1_0) && (fVar6 = param_3, param_3 < k_f_0)) {
    fVar6 = k_f_0;
  }
  return fVar6;
}
