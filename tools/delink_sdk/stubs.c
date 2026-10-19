/* M2 lib split stubs: IID_IUnknown (uuid.lib is not linked), errno (rtpng gzio.obj is built against single thread libc) */
typedef struct { unsigned long a; unsigned short b, c; unsigned char d[8]; } GUID_;
const GUID_ IID_IUnknown = {0x00000000, 0x0000, 0x0000, {0xC0,0,0,0,0,0,0,0x46}};
int errno;
/* source built Lua / qhull (plan_srclib.py): stdio the original exe does not link (Lua lua_dofile / parse_file and qhull's file readers are dead code there) */
void *fopen(const char *n, const char *m) { return 0; }
void *freopen(const char *n, const char *m, void *f) { return 0; }
char *fgets(char *s, int n, void *f) { return 0; }
int fgetc(void *f) { return -1; }
int ungetc(int c, void *f) { return -1; }
