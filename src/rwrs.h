/* RenderWare render state ids (rwplcore.h) and the call through RwEngineInstance->dOpenDevice.fpRenderStateSet (cdecl, +0x20). */
#ifndef RWRS_H
#define RWRS_H
extern char *RwEngineInstance;
enum { RS_TEXTURERASTER = 1, RS_ZTEST = 6, RS_ZWRITE = 8, RS_SRCBLEND = 10, RS_DESTBLEND = 11, RS_FOGENABLE = 14, RS_CULLMODE = 20 };
#define RS_SET(a, b) (*(void (**)(int, int))(RwEngineInstance + 0x20))(a, b)
#endif
