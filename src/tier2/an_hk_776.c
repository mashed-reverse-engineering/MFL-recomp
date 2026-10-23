#include "mfl_tier2.h"
/*0042b0c0*/ extern int course_get_ai_polygon_count();
/*0042b210*/ extern char *course_get_record_point();
/*004749f0*/ extern undefined __cdecl spatial_grid_create(undefined4 p0, undefined4 p1, undefined4 p2);
/*00474ac0*/ extern int __cdecl spatial_grid_get(undefined4 p0, undefined4 p1, void *p2, void *p3, void *p4, void *p5);
/*00474ae0*/ extern int course_cell_bounds_from_position();
/*00474ba0*/ extern undefined __cdecl spatial_grid_set(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, void *p4);
/*00474cb0*/ extern int course_height_grid_build();
/* spatial grid is 10x10 cells (from the header string), stored at course + 0x50 (inferred) */
#define SPATIAL_GRID_SIDE (10)
#define COURSE_SPATIAL_GRID_OFFSET (0x50)
/* each polygon samples record point 0 and record point 3 (inferred) */
#define RECORD_POINT_IDX_B (3)
#define POLYGON_SAMPLE_POINTS (2)
/* course_ai_polygons_build_grid */

/* "creates a 10x10 spatial grid and inserts the AI polygons" */

/*
 * Builds the AI spatial grid of a course (inferred).
 * Creates the grid, then for every AI polygon takes its two sampled record points, finds the grid cell
 * each one falls in, and if that cell is still empty stores course_height_grid_build's value there.
 * course: the course object (inferred), its grid lives at course + 0x50. Returns nothing.
 */
void course_ai_polygons_build_grid(int course)

{
  undefined4 saved_cell_x;
  int polygon_index;
  undefined4 *record_point;
  int cell_entry;
  undefined4 cell_value;
  int point_slot;
  undefined4 *position;
  undefined1 *bounds;
  undefined4 *cell_x_out;
  undefined4 point_index;
  struct {
    undefined4 cell_y;
    undefined4 cell_x;
    undefined4 pos_x;
    undefined4 pos_y;
    undefined4 pos_z;
    undefined1 bounds [24];
  } frame;

  spatial_grid_create(SPATIAL_GRID_SIDE,SPATIAL_GRID_SIDE,course + COURSE_SPATIAL_GRID_OFFSET);
  /* polygon_index holds the polygon count until the loop starts */
  polygon_index = course_get_ai_polygon_count();
  if (0 < polygon_index) {
    polygon_index = 0;
    /* Outer loop: each AI polygon. */
    do {
      point_slot = 0;
      /* Inner loop: the two sampled record points (0, then RECORD_POINT_IDX_B). */
      do {
        if (point_slot == 0) {
          point_index = 0;
        }
        else {
          point_index = RECORD_POINT_IDX_B;
        }
        record_point = (undefined4 *)course_get_record_point(polygon_index,point_index);
        frame.pos_x = *record_point;
        frame.pos_y = record_point[1];
        frame.pos_z = record_point[2];
        /* record_point is reused here as &frame.cell_y */
        record_point = &frame.cell_y;
        cell_x_out = &frame.cell_x;
        bounds = frame.bounds;
        position = &frame.pos_x;
        course_cell_bounds_from_position(&frame.pos_x, frame.bounds, &frame.cell_x, &frame.cell_y);
        saved_cell_x = frame.cell_x;
        /* point_index is reused here to hold cell_y */
        point_index = frame.cell_y;
        cell_entry = spatial_grid_get((undefined4)frame.cell_x,(undefined4)frame.cell_y,position,bounds,cell_x_out,record_point);
        /* Cell is empty: compute its value and store it. */
        if (cell_entry == 0) {
          bounds = frame.bounds;
          /* cell_entry is reused here to pass course on */
          cell_entry = course;
          cell_value = course_height_grid_build(course, frame.bounds);
          spatial_grid_set(saved_cell_x,point_index,cell_value,cell_entry,bounds);
        }
        point_slot = point_slot + 1;
      } while (point_slot < POLYGON_SAMPLE_POINTS);
      polygon_index = polygon_index + 1;
      /* point_slot is reused here to re-read the polygon count for the loop bound */
      point_slot = course_get_ai_polygon_count();
    } while (polygon_index < point_slot);
  }
  return;
}


