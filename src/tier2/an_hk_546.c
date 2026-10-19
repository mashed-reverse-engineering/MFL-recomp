#include "mfl_tier2.h"
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *p0);
/*00484660*/ extern undefined __cdecl clump_set_flag_on_atomics(struct RpClump *p0, undefined4 p1);
/*00486a60*/ extern int ptank_set_colour();
/*00486b00*/ extern int ptank_set_blend_modes();
/*00486f10*/ extern undefined __cdecl particle_system_setup_ptank(uint *p0, uint p1, uint p2, struct RwTexture *p3);
/*004c3bf0*/ extern void * FUN_004c3bf0(undefined4 p0, undefined4 p1, undefined4 p2);
/*004c3d80*/ extern int texdict_load_file(char * p0, int p1, int p2);
/*004d4e70*/ extern RwMatrix * RwMatrixCreate();
/*004d5e10*/ extern struct RwTexDictionary * RwTexDictionarySetCurrent(struct RwTexDictionary *p0);
/*005df8c8*/ extern undefined g_headlight_lazer_5df8c8;
/*006a93e0*/ extern undefined g_particles_6a93e0;
/*006a9458*/ extern undefined4 g_weapon_6a9458;
/*006a9580*/ extern undefined4 g_particles_6a9580;
/*006a95b8*/ extern undefined g_load_6a95b8;
#define LAZER_COUNT (4)
#define LAZER_RECORD_WORDS (0x16) /* words per lazer record: 0x58 bytes (inferred) */
#define FLOAT_ONE (0x3f800000) /* bit pattern of 1.0f */
/* RenderWare blend factors: rwBLENDSRCALPHA / rwBLENDINVSRCALPHA (inferred from the call) */
#define BLEND_SRC_ALPHA (5)
#define BLEND_INV_SRC_ALPHA (6)
/* headlight_lazer_init */

/* "Headlight texture, lazer.dff" */

/*
 * Sets up the headlight effect (inferred from its strings and globals):
 *  - builds the particle system g_particles_6a93e0 with the "Headlight" texture,
 *    src-alpha/inv-src-alpha blending and a blue colour;
 *  - loads lazer.dff into g_headlight_lazer_5df8c8 and resets the current texture dictionary;
 *  - fills LAZER_COUNT records in g_load_6a95b8, walking down from its end: each gets an
 *    identity RwMatrix (word 0), its clump (word -0xf from the record end), and its index.
 * Takes no parameters and returns nothing.
 */
void headlight_lazer_init(void)

{
  RwTexture *headlight_texture;
  RwMatrix *matrix;
  RpClump *clump;
  int lazer_index;
  int *lazer_end;
  int *lazer;
  struct {
    undefined4 r;
    undefined4 g;
    undefined4 b;
    undefined4 a;
  } colour;
  
  /* Particle system: "Headlight" texture, src-alpha blending, blue colour (inferred). */
  headlight_texture = (RwTexture *)find_texture_dict_b("Headlight");
  colour.r = 0;
  colour.g = 0;
  colour.b = FLOAT_ONE;
  colour.a = FLOAT_ONE;
  lazer_index = LAZER_COUNT;
  particle_system_setup_ptank((uint *)&g_particles_6a93e0,0x807,0x19,headlight_texture);
  ptank_set_blend_modes(&g_particles_6a93e0,BLEND_SRC_ALPHA,BLEND_INV_SRC_ALPHA);
  ptank_set_colour(&g_particles_6a93e0,&colour.r);
  g_particles_6a9580 = texdict_load_file(&g_headlight_lazer_5df8c8,0,0);
  RwTexDictionarySetCurrent((RwTexDictionary *)0x0);
  /* Create the lazers: one record per iteration, stop at the g_weapon_6a9458 sentinel (inferred). */
  lazer_end = (int *)&g_load_6a95b8;
  do {
    lazer_index = lazer_index + -1;
    lazer = lazer_end + -LAZER_RECORD_WORDS;
    /* Identity matrix: right.x, up.y and at.z = 1, everything else 0 (inferred from the stores). */
    matrix = RwMatrixCreate();
    *lazer = (int)matrix;
    (matrix->at).z = 1.0;
    *(undefined4 *)(*lazer + 0x14) = FLOAT_ONE;
    *(undefined4 *)*lazer = FLOAT_ONE;
    *(undefined4 *)(*lazer + 0x10) = 0;
    *(undefined4 *)(*lazer + 8) = 0;
    *(undefined4 *)(*lazer + 4) = 0;
    *(undefined4 *)(*lazer + 0x24) = 0;
    *(undefined4 *)(*lazer + 0x20) = 0;
    *(undefined4 *)(*lazer + 0x18) = 0;
    *(undefined4 *)(*lazer + 0x38) = 0;
    *(undefined4 *)(*lazer + 0x34) = 0;
    *(undefined4 *)(*lazer + 0x30) = 0;
    *(uint *)(*lazer + 0xc) = *(uint *)(*lazer + 0xc) | 0x20003;
    clump = (RpClump *)FUN_004c3bf0("lazer.dff",0,0);
    lazer_end[-0xf] = (int)clump;
    clump_set_flag_on_atomics(clump, 0x0);
    lazer_end[-0x24] = lazer_index;
    lazer_end[-0x22] = -1;
    lazer_end[-0x20] = 0;
    lazer_end = lazer;
  } while (lazer != &g_weapon_6a9458);
  return;
}


