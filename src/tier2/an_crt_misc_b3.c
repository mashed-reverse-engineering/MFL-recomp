/* B3 (own CRT layer): small functions the libcmt split leaves in the image.
   FUN_004c6660 (0x4c6660): the fast memset the game links next to its archive code (mem_zero calls it): dword stores, then the byte tail.
   gap_5d2bc0 / gap_5d2c40 (0x5d2bc0 / 0x5d2c40): two IMediaSample style methods of the RenderWare Audio DirectShow sample (vtable 0x64c138, slots 16 and 18,
   __stdcall, HRESULT 0); gap_5d2c00 (an_hk_1368.c) is the matching getter of the time pair.
   RwCameraRegisterPluginStream (0x5d5920): stream registration of a camera plugin = rwPluginRegistryAddPluginStream on the camera toolkit list. */
typedef long HRESULT;

/* ---- memset ---- */

/* TIER2: unvalidated 2026-11-04 authority=diffemu */
void *FUN_004c6660(void *dst, int value, unsigned int count)
{
    unsigned int fill;
    unsigned int *word;
    unsigned char *byte;
    unsigned int n;

    if (count == 0)
        return dst;
    fill = (unsigned char)value;
    fill = fill | fill << 8;
    fill = fill | fill << 16;
    word = (unsigned int *)dst;
    for (n = count >> 2; n != 0; n--)
        *word++ = fill;
    byte = (unsigned char *)word;
    for (n = count & 3; n != 0; n--)
        *byte++ = (unsigned char)value;
    return dst;
}

/* ---- RenderWare Audio DirectShow media sample ---- */

typedef struct DsMediaSample DsMediaSample;
typedef struct DsMediaSampleVtbl {
    void *slot[7];
    HRESULT (__stdcall *IsSyncPoint)(DsMediaSample *self);      /* +0x1c */
} DsMediaSampleVtbl;
struct DsMediaSample {
    DsMediaSampleVtbl *vtbl;     /* +0x00 */
    unsigned char pad04[0x10];
    unsigned int time_flags;     /* +0x14: bit 1 = media time pair valid */
    unsigned char pad18[0x08];
    unsigned int flags;          /* +0x20: bit 2 = set while IsSyncPoint answered S_FALSE, bit 6 = discontinuity */
    unsigned char pad24[0x24];
    unsigned int media_start[2]; /* +0x48 */
    unsigned int media_end[2];   /* +0x50 */
};

#define SAMPLE_FLAG_NOT_SYNC 0x04
#define SAMPLE_FLAG_DISCONTINUITY 0x40
#define SAMPLE_MEDIA_TIME_VALID 0x02

/* SetDiscontinuity(on): flag the sample (and remember a non-sync sample), or clear both flags */
/* TIER2: unvalidated 2026-11-04 authority=diffemu */
HRESULT __stdcall gap_5d2bc0(DsMediaSample *sample, int discontinuity)
{
    if (discontinuity) {
        if (sample->vtbl->IsSyncPoint(sample) != 0)
            sample->flags = sample->flags | SAMPLE_FLAG_NOT_SYNC;
        sample->flags = sample->flags | SAMPLE_FLAG_DISCONTINUITY;
        return 0;
    }
    sample->flags = sample->flags & ~(SAMPLE_FLAG_NOT_SYNC | SAMPLE_FLAG_DISCONTINUITY);
    return 0;
}

/* SetMediaTime(start, end): both NULL clears the pair, otherwise both 64 bit times are copied */
/* TIER2: unvalidated 2026-11-04 authority=diffemu */
HRESULT __stdcall gap_5d2c40(DsMediaSample *sample, unsigned int *start, unsigned int *end)
{
    if (start == 0 && end == 0) {
        sample->time_flags = sample->time_flags & ~SAMPLE_MEDIA_TIME_VALID;
        return 0;
    }
    sample->media_start[0] = start[0];
    sample->media_start[1] = start[1];
    sample->media_end[0] = end[0];
    sample->media_end[1] = end[1];
    sample->time_flags = sample->time_flags | SAMPLE_MEDIA_TIME_VALID;
    return 0;
}

/* ---- RenderWare camera plugin stream ---- */

/*0062e534*/ extern int cameraTKList;
extern int __cdecl rwPluginRegistryAddPluginStream(void *registry, int plugin_id, void *read_cb, void *write_cb, void *size_cb);

/* TIER2: unvalidated 2026-11-04 authority=diffemu */
int __cdecl RwCameraRegisterPluginStream(int plugin_id, void *read_cb, void *write_cb, void *size_cb)
{
    return rwPluginRegistryAddPluginStream(&cameraTKList, plugin_id, read_cb, write_cb, size_cb);
}
