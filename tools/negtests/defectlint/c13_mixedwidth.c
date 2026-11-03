// expect: class 13
/* an int flag word compared with a float literal (ai_trigger_zone_target_for_car, an_hk_1003) */
void negtest_fn(float *pfVar3)
{
  *(int *)&pfVar3[2] = 0;
  if (pfVar3[2] == 0.0) {
    pfVar3[3] = 1.0f;
  }
}
