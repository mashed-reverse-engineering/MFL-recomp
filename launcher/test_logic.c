/* test_logic.c <releases.json> <MFLng-<tag>.zip> <tag of that zip>: native check of logic.c (build.sh test).
   releases.json = a saved answer of the GitHub releases API; the zip = the real release asset of <tag>. */
#include "logic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int fails;
#define CHECK(c) do { if (!(c)) { printf("FAIL line %d: %s\n", __LINE__, #c); fails++; } } while (0)

static char *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); char *b; long l;
    if (!f) { printf("cannot open %s\n", p); exit(2); }
    fseek(f, 0, SEEK_END); l = ftell(f); fseek(f, 0, SEEK_SET);
    b = (char *)malloc((size_t)l + 1); if (fread(b, 1, (size_t)l, f) != (size_t)l) exit(2); b[l] = 0; fclose(f);
    if (n) *n = (size_t)l;
    return b;
}

int main(int argc, char **argv)
{
    unsigned char h[32]; char hx[65], want[65]; Release r[40]; Installed in; ZipEntry ze[64];
    int n, i, nz, hashed = 0; size_t zn; unsigned char *z; char *sums = NULL, *bi = NULL, *js;
    if (argc != 4 && argc != 5) { printf("usage: test_logic <releases.json> <zip> <tag> [deflate vectors]\n"); return 2; }

    if (argc == 5) {      /* raw DEFLATE vectors (agents/launch_w16/mkvectors.py): exact output, then corrupted input must fail cleanly (ASan build) */
        size_t vn, pos = 0; unsigned char *v = (unsigned char *)slurp(argv[4], &vn); int nv = 0, bad = 0, fuzz = 0; unsigned seed = 12345;
        while (pos + 8 <= vn) {
            size_t rl = v[pos] | v[pos + 1] << 8 | (size_t)v[pos + 2] << 16 | (size_t)v[pos + 3] << 24, cl = v[pos + 4] | v[pos + 5] << 8 | (size_t)v[pos + 6] << 16 | (size_t)v[pos + 7] << 24;
            const unsigned char *raw = v + pos + 8, *cz = raw + rl; unsigned char *o = (unsigned char *)malloc(rl + 1), *c = (unsigned char *)malloc(cl + 1); int k;
            if (inflate_raw(cz, cl, o, rl) != 0 || memcmp(o, raw, rl)) bad++;
            if (rl && inflate_raw(cz, cl, o, rl - 1) == 0) bad++;      /* output one byte too small */
            if (inflate_raw(cz, cl ? cl - 1 : 0, o, rl) == 0 && cl > 1) bad++;      /* truncated input */
            for (k = 0; k < 40 && cl; k++) {      /* flipped bytes: any result, but no crash / overrun */
                memcpy(c, cz, cl); seed = seed * 1103515245u + 12345u; c[(seed >> 8) % cl] ^= (unsigned char)(1 + (seed >> 24) % 255);
                inflate_raw(c, cl, o, rl); fuzz++;
            }
            free(o); free(c); pos += 8 + rl + cl; nv++;
        }
        { unsigned char g[4096], o[8192]; int k; for (k = 0; k < 2000; k++) { size_t j; for (j = 0; j < sizeof g; j++) { seed = seed * 1103515245u + 12345u; g[j] = (unsigned char)(seed >> 16); }
            inflate_raw(g, (size_t)(k % 4096), o, (size_t)(k * 7 % 8192)); fuzz++; } }
        CHECK(nv == 200 && bad == 0);
        CHECK(crc32_buf((const unsigned char *)"123456789", 9) == 0xcbf43926UL);
        printf("deflate vectors: %d, failures %d, corrupted / random inputs survived: %d\n", nv, bad, fuzz);
        free(v);
    }

    sha256((const unsigned char *)"abc", 3, h); hex32(h, hx);
    CHECK(!strcmp(hx, "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"));
    sha256((const unsigned char *)"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 56, h); hex32(h, hx);
    CHECK(!strcmp(hx, "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1"));
    sha256((const unsigned char *)"", 0, h); hex32(h, hx);
    CHECK(!strcmp(hx, "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"));

    /* the real API answer */
    js = slurp(argv[1], NULL); n = parse_releases(js, r, 40); free(js);
    printf("releases: %d, first %s pre=%d url=%s digest=%.12s...\n", n, n > 0 ? r[0].tag : "-", n > 0 ? r[0].pre : -1, n > 0 ? r[0].url : "-", n > 0 ? r[0].digest : "-");
    CHECK(n >= 1 && !strcmp(r[0].tag, argv[3]) && r[0].body && strlen(r[0].body) > 20 && strlen(r[0].digest) == 64);
    CHECK(strstr(r[0].url, "/MFLng-") && !strstr(r[0].url, "maps"));
    for (i = 0; i < n; i++) CHECK(r[i].pre);      /* every release so far is a pre-release (release.yml) */
    CHECK(pick_newest(r, n, 0) == NULL);         /* channel release: nothing to offer */
    CHECK(pick_newest(r, n, 1) == &r[0]);        /* channel prerelease: the newest of all */
    z = (unsigned char *)slurp(argv[2], &zn);    /* the downloaded zip against the API's asset digest */
    sha256(z, zn, h); hex32(h, hx); CHECK(!strcmp(hx, r[0].digest));
    printf("zip sha256 %s the API digest\n", strcmp(hx, r[0].digest) ? "DIFFERS from" : "equals");
    free_releases(r, n);

    /* JSON edge cases */
    CHECK(parse_releases("{\"a\":1}", r, 4) == -1);
    n = parse_releases("[{\"tag_name\":\"r20990101-abcdef0\",\"draft\":false,\"prerelease\":false,\"body\":\"x \\\"tag_name\\\":\\\"bad\\\" \\u00e9\\n\","
                       "\"assets\":[{\"name\":\"MFLng-r20990101-abcdef0-maps.zip\",\"browser_download_url\":\"https://h/MFLng-r20990101-abcdef0-maps.zip\"},"
                       "{\"name\":\"MFLng-r20990101-abcdef0.zip\",\"digest\":null,\"browser_download_url\":\"https://h/MFLng-r20990101-abcdef0.zip\"}]},"
                       "{\"tag_name\":\"r20980101-0000000\",\"draft\":true,\"prerelease\":false,\"assets\":[]}]", r, 4);
    CHECK(n == 2 && !strcmp(r[0].tag, "r20990101-abcdef0") && !r[0].pre && !strcmp(r[0].body, "x \"tag_name\":\"bad\" \xc3\xa9\n"));
    CHECK(!strcmp(r[0].url, "https://h/MFLng-r20990101-abcdef0.zip") && r[0].digest[0] == 0 && r[1].draft);
    CHECK(pick_newest(r, n, 0) == &r[0]);
    free_releases(r, n);
    CHECK(parse_releases("[{\"tag_name\":\"r1\",\"body\":\"unterminated", r, 4) == 0);

    /* tag and channel rules */
    parse_buildinfo("cl 19.44\nrelease r20261120-e2ddfb7 prerelease\r\n", &in);
    CHECK(!strcmp(in.tag, "r20261120-e2ddfb7") && in.pre && default_channel_pre(&in));
    CHECK(update_state(&in, "r20261120-e2ddfb7") == ST_UPTODATE);
    CHECK(update_state(&in, "r20261121-1234567") == ST_UPDATE);
    CHECK(update_state(&in, "r20261119-1234567") == ST_NEWER_INSTALLED);
    CHECK(update_state(&in, "r20261120-cf82fc4") == ST_UPDATE);      /* same day, other build: offer the API's newest */
    parse_buildinfo("release r20261201-1111111\n", &in);
    CHECK(!in.pre && !default_channel_pre(&in));
    parse_buildinfo(NULL, &in);
    CHECK(!in.tag[0] && !default_channel_pre(&in) && update_state(&in, "r20261120-e2ddfb7") == ST_UPDATE);

    /* the real release zip */
    nz =zip_list(z, zn, ze, 64);
    CHECK(nz > 5);
    for (i = 0; i < nz; i++)      /* zip_get leaves room for a terminator */
        if (!strcmp(ze[i].name, "SHA256SUMS") && (sums = (char *)zip_get(&ze[i]))) sums[ze[i].usize] = 0;
        else if (!strcmp(ze[i].name, "BUILDINFO.txt") && (bi = (char *)zip_get(&ze[i]))) bi[ze[i].usize] = 0;
    CHECK(sums && bi);
    if (!sums || !bi) return 1;
    for (i = 0; i < nz; i++) {
        unsigned char *d; int listed, ok;
        if (!name_allowed(ze[i].name)) { printf("  skip %s (not an install file)\n", ze[i].name); CHECK(ze[i].name[strlen(ze[i].name) - 1] == '/'); continue; }
        d = zip_get(&ze[i]); CHECK(d != NULL); if (!d) continue;
        sha256(d, ze[i].usize, h); hex32(h, hx);
        listed = sums_lookup(sums, ze[i].name, want); ok = !listed || !strcmp(hx, want);
        if (listed) hashed++;
        CHECK(ok); CHECK(listed || !needs_hash(ze[i].name));
        printf("  %-28s %8lu B %s\n", ze[i].name, (unsigned long)ze[i].usize, listed ? (ok ? "sha256 OK" : "sha256 MISMATCH") : "(not in SHA256SUMS)");
        free(d);
    }
    CHECK(hashed >= 4);
    parse_buildinfo(bi, &in);
    /* this zip predates the `release` line: treated like any build without one (no legacy matching) */
    CHECK(!in.tag[0] && update_state(&in, argv[3]) == ST_UPDATE && !default_channel_pre(&in));
    printf("zip BUILDINFO: tag '%s' -> default channel %s\n", in.tag, default_channel_pre(&in) ? "prerelease" : "release");
    for (i = 0; i < nz; i++) if (needs_hash(ze[i].name) && ze[i].csize > 100) {      /* a flipped byte must fail (crc or inflate) */
        unsigned char *d, keep = ze[i].data[50]; ((unsigned char *)ze[i].data)[50] ^= 0x55;
        d = zip_get(&ze[i]); CHECK(d == NULL); free(d); ((unsigned char *)ze[i].data)[50] = keep; break;
    }
    CHECK(zip_list(z, 100, ze, 64) == -1);

    CHECK(name_allowed("MFLng.exe") && name_allowed("MFLng.nofix.exe") && name_allowed("MFLng-launch.exe") && name_allowed("mflrw.dll") && name_allowed("LICENSES/zlib.txt"));
    CHECK(!name_allowed("MFL.exe") && !name_allowed("MFL_original.exe") && !name_allowed("launch.exe") && !name_allowed("../MFLng.exe"));
    CHECK(!name_allowed("LICENSES/../x.txt") && !name_allowed("a/b.txt") && !name_allowed("LICENSES/") && !name_allowed("videocfg.bin") && !name_allowed("C:\\x.txt"));
    CHECK(!sums_lookup(sums, "MFLng-launch.exe", want));      /* the r20261120 zip predates the launcher */

    free(z); free(sums); free(bi);
    printf(fails ? "test_logic: %d FAILED\n" : "test_logic: all passed\n", fails);
    return fails != 0;
}
