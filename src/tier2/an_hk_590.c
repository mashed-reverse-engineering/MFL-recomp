#include "mfl_tier2.h"
/*0049c9a0*/ extern int fx_object_break_into_particles();
/*004b2b48*/ extern int _ftol();
/*004c40c0*/ extern int FUN_004c40c0(undefined4 p0);
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
#define ROTATE_DEGREES (10.0)
#define ALPHA_MAX (255.0f)
#define RGB_WHITE (0xffffff)
/* fx_object_update_frame */

/* "normalises the direction and translates the object frame" */

/*
 * Per-frame update of one fx object (inferred from the body).
 * fx_object_ptr: int array of the fx object: [0] = RwObject * (its parent frame is at +4),
 *   [1..3] = RwV3d translation/direction. The slot is reused below as the colour word.
 * fade_fraction: 0.0 = fully opaque, 1.0 = fully transparent (inferred).
 * Moves the frame by the raw vector, spins it ROTATE_DEGREES about the normalised direction,
 * fades the emitter colour, then calls fx_object_break_into_particles(object, 0).
 */
void __cdecl fx_object_update_frame(int *fx_object_ptr,undefined4 fade_fraction)

{
  RwFrame *frame;
  int *fx_object;
  RwRGBA col;
  int fx_emitter;
  RwV3d direction;

  fx_object = fx_object_ptr;
  frame = RwObject_parent((RwFrame *)((struct RwObject *)*fx_object_ptr));
  RwV3dNormalize(&direction,(RwV3d *)(fx_object_ptr + 1));
  RwFrameTranslate(frame,(RwV3d *)(fx_object_ptr + 1),rwCOMBINEPOSTCONCAT);
  RwFrameRotate(frame,&direction,ROTATE_DEGREES,rwCOMBINEPRECONCAT);
  fx_emitter = FUN_004c40c0(*fx_object_ptr);
  /* emitter present: store white with alpha = ALPHA_MAX * (1 - fade_fraction) in its +4 slot */
  if (fx_emitter != 0) {
    col.red = 0xff;
    col.green = 0xff;
    col.blue = 0xff;
    col.alpha = ((int)(ALPHA_MAX - (*(float *)&fade_fraction * ALPHA_MAX)));
    *(int *)(fx_emitter + 4) = *(int *)&col;
  }
  fx_object_break_into_particles(*fx_object,0);
  return;
}


