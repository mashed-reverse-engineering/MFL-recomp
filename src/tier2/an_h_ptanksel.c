#include "mfl_tier2.h"
/* RenderWare particle tank plugin (rpptank): rpPTankGENInstanceSelectCB (0x546d90), which picks the generic instancing
   callbacks of a tank from its data flags.  Written from the disassembly of MFL.exe.  It only stores callback
   addresses; nine of the callbacks are trap rows the original references here too (tools/delink/trap_caller_allow.txt). */

enum {                                    /* RpPTankDataFlags used here */
  rpPTANKDFLAGCOLOR = 0x00000002,
  rpPTANKDFLAGSIZE = 0x00000004,
  rpPTANKDFLAGMATRIX = 0x00000008,
  rpPTANKDFLAGNORMAL = 0x00000010,
  rpPTANKDFLAG2DROTATE = 0x00000020,
  rpPTANKDFLAGVTXCOLOR = 0x00000040,
  rpPTANKDFLAGVTX2TEXCOORDS = 0x00000080,
  rpPTANKDFLAGVTX4TEXCOORDS = 0x00000100,
  rpPTANKDFLAGCNSMATRIX = 0x00008000,
  rpPTANKDFLAGCNSVTXCOLOR = 0x00040000,
  rpPTANKDFLAGCNSVTX2TEXCOORDS = 0x00080000,
  rpPTANKDFLAGCNSVTX4TEXCOORDS = 0x00100000,
  rpPTANKDFLAGUSECENTER = 0x01000000
};

typedef void (*PTankCallback)(void);

typedef struct RpPTankAtomicExtPrv {      /* the fields used here */
  char pad00[0x24];
  PTankCallback ins_setup;                /* 0x24 */
  PTankCallback ins_pos;                  /* 0x28 */
  PTankCallback ins_uv;                   /* 0x2c */
  PTankCallback ins_colors;               /* 0x30 */
  PTankCallback ins_normals;              /* 0x34 */
  PTankCallback ins_ending;               /* 0x38 */
  char pad3c[0xa0 - 0x3c];
  RwUInt32 data_flags;                    /* 0xa0: publicData.format.dataFlags */
} RpPTankAtomicExtPrv;

/*007da6bc*/ extern RwInt32 _rpPTankAtomicDataOffset;

/*005485c0*/ extern void rpPTankGENInsSetup(void);
/*00548910*/ extern void rpPTankGENInsEnding(void);
/*00547fd0*/ extern void rpPTankGENInsPosCCPM(void);
/*00547ea0*/ extern void rpPTankGENInsPosNCPM(void);
/*005470e0*/ extern void rpPTankGENInsPosCCCSNR(void);
/*00546f50*/ extern void rpPTankGENInsPosNCCSNR(void);
/*00547800*/ extern void rpPTankGENInsPosCCCSPR(void);
/*005472a0*/ extern void rpPTankGENInsPosNCCSPR(void);
/*00547c20*/ extern void rpPTankGENInsPosCCPSPR(void);
/*005475f0*/ extern void rpPTankGENInsPosNCPSPR(void);
/*00547a70*/ extern void rpPTankGENInsPosCCPSNR(void);
/*005474a0*/ extern void rpPTankGENInsPosNCPSNR(void);
/*00548380*/ extern void rpPTankGENInsUV2(void);
/*005483f0*/ extern void rpPTankGENInsUV4(void);
/*00548470*/ extern void rpPTankGENInsUV2C(void);
/*005484e0*/ extern void rpPTankGENInsUV4C(void);
/*00548130*/ extern void rpPTankGENInsColorPP(void);
/*00548190*/ extern void rpPTankGENInsColorPV(void);
/*00548250*/ extern void rpPTankGENInsColorPVC(void);
/*00548330*/ extern void rpPTankGENInsColorPPC(void);
/*00548550*/ extern void rpPTankGENInsNormals(void);

#define HAS(flags, f) (((flags) & (f)) == (f))
#define PICK_CENTER(prv, cc, nc) ((prv)->ins_pos = HAS((prv)->data_flags, rpPTANKDFLAGUSECENTER) ? (cc) : (nc))

/* TIER2: unvalidated 2026-11-14 authority=diffemu */
void rpPTankGENInstanceSelectCB(void *atomic)
{
  RpPTankAtomicExtPrv *prv = *(RpPTankAtomicExtPrv **)((char *)atomic + _rpPTankAtomicDataOffset);

  prv->ins_setup = rpPTankGENInsSetup;

  if (HAS(prv->data_flags, rpPTANKDFLAGMATRIX)) {
    PICK_CENTER(prv, rpPTankGENInsPosCCPM, rpPTankGENInsPosNCPM);
  } else if (!HAS(prv->data_flags, rpPTANKDFLAGCNSMATRIX) && HAS(prv->data_flags, rpPTANKDFLAG2DROTATE)) {
    if (HAS(prv->data_flags, rpPTANKDFLAGSIZE)) {
      PICK_CENTER(prv, rpPTankGENInsPosCCPSPR, rpPTankGENInsPosNCPSPR);
    } else {
      PICK_CENTER(prv, rpPTankGENInsPosCCCSPR, rpPTankGENInsPosNCCSPR);
    }
  } else if (!HAS(prv->data_flags, rpPTANKDFLAGCNSMATRIX) && HAS(prv->data_flags, rpPTANKDFLAGSIZE)) {
    PICK_CENTER(prv, rpPTankGENInsPosCCPSNR, rpPTankGENInsPosNCPSNR);
  } else {
    PICK_CENTER(prv, rpPTankGENInsPosCCCSNR, rpPTankGENInsPosNCCSNR);      /* constant matrix, or neither size nor rotation */
  }

  if (HAS(prv->data_flags, rpPTANKDFLAGVTX2TEXCOORDS)) {
    prv->ins_uv = rpPTankGENInsUV2;
  } else if (HAS(prv->data_flags, rpPTANKDFLAGVTX4TEXCOORDS)) {
    prv->ins_uv = rpPTankGENInsUV4;
  } else if (HAS(prv->data_flags, rpPTANKDFLAGCNSVTX2TEXCOORDS)) {
    prv->ins_uv = rpPTankGENInsUV2C;
  } else if (HAS(prv->data_flags, rpPTANKDFLAGCNSVTX4TEXCOORDS)) {
    prv->ins_uv = rpPTankGENInsUV4C;
  } else {
    prv->ins_uv = NULL;
  }

  if (HAS(prv->data_flags, rpPTANKDFLAGCOLOR)) {
    prv->ins_colors = rpPTankGENInsColorPP;
  } else if (HAS(prv->data_flags, rpPTANKDFLAGVTXCOLOR)) {
    prv->ins_colors = rpPTankGENInsColorPV;
  } else if (HAS(prv->data_flags, rpPTANKDFLAGCNSVTXCOLOR)) {
    prv->ins_colors = rpPTankGENInsColorPVC;
  } else {
    prv->ins_colors = rpPTankGENInsColorPPC;
  }

  prv->ins_ending = rpPTankGENInsEnding;
  prv->ins_normals = HAS(prv->data_flags, rpPTANKDFLAGNORMAL) ? rpPTankGENInsNormals : NULL;
}
