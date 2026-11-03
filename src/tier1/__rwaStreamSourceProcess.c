/* __rwaStreamSourceProcess and its static helpers: RenderWare Audio (Criterion Software)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_247430.c, functions sub_2475D0 (0x005c2240, static), sub_248410 (0x005c30f0)), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; these functions stay in one file because cl 13.00 compiles a static function and its callers in the same
 * file with a register calling convention. Changes: function names (sub_244F70 -> rwa_unit2449c0_005bbd70; sub_2475D0 -> rwa_unit247430_005c2240; sub_248410 -> __rwaStreamSourceProcess; sub_24D5B0 -> _rwaStreamFileRequestQueueGet), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe (tools/cmpfn.py, relocations masked): rwa_unit247430_005c2240 0x005c2240, __rwaStreamSourceProcess 0x005c30f0. Static functions keep their linkage: the
 * overlay replaces only the external ones, the static copies serve the callers in this file. */
#include "rwa.h"
#include "rwaint.h"
extern void sub_2449E0(rwaCacheClient *client, RwUInt32 offset, rwaCacheCallBack callback,
                       void *data);
extern void rwa_unit2449c0_005bbd70(rwaCacheClient *client, void *dest, RwUInt32 size,
                       rwaCacheCallBack callback, void *data);
extern RwBool _rwaStreamFileRequestQueueGet(rwaReadCache *cache, RwUInt32 *out);
void sub_248810(void *file, RwUInt32 bytes, RwInt32 status, RwInt32 event, void *data);
extern RwUInt8 * sub_247660(rwaDecoderState *state);
extern void sub_248810(void *, RwUInt32, RwInt32, RwInt32, void *);
extern RwUInt8 *sub_247660(rwaDecoderState *);
extern void sub_248810(void *file, RwUInt32 bytes, RwInt32 status, RwInt32 event, void *data);

/* MFL.exe 0x005c2240 */
static void
rwa_unit247430_005c2240(rwaDecoderState *state)
{
    if (!(state->flags & 0x2FF8))
    {
        RwUInt32    requests = state->requests;

        if (requests & 4)
        {
            state->requests = requests & ~4;
            state->flags |= 8;
            rwa_unit2449c0_005bbd70(&state->client, (void *)state->unkDC, state->unkE0,
                       (rwaCacheCallBack)sub_248810, state);
        }
        else if (requests & 1)
        {
            state->requests = requests & ~1;
            state->flags |= 0x20;
            sub_2449E0(&state->client, state->unkE4, (rwaCacheCallBack)sub_248810, state);
        }
    }
}

/* Copies the look-ahead chunks into the playing chunks, track by track and
 * channel by channel (the samples of a track's channels are interleaved in
 * units of its channel step), until a playing chunk is filled. */
/* MFL.exe 0x005c30f0 */
void
__rwaStreamSourceProcess(rwaDecoderState *state)
{
    rwaStreamRegion *regions;

    if ((state->cache.unk06 != state->cache.unk08 || state->cache.unk0A)
        && !(state->flags & 0x22FF8) /* flag mask, not an address */
        && _rwaStreamFileRequestQueueGet(&state->cache, &state->unkD8))
    {
        rwa_unit247430_005c2240(state);
    }

    regions = state->regions;
    if (!(regions[state->unk15C].flags & 1) && state->aheadBuffer)
    {
        rwaStreamRegion *entries = state->entries;

        if (!(entries[state->unk174].flags & 6))
        {
            /* Evidence: 0x2484AC reloads the look-ahead index after the
             * preceding flag test. Qualify this access to preserve that
             * reload; this does not establish the original declaration. */
            if (regions[state->unk154].unk0C || entries[((volatile rwaDecoderState *)state)->unk174].unk0C
                || ((regions[state->unk154].flags & 0x10) && state->loops != -1))
            {
                state->flags |= 0x10000;
                sub_247660(state);
            }
            else if (entries[state->entry].flags == 1)
            {
                state->flags &= ~0x10000;
            }
        }
    }
}
