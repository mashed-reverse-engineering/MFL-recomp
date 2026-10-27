#include "mfl_tier2.h"
/*0044aa50*/ extern undefined __cdecl camera_chase_update(void *p0, undefined4 p1);
/*0044b110*/ extern undefined __cdecl camera_follow_update(void *p0, undefined4 p1);
/*0044ef90*/ extern undefined __cdecl camera_record_update(void *p0, undefined4 p1);
/*008c5940*/ extern undefined4 g_camera_records_8c5940;
/*008c7e18*/ extern int g_camera_records_8c7e18;

/* dwords per camera record (0x36 * 4 = 216 bytes) */
#define CAMERA_RECORD_DWORDS 0x36

/* mode stored in dword 1 of each record */
enum {
  CAMERA_MODE_RECORD = 0,              /* camera_record_update */
  CAMERA_MODE_CHASE = 1,               /* camera_chase_update */
  CAMERA_MODE_FOLLOW = 2               /* camera_follow_update */
};


/* camera_records_update_all */

/* "calls camera_record_update for every active record" */

/* Runs the update routine that matches each active camera record's mode. */
void __cdecl camera_records_update_all(undefined4 update_arg)
{
  int record_index;
  undefined4 *camera_record;

  record_index = 0;
  if (0 < g_camera_records_8c7e18) {                    /* number of active records */
    camera_record = &g_camera_records_8c5940;
    do {
      if (camera_record[1] == CAMERA_MODE_RECORD) {
        camera_record_update(camera_record,update_arg);
      }
      if (camera_record[1] == CAMERA_MODE_CHASE) {
        camera_chase_update(camera_record,update_arg);
      }
      if (camera_record[1] == CAMERA_MODE_FOLLOW) {
        camera_follow_update(camera_record,update_arg);
      }
      record_index = record_index + 1;
      camera_record = camera_record + CAMERA_RECORD_DWORDS;
    } while (record_index < g_camera_records_8c7e18);
  }
  return;
}
