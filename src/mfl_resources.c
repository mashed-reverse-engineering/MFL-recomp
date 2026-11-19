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
 * Port note: without Win32 a small PE .rsrc reader of the same file replaces this.
 * With MFL_FIX_PREFS the settings file beside the exe is read here too (mfl_pref, end of file). */
#include "mfl_fix.h"
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

#if _MSC_VER >= 1900
/* CC=v143 only (W16 DS; VC7 cl 13 = 1300 never sees this): the held-back data rows (media-like: sky texture, image rows, D3D9 shader
 * bytecode; data/link/gen/held_fill.inc) are {0} in this build.  Each is read from the original exe just found, at file offset
 * va - 0x400000 (.rdata / .data of the retail MFL.exe: raw offset = RVA).  Their users (shader creation, sky, image load) run after this. */
extern __declspec(dllimport) void *__stdcall CreateFileA(const char *path, unsigned long access, unsigned long share, void *security,
                                                        unsigned long creation, unsigned long flags, void *template_file);
extern __declspec(dllimport) int __stdcall ReadFile(void *file, void *data, unsigned long size, unsigned long *done, void *overlapped);
extern __declspec(dllimport) int __stdcall CloseHandle(void *handle);
extern __declspec(dllimport) unsigned long __stdcall SetFilePointer(void *file, long distance, long *high, unsigned long method);
extern __declspec(dllimport) int __stdcall VirtualProtect(void *at, unsigned long size, unsigned long protect, unsigned long *old);
struct mfl_held_row { const void *at; unsigned long va, size; };
extern const struct mfl_held_row mfl_held[];      /* ends with a size 0 row */

static void held_fill(const char *exe)
{
  void *file = CreateFileA(exe, 0x80000000UL /* GENERIC_READ */, 1 /* FILE_SHARE_READ */, 0, 3 /* OPEN_EXISTING */, 0, 0);
  unsigned long done, old;
  int n;
  if (file == (void *)-1) return;      /* ponytail: rows stay zero (exe just opened as a module, so not expected); a message box if it ever shows */
  for (n = 0; mfl_held[n].size != 0; n++) {
    VirtualProtect((void *)mfl_held[n].at, mfl_held[n].size, 0x04 /* PAGE_READWRITE */, &old);      /* the shader rows are const (.rdata) */
    SetFilePointer(file, (long)(mfl_held[n].va - 0x400000UL), 0, 0 /* FILE_BEGIN */);
    ReadFile(file, (void *)mfl_held[n].at, mfl_held[n].size, &done, 0);
    VirtualProtect((void *)mfl_held[n].at, mfl_held[n].size, old, &old);
  }
  CloseHandle(file);
}
#endif

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
#if _MSC_VER >= 1900
      held_fill(path);
#endif
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

#if MFL_FIX_PREFS
/* MFL_FIX_PREFS: mflng_settings.cfg beside the exe.  One `key=value` per line (spaces around `=` allowed); the value is 0 or 1.
 * Unknown keys, comment lines (# or ;) and any other value are ignored, so the key keeps its default; no file = every default.
 * Every default is the original behaviour.  Read once, on the first mfl_pref call; nothing writes the file yet (W16: menu page). */
extern __declspec(dllimport) void *__stdcall CreateFileA(const char *path, unsigned long access, unsigned long share, void *security,
                                                        unsigned long creation, unsigned long flags, void *template_file);
extern __declspec(dllimport) int __stdcall ReadFile(void *file, void *data, unsigned long size, unsigned long *done, void *overlapped);
extern __declspec(dllimport) int __stdcall CloseHandle(void *handle);

#define GENERIC_READ 0x80000000UL
#define FILE_SHARE_READ 1
#define OPEN_EXISTING 3
#define INVALID_HANDLE ((void *)-1)
#define PREFS_FILE_MAX 4096      /* a longer file is read up to here */

static const struct { const char *name; int fallback; } k_prefs[MFL_PREF_COUNT] = {      /* order = enum mfl_pref_key (mfl_fix.h) */
  { "skip_intro", 0 },
  { "skip_save_wait", 0 },
  { "unlock_all", 0 },
  { "winner_cam_rotation", 1 },
};
int mfl_prefs[MFL_PREF_COUNT];      /* the values in force (a global, so a test run can read them by the map) */

static int is_blank(char c) { return c == ' ' || c == '\t' || c == '\r'; }

/* one line [line, end): a known key followed by `=` and a lone 0 or 1 sets that key */
static void prefs_parse_line(const char *line, const char *end)
{
  const char *p;
  int k, n;

  while (line < end && is_blank(line[0])) line++;
  while (end > line && is_blank(end[-1])) end--;
  for (k = 0; k < MFL_PREF_COUNT; k++) {
    for (n = 0, p = line; k_prefs[k].name[n] != '\0' && p < end && *p == k_prefs[k].name[n]; n++, p++) ;
    if (k_prefs[k].name[n] != '\0') continue;
    while (p < end && is_blank(*p)) p++;
    if (p == end || *p++ != '=') continue;
    while (p < end && is_blank(*p)) p++;
    if (end - p == 1 && (*p == '0' || *p == '1')) mfl_prefs[k] = *p - '0';
    return;
  }
}

int mfl_pref(int key)
{
  static int read_done;
  static char path[PATH_SIZE];
  static char text[PREFS_FILE_MAX];
  unsigned long dir_len, size = 0;
  const char *line, *end;
  void *file;
  int k;

  if (!read_done) {
    read_done = 1;
    for (k = 0; k < MFL_PREF_COUNT; k++) mfl_prefs[k] = k_prefs[k].fallback;
    dir_len = GetModuleFileNameA(0, path, PATH_SIZE);
    while (dir_len != 0 && path[dir_len - 1] != '\\') dir_len--;
    file = CreateFileA(path_in_dir(path, dir_len, "mflng_settings.cfg"), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (file != INVALID_HANDLE) {
      if (!ReadFile(file, text, PREFS_FILE_MAX, &size, 0)) size = 0;
      CloseHandle(file);
    }
    for (line = text; line < text + size; line = end + 1) {
      for (end = line; end < text + size && *end != '\n'; end++) ;
      prefs_parse_line(line, end);
    }
  }
  return mfl_prefs[key];
}
#endif
