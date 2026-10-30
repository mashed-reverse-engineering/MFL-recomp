/* compile: /G5 */
/* Cell table lookup: scales the cell position by m[3] and rotates the cell direction by the 3x3 rows of m (stride 4). */
typedef struct { float pos[3]; float pad0; float dir[3]; float pad1; } RwpCell;
typedef struct { RwpCell *cells; } RwpCellTable;
void rwp_batch_0057da00(RwpCellTable *t, float *out, float *m, int a, int b, int c)
{
  RwpCell *e;
  RwpCell *f;
  out[0] = t->cells[c + (b + a * 4) * 2].pos[0] * m[3];
  out[1] = t->cells[c + (b + a * 4) * 2].pos[1] * m[3];
  out[2] = t->cells[c + (b + a * 4) * 2].pos[2] * m[3];
  e = &t->cells[c + (b + a * 4) * 2];
  out[4] = e->dir[2] * m[2] + e->dir[1] * m[1] + e->dir[0] * m[0];
  f = &t->cells[c + (b + a * 4) * 2];
  out[5] = f->dir[2] * m[6] + f->dir[1] * m[5] + f->dir[0] * m[4];
  out[6] = t->cells[c + (b + a * 4) * 2].dir[2] * m[10] + t->cells[c + (b + a * 4) * 2].dir[1] * m[9] + t->cells[c + (b + a * 4) * 2].dir[0] * m[8];
}
