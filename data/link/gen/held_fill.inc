/* W16 DS, CC=v143 only (dsform.py join --held-zero appends this to data.c): the held-back rows are {0} in this build;
 * mfl_held_fill (recomp/mfl_resources.c) reads each from the user's MFL_original.exe at file offset va - 0x400000. */
extern const char g_d3d9_shaders_5e1838[];
extern const char g_d3d9_shaders_5e1920[];
extern const char g_d3d9_shaders_5e1980[];
extern const char g_d3d9_shaders_5e19f0[];
struct mfl_held_row { const void *at; unsigned long va, size; };
const struct mfl_held_row mfl_held[] = {
  {g_d3d9_shaders_5e1838, 0x005e1838, 232},
  {g_d3d9_shaders_5e1920, 0x005e1920, 96},
  {g_d3d9_shaders_5e1980, 0x005e1980, 112},
  {g_d3d9_shaders_5e19f0, 0x005e19f0, 108},
  {g_skydome_follow_5fb164__t, 0x005fb168, 20120},
  {g_png_do_600000, 0x00600000, 128},
  {g_png_build_600080, 0x00600080, 8064},
  {g_png_init_602000, 0x00602000, 5496},
  {0, 0, 0}
};
