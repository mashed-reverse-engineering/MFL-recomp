#include "mfl_tier2.h"
/*00561120*/ extern void RtFSManagerSyncAllFiles();
/*005b6650*/ extern void FUN_005b6650(void);
/*006ae108*/ extern undefined4 g_sound_streams_enabled;
/*006ae10c*/ extern undefined4 g_sound_streams_paused;
/* sound_streams_pause */
/* "RtFSManagerSyncAllFiles (when param is 0) and stops the stream thread" */
/* Pauses the sound streams if they are enabled and not already paused.
 * skip_file_sync: 0 = call RtFSManagerSyncAllFiles() before pausing,
 *                 nonzero = pause without syncing files.
 * Returns 1 in every case (inferred: success flag). */
undefined4 sound_streams_pause(int skip_file_sync)
{
  if ((g_sound_streams_enabled != 0) && (g_sound_streams_paused == 0)) {
    if (skip_file_sync == 0) {
      RtFSManagerSyncAllFiles();
    }
    FUN_005b6650();
    g_sound_streams_paused = 1;
    return 1;
  }
  return 1;
}
