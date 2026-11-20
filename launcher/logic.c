/* logic.c: see logic.h.  Plain C, no library beyond the C runtime (own inflate and CRC-32 below: the game's zlib 1.1.3 is not used here). */
#include "logic.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ---- JSON: just enough to walk the releases array; strings are copied unescaped ---- */
static const char *ws(const char *p) { while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++; return p; }

static void put_utf8(char **o, char *end, unsigned c)
{
    char b[4]; int n = 0, i;
    if (c < 0x80) b[n++] = (char)c;
    else if (c < 0x800) { b[n++] = (char)(0xC0 | c >> 6); b[n++] = (char)(0x80 | (c & 63)); }
    else { b[n++] = (char)(0xE0 | c >> 12); b[n++] = (char)(0x80 | (c >> 6 & 63)); b[n++] = (char)(0x80 | (c & 63)); }
    for (i = 0; i < n && *o < end; i++) *(*o)++ = b[i];
}

/* p at '"'; copies up to cap-1 bytes into out (out may be NULL = skip); returns the char after the closing quote, NULL on error */
static const char *jstr(const char *p, char *out, size_t cap)
{
    char *o = out, *end = out ? out + cap - 1 : NULL;
    if (*p++ != '"') return NULL;
    for (;;) {
        unsigned c = (unsigned char)*p++;
        if (!c) return NULL;
        if (c == '"') break;
        if (c == '\\') {
            c = (unsigned char)*p++;
            switch (c) {
            case 'n': c = '\n'; break; case 't': c = '\t'; break; case 'r': c = '\r'; break;
            case 'b': c = '\b'; break; case 'f': c = '\f'; break;
            case 'u': { unsigned v = 0; int i; for (i = 0; i < 4; i++) { int h = *p++; if (!isxdigit(h)) return NULL;
                        v = v * 16 + (unsigned)(isdigit(h) ? h - '0' : (tolower(h) - 'a' + 10)); }
                        c = (v >= 0xD800 && v < 0xE000) ? '?' : v; break; }      /* ponytail: surrogate pairs (emoji) shown as '?' */
            case 0: return NULL;
            default: break;      /* \" \\ \/ */
            }
        }
        if (o) put_utf8(&o, end, c);
    }
    if (o) *o = 0;
    return p;
}

static const char *jskip(const char *p, int depth)
{
    p = ws(p);
    if (depth > 64) return NULL;
    if (*p == '"') return jstr(p, NULL, 0);
    if (*p == '{' || *p == '[') {
        char close = *p == '{' ? '}' : ']';
        p = ws(p + 1);
        if (*p == close) return p + 1;
        for (;;) {
            if (close == '}') { if (!(p = jstr(ws(p), NULL, 0))) return NULL; p = ws(p); if (*p++ != ':') return NULL; }
            if (!(p = jskip(p, depth + 1))) return NULL;
            p = ws(p);
            if (*p == ',') { p++; continue; }
            return *p == close ? p + 1 : NULL;
        }
    }
    if (!*p) return NULL;
    while (*p && !strchr(",}] \t\r\n", *p)) p++;      /* number, true, false, null */
    return p;
}

/* iterate the keys of the object at p ('{'); calls f(key, value pointer, ctx) and returns the char after '}' */
typedef void (*keyfn)(const char *key, const char *val, void *ctx);
static const char *jobj(const char *p, keyfn f, void *ctx)
{
    char key[64];
    p = ws(p);
    if (*p++ != '{') return NULL;
    p = ws(p);
    if (*p == '}') return p + 1;
    for (;;) {
        const char *v;
        if (!(p = jstr(ws(p), key, sizeof key))) return NULL;
        p = ws(p); if (*p++ != ':') return NULL;
        v = ws(p);
        f(key, v, ctx);
        if (!(p = jskip(v, 1))) return NULL;
        p = ws(p);
        if (*p == ',') { p++; continue; }
        return *p == '}' ? p + 1 : NULL;
    }
}

typedef struct { char name[128], url[512], digest[80]; } Asset;
static void asset_key(const char *k, const char *v, void *ctx)
{
    Asset *a = (Asset *)ctx;
    if (*v != '"') return;
    if (!strcmp(k, "name")) jstr(v, a->name, sizeof a->name);
    else if (!strcmp(k, "browser_download_url")) jstr(v, a->url, sizeof a->url);
    else if (!strcmp(k, "digest")) jstr(v, a->digest, sizeof a->digest);
}

static void rel_key(const char *k, const char *v, void *ctx)
{
    Release *r = (Release *)ctx;
    if (!strcmp(k, "tag_name") && *v == '"') jstr(v, r->tag, sizeof r->tag);
    else if (!strcmp(k, "prerelease")) r->pre = !strncmp(v, "true", 4);
    else if (!strcmp(k, "draft")) r->draft = !strncmp(v, "true", 4);
    else if (!strcmp(k, "body") && *v == '"') {
        const char *e = jstr(v, NULL, 0);
        if (e && (r->body = (char *)malloc((size_t)(e - v) + 1))) jstr(v, r->body, (size_t)(e - v) + 1);
    } else if (!strcmp(k, "assets") && *v == '[') {
        const char *p = ws(v + 1);
        while (*p == '{') {
            Asset a; memset(&a, 0, sizeof a);
            if (!(p = jobj(p, asset_key, &a))) return;
            /* GitHub sends tag_name before assets; parse_releases checks the url against the tag again in any case */
            if (!strncmp(a.name, "MFLng-", 6) && !strncmp(a.name + 6, r->tag, strlen(r->tag)) && !strcmp(a.name + 6 + strlen(r->tag), ".zip")) {
                strncpy(r->url, a.url, sizeof r->url - 1);
                if (!strncmp(a.digest, "sha256:", 7)) strncpy(r->digest, a.digest + 7, 64);
            }
            p = ws(p); if (*p == ',') p = ws(p + 1);
        }
    }
}

int parse_releases(const char *json, Release *out, int max)
{
    const char *p = ws(json);
    int n = 0;
    if (*p++ != '[') return -1;
    p = ws(p);
    while (*p == '{' && n < max) {
        Release *r = &out[n]; char want[96];
        memset(r, 0, sizeof *r);
        if (!(p = jobj(p, rel_key, r))) { free(r->body); return n; }
        strcpy(want, "/MFLng-"); strncat(want, r->tag, 64); strcat(want, ".zip");      /* the asset must be MFLng-<tag>.zip */
        if (strlen(r->url) < strlen(want) || strcmp(r->url + strlen(r->url) - strlen(want), want)) r->url[0] = r->digest[0] = 0;
        n++;
        p = ws(p); if (*p == ',') p = ws(p + 1);
    }
    return n;
}

void free_releases(Release *r, int n) { while (n-- > 0) { free(r[n].body); r[n].body = NULL; } }

const Release *pick_newest(const Release *r, int n, int allow_pre)
{
    int i;
    for (i = 0; i < n; i++)
        if (!r[i].draft && r[i].url[0] && r[i].tag[0] && (allow_pre || !r[i].pre)) return &r[i];
    return NULL;
}

/* ---- BUILDINFO.txt and the tag rules ---- */
void parse_buildinfo(const char *t, Installed *in)      /* the `release <tag> [prerelease]` line written by release.yml */
{
    memset(in, 0, sizeof *in);
    while (t && *t) {
        const char *e = strchr(t, '\n'); size_t len = e ? (size_t)(e - t) : strlen(t);
        char line[160]; if (len >= sizeof line) len = sizeof line - 1;
        memcpy(line, t, len); line[len] = 0;
        while (len && (line[len - 1] == '\r' || line[len - 1] == ' ')) line[--len] = 0;
        if (!strncmp(line, "release ", 8)) {
            char *sp = strchr(line + 8, ' ');
            if (sp) { *sp = 0; in->pre = !strcmp(sp + 1, "prerelease"); }
            strncpy(in->tag, line + 8, sizeof in->tag - 1);
        }
        t = e ? e + 1 : NULL;
    }
}

/* channel rule: the installed build is a pre-release -> prerelease; a release, or no release line (a local build) -> release */
int default_channel_pre(const Installed *in) { return in->pre; }

static long tag_date(const char *t) { return (t[0] == 'r') ? strtol(t + 1, NULL, 10) : 0; }

/* same tag: up to date; installed tag dated later: newer; anything else (also no installed tag): offer the newest */
int update_state(const Installed *in, const char *newest)
{
    if (!in->tag[0]) return ST_UPDATE;
    if (!strcmp(in->tag, newest)) return ST_UPTODATE;
    return tag_date(in->tag) > tag_date(newest) ? ST_NEWER_INSTALLED : ST_UPDATE;
}

/* ---- SHA-256 (FIPS 180-4) ---- */
static const unsigned long K[64] = {
 0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
 0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
 0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
 0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2};
#define ROR(x, n) ((((x) >> (n)) | ((x) << (32 - (n)))) & 0xffffffffUL)
static void block(unsigned long h[8], const unsigned char *b)
{
    unsigned long w[64], a[8], t1, t2; int i;
    for (i = 0; i < 16; i++) w[i] = (unsigned long)b[4 * i] << 24 | (unsigned long)b[4 * i + 1] << 16 | (unsigned long)b[4 * i + 2] << 8 | b[4 * i + 3];
    for (; i < 64; i++)
        w[i] = (w[i - 16] + (ROR(w[i - 15], 7) ^ ROR(w[i - 15], 18) ^ (w[i - 15] >> 3)) + w[i - 7] + (ROR(w[i - 2], 17) ^ ROR(w[i - 2], 19) ^ (w[i - 2] >> 10))) & 0xffffffffUL;
    memcpy(a, h, sizeof a);
    for (i = 0; i < 64; i++) {
        t1 = (a[7] + (ROR(a[4], 6) ^ ROR(a[4], 11) ^ ROR(a[4], 25)) + ((a[4] & a[5]) ^ (~a[4] & a[6])) + K[i] + w[i]) & 0xffffffffUL;
        t2 = ((ROR(a[0], 2) ^ ROR(a[0], 13) ^ ROR(a[0], 22)) + ((a[0] & a[1]) ^ (a[0] & a[2]) ^ (a[1] & a[2]))) & 0xffffffffUL;
        memmove(a + 1, a, 7 * sizeof a[0]); a[4] = (a[4] + t1) & 0xffffffffUL; a[0] = (t1 + t2) & 0xffffffffUL;
    }
    for (i = 0; i < 8; i++) h[i] = (h[i] + a[i]) & 0xffffffffUL;
}
void sha256(const unsigned char *p, size_t n, unsigned char out[32])
{
    unsigned long h[8] = {0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19};
    unsigned char t[128]; size_t i, r = n % 64, tl = r < 56 ? 64 : 128;
    unsigned long long bits = (unsigned long long)n * 8;
    for (i = 0; i + 64 <= n; i += 64) block(h, p + i);
    memset(t, 0, sizeof t); memcpy(t, p + i, r); t[r] = 0x80;
    for (i = 0; i < 8; i++) t[tl - 1 - i] = (unsigned char)(bits >> (8 * i));
    block(h, t); if (tl == 128) block(h, t + 64);
    for (i = 0; i < 32; i++) out[i] = (unsigned char)(h[i / 4] >> (24 - 8 * (i % 4)));
}
void hex32(const unsigned char h[32], char out[65]) { int i; for (i = 0; i < 32; i++) { out[2 * i] = "0123456789abcdef"[h[i] >> 4]; out[2 * i + 1] = "0123456789abcdef"[h[i] & 15]; } out[64] = 0; }

int sums_lookup(const char *s, const char *name, char hex[65])      /* `<64 hex>  <name>` lines (sha256sum format, '*' = binary mode) */
{
    size_t nl = strlen(name);
    while (s && *s) {
        const char *e = strchr(s, '\n'), *f = s + 64;
        size_t len = e ? (size_t)(e - s) : strlen(s);
        if (len > 66 && f[0] == ' ' && (f[1] == ' ' || f[1] == '*')) {
            size_t fl = len - 66; while (fl && (f[2 + fl - 1] == '\r' || f[2 + fl - 1] == ' ')) fl--;
            if (fl == nl && !memcmp(f + 2, name, nl)) { int i; for (i = 0; i < 64; i++) hex[i] = (char)tolower((unsigned char)s[i]); hex[64] = 0; return 1; }
        }
        s = e ? e + 1 : NULL;
    }
    return 0;
}

/* ---- zip ---- */
static unsigned rd16(const unsigned char *p) { return p[0] | p[1] << 8; }
static unsigned long rd32(const unsigned char *p) { return (unsigned long)rd16(p) | (unsigned long)rd16(p + 2) << 16; }

int zip_list(const unsigned char *z, size_t n, ZipEntry *out, int max)
{
    size_t i, cd, cnt, k;
    if (n < 22) return -1;
    for (i = n - 22;; i--) {      /* end of central directory record, at most 64 KB comment */
        if (rd32(z + i) == 0x06054b50UL) break;
        if (i == 0 || n - i > 65557) return -1;
    }
    cnt = rd16(z + i + 10); cd = rd32(z + i + 16);
    for (k = 0; k < cnt && (int)k < max; k++) {
        ZipEntry *e = &out[k]; size_t nl, el, cl, lo, ld;
        if (cd + 46 > n || rd32(z + cd) != 0x02014b50UL) return -1;
        nl = rd16(z + cd + 28); el = rd16(z + cd + 30); cl = rd16(z + cd + 32);
        if (cd + 46 + nl > n || nl >= sizeof e->name) return -1;
        memset(e, 0, sizeof *e);
        memcpy(e->name, z + cd + 46, nl);
        if (rd16(z + cd + 8) & 1) return -1;      /* encrypted */
        e->method = rd16(z + cd + 10); e->crc = (unsigned)rd32(z + cd + 16);
        e->csize = rd32(z + cd + 20); e->usize = rd32(z + cd + 24); lo = rd32(z + cd + 42);
        if (lo + 30 > n || rd32(z + lo) != 0x04034b50UL) return -1;
        ld = lo + 30 + rd16(z + lo + 26) + rd16(z + lo + 28);
        if (ld > n || e->csize > n - ld) return -1;
        e->data = z + ld;
        cd += 46 + nl + el + cl;
    }
    return (int)k;
}

/* ---- raw DEFLATE (RFC 1951), canonical-Huffman decoding in the style of zlib's contrib/puff: every read and write bounds-checked ---- */
typedef struct { const unsigned char *in; size_t inlen, inpos; unsigned long bitbuf; int bitcnt, err; unsigned char *out; size_t outlen, outpos; } Inf;
typedef struct { short count[16], symbol[288]; } Huff;

static unsigned bits(Inf *s, int need)
{
    unsigned long v = s->bitbuf;
    while (s->bitcnt < need) {
        if (s->inpos >= s->inlen) { s->err = 1; return 0; }
        v |= (unsigned long)s->in[s->inpos++] << s->bitcnt; s->bitcnt += 8;
    }
    s->bitbuf = v >> need; s->bitcnt -= need;
    return (unsigned)(v & ((1UL << need) - 1));
}
/* -> 0 complete, > 0 incomplete, < 0 over-subscribed */
static int build(Huff *h, const short *len, int n)
{
    short offs[16]; int i, left = 1;
    memset(h->count, 0, sizeof h->count);
    for (i = 0; i < n; i++) h->count[len[i]]++;
    if (h->count[0] == n) return 0;
    for (i = 1; i < 16; i++) { left <<= 1; left -= h->count[i]; if (left < 0) return left; }
    offs[1] = 0;
    for (i = 1; i < 15; i++) offs[i + 1] = (short)(offs[i] + h->count[i]);
    for (i = 0; i < n; i++) if (len[i]) h->symbol[offs[len[i]]++] = (short)i;
    return left;
}
static int decode(Inf *s, const Huff *h)
{
    int code = 0, first = 0, index = 0, len;
    for (len = 1; len < 16; len++) {
        code |= (int)bits(s, 1);
        if (s->err) return -1;
        if (code - h->count[len] < first) return h->symbol[index + (code - first)];
        index += h->count[len]; first += h->count[len]; first <<= 1; code <<= 1;
    }
    s->err = 1; return -1;
}
static const short LBASE[29] = {3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31,35,43,51,59,67,83,99,115,131,163,195,227,258};
static const short LEXT[29] = {0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0};
static const short DBASE[30] = {1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577};
static const short DEXT[30] = {0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13};
static int codes(Inf *s, const Huff *lc, const Huff *dc)
{
    for (;;) {
        int sym = decode(s, lc), len; size_t dist;
        if (sym < 0) return -1;
        if (sym < 256) { if (s->outpos >= s->outlen) return -1; s->out[s->outpos++] = (unsigned char)sym; continue; }
        if (sym == 256) return 0;
        if ((sym -= 257) >= 29) return -1;
        len = LBASE[sym] + (int)bits(s, LEXT[sym]);
        if ((sym = decode(s, dc)) < 0 || sym >= 30) return -1;
        dist = (size_t)DBASE[sym] + bits(s, DEXT[sym]);
        if (s->err || dist > s->outpos || (size_t)len > s->outlen - s->outpos) return -1;
        while (len--) { s->out[s->outpos] = s->out[s->outpos - dist]; s->outpos++; }
    }
}
static int dynamic(Inf *s)
{
    static const short ORD[19] = {16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15};
    short len[320]; Huff lc, dc; int nlen, ndist, ncode, i, l, err;
    nlen = (int)bits(s, 5) + 257; ndist = (int)bits(s, 5) + 1; ncode = (int)bits(s, 4) + 4;
    if (s->err || nlen > 286 || ndist > 30) return -1;
    memset(len, 0, sizeof len);
    for (i = 0; i < ncode; i++) len[ORD[i]] = (short)bits(s, 3);
    if (s->err || build(&lc, len, 19) != 0) return -1;      /* the code-length code must be complete */
    for (i = 0; i < nlen + ndist;) {
        int sym = decode(s, &lc), rep;
        if (sym < 0) return -1;
        if (sym < 16) { len[i++] = (short)sym; continue; }
        if (sym == 16) { if (i == 0) return -1; l = len[i - 1]; rep = 3 + (int)bits(s, 2); }
        else { l = 0; rep = sym == 17 ? 3 + (int)bits(s, 3) : 11 + (int)bits(s, 7); }
        if (s->err || i + rep > nlen + ndist) return -1;
        while (rep--) len[i++] = (short)l;
    }
    if (len[256] == 0) return -1;
    /* an incomplete code is allowed only as a single code of length 1 (RFC 1951 3.2.7, as puff) */
    if ((err = build(&lc, len, nlen)) < 0 || (err > 0 && nlen - lc.count[0] != 1)) return -1;
    if ((err = build(&dc, len + nlen, ndist)) < 0 || (err > 0 && ndist - dc.count[0] != 1)) return -1;
    return codes(s, &lc, &dc);
}
static int fixed(Inf *s)
{
    short len[318]; Huff lc, dc; int i;
    for (i = 0; i < 144; i++) len[i] = 8;
    for (; i < 256; i++) len[i] = 9;
    for (; i < 280; i++) len[i] = 7;
    for (; i < 288; i++) len[i] = 8;
    for (; i < 318; i++) len[i] = 5;
    build(&lc, len, 288); build(&dc, len + 288, 30);
    return codes(s, &lc, &dc);
}
int inflate_raw(const unsigned char *in, size_t inlen, unsigned char *out, size_t outlen)
{
    Inf s; int last, r;
    memset(&s, 0, sizeof s); s.in = in; s.inlen = inlen; s.out = out; s.outlen = outlen;
    do {
        unsigned type;
        last = (int)bits(&s, 1); type = bits(&s, 2);
        if (s.err) return -1;
        if (type == 0) {      /* stored: drop the bits up to the byte border, LEN, NLEN */
            size_t n;
            s.bitbuf = 0; s.bitcnt = 0;
            if (s.inlen - s.inpos < 4) return -1;
            n = s.in[s.inpos] | (size_t)s.in[s.inpos + 1] << 8;
            if ((s.in[s.inpos + 2] | s.in[s.inpos + 3] << 8) != (~n & 0xffff)) return -1;
            s.inpos += 4;
            if (n > s.inlen - s.inpos || n > s.outlen - s.outpos) return -1;
            memcpy(s.out + s.outpos, s.in + s.inpos, n); s.inpos += n; s.outpos += n; r = 0;
        } else if (type == 1) r = fixed(&s);
        else if (type == 2) r = dynamic(&s);
        else return -1;
        if (r || s.err) return -1;
    } while (!last);
    return s.outpos == outlen ? 0 : -1;
}

unsigned long crc32_buf(const unsigned char *p, size_t n)
{
    static unsigned long t[256]; unsigned long c; size_t i; int k;
    if (!t[1]) for (i = 0; i < 256; i++) { for (c = (unsigned long)i, k = 0; k < 8; k++) c = c & 1 ? 0xedb88320UL ^ (c >> 1) : c >> 1; t[i] = c; }
    for (c = 0xffffffffUL, i = 0; i < n; i++) c = t[(c ^ p[i]) & 0xff] ^ (c >> 8);
    return c ^ 0xffffffffUL;
}

unsigned char *zip_get(const ZipEntry *e)
{
    unsigned char *o; int bad;
    if (e->usize > 64UL * 1024 * 1024 || (e->method != 0 && e->method != 8)) return NULL;
    if (!(o = (unsigned char *)malloc(e->usize + 1))) return NULL;
    if (e->method == 0) { bad = e->csize != e->usize; if (!bad) memcpy(o, e->data, e->usize); }      /* stored */
    else bad = inflate_raw(e->data, e->csize, o, e->usize) != 0;
    if (bad || crc32_buf(o, e->usize) != (e->crc & 0xffffffffUL)) { free(o); return NULL; }
    return o;
}

static int plain(const char *s) { for (; *s; s++) if (!(isalnum((unsigned char)*s) || *s == '.' || *s == '_' || *s == '-')) return 0; return 1; }
static int ends(const char *s, const char *x) { size_t a = strlen(s), b = strlen(x); return a >= b && !strcmp(s + a - b, x); }
/* MFLng*.exe, mfl*.dll, *.txt, SHA256SUMS at the top, LICENSES/<x>.txt: never MFL.exe, MFL_original.exe, launch.exe or a game file */
int name_allowed(const char *n)
{
    if (!strncmp(n, "LICENSES/", 9)) return plain(n + 9) && ends(n + 9, ".txt") && n[9] != '.';
    if (!plain(n) || n[0] == '.') return 0;
    return (!strncmp(n, "MFLng", 5) && ends(n, ".exe")) || (!strncmp(n, "mfl", 3) && ends(n, ".dll")) || ends(n, ".txt") || !strcmp(n, "SHA256SUMS");
}
int needs_hash(const char *n) { return ends(n, ".exe") || ends(n, ".dll"); }
