// cluster-entries: shockwave_load_models
/* Installed cluster shockwave_load_models (tools/cluster_install.py from output/cluster_pilot_zx/shockwave_load_models.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*0042f780*/ extern int __cdecl clump_load_by_name(char *p0, undefined4 p1, undefined4 p2);
/*004c3f60*/ extern RpAtomic * clump_first_atomic(RpClump * p0);
/*004c4120*/ extern void FUN_004c4120(undefined4 p0, undefined4 p1);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004f6ae0*/ extern struct RpAtomic * RpAtomicClone(struct RpAtomic *p0);
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*00559a00*/ extern int __cdecl FUN_00559a00(int a1);
/*006c51a8*/ extern undefined4 g_gfx_6c51a8;
/*006c51f4*/ extern undefined4 g_shockwave_load_6c51f4;
/*006c57b0*/ extern undefined4 g_gfx_6c57b0;
/*006c57b8*/ extern undefined4 g_gfx_6c57b8;
/*006c57bc*/ extern undefined4 g_shockwave_load_6c57bc;
/*006c57c0*/ extern undefined4 g_shockwave_load_6c57c0;
/*006c57c4*/ extern undefined4 g_shockwave_load_6c57c4;
/*006c57c8*/ extern undefined4 g_shockwave_load_6c57c8;
/*006c57cc*/ extern undefined4 g_shockwave_load_6c57cc;
/*006c57d0*/ extern undefined4 g_shockwave_load_6c57d0;
/*006c57d8*/ extern undefined4 g_shockwave_load_6c57d8;
/*006c57dc*/ extern undefined4 g_shockwave_load_6c57dc;
/*006c57e0*/ extern undefined4 g_shockwave_load_6c57e0;
/*006c57e8*/ extern undefined4 g_load_6c57e8;
/*006c57ec*/ extern undefined4 g_load_6c57ec;
/*006c57f0*/ extern undefined4 g_load_6c57f0;
/*006c583c*/ extern undefined4 g_shockwave_load_6c583c;
/*006c5df8*/ extern undefined4 g_gfx_6c5df8;
/*006c5dfc*/ extern undefined4 g_load_6c5dfc;
/*006c5e00*/ extern undefined4 g_shockwave_load_6c5e00;
/*006c5e04*/ extern undefined4 g_shockwave_load_6c5e04;
/*006c5e08*/ extern undefined4 g_load_6c5e08;
/*006c5e0c*/ extern undefined4 g_load_6c5e0c;
extern char g_shockwave_load_6c57f4[];
extern char g_shockwave_load_6c5e3c[];

/* IEEE-754 bit pattern of 1.0f */
/* dwords per entry in the two model tables (inferred: 96-byte entry) */
#define ENTRY_STRIDE_DWORDS (0x18)

/* shockwave_load_models */

/* "shockwave.dff and hemisphere.dff clumps" */

/*
 * Loads the shockwave.dff and hemisphere.dff clumps and keeps the first atomic of
 * each as a template (g_shockwave_load_6c5e00 and g_shockwave_load_6c5e04).
 * Fills two tables of entries (ENTRY_STRIDE_DWORDS dwords each): every entry gets
 * a clone of its template, its own new frame, an index, and a kind.
 * Takes no parameters and returns nothing.
 */
static int effect_sprite_reset_r(undefined4 * p0);
extern void effect_sprite_reset(void);
void shockwave_load_models(void)

{
  RpAtomic *cloned_atomic;
  RwFrame *new_frame;
  undefined4 *entry;
  int entry_index;
  char *model_name;
  undefined4 load_arg_1;
  undefined4 load_arg_2;

  /* load_arg_1 / load_arg_2 are both 0 here; meaning unknown (clump_load_by_name params) */
  load_arg_2 = 0;
  load_arg_1 = 0;
  model_name = "shockwave.dff";
  g_gfx_6c5df8 = clump_load_by_name("shockwave.dff",load_arg_1,load_arg_2);
  g_shockwave_load_6c5e00 = (RpAtomic *)clump_first_atomic(g_gfx_6c5df8);
  g_load_6c5e08 = FUN_00559a00(g_shockwave_load_6c5e00);
  FUN_004c4120(g_shockwave_load_6c5e00,0);
  load_arg_2 = 0;
  load_arg_1 = 0;
  model_name = "hemisphere.dff";
  g_load_6c5dfc = clump_load_by_name("hemisphere.dff",load_arg_1,load_arg_2);
  g_shockwave_load_6c5e04 = (RpAtomic *)clump_first_atomic(g_load_6c5dfc);
  g_load_6c5e0c = FUN_00559a00(g_shockwave_load_6c5e04);
  FUN_004c4120(g_shockwave_load_6c5e04,0);
  /* reset globals used by the effect: 0x20003 OR'd into one flag word (meaning unknown),
     float 1.0 in three scale/alpha globals (inferred), zero in the rest */
  g_shockwave_load_6c57c4 = g_shockwave_load_6c57c4 | 0x20003;
  g_shockwave_load_6c57e0 = FLOAT_ONE_BITS;
  g_shockwave_load_6c57cc = FLOAT_ONE_BITS;
  g_gfx_6c57b8 = FLOAT_ONE_BITS;
  g_shockwave_load_6c57c8 = 0;
  g_shockwave_load_6c57c0 = 0;
  g_shockwave_load_6c57bc = 0;
  g_shockwave_load_6c57dc = 0;
  g_shockwave_load_6c57d8 = 0;
  g_shockwave_load_6c57d0 = 0;
  g_load_6c57f0 = 0;
  g_load_6c57ec = 0;
  g_load_6c57e8 = 0;
  /* table 1: clones of the shockwave template, kind 0
     (entry[5] = index, entry[6] = kind; inferred) */
  entry_index = 0;
  entry = &g_shockwave_load_6c583c;
  do {
    cloned_atomic = RpAtomicClone(g_shockwave_load_6c5e00);
    *entry = cloned_atomic;
    new_frame = RwFrameCreate();
    RpAtomicSetFrame((RpAtomic *)*entry,new_frame);
    entry[5] = entry_index;
    entry[6] = 0;
    effect_sprite_reset_r(entry + -0x11);
    entry = entry + ENTRY_STRIDE_DWORDS;
    entry_index = entry_index + 1;
  } while ((LONG_PTR)entry < ((LONG_PTR)&g_shockwave_load_6c5e3c));
  /* table 2: clones of the hemisphere template, kind 1 */
  entry_index = 0;
  entry = &g_shockwave_load_6c51f4;
  do {
    cloned_atomic = RpAtomicClone(g_shockwave_load_6c5e04);
    *entry = cloned_atomic;
    new_frame = RwFrameCreate();
    RpAtomicSetFrame((RpAtomic *)*entry,new_frame);
    entry[5] = entry_index;
    entry[6] = 1;
    effect_sprite_reset_r(entry + -0x11);
    entry = entry + ENTRY_STRIDE_DWORDS;
    entry_index = entry_index + 1;
  } while ((LONG_PTR)entry < ((LONG_PTR)&g_shockwave_load_6c57f4));
  /* clear two 4-byte globals */
  mem_zero(&g_gfx_6c57b0,4);
  mem_zero(&g_gfx_6c51a8,4);
  return;
}

/* effect_sprite_reset_r: C -> original effect_sprite_reset (registers eax) */
__declspec(naked) int effect_sprite_reset_r(undefined4 * p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call effect_sprite_reset
        ret
    }
}
