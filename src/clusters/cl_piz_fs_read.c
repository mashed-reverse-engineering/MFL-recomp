// cluster-entries: piz_fs_read piz_fs_open piz_fs_seek piz_fs_close piz_open_archive
/* Installed cluster piz_fs_read (tools/cluster_install.py from output/cluster_pilot_t18/piz_fs_read.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* compile: /GS (piz_open_archive keeps the stack cookie of the original) */
#include "mfl_tier2.h"
/* The game's own RenderWare file system over a .piz archive file (the Rt file system "piz"): archive header + a table of
   up to 256 entries of 0x80 bytes, files read through a 0x20000 byte window cache.  0x4c6740..0x4c7150.
   The original calls four small helpers with register arguments (piz_open_file eax = path, piz_note_result ecx = destination, eax = value,
   piz_fill_cache eax = file, ebx = offset, ecx = length, piz_align_down eax = value, ecx = alignment); here they are the static functions
   open_file / fill_cache / align_down below (piz_note_result is the store `file->last_result = value`). */

/* 0x80 byte entry of the archive file table */
struct PizEntry {
  char name[0x74];
  unsigned data_offset;         /* 0x74: offset of the data in the archive */
  unsigned size;                /* 0x78 */
  unsigned hash;                /* 0x7c: never read */
};

/* one open file of the file system (0x20078 bytes) */
struct PizFile {
  char pad0[8];
  int last_result;              /* 0x08: written by piz_note_result */
  char pad1[0x58];
  char cache[0x20000];          /* 0x64: read window */
  struct PizEntry *entry;       /* 0x20064 */
  unsigned cache_start;         /* 0x20068: file offsets covered by cache */
  unsigned cache_end;           /* 0x2006c */
  unsigned pos;                 /* 0x20070 */
  int pad2;
};

struct PizOverlapped {
  unsigned internal, internal_high, offset, offset_high;
  void *event;
};

typedef void (*PizProgressFunc)(char *archive, char *file, int user);

/* The Rt file system object (0x58 bytes) */
struct MflRtFileSystem {
  struct MflRtFileSystem *next;
  char pad0[0x10];
  void *(*get_object)(int fs, int index);       /* 0x14 */
  void (*close)(void);                          /* 0x18 */
  char pad1[0x0c];
  int (*open)(void *fs, struct PizFile *file, char *name);          /* 0x28 */
  void (*file_close)(struct PizFile *file);                         /* 0x2c */
  unsigned (*read)(struct PizFile *file, void *dst, unsigned n);    /* 0x30 */
  char pad2[4];
  __int64 (*seek)(struct PizFile *file, int offset, int mode);      /* 0x38 */
  char pad3[8];
  int (*is_eof)(struct PizFile *file);                              /* 0x44 */
  char pad4[4];
  int (*exists)(void *fs, char *name);                              /* 0x4c */
  char pad5[8];
};

/*0050b5e0*/ extern void nop_stub(void);
/*00560cf0*/ extern void __cdecl FUN_00560cf0(void *a);
/*00560d00*/ extern void *FUN_00560d00(void);
/*00560a60*/ extern RwBool RtFSManagerRegister(struct MflRtFileSystem *fs);
/*00560b00*/ extern struct MflRtFileSystem *RtFSManagerGetFileSystemFromName(RwChar *name);
/*00561900*/ extern int _rtFSInit(struct MflRtFileSystem *fs, RwInt32 p1, RwChar *name, RwChar *p3);
/*004b34a0*/ extern char *__cdecl _strncpy(char *dst, char *src, size_t n);
/*004b4358*/ extern int __cdecl _fclose(FILE *f);
/*005d3133*/ extern int __cdecl _fseek(FILE *f, long offset, int origin);
/*004b49bf*/ extern size_t __cdecl _fread(void *dst, size_t size, size_t count, FILE *f);
/*004c2b6f*/ extern int __cdecl __stricmp(char *a, char *b);
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
extern __declspec(dllimport) int __stdcall CloseHandle(void *handle);
extern __declspec(dllimport) int __stdcall ReadFile(void *handle, void *buf, unsigned n, unsigned *got, struct PizOverlapped *ov);
extern __declspec(dllimport) int __stdcall GetOverlappedResult(void *handle, struct PizOverlapped *ov, unsigned *got, int wait);
extern __declspec(dllimport) int __stdcall GetLastError(void);
extern __declspec(dllimport) void * __stdcall CreateFileA(char *name, unsigned access, unsigned share, void *security, unsigned disposition, unsigned flags, void *templ);
/*004b4531*/ extern FILE *__cdecl crt_fopen_body(char *name, char *mode);

/*0062d88c*/ extern RwChar *PTR_g_piz_fs_5e9c7c_0062d88c;      /* file system name */
/*0062d890*/ extern RwChar *PTR_g_piz_fs_5e9c74_0062d890;
/*007d1f78*/ extern void *g_piz_7d1f78;                   /* archive file handle (or FILE * when g_piz_7d1f80 & 1) */
/*007d1f7c*/ extern int g_piz_7d1f7c;                     /* archive is open */
/*007d1f80*/ extern unsigned g_piz_7d1f80;                /* bit 0: use stdio instead of overlapped Win32 reads */
/*007d1f84*/ extern struct MflRtFileSystem *g_piz_7d1f84;
/*007d1f88*/ extern void *g_piz_7d1f88;
/*007d1f8c*/ extern PizProgressFunc g_piz_7d1f8c;
/*007d1f90*/ extern int g_piz_7d1f90;
/*007d1f94*/ extern unsigned g_piz_7d1f94;                /* archive offsets covered by the window g_piz_8df020 */
/*007d1f98*/ extern unsigned g_piz_7d1f98;
/*008dad40*/ extern char g_piz_8dad40[0x100];             /* name of the file being opened */
/*008dae40*/ extern unsigned g_piz_8dae40[0x102b];        /* archive state block (name first) */
/*008daebf*/ extern char g_piz_open_8daebf;
/*008daec0*/ extern char g_piz_fs_8daec0[0x100][0x40];    /* log of the first 256 opened names */
/*008deec0*/ extern unsigned g_piz_fs_8deec0;             /* counters: exists, opens, reads, seeks, closes, eofs */
/*008deec4*/ extern unsigned g_piz_fs_8deec4;
/*008deec8*/ extern unsigned g_piz_fs_8deec8;
/*008deecc*/ extern unsigned g_piz_fs_8deecc;
/*008deed0*/ extern unsigned g_piz_fs_8deed0;
/*008deed4*/ extern unsigned g_piz_fs_8deed4;
/*008deed8*/ extern unsigned g_piz_fs_8deed8;             /* reads served by the file cache: calls, bytes */
/*008deedc*/ extern unsigned g_piz_fs_8deedc;
/*008deee0*/ extern unsigned g_piz_fs_8deee0;             /* reads served by the window: calls, bytes */
/*008deee4*/ extern unsigned g_piz_fs_8deee4;
/*008deee8*/ extern unsigned g_piz_fs_8deee8;             /* reads that had to refill the cache */
/*008def00*/ extern char g_piz_8def00[0x100];             /* name of the archive */
/*008df000*/ extern unsigned g_piz_open_8df000;           /* header fields */
/*008df004*/ extern unsigned g_piz_open_8df004;
/*008df008*/ extern unsigned g_piz_8df008;                /* entry count */
/*008df00c*/ extern unsigned g_piz_open_8df00c;
/*008df020*/ extern char g_piz_8df020[0x20000];           /* window */
/*008ff020*/ extern char g_data_8ff020[];                 /* file objects (0x20078 each) */
/*008ff058*/ extern struct MflRtFileSystem *g_piz_fs_8ff058;
/*008ff080*/ extern int g_piz_fs_8ff080;
/*0091f088*/ extern int g_data_91f088;
/*0091f08c*/ extern int g_data_91f08c;
/*0091f090*/ extern int g_data_91f090;
/*0091f0d0*/ extern struct MflRtFileSystem *g_piz_fs_91f0d0;
/*0091f0f8*/ extern int g_piz_fs_91f0f8;
/*0093f100*/ extern int g_data_93f100;
/*0093f104*/ extern int g_data_93f104;
/*0093f108*/ extern int g_data_93f108;
/*0093f120*/ extern struct PizEntry g_piz_93f120[0x100];  /* entry table */

int piz_fs_open(void *fs, struct PizFile *file, char *name);
void piz_fs_close(struct PizFile *file);
unsigned piz_fs_read(struct PizFile *file, void *dst, unsigned count);
int piz_fs_eof(struct PizFile *file);
__int64 piz_fs_seek(struct PizFile *file, int offset, int mode);
int piz_fs_exists(void *fs, char *name);
int __cdecl piz_stricmp(char *a, char *b);

/* already C elsewhere */
extern void piz_get_state(unsigned *out);
extern void piz_reset_counters(void);
extern void piz_fs_forget(void);
extern int piz_is_open(void);
extern void piz_close_file(void);

#define PIZ_SECTOR 0x800
#define PIZ_WINDOW 0x20000


/* opens the archive file (stdio when bit 0 of g_piz_7d1f80 is set); returns nonzero on success */
static int open_file(char *path)
{
  if ((g_piz_7d1f80 & 1) != 0) {
    g_piz_7d1f78 = crt_fopen_body(path, "rb");
    return (int)g_piz_7d1f78;
  }
  g_piz_7d1f78 = CreateFileA(path, 0x80000000, 0, 0, 3, 0x60000000, 0);
  if (GetLastError() == 0 && g_piz_7d1f78 != (void *)-1) {
    return 1;
  }
  return 0;
}

static int align_down(int value, int alignment)
{
  return value / alignment * alignment;
}

/* loads PIZ_WINDOW bytes of the file at offset into its cache and mirrors them to the global window */
static void fill_cache(struct PizFile *file, unsigned offset, unsigned length)
{
  unsigned got;

  got = length;
  piz_read_sectors(file->entry->data_offset + offset, file->cache, length, &got);
  file->cache_start = offset;
  file->cache_end = got + offset;
  g_piz_7d1f94 = file->entry->data_offset + offset;
  g_piz_7d1f98 = file->entry->data_offset + file->cache_end;
  memcpy(g_piz_8df020, file->cache, PIZ_WINDOW);
}

/* Reads the header and the file table of an archive; returns 1 on success. */
int piz_open_archive(char *path)
{
  unsigned got;
  unsigned *header;
  char buf[PIZ_SECTOR];
  unsigned offset, left, n, ok;

  memset(g_piz_8dae40, 0, sizeof(g_piz_8dae40));
  _strncpy((char *)g_piz_8dae40, path, 0x80);
  g_piz_open_8daebf = 0;
  if (__stricmp(g_piz_8def00, path) != 0) {
    g_piz_7d1f94 = 0;
    g_piz_7d1f98 = 0;
    memset(g_piz_8df020, 0, PIZ_WINDOW);
  }
  strcpy(g_piz_8def00, path);
  if (g_piz_7d1f8c != 0) {
    g_piz_7d1f8c(g_piz_8def00, g_piz_8dad40, g_piz_7d1f90);
  }
  piz_reset_counters();
  g_piz_7d1f78 = 0;
  if (!open_file(path)) {
    return 0;
  }
  g_piz_7d1f7c = 1;
  piz_read_sectors(0, buf, PIZ_SECTOR, &got);
  header = (unsigned *)buf;
  g_piz_open_8df000 = header[0];
  g_piz_open_8df00c = header[3];
  g_piz_open_8df004 = header[1];
  g_piz_8df008 = header[2];
  memset(g_piz_93f120, 0, sizeof(g_piz_93f120));
  offset = PIZ_SECTOR;
  left = header[2];
  n = 0;
  while (left != 0) {
    piz_read_sectors(offset, buf, PIZ_SECTOR, &got);
    offset += PIZ_SECTOR;
    ok = left < 0x10 ? left << 7 : PIZ_SECTOR;
    memcpy((char *)g_piz_93f120 + n, buf, ok);
    n += PIZ_SECTOR;
    left -= ok >> 7;
  }
  g_piz_7d1f88 = FUN_00560d00();
  FUN_00560cf0(g_piz_7d1f84);
  return 1;
}

/* Opens a file of the archive by name (case-insensitive); 1 = ok, 3 = not found. */
int piz_fs_open(void *fs, struct PizFile *file, char *name)
{
  unsigned i;
  struct PizEntry *entry;

  if (g_piz_fs_8deec4 < 0x100) {
    _strncpy(g_piz_fs_8daec0[g_piz_fs_8deec4], name, 0x40);
    g_piz_fs_8daec0[g_piz_fs_8deec4][0x3f] = 0;
  }
  g_piz_fs_8deec4++;
  strcpy(g_piz_8dad40, name);
  i = 0;
  if (g_piz_8df008 != 0) {
    entry = g_piz_93f120;
    do {
      if (piz_stricmp(entry->name, name) == 0) {
        file->entry = entry;
        memset(file->cache, 0, PIZ_WINDOW);
        file->cache_end = 0;
        file->cache_start = 0;
        file->pos = 0;
        file->last_result = 0;
        return 1;
      }
      i++;
      entry++;
    } while (i < g_piz_8df008);
  }
  return 3;
}

/* Reads count bytes at the file position; served from the file cache, the global window, or refilled in 0x20000 byte steps. */
unsigned piz_fs_read(struct PizFile *file, void *dst, unsigned count)
{
  unsigned size, pos, end, base, chunk, total, aligned;
  char *src, *out;

  g_piz_fs_8deec8++;
  if (count == 0) {
    return 0;
  }
  size = file->entry->size;
  pos = file->pos;
  total = 0;
  end = pos + count;
  if (end > size) {
    count = size - pos;
    end = size;
  }
  if (count == 0) {
    return 0;
  }
  if (pos >= file->cache_start && end <= file->cache_end) {
    memcpy(dst, file->cache + (pos - file->cache_start), count);
    file->pos += count;
    file->last_result = file->pos;
    g_piz_fs_8deedc += count;
    g_piz_fs_8deed8++;
    return count;
  }
  base = file->entry->data_offset;
  if (base + pos >= g_piz_7d1f94 && end + base <= g_piz_7d1f98) {
    memcpy(dst, g_piz_8df020 + (base + pos - g_piz_7d1f94), count);
    file->pos += count;
    file->last_result = file->pos;
    g_piz_fs_8deee4 += count;
    g_piz_fs_8deee0++;
    return count;
  }
  g_piz_fs_8deee8++;
  aligned = align_down(pos, PIZ_SECTOR);
  src = file->cache + (pos - aligned);
  out = (char *)dst;
  do {
    fill_cache(file, aligned, PIZ_WINDOW);
    chunk = count;
    if (end > file->cache_end) {
      chunk = PIZ_WINDOW - (src - file->cache);
    }
    memcpy(out, src, chunk);
    out += chunk;
    total += chunk;
    count -= chunk;
    src = file->cache;
    aligned = align_down(pos + total, PIZ_SECTOR);
  } while (count != 0);
  file->pos += total;
  file->last_result = file->pos;
  return total;
}

/* mode 1 = absolute, 2 = relative, 3 = from the end; returns the new position (the high word is whatever was on the stack). */
__int64 piz_fs_seek(struct PizFile *file, int offset, int mode)
{
  __int64 result;

  g_piz_fs_8deecc++;
  if (mode == 1) {
    file->pos = offset;
  } else if (mode == 2) {
    file->pos += offset;
  } else if (mode == 3) {
    file->pos = file->entry->size + offset;
  }
  file->last_result = file->pos;
  *(unsigned *)&result = file->pos;
  return result;
}

void piz_fs_close(struct PizFile *file)
{
  g_piz_fs_8deed0++;
  file->entry = 0;
  file->cache_start = 0;
  file->cache_end = 0;
  file->pos = 0;
  file->last_result = 0;
}

