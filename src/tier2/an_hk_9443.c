#include "mfl_tier2.h"
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(char *name);
/*00449700*/ extern void __cdecl course_sky_atomic_create(int texture, float a, float b);
/*004546e0*/ extern void __cdecl SUB_004546e0(char *course);
/*0048d900*/ extern void __cdecl SUB_0048d900(struct RwTexture *texture);
/*0048db30*/ extern void __cdecl SUB_0048db30(RwV3d *position, int a, char *b, float radius);
/*004a5d10*/ extern int __cdecl object_frames_create_with_matrices(int atomic, int count, char *course_data);
/*004c3f60*/ extern int __cdecl FUN_004c3f60(int clump);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *v);
/*004d5d90*/ extern struct RwTexture * RwTexDictionaryFindNamedTexture(struct RwTexDictionary *dict, char *name);
/*0050b5e0*/ extern void __cdecl nop_stub(char *course);
/*005ddb64*/ extern char s_smoke_005ddb64[];
/*006a1870*/ extern int g_course_neustein_6a1870;
/*006a1d88*/ extern int g_course_neustein_6a1d88;
/*006a1d8c*/ extern int g_course_neustein_6a1d8c;
/*006a1d90*/ extern int g_course_neustein_6a1d90;
/* course_neustein_cb0 */
/* "builds the Neustein course objects: sky smoke, tree sprite texture, one tracked object per entry of the three object tables (the third table measures the length of a second table entry, as the original does), and the three frame sets" */
/* TIER2: unvalidated 2026-10-21 authority=diffemu */
void course_neustein_cb0(char *course)
{
  RwV3d position;
  int i;
  int atomic;
  char *entry;

  nop_stub(course);
  SUB_004546e0(course);
  course_sky_atomic_create(find_texture_dict_b(s_smoke_005ddb64), 1.0f, -2.0f);
  SUB_0048d900(RwTexDictionaryFindNamedTexture(COURSE_TEXDICT(course), "TreeSprite"));
  entry = course + 0x20;
  for (i = 0; i < *(int *)(course + 0xc000); i++) {
    position = *(RwV3d *)(entry + 0x10);
    SUB_0048db30(&position, COURSE_FX_SOURCE_COUNT(course, 0), course + 0xc00, RwV3dLength((RwV3d *)entry));
    g_course_neustein_6a1870 = g_course_neustein_6a1870 + 1;
    entry = entry + 0x40;
  }
  entry = course + 0x1820;
  for (i = 0; i < COURSE_FX_SOURCE_COUNT(course, 1); i++) {
    position = *(RwV3d *)(entry + 0x10);
    SUB_0048db30(&position, COURSE_FX_SOURCE_COUNT(course, 0), course + 0xc00, RwV3dLength((RwV3d *)entry));
    g_course_neustein_6a1870 = g_course_neustein_6a1870 + 1;
    entry = entry + 0x40;
  }
  entry = course + 0x1820;
  for (i = 0; i < *(int *)(course + 0xc014); i++) {
    position = *(RwV3d *)(entry + 0x2410);
    SUB_0048db30(&position, COURSE_FX_SOURCE_COUNT(course, 0), course + 0xc00, RwV3dLength((RwV3d *)entry));
    g_course_neustein_6a1870 = g_course_neustein_6a1870 + 1;
    entry = entry + 0x40;
  }
  atomic = FUN_004c3f60(*(int *)(course + 0xc11c));
  g_course_neustein_6a1d88 = object_frames_create_with_matrices(atomic, *(int *)(course + 0xc000), course);
  g_course_neustein_6a1d8c = object_frames_create_with_matrices(atomic, COURSE_FX_SOURCE_COUNT(course, 1), course + 0x1800);
  g_course_neustein_6a1d90 = object_frames_create_with_matrices(atomic, *(int *)(course + 0xc014), course + 0x3c00);
}
