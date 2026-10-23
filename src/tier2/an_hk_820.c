#include "mfl_tier2.h"
/*0040ac30*/ extern int line_exceptions_save();
/*0042d9a0*/ extern int store_u32();
/*0042d9b0*/ extern int rw_stream_chunk_copy();
/*005543d0*/ extern int rw_first_field(int * p0);
/*00651ac0*/ extern undefined4 g_line_exceptions;
/*00651ac4*/ extern undefined4 g_gamesave_651ac4;
/*00651ac8*/ extern undefined g_camera_pick_651ac8;
/*00652d00*/ extern undefined4 g_camera_pick_652d00;
/*0068b8f8*/ extern char g_race_phase_arg[];

#define CHUNK_CAMERA_PICK_INDEX (0xc)
#define CHUNK_LINE_EXCEPTION (0xd)
#define CHUNK_GAMESAVE_ENTRY (0xe)
#define CHUNK_CAMERA_PICK_ENTRY (0xf)
#define CHUNK_LINE_EXCEPTIONS_SAVE (0x10)
#define PICK_RECORD_WORDS (3)
#define PICK_RECORD_BYTES (0xc)
/* course_editor_read_chunk */

/* course_editor_read_chunk: reads the next chunk id from the race-phase stream (g_race_phase_arg)
   and dispatches on it. Ids 0xC-0xF each read one 32-bit value from the stream and store it in
   the table entry for the current camera pick (g_camera_pick_652d00); id 0x10 saves the line
   exceptions instead. Unknown ids are ignored. No parameters, no return value. */

/* "switch on the chunk id from the 0x68b8f8 stream (0xC,0xD,0xE ...) storing to camera pick / line
   exceptions" */

void course_editor_read_chunk(void)

{
  undefined4 chunk_id;
  undefined4 chunk_value;

  chunk_id = rw_first_field((int *)(((char *)&g_race_phase_arg)));
  switch(chunk_id) {
  /* 0xC: set the current camera pick index. */
  case CHUNK_CAMERA_PICK_INDEX:
    rw_stream_chunk_copy(((char *)&g_race_phase_arg),&chunk_value);
    store_u32(((char *)&g_race_phase_arg),0);
    g_camera_pick_652d00 = chunk_value;
    return;
  /* 0xD: line exception value for the current pick (PICK_RECORD_WORDS words per pick). */
  case CHUNK_LINE_EXCEPTION:
    rw_stream_chunk_copy(((char *)&g_race_phase_arg),&chunk_value);
    store_u32(((char *)&g_race_phase_arg),0);
    (&g_line_exceptions)[g_camera_pick_652d00 * PICK_RECORD_WORDS] = chunk_value;
    return;
  /* 0xE: gamesave value for the current pick (same layout as 0xD, table g_gamesave_651ac4). */
  case CHUNK_GAMESAVE_ENTRY:
    rw_stream_chunk_copy(((char *)&g_race_phase_arg),&chunk_value);
    store_u32(((char *)&g_race_phase_arg),0);
    (&g_gamesave_651ac4)[g_camera_pick_652d00 * PICK_RECORD_WORDS] = chunk_value;
    return;
  /* 0xF: camera pick value for the current pick (byte table g_camera_pick_651ac8, PICK_RECORD_BYTES
     per pick). Breaks out to the final return. */
  case CHUNK_CAMERA_PICK_ENTRY:
    rw_stream_chunk_copy(((char *)&g_race_phase_arg),&chunk_value);
    store_u32(((char *)&g_race_phase_arg),0);
    *(undefined4 *)((char *)&g_camera_pick_651ac8 + g_camera_pick_652d00 * PICK_RECORD_BYTES) = chunk_value;
    break;
  /* 0x10: no value in the stream; save the line exceptions. */
  case CHUNK_LINE_EXCEPTIONS_SAVE:
    store_u32(((char *)&g_race_phase_arg),0);
    line_exceptions_save();
    return;
  }
  return;
}


