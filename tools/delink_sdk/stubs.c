/* M2 lib split stubs: IID_IUnknown (uuid.lib is not linked), errno (rtpng gzio.obj is built against single thread libc) */
typedef struct { unsigned long a; unsigned short b, c; unsigned char d[8]; } GUID_;
const GUID_ IID_IUnknown = {0x00000000, 0x0000, 0x0000, {0xC0,0,0,0,0,0,0,0x46}};
int errno;
