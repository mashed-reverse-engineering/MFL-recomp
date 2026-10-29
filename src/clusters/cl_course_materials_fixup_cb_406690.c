// cluster-entries: course_materials_fixup_cb_406690
/* Installed cluster course_materials_fixup_cb_406690 (tools/cluster_install.py from output/cluster_pilot_kg/course_materials_fixup_cb_406690.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004b3404*/ extern int toupper();
/*00554370*/ extern int __cdecl FUN_00554370(int p0);
/*005543a0*/ extern int __cdecl FUN_005543a0(int p0, int p1);
/*005543d0*/ extern char * __cdecl FUN_005543d0(int p0);
/*005543e0*/ extern int __cdecl FUN_005543e0(int p0);
/*005543f0*/ extern int __cdecl rp_user_data_array_get_int(int p0, int p1);
/*00554450*/ extern int __cdecl RpUserDataGetFormatSize(int p0);
/*00544430*/ extern int __cdecl FUN_00544430(int p0);
/*00481aa0*/ extern void __cdecl object_set_pipeline_08(void *material, int p1);
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *p0);
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0, ...);
/*004d5d90*/ extern RwTexture * __cdecl RwTexDictionaryFindNamedTexture(RwTexDictionary *p0, RwChar *p1);
/*004f8230*/ extern RpMaterial * __cdecl RpMaterialSetTexture(RpMaterial *p0, RwTexture *p1);
/*0089de04*/ extern int g_app_89de04;
/*00843e80*/ extern int g_course_843e80;
/* compile: /GS */

/* course_materials_fixup_cb_406690 */

/* the material extension record of the JACFX effect plugin (at material + g_app_89de04) */
typedef struct MaterialFx {
  int type;            /* JACFX.CFXTYPE */
  RwTexture *base_texture;    /* the texture the material had before the fixup */
  RwTexture *fx_texture;      /* JACFX.BASETEXTURE looked up in the course dictionary */
  int blend;           /* JACFX.CFXBLEND */
  float scale_a;
  float scale_b;
  int reserved[4];
  int unused_28;
  float one;
} MaterialFx;

/*
 * Callback of course_materials_fixup (PUSH 0x406690): reads the user data array entries of a material (the names, upper-cased, are compared
 * with JACFX.CFXTYPE / CFXBLEND / BASETEXTURE) and fills the material's JACFX extension record; type 3 materials get 0x33 stored at +0x51 of the fx texture.
 * Returns the material, or 0 when there is no texture dictionary.
 */
int course_materials_fixup_cb_406690(int material, RwTexDictionary *param_2)

{
  char upper_name [64];
  int count;
  int entry;
  int ud;
  int i;
  char c;
  char *name;
  char *a;
  char *b;
  int remaining;
  int equal;
  int length;
  RwChar *tex_name;
  RwTexture *tex;
  MaterialFx *fx;

  count = FUN_00554370(material);
  if (param_2 == (RwTexDictionary *)0x0) {
    return 0;
  }
  fx = (MaterialFx *)(material + g_app_89de04);
  entry = 0;
  if (0 < count) {
    do {
      ud = FUN_005543a0(material, entry);
      name = FUN_005543d0(ud);
      b = name;
      do {
        c = *b;
        b = b + 1;
      } while (c != '\0');
      length = (int)b - (int)(name + 1);
      i = 0;
      if (0 < length) {
        do {
          upper_name[i] = (char)toupper((int)name[i]);
          i = i + 1;
        } while (i < length);
      }
      upper_name[i] = '\0';
      RpUserDataGetFormatSize(FUN_005543e0(ud));
      FUN_00544430(ud);
      remaining = 0xe;
      equal = 1;
      a = upper_name;
      b = "JACFX.CFXTYPE";
      do {
        if (remaining == 0) break;
        remaining = remaining + -1;
        equal = *a == *b;
        a = a + 1;
        b = b + 1;
      } while (equal);
      if (equal) {
        fx->type = rp_user_data_array_get_int(ud, 0);
        fx->unused_28 = 0;
        fx->one = 1.0f;
        fx->scale_a = 1.0f / 512.0f;
        fx->scale_b = 1.0f / 448.0f;
        fx->reserved[0] = 0;
        fx->reserved[1] = 0;
        fx->reserved[2] = 0;
        fx->reserved[3] = 0;
        object_set_pipeline_08((void *)material, 0);
      }
      remaining = 0xf;
      equal = 1;
      a = upper_name;
      b = "JACFX.CFXBLEND";
      do {
        if (remaining == 0) break;
        remaining = remaining + -1;
        equal = *a == *b;
        a = a + 1;
        b = b + 1;
      } while (equal);
      if (equal) {
        fx->blend = rp_user_data_array_get_int(ud, 0);
      }
      remaining = 0x12;
      equal = 1;
      a = upper_name;
      b = "JACFX.BASETEXTURE";
      do {
        if (remaining == 0) break;
        remaining = remaining + -1;
        equal = *a == *b;
        a = a + 1;
        b = b + 1;
      } while (equal);
      if (equal) {
        tex_name = (RwChar *)rp_user_data_array_get_int(ud, 0);
        tex = RwTexDictionaryFindNamedTexture(param_2, tex_name);
        fx->fx_texture = tex;
        if (tex == (RwTexture *)0x0) {
          fx->fx_texture = (RwTexture *)find_texture_dict_b(tex_name);
        }
        if (fx->base_texture == (RwTexture *)0x0) {
          tex = *(RwTexture **)material;
          fx->base_texture = tex;
          if (tex == (RwTexture *)0x0) {
            FID_conflict__wprintf("HUGE ERROR: we have a texture missing!!!\n");
          }
          else {
            FID_conflict__wprintf("setting texture name %s\n", RwTexture_name(tex));
          }
          if (g_course_843e80 != 1) {
            RpMaterialSetTexture((RpMaterial *)material, fx->fx_texture);
          }
        }
      }
      entry = entry + 1;
    } while (entry < count);
  }
  if (fx->type == 3) {
    *((unsigned char *)fx->fx_texture + 0x51) = 0x33;
  }
  return material;
}

