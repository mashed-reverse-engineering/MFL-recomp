/* launcher.c: MFLng-launch.exe.  Shows the original launcher picture (read at run time from the user's own launch.exe), starts
   MFLng.exe / MFLng.nofix.exe, checks the GitHub releases and updates the MFLng files (and itself) through an apply step (the same exe in --apply-update mode).
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
#define CREDIT L"MFL-recomp by SciLor and contributors. Thanks to L2KSolkov (codfh-win32) and tgsm (pbg)."
#define NOTICE L"You need your own copy of the game. Not affiliated with the game's publisher or developer."
#define WFMT(b, ...) (_snwprintf(b, sizeof(b) / sizeof(WCHAR) - 1, __VA_ARGS__), b[sizeof(b) / sizeof(WCHAR) - 1] = 0)
#define MAXF 48
enum { ID_PLAY = 100, ID_NOFIX, ID_CHECK, ID_UPDATE, ID_EXIT, ID_AUTO, ID_CHAN, ID_WEB, WM_CHECKED = WM_APP, WM_STAGED };
enum { PIC_W = 640, PIC_H = 480, PANEL_H = 206 };

static WCHAR g_exe[MAX_PATH], g_dir[MAX_PATH], g_data[MAX_PATH], g_ini[MAX_PATH], g_testzip[MAX_PATH], g_err[512];
static HWND g_wnd, h_play, h_nofix, h_check, h_update, h_auto, h_chan, h_status, h_log;
static HBRUSH g_dark;
static unsigned char *g_pic; static const BITMAPINFO *g_bi; static const unsigned char *g_bits;
static Installed g_inst;
static int g_auto, g_pre, g_force_elev, g_nrel, g_busy;
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
static void logf_(const WCHAR *fmt, ...)      /* <data>\update.log, UTF-8 */
{
    WCHAR w[1024], p[MAX_PATH]; char u[3072]; va_list a; SYSTEMTIME t; HANDLE h; DWORD put; int n;
    GetLocalTime(&t);
    n = _snwprintf(w, 32, L"%04u-%02u-%02u %02u:%02u:%02u ", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
    va_start(a, fmt); _vsnwprintf(w + n, 1000 - n, fmt, a); va_end(a); w[1020] = 0; wcscat(w, L"\r\n");
    n = WideCharToMultiByte(CP_UTF8, 0, w, -1, u, sizeof u, NULL, NULL) - 1;
    pj(p, g_data, L"update.log");
    h = CreateFileW(p, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL);
    if (h != INVALID_HANDLE_VALUE) { if (n > 0) WriteFile(h, u, (DWORD)n, &put, NULL); CloseHandle(h); }
}
/* delete the files of <dir> and <dir>\LICENSES (one level each); olds: only `<allowed release name>.old` (left by an update), else all */
static int del_files(const WCHAR *dir, int olds)
{
    WCHAR pat[MAX_PATH], p[MAX_PATH], sub[MAX_PATH]; WIN32_FIND_DATAW f; HANDLE h; int pass, left = 0;
    pj(sub, dir, L"LICENSES");
    for (pass = 0; pass < 2; pass++) {
        const WCHAR *d = pass ? dir : sub;
        pj(pat, d, olds ? L"*.old" : L"*");
        if ((h = FindFirstFileW(pat, &f)) == INVALID_HANDLE_VALUE) continue;
        do {
            char nm[140]; size_t l;
            if (f.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
            _snprintf(nm, 20, "%s", pass ? "" : "LICENSES/"); nm[20] = 0;
            if (olds && (!WideCharToMultiByte(CP_UTF8, 0, f.cFileName, -1, nm + strlen(nm), 110, NULL, NULL) || (l = strlen(nm)) < 5
                         || strcmp(nm + l - 4, ".old") || (nm[l - 4] = 0, !name_allowed(nm)))) continue;
            pj(p, d, f.cFileName);
            if (!DeleteFileW(p)) left++;
        } while (FindNextFileW(h, &f));
        FindClose(h);
    }
    return left;
}
static void clear_tree(const WCHAR *dir)      /* only the staging / backup trees under <data> */
{
    WCHAR sub[MAX_PATH];
    del_files(dir, 0); pj(sub, dir, L"LICENSES"); RemoveDirectoryW(sub); RemoveDirectoryW(dir);
}
/* staged / installable names under <dir>: allowed top files + LICENSES\*.txt, as zip names ("LICENSES/x.txt") */
static int list_names(const WCHAR *dir, char names[][128], int max)
{
    WCHAR pat[MAX_PATH], sub[MAX_PATH]; WIN32_FIND_DATAW f; HANDLE h; int n = 0, pass;
    pj(sub, dir, L"LICENSES");
    for (pass = 0; pass < 2; pass++) {
        pj(pat, pass ? sub : dir, L"*");
        if ((h = FindFirstFileW(pat, &f)) == INVALID_HANDLE_VALUE) continue;
        do {
            char nm[128];
            if (f.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY || n >= max) continue;
            _snprintf(nm, 127, "%s", pass ? "LICENSES/" : ""); nm[127] = 0;
            if (!WideCharToMultiByte(CP_UTF8, 0, f.cFileName, -1, nm + strlen(nm), 100, NULL, NULL)) continue;
            if (name_allowed(nm)) strcpy(names[n++], nm);
        } while (FindNextFileW(h, &f));
        FindClose(h);
    }
    return n;
}
static void wname(WCHAR *o, const WCHAR *dir, const char *name)      /* <dir>\<zip name with '\'> */
{
    WCHAR *w = wide(name), *c; for (c = w; c && *c; c++) if (*c == '/') *c = '\\';
    pj(o, dir, w ? w : L"?"); free(w);
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
        unsigned char *z = read_file(g_testzip, &n); ZipEntry e[MAXF]; int i, m = z ? zip_list(z, n, e, MAXF) : -1;
        memset(g_rel, 0, sizeof g_rel); k = 0;
        for (i = 0; i < m; i++) if (!strcmp(e[i].name, "BUILDINFO.txt")) {
            char *t = (char *)zip_get(&e[i]); Installed in;
            if (t) { t[e[i].usize] = 0; parse_buildinfo(t, &in); free(t);
                     strcpy(g_rel[0].tag, in.tag); g_rel[0].pre = in.pre; strcpy(g_rel[0].url, "test"); g_rel[0].body = _strdup("Test update from a local zip (--test-update-zip)."); k = 1; }
        }
        free(z);
        if (!k) WFMT(g_err, L"test zip has no BUILDINFO.txt release line");
    } else if (http_get(WSTR(REPO_API), &j, &n, 5000)) {
        free_releases(g_rel, g_nrel); g_nrel = 0;
        k = parse_releases(j, g_rel, 30);
        if (k < 0) WFMT(g_err, L"Unexpected answer from GitHub.");
        free(j);
    }
    PostMessageW(g_wnd, WM_CHECKED, (WPARAM)k, 0);
    return 0;
}
static void status(const WCHAR *fmt, ...) { WCHAR b[600]; va_list a; va_start(a, fmt); _vsnwprintf(b, 599, fmt, a); va_end(a); b[599] = 0; SetWindowTextW(h_status, b); }
static void installed_name(WCHAR *o, size_t cap)
{
    if (g_inst.tag[0]) _snwprintf(o, cap - 1, L"%hs", g_inst.tag); else wcsncpy(o, L"unknown build", cap - 1);
    o[cap - 1] = 0;
}
static void show_state(void)
{
    WCHAR inst[80], *body, *crlf, *o, *i; int st;
    installed_name(inst, 80);
    g_new = pick_newest(g_rel, g_nrel, g_pre);
    EnableWindow(h_update, FALSE);
    SetWindowTextW(h_log, L"");
    if (!g_new) { status(L"No release found on the %s channel. Installed: %s.", g_pre ? L"pre-release" : L"release", inst); return; }
    st = update_state(&g_inst, g_new->tag);
    if (st == ST_UPTODATE) status(L"Up to date: %hs%s.", g_new->tag, g_new->pre ? L" (pre-release)" : L"");
    else if (st == ST_NEWER_INSTALLED) status(L"Installed %s is newer than %hs.", inst, g_new->tag);
    else { status(L"Update available: %hs%s. Installed: %s.", g_new->tag, g_new->pre ? L" (pre-release)" : L"", inst); EnableWindow(h_update, TRUE); }
    if (!(body = wide(g_new->body ? g_new->body : ""))) return;
    if ((crlf = o = (WCHAR *)calloc(wcslen(body) * 2 + 1, sizeof(WCHAR)))) {      /* the edit control wants \r\n */
        for (i = body; *i; i++) { if (*i == '\n' && (i == body || i[-1] != '\r')) *o++ = '\r'; *o++ = *i; }
        SetWindowTextW(h_log, crlf);
    }
    free(crlf); free(body);
}
static void start_check(void)
{
    HANDLE t;
    if (g_busy) return;
    g_busy = 1; EnableWindow(h_check, FALSE); EnableWindow(h_update, FALSE);
    status(L"Checking GitHub for updates...");
    if ((t = CreateThread(NULL, 0, check_thread, NULL, 0, NULL))) CloseHandle(t); else { g_busy = 0; EnableWindow(h_check, TRUE); }
}

/* ---- download + verify + stage (thread) ---- */
static DWORD WINAPI stage_thread(LPVOID unused)
{
    unsigned char *z = NULL, h[32]; char hx[65], want[65], *sums = NULL; size_t n = 0; ZipEntry e[MAXF]; int m, i, ok = 0, bins = 0;
    WCHAR st[MAX_PATH], p[MAX_PATH], *url = NULL;
    (void)unused; g_err[0] = 0;
    if (g_testzip[0]) z = read_file(g_testzip, &n);
    else if ((url = wide(g_new->url)) && http_get(url, (char **)&z, &n, 30000)) {
        sha256(z, n, h); hex32(h, hx);
        if (g_new->digest[0] && strcmp(hx, g_new->digest)) { WFMT(g_err, L"The download does not match GitHub's SHA-256 of the asset."); goto out; }
    }
    if (!z) { if (!g_err[0]) WFMT(g_err, L"Download failed."); goto out; }
    if ((m = zip_list(z, n, e, MAXF)) <= 0) { WFMT(g_err, L"The download is not a valid zip."); goto out; }
    for (i = 0; i < m; i++) if (!strcmp(e[i].name, "SHA256SUMS") && (sums = (char *)zip_get(&e[i]))) sums[e[i].usize] = 0;
    if (!sums) { WFMT(g_err, L"The zip has no SHA256SUMS."); goto out; }
    pj(st, g_data, L"update"); clear_tree(st);
    CreateDirectoryW(g_data, NULL); CreateDirectoryW(st, NULL); pj(p, st, L"LICENSES"); CreateDirectoryW(p, NULL);
    for (i = 0; i < m; i++) {
        unsigned char *d; int listed;
        if (!name_allowed(e[i].name)) continue;
        if (!(d = zip_get(&e[i]))) { WFMT(g_err, L"%hs: damaged in the zip.", e[i].name); goto out; }
        sha256(d, e[i].usize, h); hex32(h, hx); listed = sums_lookup(sums, e[i].name, want);
        if ((needs_hash(e[i].name) && !listed) || (listed && strcmp(hx, want))) { free(d); WFMT(g_err, L"%hs: SHA-256 does not match SHA256SUMS.", e[i].name); goto out; }
        bins += needs_hash(e[i].name);
        wname(p, st, e[i].name);
        if (!write_file(p, d, e[i].usize)) { free(d); WFMT(g_err, L"Cannot write %s.", p); goto out; }
        free(d);
    }
    if (!bins) { WFMT(g_err, L"The zip holds no program files."); goto out; }
    ok = 1;
out:
    if (!ok) { pj(st, g_data, L"update"); clear_tree(st); }
    free(z); free(sums); free(url);
    PostMessageW(g_wnd, WM_STAGED, (WPARAM)ok, 0);
    return 0;
}

/* staged tree ready: start this same exe again as the apply step (elevated only when the install dir is not writable) and exit.
   No helper copy: the apply step renames the running launcher exe (Windows allows renaming a running image) and puts the new one in place.
   Elevated, the started exe is the INSTALLED one in the protected folder, which a non-admin process cannot have swapped. */
static void start_apply(void)
{
    WCHAR st[MAX_PATH], t[MAX_PATH], args[MAX_PATH + 64]; char names[MAXF][128];
    HANDLE h; DWORD err = 0; int i, n, elev = 0;
    pj(st, g_data, L"update");
    pj(t, g_dir, L"mflng-write-test.tmp");
    h = CreateFileW(t, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, NULL);
    if (h == INVALID_HANDLE_VALUE) err = GetLastError(); else CloseHandle(h);
    if (err == ERROR_ACCESS_DENIED || g_force_elev) elev = 1;
    else if (err) { status(L"Cannot write to the install folder (error %lu). Nothing was changed.", err); clear_tree(st); return; }
    n = list_names(st, names, MAXF);
    for (i = 0; i < n; i++) {
        DWORD a; wname(t, g_dir, names[i]); a = GetFileAttributesW(t);
        if (a != INVALID_FILE_ATTRIBUTES && (a & FILE_ATTRIBUTE_READONLY)) { status(L"%s is read-only. Nothing was changed.", t); clear_tree(st); return; }
    }
    if (elev && MessageBoxW(g_wnd, L"The install folder needs administrator rights to update. Continue with administrator rights?", L"MFLng update", MB_YESNO | MB_ICONQUESTION) != IDYES) {
        status(L"Update cancelled. Nothing was changed."); clear_tree(st); return;
    }
    WFMT(args, L"--apply-update %lu \"%s\"%s", GetCurrentProcessId(), g_data, elev ? L" --elevated" : L"");
    if (elev) {
        SHELLEXECUTEINFOW se; memset(&se, 0, sizeof se); se.cbSize = sizeof se; se.fMask = SEE_MASK_NOASYNC; se.hwnd = g_wnd;
        se.lpVerb = L"runas"; se.lpFile = g_exe; se.lpParameters = args; se.lpDirectory = g_dir; se.nShow = SW_SHOWNORMAL;
        if (!ShellExecuteExW(&se)) {
            err = GetLastError(); clear_tree(st);
            if (err == ERROR_CANCELLED) status(L"Administrator rights were not granted. Nothing was changed.");
            else status(L"Cannot start the update step (error %lu). Nothing was changed.", err);
            return;
        }
    } else {
        STARTUPINFOW si; PROCESS_INFORMATION pi; WCHAR cmd[2 * MAX_PATH + 64];
        memset(&si, 0, sizeof si); si.cb = sizeof si; WFMT(cmd, L"\"%s\" %s", g_exe, args);
        if (!CreateProcessW(g_exe, cmd, NULL, NULL, FALSE, 0, NULL, g_dir, &si, &pi)) { status(L"Cannot start the update step (error %lu).", GetLastError()); clear_tree(st); return; }
        CloseHandle(pi.hThread); CloseHandle(pi.hProcess);
    }
    DestroyWindow(g_wnd);      /* the apply step waits for this process to end */
}

/* ---- apply step: <launcher exe> --apply-update <pid> <data dir> [--elevated] ----
   Touches only the install dir (the allowed release names and their .new / .old), <data>\update, <data>\backup, <data>\backup.prev and
   <data>\update*.log.  Each staged file is read into memory, checked against the staged SHA256SUMS, and written from that same buffer:
   a file swapped in the user-writable staging dir after the check is never what lands in the install dir. */
static int apply_update(DWORD pid, int elevated)
{
    WCHAR st[MAX_PATH], bk[MAX_PATH], bp[MAX_PATH], p[MAX_PATH], q[MAX_PATH], o[MAX_PATH], lg[MAX_PATH], lg1[MAX_PATH], *self = wcsrchr(g_exe, L'\\') + 1;
    char names[MAXF][128], hx[65], want[65], *sums = NULL; unsigned char *data[MAXF], h[32]; size_t len[MAXF];
    int n = 0, i, k, ok = 0, rotated = 0, had[MAXF], copied[MAXF], aside[MAXF], placed[MAXF]; HANDLE pr, mx;
    mx = CreateMutexW(NULL, FALSE, L"MFLngLauncherUpdate");      /* a launcher started meanwhile leaves the staging dir alone */
    memset(data, 0, sizeof data); memset(had, 0, sizeof had); memset(copied, 0, sizeof copied); memset(aside, 0, sizeof aside); memset(placed, 0, sizeof placed);
    pj(st, g_data, L"update"); pj(bk, g_data, L"backup"); pj(bp, g_data, L"backup.prev"); pj(lg, g_data, L"update.log"); pj(lg1, g_data, L"update.prev.log");
    MoveFileExW(lg, lg1, MOVEFILE_REPLACE_EXISTING);      /* keep the previous log only */
    logf_(L"apply %hs (%s): pid %lu, install dir %s, data dir %s, elevated %d", LAUNCHER_VERSION, self, pid, g_dir, g_data, elevated);
    if ((pr = OpenProcess(SYNCHRONIZE, FALSE, pid))) {
        DWORD w = WaitForSingleObject(pr, 15000); CloseHandle(pr);
        if (w == WAIT_TIMEOUT) { logf_(L"FAIL: the launcher (pid %lu) did not exit within 15 s", pid); goto end; }
    }
    n = list_names(st, names, MAXF);
    pj(p, st, L"SHA256SUMS");
    if (!(sums = (char *)read_file(p, NULL))) { logf_(L"FAIL: no staged SHA256SUMS"); goto end; }
    for (i = 0; i < n; i++) {      /* verify everything before touching the install dir */
        int listed;
        wname(p, st, names[i]);
        if (!(data[i] = read_file(p, &len[i]))) { logf_(L"FAIL: cannot read %s", p); goto end; }
        sha256(data[i], len[i], h); hex32(h, hx); listed = sums_lookup(sums, names[i], want);
        if ((needs_hash(names[i]) && !listed) || (listed && strcmp(hx, want))) { logf_(L"FAIL: %hs does not match SHA256SUMS", names[i]); goto end; }    }
    pj(p, g_dir, L"LICENSES"); CreateDirectoryW(p, NULL);
    for (i = 0; i < n; i++) {      /* phase 1: <name>.new beside each target */
        wname(p, g_dir, names[i]); wcscat(p, L".new");
        if (!write_file(p, data[i], len[i])) { logf_(L"FAIL: cannot write %s (error %lu)", p, GetLastError()); goto rollback; }
    }
    clear_tree(bp);      /* one backup level: the last one waits in backup.prev until this update has succeeded */
    if (exists(bk) && !(rotated = MoveFileExW(bk, bp, 0))) { logf_(L"FAIL: cannot move %s to %s (error %lu)", bk, bp, GetLastError()); goto rollback; }
    CreateDirectoryW(bk, NULL); pj(p, bk, L"LICENSES"); CreateDirectoryW(p, NULL);
    for (k = 0; k < n; k++) {      /* phase 2: copy old -> backup, old -> <name>.old (works for a running exe), .new -> name */
        wname(p, g_dir, names[k]); wname(q, bk, names[k]); wcscpy(o, p); wcscat(o, L".old");
        if ((had[k] = exists(p))) {
            if (!(copied[k] = CopyFileW(p, q, FALSE))) { logf_(L"FAIL: cannot copy %s to the backup (error %lu)", p, GetLastError()); goto rollback; }
            if (!(aside[k] = MoveFileExW(p, o, MOVEFILE_REPLACE_EXISTING))) { logf_(L"FAIL: cannot rename %s (error %lu)", p, GetLastError()); goto rollback; }
        }
        wcscpy(q, p); wcscat(q, L".new");
        if (!(placed[k] = MoveFileExW(q, p, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))) { logf_(L"FAIL: cannot rename %s (error %lu)", q, GetLastError()); goto rollback; }
        logf_(L"replaced %hs%s", names[k], had[k] ? L"" : L" (new file)");
    }
    ok = 1; clear_tree(bp);
    for (k = 0; k < n; k++) if (aside[k]) {      /* the running exe's .old goes at the next start (or at reboot when elevated) */
        wname(o, g_dir, names[k]); wcscat(o, L".old");
        if (!DeleteFileW(o) && elevated) MoveFileExW(o, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
    }
    logf_(L"update applied: %d files; previous files in %s", n, bk);    goto end;
rollback:
    for (k = 0; k < n; k++) {
        wname(p, g_dir, names[k]); wcscpy(o, p); wcscat(o, L".old"); wname(q, bk, names[k]);
        if (placed[k]) DeleteFileW(p);
        if (aside[k] && !MoveFileExW(o, p, MOVEFILE_REPLACE_EXISTING) && !CopyFileW(q, p, FALSE)) logf_(L"ROLLBACK FAILED for %s (error %lu): copy it back from %s", p, GetLastError(), q);
        wcscat(p, L".new"); DeleteFileW(p);
    }
    clear_tree(bk);
    if (rotated) MoveFileExW(bp, bk, 0);      /* the backup of the update before stays the one level */
    logf_(L"rolled back; the previous version stays");
end:
    for (i = 0; i < MAXF; i++) free(data[i]);
    free(sums); clear_tree(st);
    if (!ok) { WCHAR m[MAX_PATH + 96]; WFMT(m, L"The update failed and nothing was changed (the previous version stays).\nDetails: %s", lg); MessageBoxW(NULL, m, L"MFLng update", MB_OK | MB_ICONERROR); }
    wcscpy(p, g_exe);      /* the path this process started from: now the new (or the restored) launcher */
    if (mx) CloseHandle(mx);
    if (elevated) ShellExecuteW(NULL, L"open", L"explorer.exe", p, NULL, SW_SHOWNORMAL);      /* explorer starts it unelevated (the user's shell) */
    else { STARTUPINFOW si; PROCESS_INFORMATION pi; WFMT(q, L"\"%s\"", p); memset(&si, 0, sizeof si); si.cb = sizeof si;
           if (CreateProcessW(p, q, NULL, NULL, FALSE, 0, NULL, g_dir, &si, &pi)) { CloseHandle(pi.hThread); CloseHandle(pi.hProcess); } }
    return ok ? 0 : 1;
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

/* ---- window ---- */
static void play(const WCHAR *name)
{
    WCHAR p[MAX_PATH], cmd[MAX_PATH + 4]; STARTUPINFOW si; PROCESS_INFORMATION pi;
    pj(p, g_dir, name); WFMT(cmd, L"\"%s\"", p); memset(&si, 0, sizeof si); si.cb = sizeof si;
    if (CreateProcessW(p, cmd, NULL, NULL, FALSE, 0, NULL, g_dir, &si, &pi)) { CloseHandle(pi.hThread); CloseHandle(pi.hProcess); DestroyWindow(g_wnd); }
    else status(L"Cannot start %s (error %lu).", name, GetLastError());
}
static HWND ctl(const WCHAR *cls, const WCHAR *text, DWORD style, int x, int y, int w, int h, int id)
{
    HWND c = CreateWindowExW(0, cls, text, WS_CHILD | WS_VISIBLE | style, x, PIC_H + y, w, h, g_wnd, (HMENU)(INT_PTR)id, GetModuleHandleW(NULL), NULL);
    SendMessageW(c, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), FALSE);
    return c;
}
static void save_settings(void)
{
    WritePrivateProfileStringW(L"launcher", L"update_at_startup", g_auto ? L"on" : L"off", g_ini);
    WritePrivateProfileStringW(L"launcher", L"channel", g_pre ? L"prerelease" : L"release", g_ini);
}
static LRESULT CALLBACK wndproc(HWND w, UINT m, WPARAM wp, LPARAM lp)
{
    switch (m) {
    case WM_CREATE: {
        WCHAR p[MAX_PATH];
        g_wnd = w;
        h_play = ctl(L"BUTTON", L"Play", BS_DEFPUSHBUTTON, 10, 10, 110, 30, ID_PLAY);
        h_nofix = ctl(L"BUTTON", L"Play (original behaviour)", 0, 125, 10, 170, 30, ID_NOFIX);
        h_check = ctl(L"BUTTON", L"Check for updates", 0, 300, 10, 130, 30, ID_CHECK);
        h_update = ctl(L"BUTTON", L"Update", WS_DISABLED, 435, 10, 90, 30, ID_UPDATE);
        ctl(L"BUTTON", L"Exit", 0, 530, 10, 100, 30, ID_EXIT);
        h_auto = ctl(L"BUTTON", L"Check for updates at start-up", BS_AUTOCHECKBOX, 10, 50, 220, 20, ID_AUTO);
        ctl(L"STATIC", L"Update channel:", 0, 240, 53, 90, 18, 0);
        h_chan = ctl(L"COMBOBOX", L"", CBS_DROPDOWNLIST | WS_VSCROLL, 330, 49, 130, 100, ID_CHAN);
        ctl(L"BUTTON", L"GitHub repository", 0, 490, 48, 140, 24, ID_WEB);
        h_status = ctl(L"STATIC", L"", SS_LEFTNOWORDWRAP, 10, 80, 620, 18, 0);
        h_log = ctl(L"EDIT", L"", WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL, 10, 100, 340, 100, 0);
        ctl(L"STATIC", L"MFLng-launch " WSTR(LAUNCHER_VERSION) L"\n" CREDIT L"\nLicences: see the LICENSES folder.\n" NOTICE, 0, 360, 100, 270, 100, 0);
        SendMessageW(h_chan, CB_ADDSTRING, 0, (LPARAM)L"Releases"); SendMessageW(h_chan, CB_ADDSTRING, 0, (LPARAM)L"Pre-releases");
        SendMessageW(h_chan, CB_SETCURSEL, (WPARAM)g_pre, 0);
        SendMessageW(h_auto, BM_SETCHECK, g_auto ? BST_CHECKED : BST_UNCHECKED, 0);
        pj(p, g_dir, L"MFLng.exe"); EnableWindow(h_play, exists(p));
        pj(p, g_dir, L"MFLng.nofix.exe"); EnableWindow(h_nofix, exists(p));
        { WCHAR inst[80]; installed_name(inst, 80); status(L"Installed: %s.", inst); }
        if (g_auto || g_testzip[0]) start_check();
        return 0; }
    case WM_ERASEBKGND: return 1;
    case WM_PAINT: {
        PAINTSTRUCT ps; HDC dc = BeginPaint(w, &ps); RECT r = {0, 0, PIC_W, PIC_H + PANEL_H};
        FillRect(dc, &r, g_dark);
        if (g_bi) { SetStretchBltMode(dc, HALFTONE); StretchDIBits(dc, 0, 0, PIC_W, PIC_H, 0, 0, g_bi->bmiHeader.biWidth, g_bi->bmiHeader.biHeight, g_bits, g_bi, DIB_RGB_COLORS, SRCCOPY); }
        else {      /* no launch.exe / Setup.bmp beside the launcher: plain dark window with the name */
            HFONT f = CreateFontW(56, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, ANTIALIASED_QUALITY, 0, L"Arial"), o = (HFONT)SelectObject(dc, f);
            RECT t = {0, 170, PIC_W, 300};
            SetBkMode(dc, TRANSPARENT); SetTextColor(dc, RGB(240, 240, 240));
            DrawTextW(dc, L"Mashed: Fully Loaded\nMFLng", -1, &t, DT_CENTER);
            SelectObject(dc, o); DeleteObject(f);
        }
        EndPaint(w, &ps); return 0; }
    case WM_CTLCOLORSTATIC: case WM_CTLCOLORBTN:
        SetTextColor((HDC)wp, RGB(230, 230, 230)); SetBkColor((HDC)wp, RGB(24, 24, 28)); return (LRESULT)g_dark;
    case WM_COMMAND:
        switch (LOWORD(wp)) {
        case ID_PLAY: play(L"MFLng.exe"); break;
        case ID_NOFIX: play(L"MFLng.nofix.exe"); break;
        case ID_CHECK: start_check(); break;
        case ID_EXIT: DestroyWindow(w); break;
        case ID_WEB: ShellExecuteW(w, L"open", WSTR(REPO_URL), NULL, NULL, SW_SHOWNORMAL); break;
        case ID_AUTO: g_auto = SendMessageW(h_auto, BM_GETCHECK, 0, 0) == BST_CHECKED; save_settings(); break;
        case ID_CHAN: if (HIWORD(wp) == CBN_SELCHANGE) { g_pre = (int)SendMessageW(h_chan, CB_GETCURSEL, 0, 0) == 1; save_settings(); if (g_nrel) show_state(); } break;
        case ID_UPDATE: {
            HANDLE t;
            if (g_busy || !g_new) break;
            g_busy = 1; EnableWindow(h_update, FALSE); EnableWindow(h_check, FALSE);
            status(L"Downloading and checking %hs...", g_new->tag);
            if ((t = CreateThread(NULL, 0, stage_thread, NULL, 0, NULL))) CloseHandle(t); else g_busy = 0;
            break; }
        }
        return 0;
    case WM_CHECKED:
        g_busy = 0; EnableWindow(h_check, TRUE);
        if ((int)wp >= 0) { g_nrel = (int)wp; show_state(); } else status(L"%s", g_err);
        return 0;
    case WM_STAGED:
        g_busy = 0; EnableWindow(h_check, TRUE);
        if (wp) { status(L"Verified. Installing..."); start_apply(); } else { status(L"Update failed: %s Nothing was changed.", g_err); EnableWindow(h_update, TRUE); }
        return 0;
    case WM_DESTROY: PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(w, m, wp, lp);
}

static DWORD WINAPI cleanup_thread(LPVOID unused)      /* leftovers of the last update: the apply step that started us may still be exiting */
{
    WCHAR st[MAX_PATH]; HANDLE mx; int i;
    (void)unused; pj(st, g_data, L"update");
    for (i = 0; i < 30 && del_files(g_dir, 1); i++) Sleep(100);      /* <name>.old of the files it replaced (a protected dir: the reboot does it) */
    if ((mx = OpenMutexW(SYNCHRONIZE, FALSE, L"MFLngLauncherUpdate"))) { CloseHandle(mx); return 0; }      /* an update is being applied */
    clear_tree(st);      /* a staging dir left by a crash or a closed launcher */
    return 0;
}

int WINAPI wWinMain(HINSTANCE hi, HINSTANCE prev, LPWSTR cl, int show)
{
    int argc, i; WCHAR **argv = CommandLineToArgvW(GetCommandLineW(), &argc), p[MAX_PATH], v[32], base[MAX_PATH]; WNDCLASSW wc; MSG msg; RECT r; HANDLE t;
    char *bi;
    (void)prev; (void)cl;
    GetModuleFileNameW(NULL, g_exe, MAX_PATH);
    wcscpy(g_dir, g_exe); *wcsrchr(g_dir, L'\\') = 0;
    if (argc >= 4 && !wcscmp(argv[1], L"--apply-update")) {
        wcsncpy(g_data, argv[3], MAX_PATH - 1);
        return apply_update(wcstoul(argv[2], NULL, 10), argc > 4 && !wcscmp(argv[4], L"--elevated"));
    }
    for (i = 1; i < argc; i++) {
        if (!wcscmp(argv[i], L"--test-update-zip") && i + 1 < argc) GetFullPathNameW(argv[++i], MAX_PATH, g_testzip, NULL);
        else if (!wcscmp(argv[i], L"--test-force-elevate")) g_force_elev = 1;
    }
    /* data dir: %LOCALAPPDATA%\mflng-launch (MFL keeps its own files beside the exe, a folder that may be protected); portable.txt -> <exe dir>\mflng-launch */
    pj(p, g_dir, L"portable.txt");
    if (exists(p) || !GetEnvironmentVariableW(L"LOCALAPPDATA", base, MAX_PATH)) pj(g_data, g_dir, L"mflng-launch");
    else pj(g_data, base, L"mflng-launch");
    CreateDirectoryW(g_data, NULL);
    pj(g_ini, g_data, L"mflng-launch.ini");
    if ((t = CreateThread(NULL, 0, cleanup_thread, NULL, 0, NULL))) CloseHandle(t);
    pj(p, g_dir, L"BUILDINFO.txt"); bi = (char *)read_file(p, NULL); parse_buildinfo(bi, &g_inst); free(bi);
    GetPrivateProfileStringW(L"launcher", L"update_at_startup", L"on", v, 32, g_ini); g_auto = _wcsicmp(v, L"off") != 0;
    GetPrivateProfileStringW(L"launcher", L"channel", L"", v, 32, g_ini);
    g_pre = !_wcsicmp(v, L"prerelease") ? 1 : !_wcsicmp(v, L"release") ? 0 : default_channel_pre(&g_inst);
    load_picture();
    g_dark = CreateSolidBrush(RGB(24, 24, 28));
    memset(&wc, 0, sizeof wc);
    wc.lpfnWndProc = wndproc; wc.hInstance = hi; wc.hCursor = LoadCursor(NULL, IDC_ARROW); wc.lpszClassName = L"MFLngLauncher"; wc.hbrBackground = g_dark;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassW(&wc);
    r.left = r.top = 0; r.right = PIC_W; r.bottom = PIC_H + PANEL_H;
    AdjustWindowRect(&r, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);
    CreateWindowW(L"MFLngLauncher", L"Mashed: Fully Loaded - MFLng-launch " WSTR(LAUNCHER_VERSION), WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
                  CW_USEDEFAULT, CW_USEDEFAULT, r.right - r.left, r.bottom - r.top, NULL, NULL, hi, NULL);
    (void)show;
    while (GetMessageW(&msg, NULL, 0, 0) > 0) { if (!IsDialogMessageW(g_wnd, &msg)) { TranslateMessage(&msg); DispatchMessageW(&msg); } }
    LocalFree(argv);
    return 0;
}
