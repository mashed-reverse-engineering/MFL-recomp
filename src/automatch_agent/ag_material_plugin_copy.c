#include "mfl_tier2.h"
/* layout of *dst in material_plugin_copy, reconstructed from the byte offsets it is accessed at (explicit padding: the offsets are the original ones, every field sits at its natural alignment; fields are named by offset) */
typedef struct material_plugin_copy_dst_layout {
    int f_0x0;
    int f_0x4;
    char pad_0x8[4];
    int f_0xc;
    int f_0x10;
    int f_0x14;
    int f_0x18;
    int f_0x1c;
    int f_0x20;
    int f_0x24;
    int f_0x28;
    int f_0x2c;
} material_plugin_copy_dst_layout;
/* layout of *src in material_plugin_copy, reconstructed from the byte offsets it is accessed at (explicit padding: the offsets are the original ones, every field sits at its natural alignment; fields are named by offset) */
typedef struct material_plugin_copy_src_layout {
    int f_0x0;
    int f_0x4;
    char pad_0x8[4];
    int f_0xc;
    int f_0x10;
    int f_0x14;
    int f_0x18;
    int f_0x1c;
    int f_0x20;
    int f_0x24;
    int f_0x28;
    int f_0x2c;
} material_plugin_copy_src_layout;


/*0089de04*/ extern char *g_app_89de04;


int material_plugin_copy(int param_1,int param_2)
{
  material_plugin_copy_dst_layout *dst = (material_plugin_copy_dst_layout *)((char *)g_app_89de04 + param_1);
  material_plugin_copy_src_layout *src = (material_plugin_copy_src_layout *)((char *)g_app_89de04 + param_2);

  dst->f_0x0 = src->f_0x0;
  dst->f_0x4 = src->f_0x4;
  dst->f_0xc = src->f_0xc;
  dst->f_0x10 = src->f_0x10;
  dst->f_0x18 = src->f_0x18;
  dst->f_0x20 = src->f_0x20;
  dst->f_0x28 = src->f_0x28;
  dst->f_0x14 = src->f_0x14;
  dst->f_0x1c = src->f_0x1c;
  dst->f_0x24 = src->f_0x24;
  dst->f_0x2c = src->f_0x2c;
  return param_1;
}
