/* compile: /G5 */
/* Far-field bounds of entry idx (low 16 bits): the entry's box grown by the field margin (hi + margin, lo - margin). The entry is re-addressed per use, which fixes the register roles. */
typedef struct { float lo[3]; float pad0; float hi[3]; float pad1; } RwpFarEntry;
typedef struct { RwpFarEntry e[0x600]; float gap[5]; float margin; } RwpFarField;
typedef struct { float lo[3]; float pad0; float hi[3]; } RwpFarBounds;
void rwp_farfield_00576bf0(RwpFarField *f, int idx, RwpFarBounds *o)
{
  o->hi[0] = f->e[idx & 0xffff].hi[0] + f->margin;
  o->hi[1] = f->e[idx & 0xffff].hi[1] + f->margin;
  o->hi[2] = f->e[idx & 0xffff].hi[2] + f->margin;
  o->lo[0] = f->e[idx & 0xffff].lo[0] - f->margin;
  o->lo[1] = f->e[idx & 0xffff].lo[1] - f->margin;
  o->lo[2] = f->e[idx & 0xffff].lo[2] - f->margin;
}
