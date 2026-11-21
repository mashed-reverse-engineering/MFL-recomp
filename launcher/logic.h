/* logic.h: the platform-free part of the MFLng launcher (no windows.h): release list from the GitHub API answer, tag and channel rules,
   BUILDINFO.txt, SHA-256, SHA256SUMS, a minimal zip reader with its own raw inflate and CRC-32, and the update itself (staging a zip,
   applying it, rollback) over a small file layer.  Tested natively by test_logic.c. */
#ifndef MFLNG_LAUNCHER_LOGIC_H
#define MFLNG_LAUNCHER_LOGIC_H
#include <stddef.h>

#define REPO_API "https://api.github.com/repos/mashed-reverse-engineering/MFL-recomp/releases?per_page=30"
#define REPO_URL "https://github.com/mashed-reverse-engineering/MFL-recomp"

typedef struct {
    char tag[64];
    int pre, draft;
    char *body;            /* UTF-8, unescaped, malloc'd (free_releases) */
    char url[512];         /* browser_download_url of MFLng-<tag>.zip, "" if the release has none */
    char digest[65];       /* lower-case hex of the asset's "digest":"sha256:..." if the API gave one */
} Release;

int parse_releases(const char *json, Release *out, int max);      /* -> count, -1 = not a JSON array */
void free_releases(Release *r, int n);
/* newest = the first in API order (created_at, newest first) that is not a draft, has the zip asset and fits the channel */
const Release *pick_newest(const Release *r, int n, int allow_pre);

typedef struct { char tag[64]; int pre; } Installed;      /* from the `release <tag> [prerelease]` line release.yml writes */
void parse_buildinfo(const char *text, Installed *in);      /* text may be NULL (no BUILDINFO.txt) */
int default_channel_pre(const Installed *in);               /* 1 = prerelease channel by default */

enum { ST_UPTODATE, ST_UPDATE, ST_NEWER_INSTALLED };
int update_state(const Installed *in, const char *newest_tag);

void sha256(const unsigned char *p, size_t n, unsigned char out[32]);
void hex32(const unsigned char h[32], char out[65]);
int sums_lookup(const char *sums, const char *name, char hex[65]);   /* 1 = listed */

typedef struct { char name[128]; unsigned method, crc; size_t csize, usize; const unsigned char *data; } ZipEntry;
int inflate_raw(const unsigned char *in, size_t inlen, unsigned char *out, size_t outlen);   /* 0 = exactly outlen bytes */
unsigned long crc32_buf(const unsigned char *p, size_t n);
int zip_list(const unsigned char *z, size_t n, ZipEntry *out, int max);    /* -> count, -1 = not a zip / corrupt / more than max entries */
unsigned char *zip_get(const ZipEntry *e);      /* malloc'd usize bytes (crc checked), NULL on error */
int name_allowed(const char *name);             /* the release files the launcher may install (never game files) */
int name_ours(const char *name);                /* name_allowed + the names of the layouts before it: what an update may remove */
int needs_hash(const char *name);               /* .exe / .dll must be listed in SHA256SUMS */

/* ---- the update itself: stage a release zip, apply the staged tree to the game folder ----
   Runs over this small file layer: launcher.c implements it with Win32, test_logic.c with POSIX (and injected failures).
   Paths are UTF-8; `name` arguments are zip names ('/' between the parts). */
#define UPD_MAXF 96       /* zip entries / files of one update */
#define UPD_PATH 2048
unsigned char *os_read(const char *path, size_t *n);          /* malloc'd, one 0 byte after the content; NULL = cannot read */
int os_write(const char *path, const void *b, size_t n);      /* the complete file or nothing */
int os_exists(const char *path);
int os_rename(const char *from, const char *to);              /* a file (replaces a file at `to`) or a folder */
int os_copy(const char *from, const char *to);
int os_delete(const char *path);                              /* 1 = the file is gone */
int os_mkdir(const char *path);                               /* 1 = created by this call */
int os_rmdir(const char *path);                               /* an empty folder only */
void os_rmtree(const char *path);                             /* a folder with all it holds; a link in it is removed, never followed (only <data>/backup*) */
void os_list(const char *dir, void (*f)(const char *name, void *ctx), void *ctx);   /* the files of dir, no folders */
long os_err(void);                                            /* error code of the last failed call, for the log */
void os_log(const char *fmt, ...);                            /* one line of update.log */

void upd_path(char *out, const char *dir, const char *name);  /* out[UPD_PATH] = <dir><separator><name> */
int upd_staged(const char *stage, char names[][128], int max);   /* the installable files of a staged tree -> count */
void upd_clear(const char *dir);                              /* delete a staging / backup tree (its known folders, one level each) */
int upd_clean(const char *install);                           /* delete <release file>.old / .new left in the game folder -> files that stay */
/* zip -> <stage>: 1 = verified and staged; 0 = refused, err says why, nothing is left.  tag: the BUILDINFO release tag it must have (NULL = any) */
int upd_stage(const unsigned char *zip, size_t n, const char *stage, const char *tag, char *err, size_t cap);
/* <data>/update -> <install>, old files to <data>/backup.  self = file name of the running launcher (never removed).
   1 = installed; 2 = installed, but the launcher exe is in use and could not be moved: the new one lies beside it as <name>.new.exe
   for the next start; 0 = failed, the game folder is as before; -1 = failed and not everything could be put back (<data>/backup has the files) */
int upd_apply(const char *install, const char *data, const char *self);
#endif
