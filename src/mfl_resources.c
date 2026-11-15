/* mfl_resources.c: where the game's resources come from (platform layer, not an original function).
 * Our exe links no .rsrc.  The dialogs, strings, bitmaps and the RWTEXDICTIONARY load icon are read at run time from the
 * user's own original exe beside ours, mapped as a data file (no code of it runs).  The handle goes into g_app_7e7700, the
 * one module handle every resource call of the game uses (LoadStringA, LoadBitmapA, DialogBoxParamA, resource_load).
 * window_create (byte-matched) stores our hinstance there first; app_initialise_on_bootup calls mfl_resources_attach
 * right after it, before the first resource use.
 * Install layouts: ours as MFL.exe with the original copied to MFL_original.exe (the test harness: tools/wine-run/prepare.py),
 * or ours under another name beside the untouched MFL.exe.  A candidate counts only when it holds the video dialog
 * (our own exe has no .rsrc).  None found: message box naming the expected path, exit.
 * Only names the original exe imports have import libs (tools/mk_importlibs.py): LoadLibraryExA comes through GetProcAddress (as in recomp/release/mfl_trap.c).
 * Port note: without Win32 a small PE .rsrc reader of the same file replaces this. */
extern __declspec(dllimport) void *__stdcall GetModuleHandleA(const char *name);
extern __declspec(dllimport) void *__stdcall GetProcAddress(void *module, const char *name);
extern __declspec(dllimport) unsigned long __stdcall GetModuleFileNameA(void *module, char *path, unsigned long size);
extern __declspec(dllimport) void *__stdcall FindResourceA(void *module, const char *name, const char *type);
extern __declspec(dllimport) int __stdcall FreeLibrary(void *module);
extern __declspec(dllimport) int __stdcall MessageBoxA(void *window, const char *text, const char *caption, unsigned int type);
extern __declspec(dllimport) void __stdcall ExitProcess(unsigned int code);
extern __declspec(dllimport) int __stdcall WriteFile(void *file, const void *data, unsigned long size, unsigned long *written, void *overlapped);
typedef void *(__stdcall *load_library_ex_fn)(const char *path, void *file, unsigned long flags);
typedef unsigned long (__stdcall *get_env_fn)(const char *name, char *value, unsigned long size);
typedef void *(__stdcall *get_std_handle_fn)(unsigned long which);

/*007e7700*/ extern void *g_app_7e7700;      /* resource module handle (window_create, app_get_app_7e7700) */

#define LOAD_LIBRARY_AS_DATAFILE 0x00000002
#define RT_DIALOG ((const char *)5)
#define VIDEO_DIALOG_ID ((const char *)0x65)      /* cl_video_config_dialog.c VIDEO_DIALOG_RESOURCE_ID: in every original MFL.exe */
#define MB_ICONERROR 0x10
#define PATH_SIZE 260
#define STD_ERROR_HANDLE ((unsigned long)-12)

static const char *const k_original_names[] = { "MFL_original.exe", "MFL.exe" };
static const char k_missing[] = "The game resources (dialogs, texts, pictures) are read from your original MFL.exe.\n"
                                "None was found. Put a copy of your original MFL.exe here:\n";

/* path[0..dir_len) = our exe's directory; appends name, returns path */
static char *path_in_dir(char *path, unsigned long dir_len, const char *name)
{
  unsigned long i;
  for (i = 0; name[i] != '\0' && dir_len + i < PATH_SIZE - 1; i++) path[dir_len + i] = name[i];
  path[dir_len + i] = '\0';
  return path;
}

void mfl_resources_attach(void)
{
  static char path[PATH_SIZE];
  static char text[sizeof k_missing + PATH_SIZE];
  unsigned long dir_len, n, i, written;
  void *kernel32 = GetModuleHandleA("kernel32.dll");
  load_library_ex_fn load_library_ex;
  get_env_fn get_env;
  char nobox[4];
  void *module;

  dir_len = GetModuleFileNameA(0, path, PATH_SIZE);
  while (dir_len != 0 && path[dir_len - 1] != '\\') dir_len--;
  load_library_ex = (load_library_ex_fn)GetProcAddress(kernel32, "LoadLibraryExA");
  for (n = 0; load_library_ex != 0 && n < sizeof k_original_names / sizeof k_original_names[0]; n++) {
    module = load_library_ex(path_in_dir(path, dir_len, k_original_names[n]), 0, LOAD_LIBRARY_AS_DATAFILE);
    if (module == 0) continue;
    if (FindResourceA(module, VIDEO_DIALOG_ID, RT_DIALOG) != 0) {
      g_app_7e7700 = module;
      return;
    }
    FreeLibrary(module);      /* our own exe (no .rsrc) or another file */
  }
  for (n = 0; k_missing[n] != '\0'; n++) text[n] = k_missing[n];
  path_in_dir(path, dir_len, k_original_names[0]);
  for (i = 0; path[i] != '\0'; i++) text[n + i] = path[i];
  text[n + i++] = '\n';
  text[n + i] = '\0';
  /* MFL_TRAP_NOBOX=1 (headless test runs, as recomp/release/mfl_trap.c): the text goes to stderr, no box that would wait for a click */
  get_env = (get_env_fn)GetProcAddress(kernel32, "GetEnvironmentVariableA");
  if (get_env != 0 && get_env("MFL_TRAP_NOBOX", nobox, sizeof nobox) != 0) {
    WriteFile(((get_std_handle_fn)GetProcAddress(kernel32, "GetStdHandle"))(STD_ERROR_HANDLE), text, n + i, &written, 0);
  } else {
    MessageBoxA(0, text, "Drive to Survive", MB_ICONERROR);
  }
  ExitProcess(1);
}
