#include <stdio.h>
typedef struct { float x, y, z; } RwV3d;
extern float RwV3dLength(const RwV3d *in);
extern int RwEngineInit(void *mem, unsigned flags, unsigned arena);
extern unsigned RwEngineGetVersion(void);
int main(void)
{
    RwV3d v;
    printf("RwEngineInit -> %d\n", RwEngineInit(0, 0, 0)); fflush(stdout);
    v.x = 3.0f; v.y = 4.0f; v.z = 0.0f;
    printf("RenderWare SDK linked: RwV3dLength(3,4,0) = %f\n", (double)RwV3dLength(&v));
    return 0;
}
/* application supplied RenderWare file system hooks (the skeleton provides them in a real application): stubs */
void *RwFopen(const char *n, const char *m) { return 0; }
int RwFclose(void *f) { return 0; }
unsigned RwFread(void *p, unsigned s, unsigned n, void *f) { return 0; }
unsigned RwFwrite(const void *p, unsigned s, unsigned n, void *f) { return 0; }
int RwFseek(void *f, long o, int w) { return 0; }
long RwFtell(void *f) { return 0; }
int RwFeof(void *f) { return 1; }
int RwFexist(const char *n) { return 0; }
char *RwFgets(char *b, int n, void *f) { return 0; }
int RwFputs(const char *b, void *f) { return 0; }
int RwFflush(void *f) { return 0; }
