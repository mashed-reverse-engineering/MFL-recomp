/* mfl_fix.h: every MFL_FIX_* switch. Default 0 = original behaviour. Turn one on with /DMFL_FIX_<NAME>=1.
   Test a switch with `#if MFL_FIX_X`, never #ifdef (this header defines every switch).  Copies: recomp/automatch_near/mfl_fix.h (equal; tools/fixlint.py checks).
   A flag-on build must put its /D into build_overlay.py's flag list (and so into the object cache key) in the same change. */
#ifndef MFL_FIX_H
#define MFL_FIX_H

#ifndef MFL_FIX_TRAIL_NAN      /* trail strip refuses NaN / inf points (automatch_near/an_ok_055.c) */
#define MFL_FIX_TRAIL_NAN 0
#endif

#endif
