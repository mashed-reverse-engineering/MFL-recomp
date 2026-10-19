#include "mfl_tier2.h"
extern char PTR_g_data_6e7573_005e0994[];
/*004d5e40*/ extern struct RwTexture * RwTextureRead(RwChar *p0, RwChar *p1);
/*004ddb80*/ extern RwChar * RwImageSetPath(RwChar *p0);
/*0076df24*/ extern undefined4 g_particles_76df24;
/* fx_texture_load */

/* "RwTextureRead of the fx texture into g_particles_76df24" */

undefined4 fx_texture_load(void)

{
  /* Clear the image search path so the texture name is resolved from the working directory (inferred) */
  RwImageSetPath("");
  /* Read the texture named by PTR_g_data_6e7573_005e0994 (inferred: a texture name string) with no second name argument (inferred: no mask) */
  g_particles_76df24 = RwTextureRead((RwChar *)&PTR_g_data_6e7573_005e0994,(RwChar *)0x0);
  /* Always reports success (inferred) */
  return 1;
}


