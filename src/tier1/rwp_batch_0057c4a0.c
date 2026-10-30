/* compile: /G5 */
/* dst[k] += s * src[k] for the 6 floats (a, b) of every record k named by the index list (records are 32 bytes). */
typedef struct { float a[3]; float pad0; float b[3]; float pad1; } RwpRec;
typedef struct { int *idx; unsigned count; } RwpIndexList;
void rwp_batch_0057c4a0(RwpRec *dst, int reserved, RwpRec *src, RwpIndexList *list, float s)
{
  unsigned i;

  for (i = 0; i != list->count; i++) {
    int k = list->idx[i];

    dst[k].a[0] = s * src[k].a[0] + dst[k].a[0];
    dst[k].a[1] = s * src[k].a[1] + dst[k].a[1];
    dst[k].a[2] = s * src[k].a[2] + dst[k].a[2];
    dst[k].b[0] = s * src[k].b[0] + dst[k].b[0];
    dst[k].b[1] = s * src[k].b[1] + dst[k].b[1];
    dst[k].b[2] = s * src[k].b[2] + dst[k].b[2];
  }
}
