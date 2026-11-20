/* logic.h: the platform-free part of the MFLng launcher (no windows.h): release list from the GitHub API answer, tag and channel rules,
   BUILDINFO.txt, SHA-256, SHA256SUMS, a minimal zip reader with its own raw inflate and CRC-32.  Tested natively by test_logic.c. */
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
int zip_list(const unsigned char *z, size_t n, ZipEntry *out, int max);    /* -> count, -1 = not a zip / corrupt */
unsigned char *zip_get(const ZipEntry *e);      /* malloc'd usize bytes (crc checked), NULL on error */
int name_allowed(const char *name);             /* the release files the launcher may install (never game files) */
int needs_hash(const char *name);               /* .exe / .dll must be listed in SHA256SUMS */
#endif
