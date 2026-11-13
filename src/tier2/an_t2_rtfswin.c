/* compile: /G5 */
#include "mfl_tier2.h"
/* RenderWare's native Windows file system (rtfswin): the per-file functions the file system table points to
   (0x561190..0x5618fa).  Written from the disassembly of MFL.exe.  A file object is one 0x60 byte slot of the
   file system's table; its status is 1 closed, 2 idle, 3 busy (asynchronous request pending), 4 failed.
   Every Win32 error is stored in the RtFS manager (_rtFSManagerState()->last_error), as the original does: GetLastError
   first, then _rtFSManagerState (the order cl gives `_rtFSManagerState()->last_error = GetLastError();`, checked in the
   object; written as one statement because a separate temporary changes cl's register choice and with it which saved
   register a stubbed indirect call can clobber in diffemu). */

typedef struct WinFSOverlapped {          /* OVERLAPPED */
  DWORD internal, internal_high;
  DWORD offset, offset_high;
  HANDLE event;
} WinFSOverlapped;

typedef struct WinFSFile WinFSFile;
typedef struct WinFSystem WinFSystem;

typedef struct WinFSFileFuncs {           /* the file functions of a file system (+0x28 in it) */
  RwInt32 (*open)(WinFSystem *fs, WinFSFile *file, const RwChar *name, RwUInt32 flags);
  RwBool (*close)(WinFSFile *file);
  RwUInt32 (*read)(WinFSFile *file, void *buffer, RwUInt32 size);
  RwUInt32 (*write)(WinFSFile *file, const void *buffer, RwUInt32 size);
  uint64_t (*set_position)(WinFSFile *file, RwInt32 offset, RwInt32 mode);
  RwInt32 (*sync)(WinFSFile *file, RwBool wait);
  RwBool (*abort)(WinFSFile *file);
  RwBool (*is_eof)(WinFSFile *file);
  RwInt32 (*get_status)(WinFSFile *file);
  RwBool (*exists)(WinFSystem *fs, const RwChar *name);
} WinFSFileFuncs;

struct WinFSystem {                       /* 0x60 bytes */
  char pad00[0x0c];
  RwChar *root;                           /* 0x0c: device name the file names are built on */
  char pad10[0x18];
  WinFSFileFuncs funcs;                   /* 0x28 */
  char pad50[0x08];
  RwUInt32 sector_size;                   /* 0x58 */
  WinFSFile *files;                       /* 0x5c */
};

struct WinFSFile {                        /* 0x60 bytes */
  RwUInt32 length_lo, length_hi;          /* 0x00 */
  RwUInt32 pos_lo, pos_hi;                /* 0x08 */
  char pad10[0x08];
  RwInt32 async;                          /* 0x18: opened for overlapped I/O */
  char pad1c[0x04];
  RwInt32 status;                         /* 0x20 */
  RwInt32 opened;                         /* 0x24 */
  RwInt32 op;                             /* 0x28: 5 = asynchronous read */
  RwUInt32 transferred;                   /* 0x2c */
  void (*callback)(WinFSFile *file, RwUInt32 value, RwInt32 status, RwInt32 op, void *data);      /* 0x30: asynchronous completion */
  void *callback_data;                    /* 0x34 */
  WinFSystem *fs;                         /* 0x38 */
  char pad3c[0x04];
  HANDLE handle;                          /* 0x40 */
  WinFSOverlapped overlapped;             /* 0x44 */
  HANDLE event;                           /* 0x58 */
  char pad5c[0x04];
};

typedef struct RtFSManagerState {
  char pad00[0x10];
  DWORD last_error;                       /* 0x10 */
} RtFSManagerState;

enum { WINFS_CLOSED = 1, WINFS_IDLE = 2, WINFS_BUSY = 3, WINFS_FAILED = 4 };
enum { WINFS_OP_NONE = 1, WINFS_OP_SEEK = 4, WINFS_OP_READ = 5 };
#define WINFS_ERROR_IO_INCOMPLETE 0x3e4
#define WINFS_ERROR_IO_PENDING 0x3e5

extern __declspec(dllimport) HANDLE __stdcall CreateFileA(const char *name, DWORD access, DWORD share, void *security, DWORD creation, DWORD flags, HANDLE templ);
extern __declspec(dllimport) DWORD __stdcall GetFileSize(HANDLE file, DWORD *size_high);
extern __declspec(dllimport) DWORD __stdcall SetFilePointer(HANDLE file, LONG distance, LONG *distance_high, DWORD method);
extern __declspec(dllimport) DWORD __stdcall GetLastError(void);
extern __declspec(dllimport) RwBool __stdcall CloseHandle(HANDLE object);
extern __declspec(dllimport) HANDLE __stdcall CreateEventA(void *security, RwBool manual_reset, RwBool initial_state, const char *name);
extern __declspec(dllimport) RwBool __stdcall ReadFile(HANDLE file, void *buffer, DWORD size, DWORD *read, WinFSOverlapped *overlapped);
extern __declspec(dllimport) RwBool __stdcall WriteFile(HANDLE file, const void *buffer, DWORD size, DWORD *written, WinFSOverlapped *overlapped);
extern __declspec(dllimport) RwBool __stdcall GetOverlappedResult(HANDLE file, WinFSOverlapped *overlapped, DWORD *transferred, RwBool wait);
extern __declspec(dllimport) DWORD __stdcall GetFileAttributesA(const char *name);

/*00560ca0*/ extern RtFSManagerState *_rtFSManagerState(void);
/*005619e0*/ extern RwChar *_rtGetFileNameFromDeviceName(const RwChar *name);
/*00561a30*/ extern RwChar *_rtFSBuildFileName(RwChar *buffer, RwInt32 size, const RwChar *root, const RwChar *name, RwChar separator);
/*0053eac0*/ extern RwInt32 _rwerror(RwInt32 code, ...);      /* folded "return the first argument" body: here the low word of the 64 bit length */

/* Ends the pending asynchronous operation of file: calls its completion callback (file, position for op 4 else the
   bytes transferred, status, op, callback data) and marks it idle (op 1).  0x561b20; was a trap row, ported here
   because FRead / FSync call it. */
void _rtFSAsyncNotify(WinFSFile *file)
{
  RwInt32 op;

  if (file == NULL || file->async == 0) {
    return;
  }
  op = file->op;
  if (op != WINFS_OP_NONE && file->callback != NULL) {
    RwUInt32 value = (op == WINFS_OP_SEEK) ? file->pos_lo : file->transferred;
    file->op = WINFS_OP_NONE;
    file->callback(file, value, file->status, op, file->callback_data);
    return;
  }
  file->op = WINFS_OP_NONE;
}

/* Opens name on fs into file.  flags: 1 read, 2 write, 8 append, 0x10 asynchronous.  -> 1 ok, 3 open failed, 5 name too long. */
RwInt32 TkWinFSystemFOpen(WinFSystem *fs, WinFSFile *file, const RwChar *name, RwUInt32 flags)
{
  RwChar path[0x100];
  DWORD access, share, creation, attributes;
  HANDLE handle;

  if (_rtFSBuildFileName(path, sizeof path, fs->root, _rtGetFileNameFromDeviceName(name), '\\') == NULL) {
    return 5;
  }
  attributes = 0;
  access = 0;
  share = 0;
  if (flags & 0x10) {
    attributes = 0x60000000;              /* FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING */
    file->async = 1;
    file->status = WINFS_BUSY;
  } else {
    file->async = 0;
  }
  if (flags & 2) {
    share = 2;                            /* FILE_SHARE_WRITE */
    creation = (flags & 8) ? 4 : 2;       /* OPEN_ALWAYS : CREATE_ALWAYS */
    access = 0x40000000;                  /* GENERIC_WRITE */
    if (file->async == 0) attributes = 0x80;      /* FILE_ATTRIBUTE_NORMAL */
  } else {
    creation = 3;                         /* OPEN_EXISTING */
    attributes |= 1;                      /* FILE_ATTRIBUTE_READONLY */
  }
  if (flags & 1) {
    access |= 0x80000000;                 /* GENERIC_READ */
    share |= 1;                           /* FILE_SHARE_READ */
  }
  handle = CreateFileA(path, access, share, NULL, creation, attributes, NULL);
  file->handle = handle;
  if (handle == (HANDLE)-1) {
    return 3;
  }
  if (file->async == 0) file->status = WINFS_IDLE;
  file->opened = 1;
  file->pos_lo = 0;
  file->pos_hi = 0;
  file->length_lo = GetFileSize(handle, &file->length_hi);
  if (flags & 8) {
    file->pos_lo = SetFilePointer(file->handle, 0, NULL, 2);      /* FILE_END */
    file->pos_hi = 0;
  }
  _rtFSManagerState()->last_error = GetLastError();
  return 1;
}

RwBool TkWinFSystemFClose(WinFSFile *file)
{
  file->status = WINFS_CLOSED;
  return CloseHandle(file->handle);
}

/* Reads size bytes at the file position.  Asynchronous files start an overlapped read and return 0 (the result
   arrives through TkWinFSystemFSync); synchronous files return the bytes read. */
RwUInt32 TkWinFSystemFRead(WinFSFile *file, void *buffer, RwUInt32 size)
{
  WinFSFileFuncs *funcs;
  HANDLE handle;
  DWORD bytes_read;
  DWORD async_read;
  RwBool ok;
  RwUInt32 end;

  funcs = &file->fs->funcs;
  handle = file->handle;
  bytes_read = 0;
  if (file->async) file->op = WINFS_OP_READ;
  if (file->status != WINFS_IDLE) {
    return 0;
  }
  if (file->async) {
    async_read = 0;
    file->overlapped.event = file->event = CreateEventA(NULL, 0, 0, NULL);
    file->overlapped.offset = file->pos_lo;
    file->overlapped.offset_high = file->pos_hi;
    ok = ReadFile(handle, buffer, size, &async_read, &file->overlapped);
    _rtFSManagerState()->last_error = GetLastError();
    if (ok) {
      if (async_read != size) file->status = WINFS_FAILED;
      funcs->set_position(file, async_read, 2);
      file->status = WINFS_IDLE;
      _rtFSAsyncNotify(file);
      return bytes_read;
    }
    if (_rtFSManagerState()->last_error != WINFS_ERROR_IO_PENDING && _rtFSManagerState()->last_error != 0) {
      file->status = WINFS_FAILED;
      return bytes_read;
    }
    file->status = WINFS_BUSY;
    return bytes_read;
  }
  file->status = WINFS_BUSY;
  if (ReadFile(handle, buffer, size, &bytes_read, NULL)) {
    end = file->pos_lo + bytes_read;
    if ((RwInt32)end < (RwInt32)file->pos_lo) {      /* the original's carry test (signed): wraps to end + 0x80000001 */
      file->pos_lo = end + 0x80000001;
      file->pos_hi++;
      file->status = WINFS_IDLE;
    } else {
      file->status = WINFS_IDLE;
      file->pos_lo = end;
    }
  } else {
    file->status = WINFS_FAILED;
  }
  _rtFSManagerState()->last_error = GetLastError();
  return bytes_read;
}

/* Writes size bytes at the file position; -> the bytes written. */
RwUInt32 TkWinFSystemFWrite(WinFSFile *file, const void *buffer, RwUInt32 size)
{
  WinFSFileFuncs *funcs;
  HANDLE handle;
  DWORD written;
  DWORD async_written;
  RwBool ok;

  funcs = &file->fs->funcs;
  handle = file->handle;
  /* The original returns this variable uninitialised on the asynchronous path (and after a failed WriteFile that
     did not store): it lives in the stack slot of the argument file, so the value returned is the file pointer. */
  written = (DWORD)(mfl_uptr)file;
  if (file->async) {
    async_written = 0;
    file->overlapped.event = file->event = CreateEventA(NULL, 0, 0, NULL);
    file->overlapped.offset = file->pos_lo;
    file->overlapped.offset_high = file->pos_hi;
    ok = WriteFile(handle, buffer, size, &async_written, &file->overlapped);
    _rtFSManagerState()->last_error = GetLastError();
    if (ok) {
      if (async_written != size) file->status = WINFS_FAILED;
      funcs->set_position(file, async_written, 2);
      file->status = WINFS_IDLE;
      return written;
    }
    if (_rtFSManagerState()->last_error == WINFS_ERROR_IO_PENDING || _rtFSManagerState()->last_error == 0) {
      file->status = WINFS_BUSY;
      return written;
    }
    file->status = WINFS_FAILED;
    return written;
  }
  file->status = WINFS_BUSY;
  if (WriteFile(handle, buffer, size, &written, NULL)) {
    file->status = WINFS_IDLE;
    file->pos_lo += written;
    return written;
  }
  _rtFSManagerState()->last_error = GetLastError();
  file->status = WINFS_FAILED;
  return written;
}

/* mode 1 = from the start, 2 = from the current position, else from the end; -> the new position (high word 0). */
uint64_t TkWinFSystemSetPosition(WinFSFile *file, RwInt32 offset, RwInt32 mode)
{
  DWORD method;

  if (mode == 1) {
    method = 0;                           /* FILE_BEGIN */
  } else {
    method = (mode != 2) + 1;             /* FILE_CURRENT : FILE_END */
  }
  file->pos_lo = SetFilePointer(file->handle, offset, NULL, method);
  file->pos_hi = 0;
  return ((uint64_t)file->pos_hi << 32) | file->pos_lo;
}

/* Completes a pending asynchronous request (waits for it when wait is set); -> the file status. */
RwInt32 TkWinFSystemFSync(WinFSFile *file, RwBool wait)
{
  WinFSFileFuncs *funcs;
  DWORD transferred;

  funcs = &file->fs->funcs;
  if (file->async == 0 || file->status != WINFS_BUSY) {
    return file->status;
  }
  transferred = 0;
  file->transferred = _rwerror(file->length_lo, file->length_hi);
  if (file->op == WINFS_OP_READ) {
    if (!GetOverlappedResult(file->handle, &file->overlapped, &transferred, wait)) {
      _rtFSManagerState()->last_error = GetLastError();
      if (_rtFSManagerState()->last_error == WINFS_ERROR_IO_PENDING || _rtFSManagerState()->last_error == 0
          || _rtFSManagerState()->last_error == WINFS_ERROR_IO_INCOMPLETE) {
        return file->status;
      }
      file->transferred = 0;
      file->status = WINFS_FAILED;
      _rtFSAsyncNotify(file);
      return file->status;
    }
    funcs->set_position(file, transferred, 2);
    if (file->op == WINFS_OP_READ) file->transferred = transferred;
  }
  file->status = WINFS_IDLE;
  _rtFSAsyncNotify(file);
  return file->status;
}

RwBool TkWinFSystemFileExists(WinFSystem *fs, const RwChar *name)
{
  RwChar path[0x100];

  if (_rtFSBuildFileName(path, sizeof path, fs->root, _rtGetFileNameFromDeviceName(name), '\\') == NULL) {
    return 0;
  }
  return GetFileAttributesA(path) != (DWORD)-1;
}
