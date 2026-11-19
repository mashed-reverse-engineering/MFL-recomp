/* mfl_audio_rng.h: the second random stream for sound code (the DETERMINISTIC_AUDIO_RNG switch of mfl_fix.h, default off; output/determinism_2026-11-16.md).
 * One source for both users: ours (recomp, MSVC; included only by tier1/random_range.c under the switch) and the test shim for the control
 * (tools/d3dtrace/d3d9.c, mingw, audiorng=1).  No switch test here: the header is switch-free.
 * The generator is RenderWare's RpRandom, transcribed from MFL.exe 0x544e00 (seed) / 0x544e80 (draw): 31 words, additive lagged
 * t[a] += t[b], result t[a] >> 1, both indices advance by one and wrap at 31; the seed fills t[i] = t[i-1] * 0x41c64e6d + 0x3039 from
 * t[0] = seed, starts a = 3, b = 0 and discards 310 draws.  Integer only, so every compiler gives the same numbers.
 * The stream is seeded on its first draw with MFL_AUDIO_RNG_SEED, so it depends only on how many sound draws were made, never on time. */
#ifndef MFL_AUDIO_RNG_H
#define MFL_AUDIO_RNG_H

#define MFL_AUDIO_RNG_SEED 0x5a17a5ed      /* any constant; the same in ours and in the shim */
#define MFL_AUDIO_RNG_N 31

typedef struct MflAudioRng { unsigned int t[MFL_AUDIO_RNG_N]; int a, b, seeded; } MflAudioRng;

static unsigned int mfl_audio_rng_step(MflAudioRng *g)
{
    unsigned int r;
    g->t[g->a] += g->t[g->b];
    r = g->t[g->a] >> 1;
    if (++g->a >= MFL_AUDIO_RNG_N) g->a = 0;
    if (++g->b >= MFL_AUDIO_RNG_N) g->b = 0;
    return r;
}

static void mfl_audio_rng_seed(MflAudioRng *g, unsigned int seed)
{
    int i;
    g->t[0] = seed;
    for (i = 1; i < MFL_AUDIO_RNG_N; i++) g->t[i] = g->t[i - 1] * 0x41c64e6du + 0x3039u;
    g->a = 3;
    g->b = 0;
    for (i = 0; i < 0x136; i++) mfl_audio_rng_step(g);
    g->seeded = 1;
}

static unsigned int mfl_audio_rng_next(MflAudioRng *g)
{
    if (!g->seeded) mfl_audio_rng_seed(g, MFL_AUDIO_RNG_SEED);
    return mfl_audio_rng_step(g);
}

#endif
