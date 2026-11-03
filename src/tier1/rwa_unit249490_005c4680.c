/* rwa_unit249490_005c4680 (MFL.exe 0x005c4680): RenderWare Audio (Criterion Software), neutral (codfh sub_2494F0)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_249490.c, function sub_2494F0), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_2429D0 -> rwa_unit242910_005bcbf0; sub_242C60 -> _rwaWaveDefFormatCopyToWaveFormat; sub_2494F0 -> rwa_unit249490_005c4680), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c4680 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
#include <math.h>
extern RwBool rwa_unit242910_005bcbf0(const RwaWaveFormat *format1, const RwaWaveFormat *format2);
extern void _rwaWaveDefFormatCopyToWaveFormat(const rwaWaveFormatDesc *desc, RwaWaveFormat *format, RwBool init);
extern rwaConverter unk_42FDF0[rwaNUMCONVERTERS];
extern rwaConversionPath unk_507A28[5];
extern RwUInt32 dword_507BA4;
extern void *memcpy(void *, const void *, RwUInt32);
RwUInt32 rwa_unit249490_005c4680(const rwaWaveFormatDesc *from, RwUInt32 fromRate, const rwaWaveFormatDesc *to,
                    RwUInt32 toRate, const rwaConverter **path, const rwaConverter **steps,
                    RwUInt32 maxSteps, RwUInt32 depth, RwUInt32 best);
extern rwaConverter unk_42FDF0[51];

/* Debug _RwaWaveConvFindPath, rwad.dll 0x100AABAD: separate cache counter,
 * validity local and recursive depth updates preserve the original lifetimes. */
RwUInt32
rwa_unit249490_005c4680(const rwaWaveFormatDesc *from, RwUInt32 fromRate, const rwaWaveFormatDesc *to,
           RwUInt32 toRate, const rwaConverter **path, const rwaConverter **steps,
           RwUInt32 maxSteps, RwUInt32 depth, RwUInt32 best)
{
    RwUInt32 i;
    RwUInt32 cacheIndex;

    if (depth == maxSteps)
        return 0;
    if (!depth)
    {
        RwaWaveFormat fromFormat;
        RwaWaveFormat toFormat;
        _rwaWaveDefFormatCopyToWaveFormat(from, &fromFormat, TRUE);
        fromFormat.sampleRate = fromRate;
        _rwaWaveDefFormatCopyToWaveFormat(to, &toFormat, TRUE);
        toFormat.sampleRate = toRate;
        for (cacheIndex = 0; cacheIndex < 5; cacheIndex++)
        {
            rwaConversionPath *cached = &unk_507A28[cacheIndex];
            if (cached->count && rwa_unit242910_005bcbf0(&fromFormat, &cached->from)
                && rwa_unit242910_005bcbf0(&toFormat, &cached->to))
            {
                for (cacheIndex = 0; cacheIndex < cached->count; cacheIndex++)
                    path[cacheIndex] = cached->steps[cacheIndex];
                return cached->count;
            }
        }
        for (i = 0; i < rwaNUMCONVERTERS; i++)
        {
            if (from == unk_42FDF0[i].info->from && to == unk_42FDF0[i].info->to
                && (fromRate == toRate || (fromRate != toRate && (unk_42FDF0[i].info->flags & 1))))
            {
                path[0] = &unk_42FDF0[i];
                return 1;
            }
        }
    }
    for (i = 0; i < rwaNUMCONVERTERS; i++)
    {
        RwBool valid = FALSE;
        if (from == unk_42FDF0[i].info->from)
        {
            RwUInt32 n;
            valid = TRUE;
            for (n = 0; n < depth; n++)
            {
                if (steps[n] == &unk_42FDF0[i])
                {
                    valid = FALSE;
                    break;
                }
            }
        }
        if (valid)
        {
            if (to == unk_42FDF0[i].info->to
                && (fromRate == toRate || (fromRate != toRate && (unk_42FDF0[i].info->flags & 1))))
            {
                steps[depth] = &unk_42FDF0[i];
                memcpy(path, steps, depth * 4 + 4);
                best = 0;
                if (depth == 0)
                    return 1;
                else
                    return depth;
            }
            else
            {
                RwUInt32 rate = fromRate;
                RwUInt32 found;
                steps[depth] = &unk_42FDF0[i];
                if (depth == best)
                    return 0;
                depth++;
                if (unk_42FDF0[i].info->flags & 1)
                    rate = toRate;
                found = rwa_unit249490_005c4680(unk_42FDF0[i].info->to, rate, to, toRate,
                                   path, steps, maxSteps, depth, best);
                depth--;
                if (found > 0)
                    best = found;
            }
        }
    }
    if (best != (RwUInt32)-1)
    {
        if (!depth)
        {
            rwaConversionPath *cached;
            RwUInt32 n;
            best++;
            cached = &unk_507A28[dword_507BA4];
            _rwaWaveDefFormatCopyToWaveFormat(from, &cached->from, TRUE);
            _rwaWaveDefFormatCopyToWaveFormat(to, &cached->to, TRUE);
            cached->count = best;
            cached->from.sampleRate = fromRate;
            cached->to.sampleRate = toRate;
            for (n = 0; n < best; n++)
                cached->steps[n] = path[n];
            /* Xbox does not contain the debug DLL's unused-step clearing loop. */
            if (++dword_507BA4 == 5)
                dword_507BA4 = 0;
        }
        return best;
    }
    else
        return 0;
}
