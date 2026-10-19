#include "mfl_tier2.h"
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*008c590c*/ extern undefined4 g_course_arctic_8c590c;
/* course_arctic_cb10 */

/* Draws the Arctic course's blended overlay quad (inferred).
 * No parameters, no return value. It switches the device to
 * src-alpha / one blending with depth test and depth write off, draws through
 * func_0x0042ce00 (callee not named yet), then restores depth on and
 * src-alpha / inv-src-alpha blending. The call's floats are 320.0, 240.0,
 * 640.0, 480.0 (inferred: x, y, width, height); the colour is the low byte of
 * g_course_arctic_8c590c over 0xffdcdc (inferred). */
void course_arctic_cb10(void)

{
  RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)rwBLENDSRCALPHA);
  RwRenderStateSet(rwRENDERSTATEDESTBLEND,(void *)rwBLENDONE);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
  /* draw the overlay quad with depth test and write off */
  func_0x0042ce00(0,0x43a00000,0x43700000,0x44200000,0x43f00000,2,
                  CONCAT13(g_course_arctic_8c590c,0xffdcdc));
  /* restore depth test/write and normal alpha blending */
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)rwBLENDSRCALPHA);
  RwRenderStateSet(rwRENDERSTATEDESTBLEND,(void *)rwBLENDINVSRCALPHA);
  return;
}


