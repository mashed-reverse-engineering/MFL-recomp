#include "mfl_tier2.h"
/*004b2b48*/ extern int _ftol();
#define ALPHA_MAX (255.0f)
#define RANGE_MAX_OFFSET (4)
#define RANGE_MIN_OFFSET (8)
#define MATERIAL_COLOUR_OFFSET (4)
#define MATERIAL_ALPHA_OFFSET (7)
/* material_set_alpha_callback */

/* "RpMaterial callback: colour alpha = 0xFF - (int)value" */

/* Sets the colour alpha of an RpMaterial from a value and its range.
 * material: RpMaterial * (inferred); alpha byte is written at MATERIAL_ALPHA_OFFSET.
 * value_range: float[3] = { value, max, min } (inferred from the arithmetic).
 * alpha = 255 - clamp((value - min) / (max - min) * 255, 0, 255).
 * Returns material. */
int material_set_alpha_callback(int material,void *value_range)

{
  float scaled;
  char level;

  /* normalise value into 0..255 */
  scaled = (((*(float *)((char *)value_range + 0) - *(float *)((char *)value_range + RANGE_MIN_OFFSET)) / (*(float *)((char *)value_range + RANGE_MAX_OFFSET) - *(float *)((char *)value_range + RANGE_MIN_OFFSET))) * ALPHA_MAX);
  /* clamp to 0..255 (inferred from the ternaries) */
  level = ((int)((scaled > ALPHA_MAX) ? ALPHA_MAX : (!(scaled > ALPHA_MAX) && !(scaled < 0.0f) ? scaled : 0.0f)));
  /* alpha = 255 - level (-1 - level is ~level as a byte) */
  *(char *)(material + MATERIAL_ALPHA_OFFSET) = -1 - level;
  /* no-op store kept from the decompile (inferred: refresh of the colour word); do not drop */
  *(undefined4 *)(material + MATERIAL_COLOUR_OFFSET) = *(undefined4 *)(material + MATERIAL_COLOUR_OFFSET);
  return material;
}


