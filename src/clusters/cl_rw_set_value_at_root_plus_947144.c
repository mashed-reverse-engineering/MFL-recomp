// cluster-entries: rw_set_value_at_root_plus_947144
/* Installed cluster rw_set_value_at_root_plus_947144 (tools/cluster_install.py from output/cluster_pilot_pg2/rw_set_value_at_root_plus_947144.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00947144*/ extern int g_rw_947144;      /* byte offset of the D3D9 raster extension inside a RwRaster (RwD3D9RasterExtOffset) */

/* a RenderWare raster: only the parent link is used here (a raster that owns its pixels is its own parent) */
typedef struct Raster {
  struct Raster *parent;
} Raster;

/* rw_set_value_at_root_plus_947144: walks the parent chain of `raster` up to the root raster (the one that is its own parent) and stores
   `value` in the D3D9 extension of the root */
void rw_set_value_at_root_plus_947144(Raster *raster, int value)
{
  Raster *root = raster->parent;

  while (root != root->parent) {
    root = root->parent;
  }
  *(int *)((char *)root + g_rw_947144) = value;
}

