// cluster-entries: clump_set_texture material_set_texture_cb
/* Installed cluster clump_set_texture (tools/cluster_install.py from output/cluster_pilot_te5/clump_set_texture.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004848b0*/ extern void *__cdecl clump_for_all_materials(void *clump, void *callback_data);
/*004f8230*/ extern void *__cdecl RpMaterialSetTexture(void *material, void *texture);

/* what clump_for_all_materials hands to the material callback: the callback and its argument (here the texture) */
typedef struct MaterialCallbackData {
  void *(__cdecl *callback)(void *material, void *texture);
  void *texture;
} MaterialCallbackData;

/* material_set_texture_cb: gives the material the texture (a NULL texture leaves it alone); returns the material to continue the walk */
void *__cdecl material_set_texture_cb(void *material, void *texture)
{
  if (texture != 0) {
    RpMaterialSetTexture(material, texture);
    return material;
  }
  return material;
}

/* clump_set_texture: sets `texture` on every material of the clump; returns the clump */
void *__cdecl clump_set_texture(void *clump, void *texture)
{
  MaterialCallbackData data;

  data.callback = material_set_texture_cb;
  data.texture = texture;
  clump_for_all_materials(clump, &data);
  return clump;
}

