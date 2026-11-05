/* B3 (own CRT layer): memcpy (0x4bbe90) and memmove (0x4bdba0).  libcmt builds both from the same memcpy.asm, so the exe holds two identical
   copies and both handle overlapping blocks (copy backwards when the destination starts inside the source).  The two entry points share the
   body below (a macro: tier 2 objects are built without inlining, and the verifier runs each entry point on its own). */
#pragma function(memcpy, memmove)

#define COPY_BLOCK(dst, src, count)                                                          \
    {                                                                                        \
        unsigned char *d = (unsigned char *)(dst);                                           \
        const unsigned char *s = (const unsigned char *)(src);                               \
        if (d <= s || d >= s + (count)) {                                                    \
            /* forward: whole dwords, then the 0-3 byte tail */                              \
            for (; (count) >= 4; (count) -= 4, d += 4, s += 4)                               \
                *(unsigned int *)d = *(const unsigned int *)s;                               \
            for (; (count) != 0; (count)--)                                                  \
                *d++ = *s++;                                                                 \
        } else {                                                                             \
            /* the destination overlaps the end of the source: copy from the top down */     \
            d += (count);                                                                    \
            s += (count);                                                                    \
            for (; (count) >= 4; (count) -= 4) {                                             \
                d -= 4;                                                                      \
                s -= 4;                                                                      \
                *(unsigned int *)d = *(const unsigned int *)s;                               \
            }                                                                                \
            for (; (count) != 0; (count)--)                                                  \
                *--d = *--s;                                                                 \
        }                                                                                    \
    }

/* TIER2: unvalidated 2026-11-04 authority=diffemu */
void *__cdecl memcpy(void *dst, const void *src, unsigned int count)
{
    COPY_BLOCK(dst, src, count);
    return dst;
}

/* TIER2: unvalidated 2026-11-04 authority=diffemu */
void *__cdecl memmove(void *dst, const void *src, unsigned int count)
{
    COPY_BLOCK(dst, src, count);
    return dst;
}
