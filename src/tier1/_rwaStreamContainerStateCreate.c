/* _rwaStreamContainerStateCreate (MFL.exe 0x005c0880): RenderWare Audio (Criterion Software), rwa.dll 0.94
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_2452A0.c, function sub_245DC0), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_2417B0 -> _rwaLockedCountCreate; sub_245DC0 -> _rwaStreamContainerStateCreate), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c0880 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern RwBool _rwaLockedCountCreate(volatile RwInt32 *target, RwInt32 value);
rwaContainerCommand *rwaStreamContainerCommandInitialize(rwaContainerCommand *command);
extern void sub_2452A0(rwaDecoder *node, rwaStreamPlayer *player);

/* Sets up a stream container. */
rwaStreamPlayer *
_rwaStreamContainerStateCreate(rwaStreamPlayer *player, rwaNodeSet *set, rwaContainerCommand *command, RwUInt32 a4,
           RwUInt32 a5, void *a6, void *a7, void *a8)
{
    if (!_rwaLockedCountCreate(&player->numPlaying, 0)
        || !_rwaLockedCountCreate(&player->numPending, 0)
        || !_rwaLockedCountCreate(&player->numBusy, 0))
    {
        return NULL;
    }

    set->unk00 = 0;
    set->numNodes = 0;
    set->numEntries = 0;
    rwaStreamContainerCommandInitialize(command);
    player->sources.prev = &player->sources;
    player->sources.next = &player->sources;
    player->nodes.prev = &player->nodes;
    player->nodes.next = &player->nodes;
    player->unk48 = a4;
    player->unk4C = a5;
    player->unk30 = 0;
    player->info = NULL;
    player->decoder = NULL;
    player->unk40 = 0;
    player->unk10 = 0;
    player->callback.func = (void (*)(rwaQueueItem *, rwaDecoder *))sub_2452A0;
    player->callback.data = (rwaDecoder *)player;
    player->mode = 1;
    player->unk34 = 1.0f;
    player->state = 6;
    player->unk44 = 32;
    player->unk50 = a6;
    player->unk54 = a7;
    player->unk58 = a8;

    return player;
}
