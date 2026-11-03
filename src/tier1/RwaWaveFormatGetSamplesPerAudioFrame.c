/* RwaWaveFormatGetSamplesPerAudioFrame (MFL.exe 0x005bd090): RenderWare Audio (Criterion Software), rwa.dll 0.94
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_242910.c, function sub_242E30), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_242A30 -> RwaWaveFormatGetFrameSize; sub_242E30 -> RwaWaveFormatGetSamplesPerAudioFrame), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bd090 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern const RwaUUID unk_3ED61C;
extern const RwaUUID unk_3ED65C;
extern const RwaUUID unk_3ED5FC;
extern const RwaUUID unk_3ED5EC;
extern const RwaUUID unk_3ED5DC;
extern const RwaUUID unk_3ED5BC;
extern const RwaUUID unk_3ED5CC;
extern RwUInt32 RwaWaveFormatGetFrameSize(const RwaWaveFormat *format);
extern const RwaUUID unk_3ED63C;
extern const RwaUUID unk_3ED62C;
extern const RwaUUID unk_3ED64C;

RwUInt32
RwaWaveFormatGetSamplesPerAudioFrame(const RwaWaveFormat *format)
{
    if (!RwaUUIDCompare(format->dataType, &unk_3ED5CC))
    {
        return 1;
    }

    if (!RwaUUIDCompare(format->dataType, &unk_3ED5BC))
    {
        return 0x1C;
    }

    if (!RwaUUIDCompare(format->dataType, &unk_3ED5DC))
    {
        return 1;
    }

    if (!RwaUUIDCompare(format->dataType, &unk_3ED64C))
    {
        return 0x600;
    }

    if (!RwaUUIDCompare(format->dataType, &unk_3ED5EC))
    {
        return 0xE;
    }

    if (!RwaUUIDCompare(format->dataType, &unk_3ED5FC) ||
        !RwaUUIDCompare(format->dataType, &unk_3ED65C))
    {
        RwaWaveFormat   mono;

        if (format->noChannels == 1)
        {
            return 0x40;
        }

        mono = *format;
        mono.noChannels = 1;

        return RwaWaveFormatGetSamplesPerAudioFrame(&mono) / (RwaWaveFormatGetFrameSize(&mono) / RwaWaveFormatGetFrameSize(format));
    }

    if (!RwaUUIDCompare(format->dataType, &unk_3ED61C) ||
        !RwaUUIDCompare(format->dataType, &unk_3ED62C))
    {
        return 0x480;
    }

    return RwaUUIDCompare(format->dataType, &unk_3ED63C) ? 0x180 : 0;
}
