#include "mfl_tier2.h"
/* Physics (Rwp) object parameter block: 0xb4 bytes from the RenderWare memory functions.  Only the fields the functions below touch
   are named.  Defaults written by rwp_obj_0056e670: reference count 1, 1.0f in four slots, flags 0x38, cos(pi / 8) in six slots and
   "unbounded" limits (1e30f, with FLT_MAX in every third word) in the tail. */
struct RwpObjDef {
  int refCount;       /* 0x00 */
  float f04[3];       /* 0x04 */
  float one10;
  float f14[3];
  float f20[3];
  float one2c;
  float f30[3];
  int flags;          /* 0x3c */
  float f40;
  float one44;
  float cone48[3];    /* 0x48: cos(pi / 8) */
  float f54;
  float one58;
  float cone5c[3];    /* 0x5c: cos(pi / 8) */
  float limit[19];    /* 0x68..0xb0: 1e30f, FLT_MAX, ... */
};
#define RWP_OBJ_DEF_SIZE 0xb4
#define RWP_MEM_HINT 0x30900                /* allocation hint used by all Rwp objects */
#define RWP_OBJ_DEF_FLAGS 0x38
#define RWP_UNBOUNDED 0x7149f2ca            /* 1e30f */
#define RWP_FLT_MAX 0x7f7fffff
#define BITS(f, v) (*(int *)&(f) = (v))
extern void rwp_obj_0056e670(struct RwpObjDef *def);
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
extern double cos(double);
#define RWP_DEFAULT_CONE_ANGLE 0.39269909262657166   /* pi / 8 */

/* Allocates and initialises an object parameter block. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
struct RwpObjDef *rwp_obj_0056e640(void)
{
  struct RwpObjDef *def = (struct RwpObjDef *)RwMalloc(RWP_OBJ_DEF_SIZE, RWP_MEM_HINT);
  rwp_obj_0056e670(def);
  return def;
}

/* Sets every field of a fresh parameter block to its default. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_obj_0056e670(struct RwpObjDef *def)
{
  float cone = (float)cos(RWP_DEFAULT_CONE_ANGLE);
  int i;

  def->refCount = 1;
  def->one10 = 1.0f;
  def->one2c = 1.0f;
  def->one44 = 1.0f;
  def->one58 = 1.0f;
  for (i = 0; i < 3; i++) {
    def->f04[i] = 0.0f;
    def->f14[i] = 0.0f;
    def->f20[i] = 0.0f;
    def->f30[i] = 0.0f;
  }
  def->flags = RWP_OBJ_DEF_FLAGS;
  def->f40 = 0.0f;
  def->f54 = 0.0f;
  def->cone48[0] = cone;
  def->cone48[1] = cone;
  def->cone48[2] = cone;
  def->cone5c[0] = cone;
  def->cone5c[1] = cone;
  def->cone5c[2] = cone;
  /* two groups of three (U, U, MAX) triples with a zero word between them */
  def->limit[9] = 0.0f;
  for (i = 0; i < 9; i += 3) {
    BITS(def->limit[i], RWP_UNBOUNDED);
    BITS(def->limit[i + 1], RWP_UNBOUNDED);
    BITS(def->limit[i + 2], RWP_FLT_MAX);
    BITS(def->limit[i + 10], RWP_UNBOUNDED);
    BITS(def->limit[i + 11], RWP_UNBOUNDED);
    BITS(def->limit[i + 12], RWP_FLT_MAX);
  }
}

/* Sets the flag word. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_obj_0056e770(struct RwpObjDef *def, int flags)
{
  def->flags = flags;
}

/* Address of the first (ordinary) or the second (alternate) float triple. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
float *rwp_obj_0056e780(struct RwpObjDef *def, int alternate)
{
  return alternate ? def->f20 : def->f04;
}

/* Interior pointers handed to the solver: the objects they belong to are not typed yet. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
float *rwp_obj_0056e7a0(char *obj)
{
  return (float *)(obj + 0x40);
}
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
float *rwp_obj_0056e7b0(char *obj)
{
  return (float *)(obj + 0x54);
}
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
float *rwp_obj_0056e800(char *obj)
{
  return (float *)(obj + 0x14);
}
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
float *rwp_obj_0056e810(char *obj)
{
  return (float *)(obj + 0x20);
}
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
float *rwp_obj_0056e820(char *obj)
{
  return (float *)(obj + 0x2c);
}

/* Plain setters of the first three floats / the first word of a small parameter record. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_obj_0056e7c0(float *rec, float v)
{
  rec[0] = v;
}
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_obj_0056e7d0(float *rec, float v)
{
  rec[1] = v;
}
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_obj_0056e7e0(float *rec, float v)
{
  rec[2] = v;
}
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_obj_0056e7f0(int *rec, int v)
{
  rec[0] = v;
}

/* Cone limit setters: the record keeps cos(angle / 2) (angle in radians). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_obj_0056e830(float *rec, float angle)
{
  rec[2] = (float)cos(angle * 0.5f);
}
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_obj_0056e850(float *rec, float angle)
{
  rec[3] = (float)cos(angle * 0.5f);
}
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_obj_0056e870(float *rec, float angle)
{
  rec[4] = (float)cos(angle * 0.5f);
}
