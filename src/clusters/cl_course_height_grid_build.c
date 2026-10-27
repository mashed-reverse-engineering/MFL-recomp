// cluster-entries: course_height_grid_build
/* Installed cluster course_height_grid_build (tools/cluster_install.py from output/cluster_pilot_ql1/course_height_grid_build.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
#pragma auto_inline(off)      /* the verified (diffemu, /Ob0) and the shipped (/O2) code must have the same call structure */
extern void *memset(void *, int, unsigned int);
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*00474960*/ extern unsigned int __cdecl course_cell_hash_pack(float x_unit, float y_unit, int aux_a, int aux_b, void *unused_a, void *unused_b, void *unused_c);

#define HEIGHT_GRID_CELLS 25                  /* the grid is 25 x 25 height samples per course cell */
#define HEIGHT_GRID_HIT_SLOTS 6               /* room for the distances of one ray; the ray itself follows in the same frame object */
#define HEIGHT_GRID_CELL_FRACTION 0.041666668f /* 1 / 24: cell size = course cell size / (HEIGHT_GRID_CELLS - 1) */
#define HEIGHT_MIN_START 10000.0f             /* min / max trackers of the first pass */
#define HEIGHT_MAX_START -10000.0f
#define NO_HIT_DISTANCE -1.0f                 /* distance slots of the second pass before the ray cast fills them */

/* World box the grid covers: {x_max, y_top, z_max, x_min, y_bottom, z_min} */
typedef struct HeightGridBox {
  float x_max, y_top, z_max;
  float x_min, y_bottom, z_min;
} HeightGridBox;

/* Height grid of one course cell (0xa10 bytes): the packed height key of each of 25 x 25 samples (the 9 bit height step in the
   top bits, see course_cell_hash_pack) and the placement of the cell. */
typedef struct HeightGrid {
  unsigned int key[HEIGHT_GRID_CELLS][HEIGHT_GRID_CELLS];
  int columns;                 /* samples along x */
  int rows;                    /* samples along z */
  float cell_fraction_x;       /* 1/24 */
  float cell_fraction_z;
  float size_x;                /* x_max - x_min */
  float size_z;
  float inv_size_x;
  float inv_size_z;
  float step_x;                /* size_x / 24 */
  float step_z;                /* size_z / 24 */
  float inv_step_x;            /* 1 / step_x (also the z factor: the cells are square) */
  float inv_step_z;
  HeightGridBox box;           /* y_top / y_bottom are replaced by the measured max / min heights after the first pass */
  float height_range;          /* max - min height */
} HeightGrid;

/* The ray (start above, end below) and the three ray hit distances live in ONE frame object in the original: the hit loop of the
   first pass may run over the distance slots into the ray words, the second pass hands the id slots and the distance slots on. */
typedef struct HeightGridFrame {
  union {
    float distance[HEIGHT_GRID_HIT_SLOTS];   /* first pass */
    struct {
      int ids[3];                            /* second pass: the ids come first, then the distances */
      float distance[3];
    } second;
  } hits;
  float ray[6];                /* start x, y, z; end x, y, z */
} HeightGridFrame;

/* course_height_grid_build: allocates the height grid of a course cell for the world box `box`.  The first pass casts a vertical ray
   per sample and measures the lowest and highest ground height; the second pass casts the rays again (against the box that now ends
   at those heights) and stores the packed key of the first two hit distances and ids. */
static int course_ray_hit_surfaces_collect_r(float * p0, float * p1, int * p2, int p3);
extern void course_ray_hit_surfaces_collect(void);
HeightGrid *course_height_grid_build(int world, const HeightGridBox *box)
{
  HeightGrid *grid;
  HeightGridFrame frame;
  float min_height, max_height, range_y, height;
  int column, row, count, k;

  grid = (HeightGrid *)RwMalloc(sizeof(HeightGrid), 0);
  memset(grid, 0, sizeof(HeightGrid));
  grid->columns = HEIGHT_GRID_CELLS;
  grid->rows = HEIGHT_GRID_CELLS;
  grid->cell_fraction_x = HEIGHT_GRID_CELL_FRACTION;
  grid->cell_fraction_z = HEIGHT_GRID_CELL_FRACTION;
  grid->box = *box;
  grid->size_x = box->x_max - box->x_min;
  grid->size_z = box->z_max - box->z_min;
  grid->inv_size_x = 1.0f / grid->size_x;
  grid->inv_size_z = 1.0f / grid->size_z;
  grid->step_x = grid->size_x * HEIGHT_GRID_CELL_FRACTION;
  grid->step_z = grid->size_z * HEIGHT_GRID_CELL_FRACTION;
  grid->inv_step_x = 1.0f / grid->step_x;
  grid->inv_step_z = grid->inv_step_x;

  min_height = HEIGHT_MIN_START;
  max_height = HEIGHT_MAX_START;
  range_y = grid->box.y_top - grid->box.y_bottom;
  for (column = 0; column < grid->columns; column++) {
    for (row = 0; row < grid->rows; row++) {
      frame.ray[0] = ((float)column / (float)(grid->columns - 1)) * grid->size_x + grid->box.x_min;
      frame.ray[1] = grid->box.y_top;
      frame.ray[2] = ((float)row / (float)(grid->rows - 1)) * grid->size_z + grid->box.z_min;
      frame.ray[3] = frame.ray[0];
      frame.ray[4] = grid->box.y_bottom;
      frame.ray[5] = frame.ray[2];
      count = course_ray_hit_surfaces_collect_r(frame.ray, frame.hits.distance, 0, world);
      for (k = 0; k < count; k++) {
        height = grid->box.y_top - range_y * frame.hits.distance[k];
        if (height < min_height) {
          min_height = height;
        }
        if (height > max_height) {
          max_height = height;
        }
      }
    }
  }
  grid->box.y_top = max_height;
  grid->box.y_bottom = min_height;
  grid->height_range = max_height - min_height;

  for (column = 0; column < grid->columns; column++) {
    for (row = 0; row < grid->rows; row++) {
      frame.hits.second.ids[0] = 0;
      frame.hits.second.ids[1] = 0;
      frame.hits.second.distance[0] = NO_HIT_DISTANCE;
      frame.hits.second.distance[1] = NO_HIT_DISTANCE;
      frame.ray[0] = ((float)column / (float)(grid->columns - 1)) * grid->size_x + grid->box.x_min;
      frame.ray[1] = grid->box.y_top;
      frame.ray[2] = ((float)row / (float)(grid->rows - 1)) * grid->size_z + grid->box.z_min;
      frame.ray[3] = frame.ray[0];
      frame.ray[4] = grid->box.y_bottom;
      frame.ray[5] = frame.ray[2];
      course_ray_hit_surfaces_collect_r(frame.ray, frame.hits.second.distance, frame.hits.second.ids, world);
      grid->key[column][row] = course_cell_hash_pack(frame.hits.second.distance[0], frame.hits.second.distance[1], frame.hits.second.ids[0] + 1,
                                                 frame.hits.second.ids[1] + 1, frame.ray, frame.hits.second.distance, frame.hits.second.ids);
    }
  }
  return grid;
}

/* course_ray_hit_surfaces_collect_r: C -> original course_ray_hit_surfaces_collect (registers s0,s1,s2,edx) */
__declspec(naked) int course_ray_hit_surfaces_collect_r(float * p0, float * p1, int * p2, int p3)
{
    __asm {
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        mov edx, dword ptr [esp + 28]
        call course_ray_hit_surfaces_collect
        add esp, 12
        ret
    }
}
