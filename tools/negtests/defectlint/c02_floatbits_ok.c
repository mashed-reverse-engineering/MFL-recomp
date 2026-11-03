// expect: clean class 2
extern float g_camera_fov_factor;
void negtest_fn(void)
{
  g_camera_fov_factor = 0.75f;
}
