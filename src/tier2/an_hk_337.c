#include "mfl_tier2.h"
/*004819d0*/ extern int atomic_plugin_register();
/*00481cd0*/ extern int material_plugin_register();
/*004f5ed0*/ extern RwBool __cdecl RpWorldPluginAttach(void);
/*0053de30*/ extern RwBool __cdecl RtAnimInitialize(void);
/*0053e870*/ extern RwBool __cdecl RpSkinPluginAttach(void);
/*00543c50*/ extern RwBool __cdecl RpHAnimPluginAttach(void);
/*00544f30*/ extern int RpRandomPluginAttach();
/*00545090*/ extern int RpPTankPluginAttach();
/*00548c10*/ extern RwBool __cdecl RpPVSPluginAttach(void);
/*0054d6c0*/ extern RwBool __cdecl RpCollisionPluginAttach(void);
/*0054f0c0*/ extern RwBool __cdecl RpMatFXPluginAttach(void);
/*00554470*/ extern int RpUserDataPluginAttach();
/*00555350*/ extern int RpMorphPluginAttach();
/*00556b60*/ extern RwBool RpSplinePluginAttach();
/*00559c70*/ extern int RpPatchPluginAttach();
/*00561b70*/ extern RwBool RpUVAnimPluginAttach();
/*00567e70*/ extern RwBool __cdecl RpLODAtomicPluginAttach(void);
/*005738e0*/ extern int FUN_005738e0(void);
/*0058c900*/ extern int FUN_0058c900(void);
/* rw_plugins_attach */

/* "RpWorld / UserData / MatFX / Collision / PVS plugin attach" */

/*
 * Attaches the RenderWare plugins the game needs, in order: world, user data,
 * MatFX, collision, PVS, tank, random, spline, HAnim, skin, morph, patch,
 * LOD atomic, UV anim. Each step stops the chain on failure.
 * Returns true only if every step succeeded (inferred).
 */
bool rw_plugins_attach(void)

{
  RwBool rw_ok;
  int ok;

  rw_ok = RpWorldPluginAttach();
  if (rw_ok != 0) {
    ok = RpUserDataPluginAttach();
    if (ok != 0) {
      rw_ok = RpMatFXPluginAttach();
      if (rw_ok != 0) {
        ok = RpCollisionPluginAttach();
        if (ok != 0) {
          ok = RpPVSPluginAttach();
          if (ok != 0) {
            ok = RpPTankPluginAttach();
            if (ok != 0) {
              ok = RpRandomPluginAttach();
              if (ok != 0) {
                rw_ok = RpSplinePluginAttach();
                if (rw_ok != 0) {
                  rw_ok = RpHAnimPluginAttach();
                  if (rw_ok != 0) {
                    rw_ok = RpSkinPluginAttach();
                    if (rw_ok != 0) {
                      ok = RpMorphPluginAttach();
                      if (ok != 0) {
                        ok = RpPatchPluginAttach();
                        if (ok != 0) {
                          ok = RpLODAtomicPluginAttach();
                          if (ok != 0) {
                            rw_ok = RpUVAnimPluginAttach();
                            if (rw_ok != 0) {
                              /* anim init, then material, FUN_0058c900, FUN_005738e0 and atomic plugins (inferred) */
                              RtAnimInitialize();
                              ok = material_plugin_register();
                              if (ok != 0) {
                                ok = FUN_0058c900();
                                if (ok != 0) {
                                  ok = FUN_005738e0();
                                  if (ok != 0) {
                                    ok = atomic_plugin_register();
                                    return ok != 0;
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return false;
}


