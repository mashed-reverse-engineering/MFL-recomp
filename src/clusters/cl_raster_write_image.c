// cluster-entries: raster_write_image
/* Installed cluster raster_write_image (tools/cluster_install.py from output/cluster_pilot10/raster_write_image.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004dd550*/ extern struct RwImage * RwImageCreate(RwInt32 p0, RwInt32 p1, RwInt32 p2);
/*004dd5b0*/ extern RwBool RwImageDestroy(struct RwImage *p0);
/*004dd610*/ extern struct RwImage * RwImageAllocatePixels(struct RwImage *p0);
/*004de190*/ extern struct RwImage * RwImageWrite(struct RwImage *p0, RwChar *p1);
/*004e7440*/ extern struct RwImage * RwImageSetFromRaster(struct RwImage *p0, struct RwRaster *p1);
/* raster_write_image */

/* "creates an RwImage from the raster at +0x60 and writes it to the file name (screenshot)" */

void raster_write_image_n(int in_eax, RwChar *param_2)

{
  RwRaster *raster;
  RwImage *image;
  
  raster = *(RwRaster **)(in_eax + 0x60);
  if ((raster != (RwRaster *)0x0) &&
     (image = RwImageCreate(RwRaster_width(raster),RwRaster_height(raster),0x20), image != (RwImage *)0x0)) {
    RwImageAllocatePixels(image);
    RwImageSetFromRaster(image,raster);
    RwImageWrite(image,param_2);
    RwImageDestroy(image);
  }
  return;
}

/* entry thunk at the original address of raster_write_image: register inputs (eax,s0) -> natural C raster_write_image_n; ret 0 */
__declspec(naked) void raster_write_image(void)
{
    __asm {
        push dword ptr [esp + 4]
        push eax
        call raster_write_image_n
        add esp, 8
        ret
    }
}
