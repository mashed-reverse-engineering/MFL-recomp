/* compile: /G5 */
#include "mfl_tier2.h"
/* RenderWare's native Windows file system (rtfswin): creation of a file system (0x5616f0).  Written from the
   disassembly of MFL.exe.  The file functions themselves are in an_t2_rtfswin.c. */

typedef struct WinFSystem WinFSystem;
typedef struct WinFSFile WinFSFile;

struct WinFSystem {                       /* 0x60 bytes */
  char pad00[0x14];
  void *get_object;                       /* 0x14 */
  void *close;                            /* 0x18 */
  char pad1c[0x0c];
  void *open;                             /* 0x28: the file functions */
  void *file_close;
  void *read;
  void *write;
  void *set_position;
  void *sync;
  void *abort;
  void *is_eof;
  void *get_status;
  void *exists;                           /* 0x4c */
  char pad50[0x08];
  DWORD sector_size;                      /* 0x58: bytes per sector of the device, 0 when unknown */
  WinFSFile *files;                       /* 0x5c */
};

struct WinFSFile {                        /* 0x60 bytes */
  char pad00[0x38];
  WinFSystem *fs;                         /* 0x38 */
  char pad3c[0x24];
};

typedef void *(*WinFSCallocFunc)(size_t count, size_t size, RwUInt32 hint);
#define RwCalloc RW_GLOBAL(0x114, WinFSCallocFunc)      /* memoryFuncs.rwcalloc */
#define WINFS_MEMORY_HINT 0x401be

extern __declspec(dllimport) int __stdcall GetDiskFreeSpaceA(const char *root, DWORD *sectors_per_cluster, DWORD *bytes_per_sector, DWORD *free_clusters, DWORD *total_clusters);

/*007d2128*/ extern char *RwEngineInstance;
/*0063a380*/ extern RwChar g_data_63a380[];       /* appended to the device name for GetDiskFreeSpaceA */
/*00560b00*/ extern WinFSystem *RtFSManagerGetFileSystemFromName(const RwChar *name);
/*00561900*/ extern RwInt32 _rtFSInit(WinFSystem *fs, RwInt32 max_files, const RwChar *fs_name, const RwChar *device_name);
extern void TkWinFSGetFileObject(void), TkWinFSClose(void), TkWinFSystemFOpen(void), TkWinFSystemFClose(void),
  TkWinFSystemFRead(void), TkWinFSystemFWrite(void), TkWinFSystemSetPosition(void), TkWinFSystemFSync(void),
  TkWinFSystemAbort(void), TkWinFSystemFEof(void), TkWinFSFileGetStatus(void), TkWinFSystemFileExists(void);

/* Creates the native file system fs_name on device_name with max_files file slots; NULL when the name is taken,
   memory runs out or the RtFS initialisation fails. */
WinFSystem *RtWinFSystemInit(RwInt32 max_files, RwChar *device_name, const RwChar *fs_name)
{
  RwChar root[0x100];
  DWORD sectors_per_cluster, bytes_per_sector, free_clusters, total_clusters;
  WinFSystem *fs;
  WinFSFile *file;
  RwInt32 i;

  if (RtFSManagerGetFileSystemFromName(fs_name) != NULL) {
    return NULL;
  }
  fs = (WinFSystem *)RwMalloc(sizeof(WinFSystem), WINFS_MEMORY_HINT);
  if (fs == NULL) {
    return NULL;
  }
  fs->get_object = (void *)TkWinFSGetFileObject;
  fs->close = (void *)TkWinFSClose;
  fs->open = (void *)TkWinFSystemFOpen;
  fs->file_close = (void *)TkWinFSystemFClose;
  fs->read = (void *)TkWinFSystemFRead;
  fs->write = (void *)TkWinFSystemFWrite;
  fs->set_position = (void *)TkWinFSystemSetPosition;
  fs->sync = (void *)TkWinFSystemFSync;
  fs->abort = (void *)TkWinFSystemAbort;
  fs->is_eof = (void *)TkWinFSystemFEof;
  fs->get_status = (void *)TkWinFSFileGetStatus;
  fs->exists = (void *)TkWinFSystemFileExists;
  fs->files = (WinFSFile *)RwCalloc(max_files, sizeof(WinFSFile), WINFS_MEMORY_HINT);
  file = fs->files;
  for (i = 0; i < max_files; i++) {
    file->fs = fs;
    file++;
  }
  if (_rtFSInit(fs, max_files, fs_name, device_name) != 1) {
    RwFree(fs->files);
    RwFree(fs);
    return NULL;
  }
  rwstrcpy(root, device_name);
  rwstrcat(root, g_data_63a380);
  fs->sector_size = GetDiskFreeSpaceA(root, &sectors_per_cluster, &bytes_per_sector, &free_clusters, &total_clusters)
                  ? bytes_per_sector : 0;
  return fs;
}
