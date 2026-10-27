#include "mfl_tier2.h"
/*00481ea0*/ extern int player_marker_set_clump();
/*00481ee0*/ extern int player_marker_set_enabled();
/*006a26c0*/ extern undefined4 g_course_6a26c0;
/*006a26c4*/ extern float g_course_6a26c4;
/*006a26c8*/ extern undefined4 g_course_6a26c8;
/*006a26fc*/ extern undefined4 g_course_6a26fc;
/*006a2700*/ extern undefined4 g_course_6a2700;
/*006a2704*/ extern undefined4 g_course_6a2704;
/*006a2708*/ extern undefined4 g_course_6a2708;
/*006a270c*/ extern undefined4 g_course_6a270c;
/*006a2710*/ extern undefined4 g_course_6a2710;
/*006a2714*/ extern undefined4 g_course_6a2714;
/*006a2718*/ extern undefined4 g_course_6a2718;
/*006a271c*/ extern undefined4 g_course_6a271c;
/*006a2720*/ extern undefined4 g_course_6a2720;
/*006a2724*/ extern float g_course_6a2724;
/*006a2728*/ extern undefined4 g_course_6a2728;
/*006a275c*/ extern undefined4 g_course_6a275c;
/*006a2760*/ extern undefined4 g_course_6a2760;
/*006a2764*/ extern undefined4 g_course_6a2764;
/*006a2768*/ extern undefined4 g_course_6a2768;
/*006a276c*/ extern undefined4 g_course_6a276c;
/*006a2770*/ extern undefined4 g_course_6a2770;
/*006a2774*/ extern undefined4 g_course_6a2774;
/*006a2778*/ extern undefined4 g_course_6a2778;
/*006a277c*/ extern undefined4 g_course_6a277c;
/*006a2784*/ extern undefined4 g_course_6a2784;
/* course_sandsnow_cb2 */
/* course_storm_cb0 */
/* course_storm_cb2 */
/* course_storm_cb5 */
/* course_tierra_n_cb0 */
/* course_tierra_n_cb6 */
/* course_tierra_n_cb7 */
/* course_tierra_n_cb1 */
/* course_tierra_n_cb2 */
/* course_tierra_n_cb5 */
/* course_forest_cb0 */
/* course_forest_cb1 */
/* course_forest_cb2 */
/* course_c26_stub_return_0 */
/* "returns without work" */
/* course_object_get_float_16c */
/* "(index): float of the object record" */
/* course_c26_checkpoint_scale_value */
/* "a float table entry * 1/6" */
/* course_object_offsets_init */
/* "fills the table of 80 float offsets (10, 35, 60 ... 235 then pairs) used to place the course
   objects" */
/* course_objects_reset_physics */
/* "clears the 10 object records per group and parks their physics objects" */
/* trigger_cb_mark_hit_bit */
/* "type 0x15 trigger vs a car id (0xFF00..0xFF07): sets the bit of the object index in the bitmask
   at 0x6A26A4" */
/* course_highway_cb6 */
/* course_object_offsets_init_b */
/* "duplicate of course_object_offsets_init" */
/* IEEE-754 bits of -1.0f */
/* byte offsets in course_state of the clump pointer for each player marker (inferred) */
#define PLAYER_0_MARKER_CLUMP_OFFSET (0xc160)
#define PLAYER_1_MARKER_CLUMP_OFFSET (0xc158)
#define PLAYER_2_MARKER_CLUMP_OFFSET (0xc15c)
#define PLAYER_3_MARKER_CLUMP_OFFSET (0xc12c)
/* course_marker_positions_init */
/* "initialises six marker position triples and enables the four player markers with their clumps"
    */
/* Sets up the course marker data: clears g_course_6a2784, writes the position
   triples (inferred: two (-1, -1, 0) sentinel triples, then six float triples)
   and enables the four player markers, each bound to its clump from course_state. */
void course_marker_positions_init(char *course_state)
{
  g_course_6a2784 = 0;
  /* sentinel triple (inferred): -1.0, -1.0, 0 */
  *(float *)&g_course_6a26c4 = -1.0f;
  *(float *)&g_course_6a26c0 = -1.0f;
  g_course_6a26c8 = 0;
  /* second sentinel triple (inferred): -1.0, -1.0, 0 */
  *(float *)&g_course_6a2724 = -1.0f;
  *(float *)&g_course_6a2720 = -1.0f;
  g_course_6a2728 = 0;
  /* position triples (x, y, z) (inferred) */
  *(float *)&g_course_6a26fc = -15.037f;
  *(float *)&g_course_6a2700 = 4.93f;
  *(float *)&g_course_6a2704 = 58.152f;
  *(float *)&g_course_6a2708 = -14.978f;
  *(float *)&g_course_6a270c = 4.93f;
  *(float *)&g_course_6a2710 = 57.116f;
  *(float *)&g_course_6a2714 = -14.922f;
  *(float *)&g_course_6a2718 = 4.93f;
  *(float *)&g_course_6a271c = 56.119f;
  *(float *)&g_course_6a275c = -12.503f;
  *(float *)&g_course_6a2760 = 4.93f;
  *(float *)&g_course_6a2764 = 56.25f;
  *(float *)&g_course_6a2768 = -12.559f;
  *(float *)&g_course_6a276c = 4.93f;
  *(float *)&g_course_6a2770 = 57.247f;
  *(float *)&g_course_6a2774 = -12.618f;
  *(float *)&g_course_6a2778 = 4.93f;
  *(float *)&g_course_6a277c = 57.283f;
  /* player markers: player 0 enabled, players 1-3 disabled; each gets its clump */
  player_marker_set_enabled(0,1);
  player_marker_set_clump(0,*(undefined4 *)(course_state + PLAYER_0_MARKER_CLUMP_OFFSET));
  player_marker_set_enabled(1,0);
  player_marker_set_clump(1,*(undefined4 *)(course_state + PLAYER_1_MARKER_CLUMP_OFFSET));
  player_marker_set_enabled(2,0);
  player_marker_set_clump(2,*(undefined4 *)(course_state + PLAYER_2_MARKER_CLUMP_OFFSET));
  player_marker_set_enabled(3,0);
  player_marker_set_clump(3,*(undefined4 *)(course_state + PLAYER_3_MARKER_CLUMP_OFFSET));
  return;
}
