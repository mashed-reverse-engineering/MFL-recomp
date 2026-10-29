#include "mfl_tier2.h"
/*006c2ee0*/ extern undefined4 g_rwp_trigger_group_count;
/*006c2ee4*/ extern undefined4 g_car_6c2ee4;
/*0089de20*/ extern undefined1 g_physics_scenarios;
#define SCENARIO_ID_MAX (1999) /* ids up to this index g_physics_scenarios; larger ids are stored as string ids (inferred) */
#define NO_STRING_ID (0xffffffff)
#define TRIGGER_GROUP_STRIDE_BYTES (0x8c) /* 35 dwords per trigger group */
#define PHYSICS_SCENARIO_STRIDE (0x2510) /* bytes per g_physics_scenarios entry */
/* text_page_add_string_id */
/* "adds a string id (>= 2000) to the current page; smaller ids add a scenario entry" */
/* Adds string_id to the current trigger group (g_rwp_trigger_group_count) and returns 1. Ids above
   SCENARIO_ID_MAX are stored as string ids. Smaller ids (inferred: scenario indices) are appended to
   the group as a pointer into g_physics_scenarios, and the group's entry count goes up by one. */
undefined4 text_page_add_string_id(int string_id)
{
  char *rec;

  rec = (char *)&g_car_6c2ee4 + g_rwp_trigger_group_count * TRIGGER_GROUP_STRIDE_BYTES;
  if (string_id >= SCENARIO_ID_MAX + 1) {
    *(int *)(rec + *(int *)(rec + 0x80) * 4 + 0x3c) = string_id;
    return 1;
  }
  *(undefined4 *)(rec + *(int *)(rec + 0x80) * 4 + 0x40) = NO_STRING_ID;
  *(char **)(rec + *(int *)(rec + 0x80) * 4) = (char *)&g_physics_scenarios + string_id * PHYSICS_SCENARIO_STRIDE;
  *(int *)(rec + 0x80) = *(int *)(rec + 0x80) + 1;
  return 1;
}
