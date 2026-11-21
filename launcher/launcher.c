/* launcher.c: MFLng-launch.exe.  Shows the original launcher picture (read at run time from the user's own launch.exe), starts
   MFLng.exe / MFLng.nofix.exe, checks the GitHub releases and updates the MFLng files (and itself) through an apply step (the same exe in --apply-update mode).
   Install layout (W17 LAYOUT C): the exes and mflrw.dll in the game folder beside this exe; the backend DLLs, INSTALL.txt, BUILDINFO.txt,
   SHA256SUMS and LICENSES\ in its sub folder mflng\.  The update itself (staging, applying, rollback) is logic.c (upd_*), which the
   native tests run; this file gives it its file layer (os_*) and does what needs Windows: download, elevation, restart.
   Win32 only (user32, gdi32, shell32, advapi32, winhttp, kernel32).  Layout of the files it creates: README.md. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winhttp.h>
#include <shellapi.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "logic.h"

#define STR_(x) #x
#define STR(x) STR_(x)
#ifdef LAUNCHER_VER      /* -DLAUNCHER_VER=<token> (build.sh): no quotes on the command line, they do not survive every shell */
#define LAUNCHER_VERSION STR(LAUNCHER_VER)
#else
#define LAUNCHER_VERSION "dev"
#endif
#define W_(s) L##s
#define WSTR(s) W_(s)
#define CREDIT_SHORT L"MFL-recomp by SciLor and contributors"      /* project credit: public/.github/FUNDING.yml (the user confirms the wording) */
#define CREDIT_THANKS L"Thanks to L2KSolkov (codfh-win32) and tgsm (pbg)."      /* public/README.md Acknowledgements */
#define NOTICE L"You need your own copy of the game. Not affiliated with the game's publisher or developer."
#define WFMT(b, ...) (_snwprintf(b, sizeof(b) / sizeof(WCHAR) - 1, __VA_ARGS__), b[sizeof(b) / sizeof(WCHAR) - 1] = 0)
enum { ID_PLAY = 100, ID_NOFIX, ID_EXIT, ID_MIN, ID_CLOSE, ID_STATUS, ID_AUTO, ID_CHAN, ID_CHECK, ID_WEB, ID_ABOUT, ID_OV_UPDATE, ID_OV_CLOSE,
       WM_CHECKED = WM_APP, WM_STAGED };
enum { PIC_W = 640, PIC_H = 480 };      /* the original launcher's picture and window size (96 dpi; scaled by the system DPI) */
enum { SK_BUSY, SK_INFO, SK_ERR, SK_UPD };

/* g_exe = the installed launcher; g_self = the file this process runs from (the same, but for an apply step that runs from a copy);
   g_twin = <launcher>.new.exe once it has taken the launcher's place and can go */
static WCHAR g_exe[MAX_PATH], g_self[MAX_PATH], g_twin[MAX_PATH + 8], g_dir[MAX_PATH], g_data[MAX_PATH], g_ini[MAX_PATH], g_testzip[MAX_PATH], g_err[512], g_stat[200], *g_body;
static int g_inplace;
#define BOX_FLAGS (MB_OK | MB_SETFOREGROUND | MB_TOPMOST)      /* a box of the apply step has no window to sit on: never behind others */
static HWND g_wnd;
static unsigned char *g_pic; static const BITMAPINFO *g_bi; static const unsigned char *g_bits;
static Installed g_inst;
static int g_auto, g_pre, g_force_elev, g_nrel, g_busy, g_upd, g_stat_kind;
static Release g_rel[30];
static const Release *g_new;

/* ---- small helpers ---- */
static void pj(WCHAR *o, const WCHAR *a, const WCHAR *b) { _snwprintf(o, MAX_PATH - 1, L"%s\\%s", a, b); o[MAX_PATH - 1] = 0; }
static int exists(const WCHAR *p) { return GetFileAttributesW(p) != INVALID_FILE_ATTRIBUTES; }
static WCHAR *wide(const char *s)
{
    int n = MultiByteToWideChar(CP_UTF8, 0, s, -1, NULL, 0); WCHAR *w = (WCHAR *)calloc((size_t)n + 1, sizeof(WCHAR));
    if (w) MultiByteToWideChar(CP_UTF8, 0, s, -1, w, n);
    return w;
}
static unsigned char *read_file(const WCHAR *p, size_t *n)
{
    HANDLE h = CreateFileW(p, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    DWORD sz, got = 0; unsigned char *b = NULL;
    if (h == INVALID_HANDLE_VALUE) return NULL;
    sz = GetFileSize(h, NULL);
    if (sz < 256u * 1024 * 1024 && (b = (unsigned char *)malloc(sz + 1)) && ReadFile(h, b, sz, &got, NULL) && got == sz) { b[sz] = 0; if (n) *n = sz; }
    else { free(b); b = NULL; }
    CloseHandle(h);
    return b;
}
static int write_file(const WCHAR *p, const void *b, size_t n)      /* complete file or nothing (a failed write is deleted) */
{
    HANDLE h = CreateFileW(p, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); DWORD put = 0; int ok;
    if (h == INVALID_HANDLE_VALUE) return 0;
    ok = WriteFile(h, b, (DWORD)n, &put, NULL) && put == n && FlushFileBuffers(h);
    CloseHandle(h);
    if (!ok) DeleteFileW(p);
    return ok;
}
/* ---- the file layer of the update code (logic.h os_*): UTF-8 paths in, Win32 out ---- */
static char g_dir8[UPD_PATH], g_data8[UPD_PATH]; static int g_elev;
static WCHAR *wp(const char *s, WCHAR *w) { if (!MultiByteToWideChar(CP_UTF8, 0, s, -1, w, UPD_PATH)) w[0] = 0; return w; }
static void u8(const WCHAR *w, char *o) { if (!WideCharToMultiByte(CP_UTF8, 0, w, -1, o, UPD_PATH, NULL, NULL)) o[0] = 0; }
unsigned char *os_read(const char *p, size_t *n) { WCHAR w[UPD_PATH]; return read_file(wp(p, w), n); }
int os_write(const char *p, const void *b, size_t n) { WCHAR w[UPD_PATH]; return write_file(wp(p, w), b, n); }
int os_exists(const char *p) { WCHAR w[UPD_PATH]; return exists(wp(p, w)); }
int os_copy(const char *a, const char *b) { WCHAR x[UPD_PATH], y[UPD_PATH]; return CopyFileW(wp(a, x), wp(b, y), FALSE) != 0; }
int os_mkdir(const char *p) { WCHAR w[UPD_PATH]; return CreateDirectoryW(wp(p, w), NULL) != 0; }
int os_rmdir(const char *p) { WCHAR w[UPD_PATH]; return RemoveDirectoryW(wp(p, w)) != 0; }
static void rmtree(WCHAR *w, size_t l)      /* w[UPD_PATH] holds the folder (l characters); a link (junction) is removed, never entered */
{
    WIN32_FIND_DATAW fd; HANDLE h; size_t n;
    if (l + 2 < UPD_PATH && (wcscpy(w + l, L"\\*"), h = FindFirstFileW(w, &fd)) != INVALID_HANDLE_VALUE) {
        do {
            n = wcslen(fd.cFileName);
            if (!wcscmp(fd.cFileName, L".") || !wcscmp(fd.cFileName, L"..") || l + 2 + n >= UPD_PATH) continue;
            wcscpy(w + l + 1, fd.cFileName);
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY) SetFileAttributesW(w, fd.dwFileAttributes & ~FILE_ATTRIBUTE_READONLY);
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) DeleteFileW(w);
            else if (fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) RemoveDirectoryW(w);
            else rmtree(w, l + 1 + n);
        } while (FindNextFileW(h, &fd));
        FindClose(h);
    }
    w[l] = 0; RemoveDirectoryW(w);
}
void os_rmtree(const char *p) { WCHAR w[UPD_PATH]; size_t l = wcslen(wp(p, w)); if (l && !(GetFileAttributesW(w) & FILE_ATTRIBUTE_REPARSE_POINT)) rmtree(w, l); }
long os_err(void) { return (long)GetLastError(); }
/* a rename that failed for good: everything about it into the log, and the first one into the error box */
static WCHAR g_fail[900];
static void rename_failed(const WCHAR *x, const WCHAR *y, DWORD e)
{
    WCHAR msg[200] = L"", vol[MAX_PATH] = L"", fs[32] = L"unknown"; char x8[UPD_PATH], y8[UPD_PATH], m8[600], f8[100]; size_t l;
    DWORD ax = GetFileAttributesW(x), ay = GetFileAttributesW(y);
    FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, e, 0, msg, 199, NULL);
    for (l = wcslen(msg); l && msg[l - 1] <= L' ';) msg[--l] = 0;
    if (GetVolumePathNameW(x, vol, MAX_PATH)) GetVolumeInformationW(vol, NULL, 0, NULL, NULL, NULL, fs, 32);
    u8(x, x8); u8(y, y8);
    if (!WideCharToMultiByte(CP_UTF8, 0, msg, -1, m8, sizeof m8, NULL, NULL)) m8[0] = 0;
    if (!WideCharToMultiByte(CP_UTF8, 0, fs, -1, f8, sizeof f8, NULL, NULL)) f8[0] = 0;
    os_log("rename failed: %.900s -> %.900s: error %lu (%s); file system %s; source attributes 0x%lx; destination %s, attributes 0x%lx",
           x8, y8, (unsigned long)e, m8, f8, (unsigned long)ax, ay == INVALID_FILE_ATTRIBUTES ? "does not exist" : "exists", (unsigned long)ay);
    if (!g_fail[0]) WFMT(g_fail, L"Cannot rename %.300s (Windows error %lu: %s)\nDrive file system: %s.%s", x, e, msg, fs,
                         e == ERROR_ACCESS_DENIED || e == ERROR_SHARING_VIOLATION ?
                         L"\nA virus scanner or another program may hold the file, a second launcher window may be open, or the folder may be on a FAT/exFAT drive." : L"");
}
/* The plain move first (the only form for a folder), then over an existing file.  Access denied / in use: tried again for about 3 s
   (a virus scanner lets go of a new file after its scan), and a read-only attribute is taken off. */
int os_rename(const char *a, const char *b)
{
    WCHAR x[UPD_PATH], y[UPD_PATH]; DWORD e = 0, at; int i;
    wp(a, x); wp(b, y);
    for (i = 0; i < 12; i++) {
        if (i) Sleep(250);
        if (MoveFileExW(x, y, 0)) return 1;
        e = GetLastError(); at = GetFileAttributesW(y);
        if ((e == ERROR_ALREADY_EXISTS || e == ERROR_FILE_EXISTS) && at != INVALID_FILE_ATTRIBUTES && !(at & FILE_ATTRIBUTE_DIRECTORY)) {
            if (at & FILE_ATTRIBUTE_READONLY) SetFileAttributesW(y, at & ~FILE_ATTRIBUTE_READONLY);
            if (MoveFileExW(x, y, MOVEFILE_REPLACE_EXISTING)) return 1;
            e = GetLastError();
        }
        if (e != ERROR_ACCESS_DENIED && e != ERROR_SHARING_VIOLATION) break;
        at = GetFileAttributesW(x);
        if (at != INVALID_FILE_ATTRIBUTES && (at & FILE_ATTRIBUTE_READONLY) && !(at & FILE_ATTRIBUTE_DIRECTORY)) SetFileAttributesW(x, at & ~FILE_ATTRIBUTE_READONLY);
    }
    rename_failed(x, y, e); SetLastError(e);
    return 0;
}
int os_delete(const char *p)
{
    WCHAR w[UPD_PATH]; size_t l; DWORD e;
    if (DeleteFileW(wp(p, w)) || (e = GetLastError()) == ERROR_FILE_NOT_FOUND || e == ERROR_PATH_NOT_FOUND) return 1;
    l = wcslen(w);      /* <name>.old still in use (the launcher that ran this update): the next start deletes it; in a protected folder the reboot does */
    if (g_elev && l > 4 && !wcscmp(w + l - 4, L".old")) MoveFileExW(w, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
    return 0;
}
void os_list(const char *dir, void (*f)(const char *name, void *ctx), void *ctx)
{
    WCHAR w[UPD_PATH]; char nm[800]; WIN32_FIND_DATAW fd; HANDLE h; size_t l;
    l = wcslen(wp(dir, w));
    if (!l || l + 3 > UPD_PATH) return;
    wcscpy(w + l, L"\\*");
    if ((h = FindFirstFileW(w, &fd)) == INVALID_HANDLE_VALUE) return;
    do { if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && WideCharToMultiByte(CP_UTF8, 0, fd.cFileName, -1, nm, sizeof nm, NULL, NULL)) f(nm, ctx); } while (FindNextFileW(h, &fd));
    FindClose(h);
}
void os_log(const char *fmt, ...)      /* <data>\update.log, UTF-8 */
{
    char u[3072], p[UPD_PATH]; WCHAR w[UPD_PATH]; va_list a; SYSTEMTIME t; HANDLE h; DWORD put; int n;
    GetLocalTime(&t);
    n = _snprintf(u, 32, "%04u-%02u-%02u %02u:%02u:%02u ", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
    va_start(a, fmt); _vsnprintf(u + n, sizeof u - n - 3, fmt, a); va_end(a); u[sizeof u - 3] = 0; strcat(u, "\r\n");
    upd_path(p, g_data8, "update.log");
    h = CreateFileW(wp(p, w), FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL);
    if (h != INVALID_HANDLE_VALUE) { WriteFile(h, u, (DWORD)strlen(u), &put, NULL); CloseHandle(h); }
}

/* ---- HTTPS GET (WinHTTP) ---- */
static int http_get(const WCHAR *url, char **out, size_t *len, int timeout)
{
    URL_COMPONENTS u; WCHAR host[256], path[2048], extra[1024], hdr[32]; HINTERNET s = NULL, c = NULL, r = NULL;
    DWORD st = 0, sz = sizeof st, hl = sizeof hdr, got; size_t cap = 1 << 16, n = 0; char *b = NULL; int ok = 0;
    memset(&u, 0, sizeof u); u.dwStructSize = sizeof u;
    u.lpszHostName = host; u.dwHostNameLength = 256; u.lpszUrlPath = path; u.dwUrlPathLength = 2048; u.lpszExtraInfo = extra; u.dwExtraInfoLength = 1024;
    *out = NULL; g_err[0] = 0;
    if (!WinHttpCrackUrl(url, 0, 0, &u) || u.nScheme != INTERNET_SCHEME_HTTPS) { WFMT(g_err, L"not an https URL"); return 0; }
    wcsncat(path, extra, 2047 - wcslen(path));
    if (!(s = WinHttpOpen(L"MFLng-launch/" WSTR(LAUNCHER_VERSION), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0))) goto net;
    WinHttpSetTimeouts(s, 3000, 3000, 3000, timeout);
    if (!(c = WinHttpConnect(s, host, u.nPort, 0))) goto net;
    if (!(r = WinHttpOpenRequest(c, L"GET", path, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE))) goto net;
    WinHttpAddRequestHeaders(r, L"Accept: application/vnd.github+json\r\n", (DWORD)-1, WINHTTP_ADDREQ_FLAG_ADD);
    if (!WinHttpSendRequest(r, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) || !WinHttpReceiveResponse(r, NULL)) goto net;
    WinHttpQueryHeaders(r, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &st, &sz, WINHTTP_NO_HEADER_INDEX);
    if (st != 200) {
        hdr[0] = 0; WinHttpQueryHeaders(r, WINHTTP_QUERY_CUSTOM, L"X-RateLimit-Remaining", hdr, &hl, WINHTTP_NO_HEADER_INDEX);
        if ((st == 403 || st == 429) && (!wcscmp(hdr, L"0") || st == 429)) WFMT(g_err, L"GitHub rate limit reached (60 checks per hour without login). Try again later.");
        else WFMT(g_err, L"GitHub answered HTTP %lu.", st);
        goto done;
    }
    b = (char *)malloc(cap);
    for (;;) {
        if (!b) goto net;
        if (!WinHttpReadData(r, b + n, (DWORD)(cap - n - 1), &got)) goto net;
        if (!got) break;
        n += got;
        if (cap - n < 4096) { if (cap > (64u << 20)) { WFMT(g_err, L"download too large"); goto done; } cap *= 2; b = (char *)realloc(b, cap); }
    }
    b[n] = 0; *out = b; *len = n; b = NULL; ok = 1; goto done;
net:
    WFMT(g_err, L"Offline or GitHub not reachable (error %lu).", GetLastError());
done:
    free(b);
    if (r) WinHttpCloseHandle(r);
    if (c) WinHttpCloseHandle(c);
    if (s) WinHttpCloseHandle(s);
    return ok;
}

/* ---- update check (thread) and state ---- */
static DWORD WINAPI check_thread(LPVOID unused)
{
    char *j = NULL; size_t n = 0; int k = -2;
    (void)unused;
    if (g_testzip[0]) {      /* --test-update-zip: one fake release built from the zip's BUILDINFO.txt, no network */
        unsigned char *z = read_file(g_testzip, &n); ZipEntry e[UPD_MAXF]; int i, m = z ? zip_list(z, n, e, UPD_MAXF) : -1;
        memset(g_rel, 0, sizeof g_rel); k = 0;
        for (i = 0; i < m; i++) if (!strcmp(e[i].name, "mflng/BUILDINFO.txt")) {
            char *t = (char *)zip_get(&e[i]); Installed in;
            if (t) { t[e[i].usize] = 0; parse_buildinfo(t, &in); free(t);
                     strcpy(g_rel[0].tag, in.tag); g_rel[0].pre = in.pre; strcpy(g_rel[0].url, "test"); g_rel[0].body = _strdup("Test update from a local zip (--test-update-zip)."); k = 1; }
        }
        free(z);
        if (!k) WFMT(g_err, L"test zip has no mflng/BUILDINFO.txt release line");
    } else if (http_get(WSTR(REPO_API), &j, &n, 5000)) {
        free_releases(g_rel, g_nrel); g_nrel = 0;
        k = parse_releases(j, g_rel, 30);
        if (k < 0) WFMT(g_err, L"Unexpected answer from GitHub.");
        free(j);
    }
    PostMessageW(g_wnd, WM_CHECKED, (WPARAM)k, 0);
    return 0;
}
/* the status line at the bottom edge: SK_INFO / SK_ERR click = changelog or a new check, SK_UPD = accent colour, SK_BUSY = not clickable */
static void status_v(int kind, const WCHAR *fmt, va_list a) { _vsnwprintf(g_stat, 199, fmt, a); g_stat[199] = 0; g_stat_kind = kind; if (g_wnd) InvalidateRect(g_wnd, NULL, FALSE); }
static void status_k(int kind, const WCHAR *fmt, ...) { va_list a; va_start(a, fmt); status_v(kind, fmt, a); va_end(a); }
static void status(const WCHAR *fmt, ...) { va_list a; va_start(a, fmt); status_v(SK_ERR, fmt, a); va_end(a); }
static void installed_name(WCHAR *o, size_t cap)
{
    if (g_inst.tag[0]) _snwprintf(o, cap - 1, L"%hs", g_inst.tag); else wcsncpy(o, L"unknown build", cap - 1);
    o[cap - 1] = 0;
}
static void show_state(void)
{
    WCHAR inst[80]; int st;
    installed_name(inst, 80);
    g_new = pick_newest(g_rel, g_nrel, g_pre); g_upd = 0;
    free(g_body); g_body = NULL;
    if (!g_new) { status(L"No %s found (installed: %s). Click to check again.", g_pre ? L"pre-release" : L"release", inst); return; }
    g_body = wide(g_new->body ? g_new->body : "");
    st = update_state(&g_inst, g_new->tag);
    if (st == ST_UPTODATE) status_k(SK_INFO, L"Up to date: %hs. Click for the release notes.", g_new->tag);
    else if (st == ST_NEWER_INSTALLED) status_k(SK_INFO, L"Installed %s is newer than %hs.", inst, g_new->tag);
    else { g_upd = 1; status_k(SK_UPD, L"Update available: %hs - click to update", g_new->tag); }
}
static void start_check(void)
{
    HANDLE t;
    if (g_busy) return;
    g_busy = 1; status_k(SK_BUSY, L"Checking GitHub for updates...");
    if ((t = CreateThread(NULL, 0, check_thread, NULL, 0, NULL))) CloseHandle(t); else g_busy = 0;
}

/* ---- download + verify + stage (thread) ---- */
static DWORD WINAPI stage_thread(LPVOID unused)
{
    unsigned char *z = NULL, h[32]; char hx[65], st[UPD_PATH], err[300]; size_t n = 0; int ok = 0; WCHAR *url = NULL;
    (void)unused; g_err[0] = 0;
    if (g_testzip[0]) z = read_file(g_testzip, &n);
    else if ((url = wide(g_new->url)) && http_get(url, (char **)&z, &n, 30000)) {
        sha256(z, n, h); hex32(h, hx);
        if (g_new->digest[0] && strcmp(hx, g_new->digest)) { WFMT(g_err, L"The download does not match GitHub's SHA-256 of the asset."); goto out; }
    }
    if (!z) { if (!g_err[0]) WFMT(g_err, L"Download failed."); goto out; }
    CreateDirectoryW(g_data, NULL); upd_path(st, g_data8, "update");
    if (!(ok = upd_stage(z, n, st, g_new->tag, err, sizeof err))) WFMT(g_err, L"%hs", err);      /* every check of the zip: logic.c */
out:
    free(z); free(url);
    PostMessageW(g_wnd, WM_STAGED, (WPARAM)ok, 0);
    return 0;
}

/* staged tree ready: start the apply step and exit.  The apply step is a COPY of this exe in the data folder (APPLY_EXE), so that
   no process runs from MFLng-launch.exe while it is replaced: renaming a running image is not something every volume, virus scanner
   or leftover <name>.old allows (a user's update stopped there with error 5).  If the copy cannot be made or started, and when the
   folder needs administrator rights, the installed exe itself is the apply step: elevated, it must be the one in the protected
   folder, which a process without those rights cannot have swapped; if the rename fails there, the new launcher waits as
   <name>.new.exe for the next start (finish_swap). */
#define APPLY_EXE L"mflng-apply.exe"
static void start_apply(void)
{
    WCHAR t[UPD_PATH], args[2 * MAX_PATH + 96], hp[MAX_PATH]; char st[UPD_PATH], p[UPD_PATH], names[UPD_MAXF][128];
    HANDLE h; DWORD err = 0; int i, n, elev = 0;
    upd_path(st, g_data8, "update");
    pj(t, g_dir, L"mflng-write-test.tmp");
    h = CreateFileW(t, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, NULL);
    if (h == INVALID_HANDLE_VALUE) err = GetLastError(); else CloseHandle(h);
    if (err == ERROR_ACCESS_DENIED || g_force_elev) elev = 1;
    else if (err) { status(L"Cannot write to the install folder (error %lu). Nothing was changed.", err); upd_clear(st); return; }
    n = upd_staged(st, names, UPD_MAXF);
    for (i = 0; i < n && i < UPD_MAXF; i++) {
        DWORD a; upd_path(p, g_dir8, names[i]); a = GetFileAttributesW(wp(p, t));
        if (a != INVALID_FILE_ATTRIBUTES && (a & FILE_ATTRIBUTE_READONLY)) { status(L"%s is read-only. Nothing was changed.", t); upd_clear(st); return; }
    }
    if (elev && MessageBoxW(g_wnd, L"The install folder needs administrator rights to update. Continue with administrator rights?", L"MFLng update", MB_YESNO | MB_ICONQUESTION) != IDYES) {
        status(L"Update cancelled. Nothing was changed."); upd_clear(st); return;
    }
    if (elev) WFMT(args, L"--apply-update %lu \"%s\" --elevated", GetCurrentProcessId(), g_data);
    else WFMT(args, L"--apply-update %lu \"%s\" --for \"%s\"", GetCurrentProcessId(), g_data, g_exe);
    if (elev) {
        SHELLEXECUTEINFOW se; memset(&se, 0, sizeof se); se.cbSize = sizeof se; se.fMask = SEE_MASK_NOASYNC; se.hwnd = g_wnd;
        se.lpVerb = L"runas"; se.lpFile = g_exe; se.lpParameters = args; se.lpDirectory = g_dir; se.nShow = SW_SHOWNORMAL;
        if (!ShellExecuteExW(&se)) {
            err = GetLastError(); upd_clear(st);
            if (err == ERROR_CANCELLED) status(L"Administrator rights were not granted. Nothing was changed.");
            else status(L"Cannot start the update step (error %lu). Nothing was changed.", err);
            return;
        }
    } else {
        STARTUPINFOW si; PROCESS_INFORMATION pi; WCHAR cmd[3 * MAX_PATH + 128]; const WCHAR *run;
        pj(hp, g_data, APPLY_EXE);
        for (i = g_inplace || !CopyFileW(g_exe, hp, FALSE);; i++) {      /* the copy first; this exe itself if that does not start */
            run = i ? g_exe : hp;
            memset(&si, 0, sizeof si); si.cb = sizeof si; WFMT(cmd, L"\"%s\" %s", run, args);
            if (CreateProcessW(run, cmd, NULL, NULL, FALSE, 0, NULL, g_dir, &si, &pi)) break;
            if (i) { status(L"Cannot start the update step (error %lu).", GetLastError()); upd_clear(st); return; }
        }
        CloseHandle(pi.hThread); CloseHandle(pi.hProcess);
    }
    DestroyWindow(g_wnd);      /* the apply step waits for this process to end */
}

/* ---- apply step: <launcher exe> --apply-update <pid> <data dir> [--elevated] ----
   The work is upd_apply (logic.c): it touches only the game folder (release file names and their .new / .old), <data>\update,
   <data>\backup, <data>\backup.prev and <data>\update*.log.  Each staged file is read into memory, checked against the staged
   SHA256SUMS, and written from that same buffer.  Here: what needs Windows before and after it. */
static int reparse(const WCHAR *p) { DWORD a = GetFileAttributesW(p); return a != INVALID_FILE_ATTRIBUTES && (a & FILE_ATTRIBUTE_REPARSE_POINT); }
static int apply_update(DWORD pid, int elevated)
{
    static const WCHAR *const tree[] = { L"", L"\\update", L"\\backup", L"\\backup.prev" }, *const sub[] = { L"", L"\\mflng", L"\\mflng\\LICENSES", L"\\LICENSES" };
    static const WCHAR *const game[] = { L"MFLng.exe", L"MFLng.nofix.exe" };
    const WCHAR *why = L"The update failed and nothing was changed (the previous version stays).";
    WCHAR p[UPD_PATH], q[MAX_PATH + 4], lg[MAX_PATH], lg1[MAX_PATH], m[2 * MAX_PATH + 1400], *self = wcsrchr(g_exe, L'\\') + 1;
    char self8[UPD_PATH], p8[UPD_PATH], st[UPD_PATH]; int r = 0, i, k; HANDLE pr, mx, h; DWORD w;
    /* a launcher started meanwhile leaves the staging dir alone while this mutex exists; a second apply step does not get it and goes */
    mx = CreateMutexW(NULL, FALSE, L"MFLngLauncherUpdate");
    if (mx && WaitForSingleObject(mx, 0) == WAIT_TIMEOUT) {
        CloseHandle(mx);
        MessageBoxW(NULL, L"Another update step is still running. Its message window may be open behind other windows: close it, then update again.", L"MFLng update", BOX_FLAGS | MB_ICONWARNING);
        return 1;
    }
    g_elev = elevated; u8(self, self8); upd_path(st, g_data8, "update");
    pj(lg, g_data, L"update.log"); pj(lg1, g_data, L"update.prev.log");
    MoveFileExW(lg, lg1, MOVEFILE_REPLACE_EXISTING);      /* keep the previous log only */
    u8(g_self, p8);
    os_log("apply %s (%s, this step runs from %s): pid %lu, install dir %s, data dir %s, elevated %d", LAUNCHER_VERSION, self8, p8, (unsigned long)pid, g_dir8, g_data8, elevated);
    if ((pr = OpenProcess(SYNCHRONIZE, FALSE, pid))) {
        w = WaitForSingleObject(pr, 15000); CloseHandle(pr);
        if (w == WAIT_TIMEOUT) { os_log("FAIL: the launcher (pid %lu) did not exit within 15 s", (unsigned long)pid); goto end; }
    }
    /* with administrator rights: follow no link out of the data folder (the folder is the user's; a junction there would aim the
       deletes and the backup copies at another folder).  A check before the work, not a lock: it stops a link that lies there, not a race. */
    for (i = 0; elevated && i < 4; i++) for (k = 0; k < 4; k++) {
        WFMT(p, L"%s%s%s", g_data, tree[i], sub[k]);
        if (reparse(p)) { u8(p, p8); os_log("FAIL: %s is a link, not a folder", p8); goto end; }
    }
    /* a running game keeps its exe open: no update under it (it would go on with old code and load new DLLs) */
    for (i = 0; i < 2; i++) {
        pj(p, g_dir, game[i]);
        h = CreateFileW(p, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, NULL);
        if (h != INVALID_HANDLE_VALUE) CloseHandle(h);
        else if ((w = GetLastError()) == ERROR_SHARING_VIOLATION || w == ERROR_USER_MAPPED_FILE) {
            os_log("FAIL: %s is running; close the game and update again", i ? "MFLng.nofix.exe" : "MFLng.exe");
            why = L"The game is running. Close it and update again. Nothing was changed."; goto end;
        }
    }
    r = upd_apply(g_dir8, g_data8, self8);
    if (r < 0) why = L"The update failed and not every file could be put back. The files of the previous version are in the backup folder (see the log).";
end:
    if (r < 1) {
        upd_clear(st);
        WFMT(m, L"%s\n\n%s%sDetails: %s", why, g_fail, g_fail[0] ? L"\n\n" : L"", lg); MessageBoxW(NULL, m, L"MFLng update", BOX_FLAGS | MB_ICONERROR);
    }
    if (mx) { ReleaseMutex(mx); CloseHandle(mx); }
    /* g_exe = the installed launcher: now the new or the restored one, or (r 2) still the old one, whose start swaps the waiting
       <name>.new.exe in (wWinMain) */
    if (elevated) ShellExecuteW(NULL, L"open", L"explorer.exe", g_exe, NULL, SW_SHOWNORMAL);      /* explorer starts it unelevated (the user's shell) */
    else { STARTUPINFOW si; PROCESS_INFORMATION pi; WFMT(q, L"\"%s\"", g_exe); memset(&si, 0, sizeof si); si.cb = sizeof si;
           if (CreateProcessW(g_exe, q, NULL, NULL, FALSE, 0, NULL, g_dir, &si, &pi)) { CloseHandle(pi.hThread); CloseHandle(pi.hProcess); } }
    return r < 1;
}

/* ---- <name>.new.exe --finish-swap <pid> <launcher exe> <data dir>: an update could not replace the launcher exe while it ran and
   left the new one beside it (upd_apply answer 2).  The launcher finds it at its start, starts it this way and exits; this (the new
   exe, running from its own file) waits for that, takes the launcher's place and starts it.  The launcher then deletes the twin. */
static int finish_swap(DWORD pid, const WCHAR *target)
{
    WCHAR o[MAX_PATH + 8], m[2 * MAX_PATH + 1400], q[MAX_PATH + 16]; char t8[UPD_PATH], o8[UPD_PATH], s8[UPD_PATH]; HANDLE pr; int ok;
    STARTUPINFOW si; PROCESS_INFORMATION pi;
    if ((pr = OpenProcess(SYNCHRONIZE, FALSE, pid))) { WaitForSingleObject(pr, 15000); CloseHandle(pr); }
    WFMT(o, L"%s.old", target); u8(target, t8); u8(o, o8); u8(g_self, s8);
    if (os_rename(t8, o8)) { if (!(ok = os_copy(s8, t8))) os_rename(o8, t8); }      /* the .old goes at the launcher's start (upd_clean) */
    else ok = os_copy(s8, t8);
    os_log(ok ? "launcher replaced by %s" : "FAIL: cannot replace the launcher by %s (error %ld)", s8, os_err());
    if (!ok) {
        WFMT(m, L"The new launcher could not take the place of\n%s\n\n%s%sThe update itself is installed. Close every launcher window and copy\n%s\nover it by hand.",
             target, g_fail, g_fail[0] ? L"\n\n" : L"", g_self);
        MessageBoxW(NULL, m, L"MFLng update", BOX_FLAGS | MB_ICONWARNING);
    }
    WFMT(q, L"\"%s\"%s", target, ok ? L"" : L" --no-swap"); memset(&si, 0, sizeof si); si.cb = sizeof si;
    if (CreateProcessW(target, q, NULL, NULL, FALSE, 0, NULL, g_dir, &si, &pi)) { CloseHandle(pi.hThread); CloseHandle(pi.hProcess); }
    return !ok;
}

/* ---- picture: the 24/32-bit BMP inside the user's launch.exe (VB form data), else TOASTART\PC\Setup.bmp ---- */
static unsigned rd32(const unsigned char *p) { return p[0] | p[1] << 8 | p[2] << 16 | (unsigned)p[3] << 24; }
static int find_bmp(const unsigned char *b, size_t n)
{
    size_t i;
    for (i = 0; i + 54 <= n; i++) {
        unsigned sz, off, w, hh, bpp;
        if (b[i] != 'B' || b[i + 1] != 'M' || rd32(b + i + 14) != 40) continue;
        sz = rd32(b + i + 2); off = rd32(b + i + 10); w = rd32(b + i + 18); hh = rd32(b + i + 22); bpp = b[i + 28] | b[i + 29] << 8;
        if (w < 320 || w > 2048 || hh < 200 || hh > 2048 || (bpp != 24 && bpp != 32) || rd32(b + i + 30) != 0) continue;
        if (sz > n - i || off < 54 || off > sz || ((w * bpp / 8 + 3) & ~3u) * hh > sz - off) continue;
        g_bi = (const BITMAPINFO *)(b + i + 14); g_bits = b + i + off;
        return 1;
    }
    return 0;
}
static void load_picture(void)
{
    static const WCHAR *src[] = { L"launch.exe", L"TOASTART\\PC\\Setup.bmp" };
    WCHAR p[MAX_PATH]; size_t n; int i;
    for (i = 0; i < 2 && !g_bi; i++) {
        pj(p, g_dir, src[i]);
        if ((g_pic = read_file(p, &n)) && !find_bmp(g_pic, n)) { free(g_pic); g_pic = NULL; }
    }
}

/* ---- window: borderless like the original launch.exe, everything drawn over the picture ----
   The original (VB6 form in launch.exe) puts its menu as transparent labels in a right-aligned column over the sky: Arial Bold 8.25 pt,
   black, right edge x = 622, rows y = 80 (Play) ... 250 (Exit), 34 px apart (twips / 15 from the form data).  The extras sit as small
   shadowed text lines on the dark bottom edge; changelog and About open in a translucent overlay. */
enum { K_COL, K_LINE, K_BTN, K_OV };
typedef struct { int id, kind; RECT r; WCHAR text[200]; } Item;
static Item g_it[24]; static int g_nit;
static int g_dpi = 96, g_hover, g_down, g_focus = ID_PLAY, g_kbd, g_ov, g_ovscroll, g_ovh;
static HFONT f_col, f_line, f_body, f_title, f_big;
static int S(int v) { return MulDiv(v, g_dpi, 96); }
static int have(const WCHAR *name) { WCHAR p[MAX_PATH]; pj(p, g_dir, name); return exists(p); }

static void play(const WCHAR *name)
{
    WCHAR p[MAX_PATH], cmd[MAX_PATH + 4]; STARTUPINFOW si; PROCESS_INFORMATION pi;
    pj(p, g_dir, name); WFMT(cmd, L"\"%s\"", p); memset(&si, 0, sizeof si); si.cb = sizeof si;
    if (CreateProcessW(p, cmd, NULL, NULL, FALSE, 0, NULL, g_dir, &si, &pi)) { CloseHandle(pi.hThread); CloseHandle(pi.hProcess); DestroyWindow(g_wnd); }
    else status(L"Cannot start %s (error %lu).", name, GetLastError());
}
static void save_settings(void)
{
    WritePrivateProfileStringW(L"launcher", L"update_at_startup", g_auto ? L"on" : L"off", g_ini);
    WritePrivateProfileStringW(L"launcher", L"channel", g_pre ? L"prerelease" : L"release", g_ini);
}

/* item at (x, y) of picture coordinates (right-aligned when x < 0: right edge = -x); width from the text */
static void add(HDC dc, int id, int kind, int x, int y, const WCHAR *text)
{
    Item *it = &g_it[g_nit++]; SIZE sz;
    it->id = id; it->kind = kind; wcsncpy(it->text, text, 199); it->text[199] = 0;
    SelectObject(dc, kind == K_COL ? f_col : f_line);
    GetTextExtentPoint32W(dc, text, (int)wcslen(text), &sz);
    if (kind == K_BTN) sz.cx = S(18), sz.cy = S(16);
    it->r.left = x < 0 ? S(-x) - sz.cx : S(x); it->r.top = S(y); it->r.right = it->r.left + sz.cx; it->r.bottom = it->r.top + sz.cy;
}
static int line_x(void) { return g_nit ? MulDiv(g_it[g_nit - 1].r.right, 96, g_dpi) + 18 : 10; }
static void build_items(HDC dc)
{
    WCHAR b[200]; int y;
    g_nit = 0;
    add(dc, ID_MIN, K_BTN, 598, 4, L"");
    add(dc, ID_CLOSE, K_BTN, 618, 4, L"");
    if (g_ov) {      /* overlay links, bottom right of the panel */
        add(dc, ID_OV_CLOSE, K_OV, -584, 384, L"Close");
        if (g_ov == 1 && g_upd && !g_busy) add(dc, ID_OV_UPDATE, K_OV, -(MulDiv(g_it[g_nit - 1].r.left, 96, g_dpi) - 24), 384, L"Update now");
        return;
    }
    y = 80;      /* the original's menu rows: 34 px apart from y 80 */
    if (have(L"MFLng.exe")) { add(dc, ID_PLAY, K_COL, -622, y, L"Play"); y += 34; }
    if (have(L"MFLng.nofix.exe")) { add(dc, ID_NOFIX, K_COL, -622, y, L"Play original behaviour"); y += 34; }
    add(dc, ID_EXIT, K_COL, -622, y, L"Exit");
    if (!g_busy) add(dc, ID_STATUS, K_LINE, 10, 424, g_stat);
    add(dc, ID_AUTO, K_LINE, 10, 440, g_auto ? L"[x] Check for updates at start-up" : L"[  ] Check for updates at start-up");
    add(dc, ID_CHAN, K_LINE, line_x(), 440, g_pre ? L"Channel: Pre-releases" : L"Channel: Releases");
    if (!g_busy) add(dc, ID_CHECK, K_LINE, line_x(), 440, L"Check now");
    WFMT(b, L"%s - github.com/mashed-reverse-engineering/MFL-recomp", CREDIT_SHORT);
    add(dc, ID_WEB, K_LINE, 10, 456, b);
    add(dc, ID_ABOUT, K_LINE, line_x(), 456, L"About");
}
static int hit(int x, int y)
{
    int i; POINT pt; pt.x = x; pt.y = y;
    for (i = 0; i < g_nit; i++) { RECT r = g_it[i].r; InflateRect(&r, S(3), S(2)); if (PtInRect(&r, pt)) return g_it[i].id; }
    return 0;
}
static void text_at(HDC dc, const RECT *r, const WCHAR *s, COLORREF c, int shadow, UINT fl)
{
    RECT t = *r;
    if (shadow) { OffsetRect(&t, 1, 1); SetTextColor(dc, RGB(0, 0, 0)); DrawTextW(dc, s, -1, &t, fl); t = *r; }
    SetTextColor(dc, c); DrawTextW(dc, s, -1, &t, fl);
}
static void darken(unsigned char *bits, int w, RECT r)
{
    int x, y;
    for (y = r.top; y < r.bottom; y++) for (x = r.left; x < r.right; x++) {
        unsigned char *p = bits + ((size_t)y * w + x) * 4; p[0] = (unsigned char)(p[0] / 5 + 8); p[1] = (unsigned char)(p[1] / 5 + 8); p[2] = (unsigned char)(p[2] / 5 + 10);
    }
}
static void ov_text(WCHAR *o, size_t cap, WCHAR *title, size_t tcap)
{
    if (g_ov == 1) {
        _snwprintf(title, tcap - 1, L"%hs%s", g_new ? g_new->tag : "", g_new && g_new->pre ? L" (pre-release)" : L"");
        _snwprintf(o, cap - 1, L"%s", g_body ? g_body : L"(no release notes)");
    } else {
        _snwprintf(title, tcap - 1, L"MFLng-launch " WSTR(LAUNCHER_VERSION));
        _snwprintf(o, cap - 1, L"%s.\n%s\n\nLicences: see the folder mflng\\LICENSES next to this program.\n\n%s\n\n" WSTR(REPO_URL), CREDIT_SHORT, CREDIT_THANKS, NOTICE);
    }
    o[cap - 1] = 0; title[tcap - 1] = 0;
}
static void paint(HDC dc)
{
    int W = S(PIC_W), H = S(PIC_H), i; HDC m = CreateCompatibleDC(dc); BITMAPINFO bi; void *bits; HBITMAP hb, ob; RECT r;
    memset(&bi, 0, sizeof bi); bi.bmiHeader.biSize = sizeof bi.bmiHeader; bi.bmiHeader.biWidth = W; bi.bmiHeader.biHeight = -H;
    bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32; bi.bmiHeader.biCompression = BI_RGB;
    hb = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, NULL, 0); ob = (HBITMAP)SelectObject(m, hb);
    SetBkMode(m, TRANSPARENT);
    if (g_bi) { SetStretchBltMode(m, HALFTONE); SetBrushOrgEx(m, 0, 0, NULL); StretchDIBits(m, 0, 0, W, H, 0, 0, g_bi->bmiHeader.biWidth, g_bi->bmiHeader.biHeight, g_bits, g_bi, DIB_RGB_COLORS, SRCCOPY); }
    else {      /* no launch.exe / Setup.bmp: the same layout on a dark background */
        HBRUSH br = CreateSolidBrush(RGB(18, 18, 22)); RECT a = {0, 0, 0, 0}; a.right = W; a.bottom = H; FillRect(m, &a, br); DeleteObject(br);
        a.left = S(24); a.top = S(18); SelectObject(m, f_big); text_at(m, &a, L"MASHED", RGB(225, 225, 225), 0, DT_NOCLIP);
        a.left = S(30); a.top = S(84); SelectObject(m, f_title); text_at(m, &a, L"FULLY LOADED", RGB(230, 150, 50), 0, DT_NOCLIP);
    }
    build_items(m);
    if (g_ov) {
        WCHAR body[8192], title[160]; RECT pr, br2;
        pr.left = S(40); pr.top = S(60); pr.right = S(600); pr.bottom = S(410);
        GdiFlush(); darken((unsigned char *)bits, W, pr);
        { HBRUSH fr = CreateSolidBrush(RGB(110, 110, 110)); FrameRect(m, &pr, fr); DeleteObject(fr); }
        ov_text(body, 8192, title, 160);
        r = pr; InflateRect(&r, -S(16), -S(12)); SelectObject(m, f_title); text_at(m, &r, title, RGB(240, 240, 240), 0, DT_SINGLELINE | DT_NOPREFIX);
        br2 = pr; InflateRect(&br2, -S(16), 0); br2.top = pr.top + S(40); br2.bottom = pr.bottom - S(34);
        SelectObject(m, f_body);
        r = br2; DrawTextW(m, body, -1, &r, DT_CALCRECT | DT_WORDBREAK | DT_NOPREFIX); g_ovh = r.bottom - r.top;
        if (g_ovscroll > g_ovh - (br2.bottom - br2.top)) g_ovscroll = g_ovh - (br2.bottom - br2.top);
        if (g_ovscroll < 0) g_ovscroll = 0;
        SaveDC(m); IntersectClipRect(m, br2.left, br2.top, br2.right, br2.bottom);
        r = br2; r.top -= g_ovscroll; r.bottom = r.top + g_ovh; text_at(m, &r, body, RGB(215, 215, 215), 0, DT_WORDBREAK | DT_NOPREFIX);
        RestoreDC(m, -1);
        if (g_ovh > br2.bottom - br2.top) { r = pr; r.left += S(16); r.top = pr.bottom - S(26); SelectObject(m, f_line);
            text_at(m, &r, L"Mouse wheel / arrow keys: scroll", RGB(140, 140, 140), 0, DT_SINGLELINE | DT_NOPREFIX); }
    }
    for (i = 0; i < g_nit; i++) {
        Item *it = &g_it[i]; int hov = it->id == g_hover, foc = g_kbd && it->id == g_focus; COLORREF c;
        if (it->kind == K_BTN) {      /* flat minimize / close glyphs over the checkered corner */
            HPEN pen = CreatePen(PS_SOLID, S(1) < 1 ? 1 : S(1), hov ? RGB(255, 255, 255) : RGB(200, 200, 200)), op; RECT b = it->r; int cx = (b.left + b.right) / 2, cy = (b.top + b.bottom) / 2, d = S(4);
            if (hov || foc) { HBRUSH hbr = CreateSolidBrush(it->id == ID_CLOSE ? RGB(160, 28, 28) : RGB(60, 60, 64)); FillRect(m, &b, hbr); DeleteObject(hbr); }
            op = (HPEN)SelectObject(m, pen);
            if (it->id == ID_MIN) { MoveToEx(m, cx - d, cy + d / 2, NULL); LineTo(m, cx + d + 1, cy + d / 2); }
            else { MoveToEx(m, cx - d, cy - d, NULL); LineTo(m, cx + d + 1, cy + d + 1); MoveToEx(m, cx + d, cy - d, NULL); LineTo(m, cx - d - 1, cy + d + 1); }
            SelectObject(m, op); DeleteObject(pen);
            continue;
        }
        SelectObject(m, it->kind == K_COL ? f_col : f_line);
        if (it->kind == K_COL) c = !g_bi ? (hov ? RGB(255, 255, 255) : RGB(190, 190, 190)) : hov ? RGB(170, 22, 22) : RGB(0, 0, 0);      /* original: black over the sky */
        else if (it->id == ID_STATUS && g_stat_kind == SK_UPD) c = hov ? RGB(255, 210, 120) : RGB(255, 170, 60);
        else if (it->id == ID_WEB || it->id == ID_ABOUT) c = hov ? RGB(235, 235, 235) : RGB(150, 150, 150);
        else c = hov ? RGB(255, 255, 255) : RGB(205, 205, 205);
        text_at(m, &it->r, it->text, c, it->kind != K_COL, DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP);
        if (hov || foc) { HPEN pen = CreatePen(PS_SOLID, 1, c), op = (HPEN)SelectObject(m, pen); MoveToEx(m, it->r.left, it->r.bottom, NULL); LineTo(m, it->r.right, it->r.bottom); SelectObject(m, op); DeleteObject(pen); }
    }
    if (g_busy) { r.left = S(10); r.top = S(424); r.right = S(630); r.bottom = S(440); SelectObject(m, f_line); text_at(m, &r, g_stat, RGB(205, 205, 205), 1, DT_SINGLELINE | DT_NOPREFIX); }
    BitBlt(dc, 0, 0, W, H, m, 0, 0, SRCCOPY);
    SelectObject(m, ob); DeleteObject(hb); DeleteDC(m);
}
static void ov_open(int which) { g_ov = which; g_ovscroll = 0; g_focus = which == 1 && g_upd && !g_busy ? ID_OV_UPDATE : ID_OV_CLOSE; InvalidateRect(g_wnd, NULL, FALSE); }
static void ov_close(void) { g_ov = 0; g_focus = ID_PLAY; InvalidateRect(g_wnd, NULL, FALSE); }
static void start_update(void)
{
    HANDLE t;
    if (g_busy || !g_new) return;
    g_busy = 1; ov_close(); status_k(SK_BUSY, L"Downloading and checking %hs...", g_new->tag);
    if ((t = CreateThread(NULL, 0, stage_thread, NULL, 0, NULL))) CloseHandle(t); else g_busy = 0;
}
static void activate(int id)
{
    switch (id) {
    case ID_PLAY: play(L"MFLng.exe"); break;
    case ID_NOFIX: play(L"MFLng.nofix.exe"); break;
    case ID_EXIT: case ID_CLOSE: DestroyWindow(g_wnd); break;
    case ID_MIN: ShowWindow(g_wnd, SW_MINIMIZE); break;
    case ID_STATUS: if (g_new) ov_open(1); else start_check(); break;
    case ID_AUTO: g_auto = !g_auto; save_settings(); break;
    case ID_CHAN: g_pre = !g_pre; save_settings(); if (g_nrel) show_state(); break;
    case ID_CHECK: start_check(); break;
    case ID_WEB: ShellExecuteW(g_wnd, L"open", WSTR(REPO_URL), NULL, NULL, SW_SHOWNORMAL); break;
    case ID_ABOUT: ov_open(2); break;
    case ID_OV_UPDATE: start_update(); break;
    case ID_OV_CLOSE: ov_close(); break;
    }
    InvalidateRect(g_wnd, NULL, FALSE);
}
static void set_hover(int id) { if (id != g_hover) { g_hover = id; InvalidateRect(g_wnd, NULL, FALSE); } }
/* one launcher per install folder: a Local\ mutex and a registered window message, both named by the folder (inst_key).  A second start
   tells the first to come forward (restore, foreground) and exits; the apply step and the finish-swap step never come here. */
enum { SHOW_OK = 0x4D464C };
static UINT g_show; static WCHAR g_mxname[64];
static int first_instance(void)      /* 1 = this one runs; 0 = the launcher of this install was told and this one goes */
{
    char k[17]; WCHAR kw[17], n[64]; HANDLE mx; HWND w; DWORD_PTR r; int i;
    inst_key(k, g_dir8); for (i = 0; i < 17; i++) kw[i] = (WCHAR)k[i];
    WFMT(g_mxname, L"Local\\MFLngLauncher-%s", kw); WFMT(n, L"MFLngLauncherShow-%s", kw); g_show = RegisterWindowMessageW(n);
    for (i = 0; i < 30; i++) {      /* up to 3 s: the one before may still be on its way out (it releases the mutex when it ends) */
        mx = CreateMutexW(NULL, FALSE, g_mxname);      /* NULL + access denied: made by an elevated launcher, it exists all the same */
        if (mx && ((r = WaitForSingleObject(mx, 0)) == WAIT_OBJECT_0 || r == WAIT_ABANDONED)) return 1;      /* kept until the process ends */
        if (mx) CloseHandle(mx);
        AllowSetForegroundWindow(ASFW_ANY);      /* this start has the user's input: let the first one take the foreground */
        for (w = NULL; (w = FindWindowExW(NULL, w, L"MFLngLauncher", NULL)) != NULL;)      /* other installs ignore this message */
            if (SendMessageTimeoutW(w, g_show, 0, 0, SMTO_ABORTIFHUNG, 1000, &r) && r == SHOW_OK) return 0;
        Sleep(100);      /* window not made yet (or hung / unreachable through UIPI): try again, then run anyway */
    }
    return 1;
}
static LRESULT CALLBACK wndproc(HWND w, UINT m, WPARAM wp, LPARAM lp)
{
    if (m == g_show && m) { ShowWindow(w, IsIconic(w) ? SW_RESTORE : SW_SHOW); SetForegroundWindow(w); os_log("second instance: brought to foreground"); return SHOW_OK; }
    switch (m) {
    case WM_CREATE:
        g_wnd = w;
        { BOOL (WINAPI *f)(HWND, UINT, DWORD, void *) = (BOOL (WINAPI *)(HWND, UINT, DWORD, void *))GetProcAddress(GetModuleHandleW(L"user32.dll"), "ChangeWindowMessageFilterEx");
          if (f) f(w, g_show, 1 /* MSGFLT_ALLOW */, NULL); }      /* an elevated launcher: the unelevated second start may message it (UIPI) */
        f_col = CreateFontW(-S(11), 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, ANTIALIASED_QUALITY, 0, L"Arial");
        f_line = CreateFontW(-S(11), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, ANTIALIASED_QUALITY, 0, L"Arial");
        f_body = CreateFontW(-S(13), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, ANTIALIASED_QUALITY, 0, L"Arial");
        f_title = CreateFontW(-S(15), 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, ANTIALIASED_QUALITY, 0, L"Arial");
        f_big = CreateFontW(-S(64), 0, 0, 0, FW_HEAVY, 0, 0, 0, DEFAULT_CHARSET, 0, 0, ANTIALIASED_QUALITY, 0, L"Arial");
        { WCHAR inst[80]; installed_name(inst, 80); status(L"Installed: %s. Click to check for updates.", inst); }
        if (g_auto || g_testzip[0]) start_check();
        return 0;
    case WM_ERASEBKGND: return 1;
    case WM_PAINT: { PAINTSTRUCT ps; HDC dc = BeginPaint(w, &ps); paint(dc); EndPaint(w, &ps); return 0; }
    case WM_NCHITTEST: {
        POINT pt; pt.x = (short)LOWORD(lp); pt.y = (short)HIWORD(lp); ScreenToClient(w, &pt);
        return g_ov || hit(pt.x, pt.y) ? HTCLIENT : HTCAPTION; }      /* the picture drags the window */
    case WM_SETCURSOR:
        if (LOWORD(lp) == HTCLIENT && g_hover) { SetCursor(LoadCursor(NULL, IDC_HAND)); return TRUE; }
        break;
    case WM_MOUSEMOVE: {
        TRACKMOUSEEVENT t; t.cbSize = sizeof t; t.dwFlags = TME_LEAVE; t.hwndTrack = w; t.dwHoverTime = 0; TrackMouseEvent(&t);
        set_hover(hit((short)LOWORD(lp), (short)HIWORD(lp))); return 0; }
    case WM_MOUSELEAVE: case WM_NCMOUSEMOVE: set_hover(0); break;
    case WM_LBUTTONDOWN: g_down = hit((short)LOWORD(lp), (short)HIWORD(lp)); SetCapture(w); return 0;
    case WM_LBUTTONUP: {
        int id = hit((short)LOWORD(lp), (short)HIWORD(lp)); ReleaseCapture();
        if (id && id == g_down) activate(id); else if (g_ov && !g_down) ov_close();
        g_down = 0; return 0; }
    case WM_MOUSEWHEEL: if (g_ov) { g_ovscroll -= GET_WHEEL_DELTA_WPARAM(wp) / WHEEL_DELTA * S(40); InvalidateRect(w, NULL, FALSE); } return 0;
    case WM_KEYDOWN: {
        int i, n = g_nit, cur = -1;
        if (wp == VK_ESCAPE) { if (g_ov) ov_close(); else DestroyWindow(w); return 0; }
        if (wp == VK_RETURN || wp == VK_SPACE) { for (i = 0; i < n && g_it[i].id != g_focus; i++); activate(i < n ? g_focus : ID_PLAY); return 0; }      /* Enter = Play unless a link has the focus */
        if (g_ov && (wp == VK_UP || wp == VK_DOWN || wp == VK_PRIOR || wp == VK_NEXT)) {
            g_ovscroll += (wp == VK_UP ? -S(20) : wp == VK_DOWN ? S(20) : wp == VK_PRIOR ? -S(200) : S(200)); InvalidateRect(w, NULL, FALSE); return 0; }
        if (wp == VK_TAB && n) {      /* Tab / Shift+Tab through the links in drawing order (window buttons last) */
            int back = GetKeyState(VK_SHIFT) < 0;
            for (i = 0; i < n; i++) if (g_it[i].id == g_focus) cur = i;
            cur = cur < 0 ? 0 : (cur + (back ? n - 1 : 1)) % n;
            g_focus = g_it[cur].id; g_kbd = 1; InvalidateRect(w, NULL, FALSE); return 0; }
        break; }
    case WM_CHECKED:
        g_busy = 0;
        if ((int)wp >= 0) { g_nrel = (int)wp; show_state(); } else status(L"%s Click to retry.", g_err);
        return 0;
    case WM_STAGED:
        g_busy = 0;
        if (wp) { status_k(SK_BUSY, L"Verified. Installing..."); start_apply(); } else status(L"Update failed: %s Nothing was changed.", g_err);
        return 0;
    case WM_DESTROY: PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(w, m, wp, lp);
}

static DWORD WINAPI cleanup_thread(LPVOID unused)      /* leftovers of the last update: the apply step that started us may still be exiting */
{
    char st[UPD_PATH]; HANDLE mx; int i, left; WCHAR hp[MAX_PATH];
    (void)unused; upd_path(st, g_data8, "update"); pj(hp, g_data, APPLY_EXE);
    /* an update is being applied: its .old files are its way back, leave everything (a mutex made with administrator rights may
       refuse to open: it exists all the same).  The apply step lets go of it before it starts us. */
    if ((mx = OpenMutexW(SYNCHRONIZE, FALSE, L"MFLngLauncherUpdate")) || GetLastError() == ERROR_ACCESS_DENIED) { if (mx) CloseHandle(mx); return 0; }
    for (i = 0; i < 30; i++) {      /* <name>.old / .new of release files (a protected dir: the reboot does it), the apply step's copy, the twin */
        left = upd_clean(g_dir8);
        left += !DeleteFileW(hp) && GetLastError() != ERROR_FILE_NOT_FOUND;
        left += g_twin[0] && !DeleteFileW(g_twin) && GetLastError() != ERROR_FILE_NOT_FOUND;
        if (!left) break;
        Sleep(100);
    }
    upd_clear(st);      /* a staging dir left by a crash or a closed launcher */
    return 0;
}

int WINAPI wWinMain(HINSTANCE hi, HINSTANCE prev, LPWSTR cl, int show)
{
    int argc, i, noswap = 0; WCHAR **argv = CommandLineToArgvW(GetCommandLineW(), &argc), p[MAX_PATH], v[32], base[MAX_PATH], *dot; WNDCLASSW wc; MSG msg; HANDLE t;
    char *bi;
    (void)prev; (void)cl;
    GetModuleFileNameW(NULL, g_exe, MAX_PATH); wcscpy(g_self, g_exe);
    if (argc >= 4 && !wcscmp(argv[1], L"--apply-update")) {
        int elev = argc > 4 && !wcscmp(argv[4], L"--elevated");
        /* --for <launcher exe>: this is the copy in the data folder.  Never with administrator rights: there the step works on the
           folder it lies in, and nobody can point it at another one. */
        if (!elev && argc > 5 && !wcscmp(argv[4], L"--for") && wcsrchr(argv[5], L'\\')) { wcsncpy(g_exe, argv[5], MAX_PATH - 1); g_exe[MAX_PATH - 1] = 0; }
        wcscpy(g_dir, g_exe); *wcsrchr(g_dir, L'\\') = 0; u8(g_dir, g_dir8);
        wcsncpy(g_data, argv[3], MAX_PATH - 1); u8(g_data, g_data8);
        return apply_update(wcstoul(argv[2], NULL, 10), elev);
    }
    wcscpy(g_dir, g_exe); *wcsrchr(g_dir, L'\\') = 0; u8(g_dir, g_dir8);
    if (argc >= 5 && !wcscmp(argv[1], L"--finish-swap")) {
        wcsncpy(g_data, argv[4], MAX_PATH - 1); u8(g_data, g_data8);
        return finish_swap(wcstoul(argv[2], NULL, 10), argv[3]);
    }
    for (i = 1; i < argc; i++) {
        if (!wcscmp(argv[i], L"--test-update-zip") && i + 1 < argc) GetFullPathNameW(argv[++i], MAX_PATH, g_testzip, NULL);
        else if (!wcscmp(argv[i], L"--test-force-elevate")) g_force_elev = 1;
        else if (!wcscmp(argv[i], L"--test-apply-in-place")) g_inplace = 1;
        else if (!wcscmp(argv[i], L"--no-swap")) noswap = 1;
    }
    if (!first_instance()) return 0;
    /* data dir: %LOCALAPPDATA%\mflng-launch (the game folder may be protected); portable.txt beside this exe -> <exe dir>\mflng\mflng-launch
       (W17 LAYOUT C: inside the MFLng sub folder, so the game folder gets nothing more) */
    pj(p, g_dir, L"portable.txt");
    if (exists(p) || !GetEnvironmentVariableW(L"LOCALAPPDATA", base, MAX_PATH)) { pj(p, g_dir, L"mflng"); CreateDirectoryW(p, NULL); pj(g_data, p, L"mflng-launch"); }
    else pj(g_data, base, L"mflng-launch");
    CreateDirectoryW(g_data, NULL); u8(g_data, g_data8);
    pj(g_ini, g_data, L"mflng-launch.ini");
    /* <name>.new.exe beside this exe: a launcher an update could not put in place while this one ran.  The same bytes: done, it goes.
       Else it takes this exe's place now (finish_swap), unless that just failed (--no-swap). */
    if ((dot = wcsrchr(g_exe, L'.')) && dot > wcsrchr(g_exe, L'\\')) {
        size_t a = 0, b = 0; unsigned char *x, *y;
        WFMT(g_twin, L"%.*s.new%s", (int)(dot - g_exe), g_exe, dot);
        x = read_file(g_exe, &a); y = read_file(g_twin, &b);
        if (y && !(x && a == b && !memcmp(x, y, a))) {
            STARTUPINFOW si; PROCESS_INFORMATION pi; WCHAR cmd[4 * MAX_PATH + 64];
            g_twin[MAX_PATH - 1] = 0; WFMT(cmd, L"\"%s\" --finish-swap %lu \"%s\" \"%s\"", g_twin, GetCurrentProcessId(), g_exe, g_data); memset(&si, 0, sizeof si); si.cb = sizeof si;
            if (!noswap && CreateProcessW(g_twin, cmd, NULL, NULL, FALSE, 0, NULL, g_dir, &si, &pi)) return 0;
            g_twin[0] = 0;      /* not swapped: it stays for the next start */
        } else if (!y) g_twin[0] = 0;
        free(x); free(y);
    }
    if ((t = CreateThread(NULL, 0, cleanup_thread, NULL, 0, NULL))) CloseHandle(t);
    pj(p, g_dir, L"mflng\\BUILDINFO.txt"); bi = (char *)read_file(p, NULL); parse_buildinfo(bi, &g_inst); free(bi);
    /* an install of the flat layout before mflng\ has it beside the exes: without it the build is "unknown", the channel falls to
       Releases and a pre-release is never offered */
    if (!g_inst.tag[0]) { pj(p, g_dir, L"BUILDINFO.txt"); bi = (char *)read_file(p, NULL); parse_buildinfo(bi, &g_inst); free(bi); }
    GetPrivateProfileStringW(L"launcher", L"update_at_startup", L"on", v, 32, g_ini); g_auto = _wcsicmp(v, L"off") != 0;
    GetPrivateProfileStringW(L"launcher", L"channel", L"", v, 32, g_ini);
    g_pre = !_wcsicmp(v, L"prerelease") ? 1 : !_wcsicmp(v, L"release") ? 0 : default_channel_pre(&g_inst);
    load_picture();
    { HDC sc = GetDC(NULL); g_dpi = GetDeviceCaps(sc, LOGPIXELSX); ReleaseDC(NULL, sc); if (g_dpi < 96) g_dpi = 96; }      /* manifest: system DPI aware */
    memset(&wc, 0, sizeof wc);
    wc.lpfnWndProc = wndproc; wc.hInstance = hi; wc.hCursor = LoadCursor(NULL, IDC_ARROW); wc.lpszClassName = L"MFLngLauncher";
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassW(&wc);
    /* borderless and centred, the picture is the window (WS_MINIMIZEBOX | WS_SYSMENU: taskbar minimize and close) */
    CreateWindowExW(0, L"MFLngLauncher", L"Mashed: Fully Loaded - MFLng-launch " WSTR(LAUNCHER_VERSION), WS_POPUP | WS_MINIMIZEBOX | WS_SYSMENU | WS_VISIBLE,
                    (GetSystemMetrics(SM_CXSCREEN) - S(PIC_W)) / 2, (GetSystemMetrics(SM_CYSCREEN) - S(PIC_H)) / 2, S(PIC_W), S(PIC_H), NULL, NULL, hi, NULL);
    (void)show;
    while (GetMessageW(&msg, NULL, 0, 0) > 0) { TranslateMessage(&msg); DispatchMessageW(&msg); }
    LocalFree(argv);
    return 0;
}
