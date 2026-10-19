// cluster-entries: movie_draw_fullscreen_quad
/* Installed cluster movie_draw_fullscreen_quad (tools/cluster_install.py from output/cluster_pilot8/movie_draw_fullscreen_quad.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster movie_draw_fullscreen_quad: draws the movie texture as an Im2D quad (two triangles) over a rectangle (given in
   fractions of the camera raster) with a constant colour; rhw from the camera, z at the near buffer. */
extern float float_1_0;
extern struct RwGlobalsRelease *RwEngineInstance;

typedef struct MovieCamera {
    char unknown_00[0x60];
    RwRaster *raster;      /* 0x60 */
    char unknown_64[0x1c];
    float view_window_scale; /* 0x80 */
} MovieCamera;

/* the frame of the original function, in its order (the harness compares the memory behind pointers into it) */
typedef struct QuadFrame {
    unsigned char white[4];
    short y_offset[2];
    short x_offset[2];
    int y_offset_int;
    float raster_width;
    float raster_height;
    float x_offset_f;
    float y_offset_f;
    float full_rect[4];    /* (0, 0) - (1, 1) */
    RwIm2DVertex vertices[4];
} QuadFrame;

/* was: ECX = rectangle min (u, v) or 0, EAX = colour RGBA bytes or 0, EBX = camera, stack = rectangle max or 0, texture raster */
void movie_draw_fullscreen_quad_n(const float *min, const unsigned char *rgba, MovieCamera *camera, const float *max, void *texture)
{
    QuadFrame f;
    RwRaster *raster;
    float x0, y0, x1, y1;

    f.full_rect[0] = 0.0f;
    f.full_rect[1] = 0.0f;
    f.full_rect[2] = 1.0f;
    f.full_rect[3] = 1.0f;
    f.white[0] = 0xff;
    f.white[1] = 0xff;
    f.white[2] = 0xff;
    f.white[3] = 0xff;
    if (min == 0)
        min = &f.full_rect[0];
    if (max == 0)
        max = &f.full_rect[2];
    if (rgba == 0)
        rgba = f.white;
    raster = camera->raster;
    f.raster_width = (float)RwRaster_width(raster);
    f.raster_height = (float)RwRaster_height(raster);
    RwRasterGetOffset(raster, f.x_offset, f.y_offset);
    f.x_offset_f = (float)(int)f.x_offset[0];
    f.y_offset_int = (int)f.y_offset[0];
    f.y_offset_f = (float)(int)f.y_offset[0];
    x0 = f.raster_width * min[0] + f.x_offset_f;
    y0 = f.raster_height * min[1] + f.y_offset_f;
    x1 = f.raster_width * max[0] + f.x_offset_f;
    y1 = f.raster_height * max[1] + f.y_offset_f;
    f.vertices[0].rhw = float_1_0 / camera->view_window_scale;
    f.vertices[0].x = x0;
    f.vertices[0].y = y0;
    f.vertices[0].z = RwIm2DGetNearScreenZ();
    f.vertices[0].u = 1.0f;
    f.vertices[0].v = 0.0f;
    f.vertices[1].x = x0;
    f.vertices[1].y = y1;
    f.vertices[1].z = RwIm2DGetNearScreenZ();
    f.vertices[1].rhw = f.vertices[0].rhw;
    f.vertices[1].u = 1.0f;
    f.vertices[1].v = 1.0f;
    f.vertices[2].x = x1;
    f.vertices[2].y = y1;
    f.vertices[2].z = RwIm2DGetNearScreenZ();
    f.vertices[2].rhw = f.vertices[0].rhw;
    f.vertices[2].u = 0.0f;
    f.vertices[2].v = 1.0f;
    f.vertices[3].x = x1;
    f.vertices[3].y = y0;
    f.vertices[3].z = RwIm2DGetNearScreenZ();
    f.vertices[3].rhw = f.vertices[0].rhw;
    f.vertices[3].u = 0.0f;
    f.vertices[3].v = 0.0f;
    f.vertices[0].emissiveColor = ((unsigned int)rgba[3] << 24) | ((unsigned int)rgba[0] << 16) | ((unsigned int)rgba[1] << 8) | rgba[2];
    f.vertices[1].emissiveColor = f.vertices[0].emissiveColor;
    f.vertices[2].emissiveColor = f.vertices[0].emissiveColor;
    f.vertices[3].emissiveColor = f.vertices[0].emissiveColor;
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void *)3);
    RwRenderStateSet(rwRENDERSTATESHADEMODE, (void *)1);
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, texture);
    if (rgba[3] != 0xff)
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)1);
    RwIm2DRenderTriangle(f.vertices, 4, 0, 1, 2);
    RwIm2DRenderTriangle(f.vertices, 4, 2, 3, 0);
    if (rgba[3] != 0xff)
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)0);
    RwRenderStateSet(rwRENDERSTATESHADEMODE, (void *)2);
}

/* entry thunk at the original address of movie_draw_fullscreen_quad: register inputs (ecx,eax,ebx,s0,s1) -> natural C movie_draw_fullscreen_quad_n; ret 0 */
__declspec(naked) void movie_draw_fullscreen_quad(void)
{
    __asm {
        push dword ptr [esp + 8]
        push dword ptr [esp + 8]
        push ebx
        push eax
        push ecx
        call movie_draw_fullscreen_quad_n
        add esp, 20
        ret
    }
}
