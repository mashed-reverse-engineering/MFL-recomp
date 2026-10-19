#include "mfl_tier2.h"
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004d1e80*/ extern void FUN_004d1e80(undefined4 p0);
/*004d77e0*/ extern RwBool RwRasterDestroy(struct RwRaster *p0);

/* byte offsets of the pointers inside an object record */
#define RECORD_FRAME_OFFSET (4)
#define RECORD_RASTER0_OFFSET (0x60)
#define RECORD_RASTER1_OFFSET (100)

/* aux_frame_destroy */

/* Destroys the RwFrame and the two RwRasters held by an object record, then passes the record to FUN_004d1e80
   (inferred: frees the record). Does nothing if record is NULL. record is an int because the record is not a struct
   in ghidra_types.h; the offsets below are byte offsets into it. */

void aux_frame_destroy(char *record)

{
  if (record != 0) {
    if (*(RwFrame **)(record + RECORD_FRAME_OFFSET) != (RwFrame *)0x0) {
      RwFrameDestroy(*(RwFrame **)(record + RECORD_FRAME_OFFSET));
    }
    if (*(RwRaster **)(record + RECORD_RASTER0_OFFSET) != (RwRaster *)0x0) {
      RwRasterDestroy(*(RwRaster **)(record + RECORD_RASTER0_OFFSET));
    }
    if (*(RwRaster **)(record + RECORD_RASTER1_OFFSET) != (RwRaster *)0x0) {
      RwRasterDestroy(*(RwRaster **)(record + RECORD_RASTER1_OFFSET));
    }
    FUN_004d1e80(record);
  }
  return;
}


