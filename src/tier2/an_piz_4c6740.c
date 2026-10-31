#include "mfl_tier2.h"
/* The game's own RenderWare file system over a .piz archive file (the Rt file system "piz"): archive header + a table of
   up to 256 entries of 0x80 bytes, files read through a 0x20000 byte window cache.  0x4c6740..0x4c7150.
   Not in this file because they take their arguments in registers (not expressible in C): piz_open_file (eax = path),
   piz_note_result (ecx = destination, eax = value), piz_fill_cache (eax = file, ebx = offset, ecx = length),
   piz_align_down (eax = value, ecx = alignment); they are called through the naked adapters *_r below. */

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

/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void piz_set_progress_callback_impl(PizProgressFunc func, int user)
{
  g_piz_7d1f8c = func;
  g_piz_7d1f90 = user;
}

/* fsGetObject: the file object with that index */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void *piz_fs_get_object(int fs, int index)
{
  return g_data_8ff020 + index * sizeof(struct PizFile);
}

/* Creates and registers the "piz" file system; flags bit 0 selects stdio reads. */
/* TIER2: unvalidated 2026-10-31 authority=static */
void piz_fs_create_impl(unsigned flags)
{
  struct MflRtFileSystem *fs;
  int i;
  int *clear;

  RtFSManagerGetFileSystemFromName(PTR_g_piz_fs_5e9c7c_0062d88c);
  fs = (struct MflRtFileSystem *)RwMalloc(sizeof(struct MflRtFileSystem), 0);
  clear = (int *)fs;
  for (i = 0x16; i != 0; i--) {
    *clear = 0;
    clear++;
  }
  fs->get_object = piz_fs_get_object;
  fs->close = nop_stub;
  fs->open = piz_fs_open;
  fs->file_close = piz_fs_close;
  fs->read = piz_fs_read;
  fs->is_eof = piz_fs_eof;
  fs->seek = piz_fs_seek;
  fs->exists = piz_fs_exists;
  g_piz_7d1f80 = flags;
  g_piz_fs_8ff080 = 0;
  g_piz_fs_91f0f8 = 1;
  g_piz_7d1f84 = fs;
  g_piz_fs_8ff058 = fs;
  g_piz_fs_91f0d0 = fs;
  _rtFSInit(fs, 2, PTR_g_piz_fs_5e9c7c_0062d88c, PTR_g_piz_fs_5e9c74_0062d890);
  RtFSManagerRegister(fs);
}

/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void piz_close_archive_impl(void)
{
  piz_close_file();
  g_piz_7d1f7c = 0;
  FUN_00560cf0(g_piz_7d1f88);
  g_piz_7d1f88 = 0;
  g_piz_7d1f78 = 0;
  piz_reset_counters();
}

/* Reads size bytes at archive offset into buf (stdio, or overlapped ReadFile with the progress callback while pending). */
/* TIER2: validated 2026-10-31 authority=diffemu */
void piz_read_sectors(unsigned offset, void *buf, unsigned size, unsigned *got)
{
  struct PizOverlapped ov;
  int ok;
  int err;
  int done;

  if ((g_piz_7d1f80 & 1) != 0) {
    _fseek((FILE *)g_piz_7d1f78, offset, 0);
    *got = _fread(buf, 1, size, (FILE *)g_piz_7d1f78);
    return;
  }
  ov.internal = 0;
  ov.internal_high = 0;
  ov.offset_high = 0;
  ov.event = 0;
  ov.offset = offset;
  ReadFile(g_piz_7d1f78, buf, size, 0, &ov);
  GetLastError();
  for (;;) {
    done = 0;
    do {
      if (g_piz_7d1f8c != 0) {
        g_piz_7d1f8c(g_piz_8def00, g_piz_8dad40, g_piz_7d1f90);
      }
      ok = GetOverlappedResult(g_piz_7d1f78, &ov, got, 0);
      err = GetLastError();
      if (ok == 0) {
        if (err == 0x3e4) {
          break;
        }
      } else if (ok == 1) {
        done = 1;
      }
      if (err == 0x26 || done) {
        return;
      }
    } while (1);
  }
}

/* size of the archive entry with that name, 0 if there is none */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
unsigned piz_find_entry_size_impl(char *name)
{
  unsigned i;
  struct PizEntry *entry;

  i = 0;
  if (g_piz_8df008 != 0) {
    entry = g_piz_93f120;
    do {
      if (__stricmp(entry->name, name) == 0) {
        return entry->size;
      }
      i++;
      entry++;
    } while (i < g_piz_8df008);
  }
  return 0;
}

/*004c6ac0*/ extern int piz_open_archive(char *path);

/* TIER2: validated 2026-10-31 authority=diffemu */
int piz_open_archive_bool(char *path)
{
  return piz_open_archive(path) != 0;
}

/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int __cdecl piz_stricmp(char *a, char *b)
{
  return __stricmp(a, b);
}
