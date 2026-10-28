#include "mfl_tier2.h"
typedef int CourseId;
/*004063b0*/ extern int camera_path_get_point();
/*0042b110*/ extern int current_course_id();
/*0044a840*/ extern int camera_set_from_record();
/*00481fd0*/ extern float random_range();
/*004b2b48*/ extern int _ftol();
/*0060af58*/ extern undefined g_camera_records_60af58;
/*0060b0e0*/ extern undefined g_camera_records_60b0e0;
/*0060b268*/ extern undefined g_camera_records_60b268;
/*0060b3f0*/ extern undefined g_camera_records_60b3f0;
/*0060b5a8*/ extern undefined4 g_camera_records_60b5a8[0x1000];
/*0060b730*/ extern undefined g_camera_records_60b730;
/*0060b9a8*/ extern undefined g_camera_records_60b9a8;
/*0060bb90*/ extern undefined g_camera_records_60bb90;
/*0060bd48*/ extern undefined g_camera_records_60bd48;
/*0060bfc0*/ extern undefined g_camera_records_60bfc0;
/*0060c1a8*/ extern undefined g_camera_records_60c1a8;
/*0060c4b0*/ extern undefined g_camera_records_60c4b0;
/*0060c698*/ extern undefined g_camera_records_60c698;
/*0060c8e0*/ extern undefined g_camera_records_60c8e0;
/*0060cac8*/ extern undefined g_camera_records_60cac8;
/*0060ccb0*/ extern undefined g_camera_records_60ccb0;
/*0060ce98*/ extern undefined g_camera_records_60ce98;
/*0060cff0*/ extern undefined g_camera_records_60cff0;
/*0060d328*/ extern undefined g_camera_records_60d328;
/*0060d4b0*/ extern undefined g_camera_records_60d4b0;
/*008c592c*/ extern undefined4 g_car_8c592c;
/*008c5930*/ extern undefined4 g_camera_8c5930;
/*008c5934*/ extern undefined4 g_car_8c5934;
/*008c5940*/ extern undefined4 g_camera_records_8c5940[0x1000];
/*008c7440*/ extern undefined4 g_car_8c7440;
/*008c7e10*/ extern undefined4 g_camera_records_8c7e10;
/*008c7e18*/ extern int g_camera_records_8c7e18;
/*008c7e1c*/ extern undefined4 g_camera_8c7e1c;
/*008c7e20*/ extern undefined4 g_camera_8c7e20;
/* camera_records_init_for_course */
#define COURSE_ANGELS (1)
#define COURSE_ROUABOUT (2)
#define COURSE_EGYPT (3)
#define COURSE_EGYPT_R (4)
#define COURSE_NEUST_R (5)
#define COURSE_TIERRA_N (6)
#define COURSE_SUPERG_N (8)
#define COURSE_FOREST_R (9)
#define COURSE_SANDS (11)
#define COURSE_HIGHWAYN (12)
#define COURSE_SANDSNOW (13)
#define COURSE_STORM_R (15)
#define COURSE_NT1 (17)
#define COURSE_DEMR (18)
#define COURSE_NT1_S (20)
#define COURSE_DEM (24)
#define COURSE_SUPERG (25)
#define COURSE_CITY (26)
#define COURSE_TRAINING (30)
#define COURSE_WARZONE (33)
#define COURSE_FOREST (34)
#define COURSE_STORM (36)
#define COURSE_DUMP (37)
#define COURSE_HIGHWAY (38)
#define COURSE_NEUSTEIN (39)
#define CAMERA_SLOT_WORDS (0x36)
#define PATH_RECORD_WORDS (0xc)
#define PATH_TAG_OFFSET (7)
#define CAMERA_INDEX_UNSET (-1)
#define PATH_TAG_2_0F (0x40000000)
#define PATH_TAG_3_0F (0x40400000)
#define RANDOM_MAX_7_0F (0x40e00000)

/* "per course camera path tables (switch over current_course_id)" */

/* Loads the camera path table for the current course, once (guarded by g_camera_records_8c7e10).
   Each record in the table is passed to camera_set_from_record() into g_camera_records_8c5940.
   The first record tagged 1.0f / 2.0f / 3.0f sets g_camera_8c5930 / g_camera_8c7e20 / g_camera_8c7e1c.
   g_car_8c592c and g_car_8c5934 are overwritten on every match, so they hold the LAST matching index.
   Then stores a random value in g_car_8c7440 (purpose inferred). Takes no parameters, returns nothing. */
void camera_records_init_for_course(void)

{
  CourseId course_id;
  int record_count;
  undefined4 *dst_record;
  undefined4 *path_table;
  int *record_tag;
  int record_index;

  if (g_camera_records_8c7e10 != 1) {
    g_camera_records_8c7e10 = 1;
    course_id = current_course_id();
    /* pick the path table for this course; default covers unlisted courses */
    switch(course_id) {
    case COURSE_ANGELS:
    case COURSE_TRAINING:
      path_table = (undefined4 *)&g_camera_records_60ce98;
      break;
    case COURSE_ROUABOUT:
    case COURSE_TIERRA_N:
      path_table = (undefined4 *)&g_camera_records_60d328;
      break;
    case COURSE_EGYPT:
      path_table = (undefined4 *)&g_camera_records_60b9a8;
      break;
    case COURSE_EGYPT_R:
      path_table = (undefined4 *)&g_camera_records_60bb90;
      break;
    case COURSE_NEUST_R:
      path_table = (undefined4 *)&g_camera_records_60b268;
      break;
    default:
      path_table = (undefined4 *)&g_camera_records_60bfc0;
      break;
    case COURSE_SUPERG_N:
    case COURSE_SUPERG:
      path_table = (undefined4 *)&g_camera_records_60c8e0;
      break;
    case COURSE_FOREST_R:
      path_table = (undefined4 *)&g_camera_records_60c698;
      break;
    case COURSE_SANDS:
    case COURSE_SANDSNOW:
      path_table = (undefined4 *)&g_camera_records_60d4b0;
      break;
    case COURSE_HIGHWAYN:
    case COURSE_HIGHWAY:
      path_table = (undefined4 *)&g_camera_records_60bd48;
      break;
    case COURSE_STORM_R:
      path_table = (undefined4 *)&g_camera_records_60ccb0;
      break;
    case COURSE_NT1:
    case COURSE_NT1_S:
      path_table = (undefined4 *)&g_camera_records_60b3f0;
      break;
    case COURSE_DEMR:
      path_table = (undefined4 *)&g_camera_records_60af58;
      break;
    case COURSE_DEM:
      path_table = (undefined4 *)&g_camera_records_60b0e0;
      break;
    case COURSE_CITY:
      path_table = (undefined4 *)&g_camera_records_60c1a8;
      break;
    case COURSE_WARZONE:
      path_table = (undefined4 *)&g_camera_records_60b730;
      break;
    case COURSE_FOREST:
      path_table = (undefined4 *)&g_camera_records_60c4b0;
      break;
    case COURSE_STORM:
      path_table = (undefined4 *)&g_camera_records_60cac8;
      break;
    case COURSE_DUMP:
      path_table = (undefined4 *)&g_camera_records_60cff0;
      break;
    case COURSE_NEUSTEIN:
      path_table = g_camera_records_60b5a8;
    }
    camera_path_get_point(path_table);
    /* reset the three marker indices before scanning */
    g_camera_8c5930 = CAMERA_INDEX_UNSET;
    g_camera_8c7e20 = CAMERA_INDEX_UNSET;
    g_camera_8c7e1c = CAMERA_INDEX_UNSET;
    /* first word of the table is the record count (float bits) */
    record_count = ((int)(*(float *)path_table));
    record_index = 0;
    g_camera_records_8c7e18 = record_count;
    if (0 < record_count) {
      /* loop: one camera slot per record; tag word sits PATH_TAG_OFFSET words into each record */
      dst_record = g_camera_records_8c5940;
      record_tag = path_table + 8;
      do {
        camera_set_from_record(dst_record,record_tag + -PATH_TAG_OFFSET);
        if ((*record_tag == FLOAT_ONE_BITS) && (g_car_8c592c = record_index, g_camera_8c5930 == CAMERA_INDEX_UNSET)) {
          g_camera_8c5930 = record_index;
        }
        if ((*record_tag == PATH_TAG_2_0F) && (g_car_8c5934 = record_index, g_camera_8c7e20 == CAMERA_INDEX_UNSET)) {
          g_camera_8c7e20 = record_index;
        }
        if ((*record_tag == PATH_TAG_3_0F) && (g_camera_8c7e1c == CAMERA_INDEX_UNSET)) {
          g_camera_8c7e1c = record_index;
        }
        record_index = record_index + 1;
        dst_record = dst_record + CAMERA_SLOT_WORDS;
        record_tag = record_tag + PATH_RECORD_WORDS;
      } while (record_index < record_count);
    }
    /* reuses record_count for a random roll in [0, 7.0f] (inferred) */
    record_count = (int)random_range(0,RANDOM_MAX_7_0F);
    g_car_8c7440 = record_count << 8;
  }
  return;
}


