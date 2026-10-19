#include "mfl_tier2.h"
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/* distance_between_points */

/*
 * Returns the perpendicular distance from `point` to the infinite line through
 * `line_start` and `line_end` (inferred from the math: |(start - point) x dir| / |dir|).
 * The name says "between points" but the result is point-to-line distance.
 * Divides by zero when line_start == line_end (inferred, no guard in the code).
 * Each point is a float[3] (x, y, z).
 */

float10 distance_between_points(float *line_start,float *line_end,float *point)

{
  RwReal line_length;
  RwReal cross_length;
  RwV3d point_to_start;
  RwV3d line_dir;
  RwV3d cross;

  /* direction vector of the line: line_end - line_start, and its length */
  line_dir.x = *line_end - *line_start;
  line_dir.y = line_end[1] - line_start[1];
  line_dir.z = line_end[2] - line_start[2];
  line_length = RwV3dLength(&line_dir);
  /* vector from point to line_start (its length is computed below but unused, inferred leftover) */
  point_to_start.x = *line_start - *point;
  point_to_start.y = line_start[1] - point[1];
  point_to_start.z = line_start[2] - point[2];
  RwV3dLength(&point_to_start);
  /* cross product point_to_start x line_dir: its length is the area of the parallelogram */
  cross.x = point_to_start.z * line_dir.y - point_to_start.y * line_dir.z;
  cross.y = point_to_start.x * line_dir.z - point_to_start.z * line_dir.x;
  cross.z = point_to_start.y * line_dir.x - point_to_start.x * line_dir.y;
  cross_length = RwV3dLength(&cross);
  /* distance = parallelogram area / base length */
  return (float10)cross_length / (float10)line_length;
}


