/* rwa_unit246d50_005c19d0 (MFL.exe 0x005c19d0): RenderWare Audio (Criterion Software), neutral (codfh sub_246D70)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_246D50.c, function sub_246D70), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_246D70 -> rwa_unit246d50_005c19d0), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c19d0 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
static __forceinline void
initEntry(rwaNodeSetEntry *entry, RwUInt32 channel, RwUInt32 track, RwUInt32 kind)
{
    entry->channel = channel;
    entry->track = track;
    entry->kind = kind;
    entry->value = 1.0f;
}

/* The node set's entries for the tracks' channels: four per channel (kinds
 * 0 to 3, value 1.0); the number of entries, or 0 when they do not fit. */
RwUInt32
rwa_unit246d50_005c19d0(rwaNodeSetEntry *entries, RwUInt32 maxEntries, const rwaStreamInfo *info)
{
    RwUInt32    numChannels = 0;
    RwUInt32    track;
    RwUInt32    n;

    for (track = 0; track < info->noTracks; track++)
    {
        numChannels += info->tracks[track].format->channels;
    }
    if (numChannels * 4 > maxEntries)
    {
        return 0;
    }

    n = 0;
    for (track = 0; track < info->noTracks; track++)
    {
        RwUInt32    channel;

        for (channel = 0; channel < info->tracks[track].format->channels; channel++)
        {
            initEntry(&entries[n * 4 + 0], channel, track, 0);
            initEntry(&entries[n * 4 + 1], channel, track, 1);
            initEntry(&entries[n * 4 + 2], channel, track, 2);
            initEntry(&entries[n * 4 + 3], channel, track, 3);
            n++;
        }
    }

    return n * 4;
}
