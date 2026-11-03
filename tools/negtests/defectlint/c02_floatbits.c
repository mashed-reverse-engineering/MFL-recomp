// expect: class 2
/* int bit pattern assigned to a float: converts (game_options_reset, an_ok_082) */
extern float g_camera_fov_factor;
void negtest_fn(void)
{
  g_camera_fov_factor = 0x3f400000;
}
