// cluster-entries: rain_splash_update
/* Installed cluster rain_splash_update (tools/cluster_install.py from output/cluster_pilot_zy/rain_splash_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00886020*/ extern int g_app_886020[600];
/*007ef7f4*/ extern float g_tick_seconds;
/*005ddc18*/ extern float k_f_1_1;
/*005dd814*/ extern float k_f_6;
/*005dd340*/ extern float k_f_0_5;
/*005e64a4*/ extern float k_f_0;
/*004d1500*/ extern struct RwFrame *RwFrameScale(struct RwFrame *frame, struct RwV3d *scale, enum RwOpCombineType combine);
/*004d1640*/ extern struct RwFrame *RwFrameRotate(struct RwFrame *frame, struct RwV3d *axis, RwReal angle, enum RwOpCombineType combine);
/*004d1460*/ extern struct RwFrame *RwFrameTranslate(struct RwFrame *frame, struct RwV3d *translation, enum RwOpCombineType combine);
extern double sin(double x);
#pragma intrinsic(sin)

#define SPLASH_COUNT 30
#define SPLASH_WORDS 20
#define SPLASH_F(p, i) (*(float *)((p) + (i)))

/* Animates the 30 rain splash objects (20 word records): state 1 rises, 2 falls (sine bounce in height), 3 fades out; each gets its frame rebuilt (scale, spin angle, position). */
void rain_splash_update(void)
{
  int *splash;
  int i;
  int state;
  float age;
  double height;
  struct RwFrame *frame;

  splash = g_app_886020;
  for (i = 0; i < SPLASH_COUNT; i++, splash += SPLASH_WORDS) {
    state = splash[0];
    if (state == 0) {
      continue;
    }
    SPLASH_F(splash, 4) = SPLASH_F(splash, 7) + SPLASH_F(splash, 4);
    SPLASH_F(splash, 6) = SPLASH_F(splash, 6) + SPLASH_F(splash, 9);
    age = g_tick_seconds + SPLASH_F(splash, 0x13);
    SPLASH_F(splash, 0x13) = age;
    if (state == 3) {
      height = SPLASH_F(splash, 0xd) * k_f_1_1;
      SPLASH_F(splash, 0xd) = (float)height;
      if (age >= k_f_0_5) {      /* NaN keeps the splash (original: fcomp / test ah,1 / jne) */
        splash[0] = 0;
      }
    } else {
      height = sin((double)age * k_f_6) * SPLASH_F(splash, 8);
      if (state != 1 || !(height < k_f_0)) {      /* NaN keeps state 1 (original: fcomp / test ah,5 / jp) */
        if (state != 2) {
          goto place;
        }
      } else {
        splash[0] = 2;
      }
      if (height > SPLASH_F(splash, 0xd)) {
        splash[0] = 3;
        splash[0x13] = 0;
        height = -SPLASH_F(splash, 8);
      }
      SPLASH_F(splash, 0xd) = (float)height;
    }
  place:
    SPLASH_F(splash, 5) = (float)(height + SPLASH_F(splash, 5));
    splash[0xf] = splash[0xf] + splash[0xe];
    frame = *(struct RwFrame **)(splash[1] + 4);
    RwFrameScale(frame, (struct RwV3d *)(splash + 10), rwCOMBINEREPLACE);
    RwFrameRotate(frame, (struct RwV3d *)(splash + 0x10), (float)splash[0xf], rwCOMBINEPOSTCONCAT);
    RwFrameTranslate(frame, (struct RwV3d *)(splash + 4), rwCOMBINEPOSTCONCAT);
  }
}

