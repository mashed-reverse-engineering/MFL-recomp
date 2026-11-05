/* rwp qhull glue (0x0058cc40-0x0058d0c0): the game's replacements of qhull's fprintf / fputs, and the copy of a finished qhull 2002.1
   (LIB_QHULL=src) result into an rwp hull block.  The builder that runs qhull (FUN_0058d0c0) stays original: it passes the two 16 bit
   counts in 32 bit registers with stale upper halves, which tools/diffemu compares (see the lv handoff). */
#include "mfl_tier2.h"

/* ---- qhull 2002.1 objects as the game's build lays them out (REALfloat: coordT = float, qh_MAXoutside) ---- */
typedef struct QhSet { int maxsize; void *e[1]; } QhSet;      /* setT: e[] ends with NULL */
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
  QhSet *outsideset, *coplanarset;
  unsigned int visitid;   /* 0x34: the game stores the facet's index here */
  unsigned int id;
};
typedef struct QhVertex QhVertex;
struct QhVertex {
  QhVertex *next, *previous;   /* 0x0: the list ends with a sentinel */
  float *point;   /* 0x8 */
  QhSet *neighbors;
  unsigned int visitid;   /* 0x10: the game stores the vertex index here */
};
typedef struct QhRidge {
  QhSet *vertices;   /* the two end points */
  QhFacet *top, *bottom;
  unsigned int idflags;   /* id:24, then seen (bit 24), tested, nonconvex */
} QhRidge;
#define QH_RIDGE_SEEN 0x1000000

/*00949bfc*/ extern QhFacet *g_qh_949bfc;   /* qh facet_list */
/*00949c24*/ extern QhVertex *g_qh_949c24;   /* qh vertex_list */

/* ---- the rwp hull block (rwp_hull_alloc), as these functions use it ---- */
typedef struct MflHullEdge {
  int pad0;
  unsigned short v0, v1;   /* 0x4: end points (vertex indices) */
  unsigned short face, other;   /* 0x8: the face whose list holds the edge, the face across it */
} MflHullEdge;
typedef struct MflHullTables {
  unsigned int size;
  float (*verts)[3];   /* 0x4 */
  unsigned short *vert_ring;   /* 0x8: per vertex: first entry in ring[] (nverts + 1 entries) */
  float (*normals)[3];   /* 0xc: face normals */
  unsigned short *face_edges;   /* 0x10: per face: first edge (nfaces + 1 entries) */
  MflHullEdge *edges;   /* 0x14 */
  unsigned short *ring;   /* 0x18: per vertex, the edges leaving it, face by face around the vertex */
  unsigned short nverts, nfaces, nedges;   /* 0x1c */
} MflHullTables;

/* three floats moved as their bit patterns */
#define COPY_V3(dst, src) (((int *)(dst))[0] = ((int *)(src))[0], ((int *)(dst))[1] = ((int *)(src))[1], ((int *)(dst))[2] = ((int *)(src))[2])

typedef struct MflIobuf { char raw[0x20]; } MflIobuf;
#define QH_STDOUT ((void *)1)   /* the two selector values of the fprintf below (else an RwStream) */
#define QH_STDERR ((void *)2)
/*0062c690*/ extern MflIobuf _iob[];
/*004b42b5*/ extern int vsprintf(char *buf, const char *fmt, char *args);   /* args: the address of the first variable argument (the CRT's va_list) */
/*004b4b12*/ extern unsigned int __cdecl _fwrite(void *p, unsigned int size, unsigned int n, FILE *fp);
extern unsigned int RwStreamWrite(void *stream, const void *buf, unsigned int n);
extern void qh_makeridges(QhFacet *facet);
extern QhRidge *qh_nextridge3d(QhRidge *ridge, QhFacet *facet, QhVertex **vertexp);
extern int qh_setsize(QhSet *set);
extern void rwp_hull_prepare(MflHullTables *hull);

/* qhull's fprintf (fp, fmt, ...) in this game: the "file" is a selector, 0 = off, 1 = stdout, 2 = stderr, anything else = an
   RwStream.  Formats into a 1000 byte buffer (no length check, like the original); returns the length.  Declared with the first
   variable argument as a named parameter (first_arg; vsprintf reads the values from its address on), as tools/diffemu needs fixed
   parameters; the callers are cdecl, so this is the same function as `int fprintf(void *fp, const char *fmt, ...)`. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int FUN_0058cc40(void *fp, const char *fmt, int first_arg)
{
  char buf[1000];
  int n;

  if (fp == 0)
    return 0;
  n = vsprintf(buf, fmt, (char *)&first_arg);
  if (fp == QH_STDOUT) {
    _fwrite(buf, 1, n, (FILE *)&_iob[1]);
    return n;
  }
  if (fp == QH_STDERR) {
    _fwrite(buf, 1, n, (FILE *)&_iob[2]);
    return n;
  }
  RwStreamWrite(fp, buf, n);
  return n;
}

/* qhull's fputs (string, file) in this game: the fprintf above with the string as the format. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int FUN_0058cce0(const char *s, void *fp)
{
  return ((int (*)(void *, const char *))FUN_0058cc40)(fp, s);      /* no variable arguments */
}

/* Index range [lo, hi) of the edges of face f; an out of range face means all edges. */
#define FACE_EDGES(h, f, lo, hi) \
  ((f) < (h)->nfaces ? ((lo) = (h)->face_edges[f], (hi) = (h)->face_edges[(f) + 1]) : ((lo) = 0, (hi) = (h)->nedges))

/* First edge in [lo, hi) whose `field` is vertex v, else nedges. */
#define FIND_EDGE(h, i, lo, hi, field, v, out) \
  for ((i) = (lo); (i) < (hi); (i)++) \
    if ((v) == (h)->edges[i].field) break; \
  (out) = (i) < (hi) ? (i) : (h)->nedges

/* Copies the finished qhull result into the hull block: vertices, face normals, per face its edges (from the facet's ridges,
   oriented so that `face` is that facet), then per vertex the ring of its outgoing edges, walked face to face around the vertex.
   The qhull vertex / facet visitid fields are used as indices.  Returns the hull (after rwp_hull_prepare), or NULL when a table is
   missing. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
MflHullTables *FUN_0058cd00(MflHullTables *hull)
{
  QhVertex *v;
  QhFacet *f;
  QhRidge *r, **rp;
  MflHullEdge *ed;
  unsigned int i, nface, k, vi, start;
  unsigned short e, a, b, lo, hi, j, face, edge;
  int cnt;

  if (hull == 0 || hull->normals == 0 || hull->verts == 0 || hull->edges == 0 || hull->ring == 0)
    return 0;
  i = 0;
  for (v = g_qh_949c24; v != 0 && v->next != 0; v = v->next) {
    k = i & 0xffff;
    i++;
    COPY_V3(hull->verts[k], v->point);
    v->visitid = k;
  }
  i = 0;
  for (f = g_qh_949bfc; f != 0 && f->next != 0; f = f->next) {
    k = i & 0xffff;
    i++;
    COPY_V3(hull->normals[k], f->normal);
    f->visitid = k;
  }

  /* the edges of each face: its ridges, written backwards from the face's end */
  nface = 0;
  e = 0;
  for (f = g_qh_949bfc; f != 0 && f->next != 0; f = f->next) {
    qh_makeridges(f);
    hull->face_edges[nface & 0xffff] = e;
    if (f->ridges != 0)
      for (rp = (QhRidge **)f->ridges->e; *rp != 0; rp++)
        (*rp)->idflags &= ~QH_RIDGE_SEEN;
    cnt = qh_setsize(f->ridges);
    e += cnt;
    for (r = (QhRidge *)f->ridges->e[0]; r != 0 && !(r->idflags & QH_RIDGE_SEEN); r = qh_nextridge3d(r, f, 0)) {
      r->idflags |= QH_RIDGE_SEEN;
      e--;
      a = ((QhVertex *)r->vertices->e[0])->visitid;
      b = ((QhVertex *)r->vertices->e[1])->visitid;
      ed = &hull->edges[e];
      if (r->bottom == f) {
        ed->face = r->top->visitid;
        ed->other = r->bottom->visitid;
        ed->v0 = a;
        ed->v1 = b;
      } else {
        ed->face = r->bottom->visitid;
        ed->other = r->top->visitid;
        ed->v0 = b;
        ed->v1 = a;
      }
    }
    nface++;
    e += cnt;
  }
  hull->face_edges[nface & 0xffff] = e;

  /* per vertex: start at the first edge leaving it, then repeatedly cross to the next face through the edge that arrives at it */
  k = 0;
  for (vi = 0; (unsigned short)vi < hull->nverts; vi++) {
    hull->vert_ring[vi & 0xffff] = (unsigned short)k;
    FACE_EDGES(hull, (unsigned short)0xffff, lo, hi);
    FIND_EDGE(hull, j, lo, hi, v0, (vi & 0xffff), edge);
    start = hull->edges[edge].face;
    face = (unsigned short)start;
    do {
      FACE_EDGES(hull, face, lo, hi);
      FIND_EDGE(hull, j, lo, hi, v1, (vi & 0xffff), edge);
      face = hull->edges[edge].face;
      FACE_EDGES(hull, face, lo, hi);
      FIND_EDGE(hull, j, lo, hi, v0, (vi & 0xffff), edge);
      hull->ring[k & 0xffff] = edge;
      k++;
    } while (face != (unsigned short)start);
  }
  hull->vert_ring[vi & 0xffff] = (unsigned short)k;
  rwp_hull_prepare(hull);
  return hull;
}
