/* compile: /G5 */
#define ALIGN16(x) (((x) + 15) & ~15)
typedef struct { int pad[2]; int off; } RwpArena;
typedef struct { int pad0[4]; int a, b, c, d, e, f; } RwpOffsets;
typedef struct { int pad0[2]; int m; int pad1[12]; int count; } RwpDesc;
/* Lays out the per-element arrays of a pipeline (count + 1 entries of 4, 12, 16, 32 and 4 bytes) in a 16-byte aligned arena and stores each array offset in out; the arena cursor ends after the trailing 2 * size block. */
void rwp_pipeline_00570e80(RwpArena *ar, RwpOffsets *o, RwpDesc *d)
{
  int n = d->count + 1;
  ar->off = ALIGN16(ar->off); o->a = ar->off;
  ar->off = ALIGN16(ar->off + n * 4); o->b = ar->off;
  ar->off = ALIGN16(ar->off + n * 12); o->c = ar->off;
  ar->off = ALIGN16(ar->off + n * 16); o->d = ar->off;
  ar->off = ALIGN16(ar->off + n * 32); o->e = ar->off;
  ar->off = ALIGN16(ar->off + n * 4); o->f = ar->off;
  ar->off = ALIGN16(ar->off + d->m * 2);
}
