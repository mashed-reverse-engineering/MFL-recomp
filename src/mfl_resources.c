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
/* MFL_FIX_PREFS: mflng_settings.cfg beside the exe.  One `key=value` per line (spaces around `=` allowed); the value is one digit
 * 0..max of the key (0 or 1 for the switches), or `auto` for a key whose default is auto (-1); display_mode also takes
 * fullscreen / windowed / borderless.  Unknown keys, comment lines (# or ;) and any other value are ignored, so the key keeps its
 * default; no file = every default.  Every default is the original behaviour, except the MFL_FIX_LANG keys (read only in that
 * build) and display_mode (borderless).  Read once, on the first
 * mfl_pref call; mfl_settings_save rewrites the file (the Preferences page, recomp/mfl_settings_menu.c). */
extern __declspec(dllimport) void *__stdcall CreateFileA(const char *path, unsigned long access, unsigned long share, void *security,
                                                        unsigned long creation, unsigned long flags, void *template_file);
extern __declspec(dllimport) int __stdcall ReadFile(void *file, void *data, unsigned long size, unsigned long *done, void *overlapped);
extern __declspec(dllimport) int __stdcall CloseHandle(void *handle);

#define GENERIC_READ 0x80000000UL
#define GENERIC_WRITE 0x40000000UL
#define FILE_SHARE_READ 1
#define CREATE_ALWAYS 2
#define OPEN_EXISTING 3
#define INVALID_HANDLE ((void *)-1)
#define PREFS_FILE_MAX 4096      /* a longer file is read up to here (ponytail: the language writer keeps only these bytes of it) */

static const struct { const char *name; int fallback, max; } k_prefs[MFL_PREF_COUNT] = {      /* order = enum mfl_pref_key (mfl_fix.h) */
  { "skip_intro", 0, 1 },
  { "skip_save_wait", 0, 1 },
  { "unlock_all", 0, 1 },
  { "winner_cam_rotation", 1, 1 },
  { "language", -1, 5 },      /* -1 = auto */
  { "setup_dialogs", 0, 1 },
  { "display_mode", MFL_DISPLAY_BORDERLESS, 2 },      /* plan W17 F3: the fixes exe starts borderless */
  { "skip_flyover", 0, 1 },
};
static const char *const k_display_names[] = { "fullscreen", "windowed", "borderless" };      /* display_mode words = MFL_DISPLAY_* */
int mfl_prefs[MFL_PREF_COUNT];      /* the values in force (a global, so a test run can read them by the map) */
static char prefs_path[PATH_SIZE];
static char prefs_text[PREFS_FILE_MAX];

static int is_blank(char c) { return c == ' ' || c == '\t' || c == '\r'; }

/* line [line, end) is `name =`: returns the value start (blanks skipped), else 0 */
static const char *key_value(const char *line, const char *end, const char *name)
{
  int n;

  while (line < end && is_blank(line[0])) line++;
  for (n = 0; name[n] != '\0' && line < end && *line == name[n]; n++, line++) ;
  if (name[n] != '\0') return 0;
  while (line < end && is_blank(*line)) line++;
  if (line == end || *line++ != '=') return 0;
  while (line < end && is_blank(*line)) line++;
  return line;
}

/* one line [line, end): a known key followed by `=` and a lone digit 0..max (or auto) sets that key */
static void prefs_parse_line(const char *line, const char *end)
{
  const char *p, *q, *w;
  int k, n;

  while (end > line && is_blank(end[-1])) end--;
  for (k = 0; k < MFL_PREF_COUNT; k++) {
    p = key_value(line, end, k_prefs[k].name);
    if (p == 0) continue;
    if (end - p == 1 && *p >= '0' && *p <= '0' + k_prefs[k].max) mfl_prefs[k] = *p - '0';
    else if (k_prefs[k].fallback < 0 && end - p == 4 && p[0] == 'a' && p[1] == 'u' && p[2] == 't' && p[3] == 'o') mfl_prefs[k] = -1;
    for (n = 0; k == MFL_PREF_DISPLAY_MODE && n < 3; n++) {
      for (q = p, w = k_display_names[n]; q < end && *w != '\0' && *q == *w; q++, w++) ;
      if (q == end && *w == '\0') mfl_prefs[k] = n;
    }
    return;
  }
}

/* the settings file into prefs_text (prefs_path = its path); returns the bytes read, 0 without a file */
static unsigned long prefs_read(void)
{
  unsigned long dir_len, size = 0;
  void *file;

  dir_len = GetModuleFileNameA(0, prefs_path, PATH_SIZE);
  while (dir_len != 0 && prefs_path[dir_len - 1] != '\\') dir_len--;
  file = CreateFileA(path_in_dir(prefs_path, dir_len, "mflng_settings.cfg"), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
  if (file != INVALID_HANDLE) {
    if (!ReadFile(file, prefs_text, PREFS_FILE_MAX, &size, 0)) size = 0;
    CloseHandle(file);
  }
  return size;
}

int mfl_pref(int key)
{
  static int read_done;
  unsigned long size;
  const char *line, *end;
  int k;

  if (!read_done) {
    read_done = 1;
    for (k = 0; k < MFL_PREF_COUNT; k++) mfl_prefs[k] = k_prefs[k].fallback;
    size = prefs_read();
    for (line = prefs_text; line < prefs_text + size; line = end + 1) {
      for (end = line; end < prefs_text + size && *end != '\n'; end++) ;
      prefs_parse_line(line, end);
    }
  }
  return mfl_prefs[key];
}

/* the Preferences page: the file is written again with every line that is not one of the keys as it was (comments, unknown keys), then
 * `key=value` for every key (display_mode as its word, language -1 as auto) */
void mfl_settings_save(const int *values)
{
  static char out[PREFS_FILE_MAX + 32 * MFL_PREF_COUNT];
  unsigned long size, n = 0, written;
  const char *line, *end, *w;
  void *file;
  int k, known;

  mfl_pref(0);      /* prefs_path is set */
  size = prefs_read();
  for (line = prefs_text; line < prefs_text + size; line = end + 1) {
    for (end = line; end < prefs_text + size && *end != '\n'; end++) ;
    for (k = 0, known = 0; k < MFL_PREF_COUNT; k++) known |= key_value(line, end, k_prefs[k].name) != 0;
    if (known) continue;
    while (line < end) out[n++] = *line++;
    if (end < prefs_text + size) out[n++] = '\n';
    else if (n != 0 && out[n - 1] != '\n') { out[n++] = '\r'; out[n++] = '\n'; }
  }
  for (k = 0; k < MFL_PREF_COUNT; k++) {
    for (w = k_prefs[k].name; *w != '\0'; w++) out[n++] = *w;
    out[n++] = '=';
    if (k == MFL_PREF_DISPLAY_MODE && values[k] >= 0 && values[k] <= 2) w = k_display_names[values[k]];
    else if (values[k] < 0) w = "auto";
    else { out[n++] = (char)('0' + values[k]); w = ""; }
    while (*w != '\0') out[n++] = *w++;
    out[n++] = '\r';
    out[n++] = '\n';
  }
  file = CreateFileA(prefs_path, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
  if (file == INVALID_HANDLE) return;      /* read-only install folder: the choices hold for this run only */
  WriteFile(file, out, n, &written, 0);
  CloseHandle(file);
}
#endif

#if MFL_FIX_LANG
/* MFL_FIX_LANG: a start without the launcher's options gets the launcher's `-VS0 -CS0 -L<n>` (mfl_fix.h).
 * launch.exe (VB6) starts `MFL.exe -VS0 -CS0` plus the language the installer stored in HKLM\Software\Empire Interactive\MFL
 * "language"; its two setup buttons start `-CS1 -VS0` / `-CS0 -VS1`.  Without options the data defaults (both dialog flags 1,
 * language 0) show both dialogs, in English. */
extern __declspec(dllimport) unsigned long __stdcall GetFileAttributesA(const char *path);
typedef unsigned short (__stdcall *get_ui_language_fn)(void);
/*004a93a0*/ extern char *app_get_71908(void);      /* the command line app_parse_command_line reads */
/*0062ad34*/ extern int g_app_62ad34;      /* 1 = video setup dialog (data default 1; -vs0 / -vs1) */
/*0062ad38*/ extern int g_app_62ad38;      /* 1 = controller setup dialog (data default 1; -cs0 / -cs1) */
/*0076e874*/ extern int g_language_offset;      /* -l<n>: string table offset and game language id */

#define FILE_ATTRIBUTE_DIRECTORY 0x10
#define LANGUAGES_WITH_SPEECH 5      /* ids 0..4; 5 (USA) has English speech */

/* by game language id: the Windows primary language id, the speech folder the game loads (sound_language_path) */
static const unsigned short k_primary_langid[LANGUAGES_WITH_SPEECH] = { 0x09, 0x0c, 0x07, 0x0a, 0x10 };
static const char *const k_speech_dir[LANGUAGES_WITH_SPEECH] = {
  "toastaudio\\pc\\audio\\pcdics\\english", "toastaudio\\pc\\audio\\pcdics\\french", "toastaudio\\pc\\audio\\pcdics\\german",
  "toastaudio\\pc\\audio\\pcdics\\spanish", "toastaudio\\pc\\audio\\pcdics\\italian",
};

/* 1 when cmd holds a space-separated token <option><digit> in any case (app_parse_command_line compares with __stricmp) */
static int option_given(const char *cmd, const char *option)
{
  const char *p;
  int n;

  while (*cmd != '\0') {
    while (*cmd == ' ') cmd++;
    for (n = 0, p = cmd; option[n] != '\0' && (*p | 0x20) == option[n]; n++, p++) ;
    if (option[n] == '\0' && *p >= '0' && *p <= '9' && (p[1] == ' ' || p[1] == '\0')) return 1;
    while (*cmd != ' ' && *cmd != '\0') cmd++;
  }
  return 0;
}

/* 1 when the speech folder of game language id 1..4 is installed; the others (auto, English, USA) use the English speech.  The path is
 * relative to the current folder, as the game's own loads (sound_language_path).  Also the Preferences page's Language row filter. */
int mfl_lang_available(int language)
{
  unsigned long attr;

  if (language < 1 || language >= LANGUAGES_WITH_SPEECH) return 1;
  attr = GetFileAttributesA(k_speech_dir[language]);
  return attr != 0xffffffffUL && (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/* the game language of the Windows UI language when its speech folder is installed, else 0 (English) */
static int language_detect(void)
{
  get_ui_language_fn get_ui = (get_ui_language_fn)GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetUserDefaultUILanguage");
  int primary = get_ui != 0 ? get_ui() & 0x3ff : 0x09, n;

  for (n = 1; n < LANGUAGES_WITH_SPEECH; n++) {
    if (k_primary_langid[n] == primary && mfl_lang_available(n)) return n;
  }
  return 0;
}

void mfl_lang_apply(void)
{
  const char *cmd = app_get_71908();
  int dialogs = 0, language = -1;

#if MFL_FIX_PREFS
  dialogs = mfl_pref(MFL_PREF_SETUP_DIALOGS);
  language = mfl_pref(MFL_PREF_LANGUAGE);
#endif
  if (!option_given(cmd, "-vs")) g_app_62ad34 = dialogs;
  if (!option_given(cmd, "-cs")) g_app_62ad38 = dialogs;
  if (!option_given(cmd, "-l")) g_language_offset = language >= 0 ? language : language_detect();
}
#endif
