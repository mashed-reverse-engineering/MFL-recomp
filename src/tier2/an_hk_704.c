#include "mfl_tier2.h"
/*004b2b48*/ extern int _ftol();
#define ALPHA_MAX (255.0f)

/* value and its range: { value, max, min } */
typedef struct ValueRange {
  float value;
  float max;
  float min;
} ValueRange;

/* RpMaterial: the colour word at +4, also seen as four bytes (alpha is the last) */
typedef struct AlphaMaterial {
  char unknown_00[4];
  union {
    undefined4 word;
    char bytes[4];
  } colour;
} AlphaMaterial;

/* material_set_alpha_callback */

/* "RpMaterial callback: colour alpha = 0xFF - (int)value" */

/* Sets the colour alpha of an RpMaterial from a value and its range.
 * material: RpMaterial * (inferred); the alpha byte is the last byte of the colour word.
 * value_range: float[3] = { value, max, min } (inferred from the arithmetic).
 * alpha = 255 - clamp((value - min) / (max - min) * 255, 0, 255).
 * Returns material. */
AlphaMaterial *material_set_alpha_callback(AlphaMaterial *material,ValueRange *value_range)

{
  float scaled;
  char level;

  /* normalise value into 0..255 */
  scaled = (((value_range->value - value_range->min) / (value_range->max - value_range->min)) * ALPHA_MAX);
  /* clamp to 0..255 (inferred from the ternaries) */
  level = ((int)((scaled > ALPHA_MAX) ? ALPHA_MAX : (!(scaled > ALPHA_MAX) && !(scaled < 0.0f) ? scaled : 0.0f)));
  /* alpha = 255 - level (-1 - level is ~level as a byte) */
  material->colour.bytes[3] = -1 - level;
  /* no-op store kept from the decompile (inferred: refresh of the colour word); do not drop */
  material->colour.word = material->colour.word;
  return material;
}
