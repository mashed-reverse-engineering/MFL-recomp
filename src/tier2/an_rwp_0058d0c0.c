/* rwp convex hull builder (0x0058d0c0): runs qhull 2002.1 (LIB_QHULL=src) on a point cloud and hands the result to the copy in
   an_rwp_qhull_glue.c (FUN_0058cd00). */
#include "mfl_tier2.h"

/* ---- qhull 2002.1 objects as the game's build lays them out (the part used here; full layout in an_rwp_qhull_glue.c) ---- */
typedef struct QhSet { int maxsize; void *e[1]; } QhSet;
typedef struct QhFacet QhFacet;
struct QhFacet {
  float furthestdist, maxoutside, offset;
  float *normal;   /* 0xc */
  void *f, *center;
  QhFacet *previous;   /* 0x18 */
  QhFacet *next;   /* 0x1c: the list ends with a sentinel (next == NULL) */
  QhSet *vertices;   /* 0x20 */
  QhSet *ridges;   /* 0x24 */
  QhSet *neighbors;   /* 0x28 */
};
typedef struct MflHullTables MflHullTables;   /* the rwp hull block (an_rwp_qhull_glue.c) */

/*00949bfc*/ extern QhFacet *g_qh_949bfc;   /* qh facet_list */
/*00949c30*/ extern int g_qh_949c30;   /* qh num_facets */
/*00949c34*/ extern int g_qh_949c34;   /* qh num_vertices */
/*0063a604*/ extern char s_qhull_s_Pp_0063a604[];   /* "qhull s Pp": the default options */
extern int qh_setsize(QhSet *set);
extern int qh_new_qhull(int dim, int numpoints, float *points, int ismalloc, char *qhull_cmd, void *outfile, void *errfile);
extern void qh_freeqhull(int allmem);
extern void qh_memfreeshort(int *curlong, int *totlong);
extern MflHullTables *rwp_hull_alloc(int nverts, int nedges, int nfaces);
extern MflHullTables *FUN_0058cd00(MflHullTables *hull);

/* Convex hull of npoints points (3 floats each) through qhull ("qhull s Pp" unless options are given; messages to `out`), copied
   into a new rwp hull block (edges = sum of the facets' neighbour counts).  qhull's memory is freed in any case.  Returns the hull
   or NULL.  rwp_hull_alloc uses only the low 16 bits of the two counts (the original passes them with stale upper halves). */
/* TIER2: validated 2026-11-10 authority=diffemu */
MflHullTables *FUN_0058d0c0(int npoints, float *points, char *options, int out)
{
  MflHullTables *hull;
  QhFacet *f;
  int nedges;

  hull = 0;
  if (points == 0)
    return 0;
  if (options == 0)
    options = s_qhull_s_Pp_0063a604;
  if (qh_new_qhull(3, npoints, points, 0, options, (void *)out, (void *)out) == 0) {
    nedges = 0;
    for (f = g_qh_949bfc; f != 0 && f->next != 0; f = f->next)
      nedges += qh_setsize(f->neighbors);
    hull = rwp_hull_alloc((unsigned short)g_qh_949c34, nedges, (unsigned short)g_qh_949c30);
    if (hull != 0)
      hull = FUN_0058cd00(hull);
  }
  qh_freeqhull(0);
  qh_memfreeshort((int *)&options, (int *)&points);      /* curlong / totlong: the original reuses these two dead parameter slots */
  return hull;
}
