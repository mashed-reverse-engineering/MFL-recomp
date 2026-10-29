#include "mfl_tier2.h"
extern __declspec(dllimport) unsigned long __stdcall GetCurrentDirectoryA(unsigned long size, char *buffer);
extern __declspec(dllimport) char * __stdcall CharUpperA(char *text);
extern __declspec(dllimport) unsigned long __stdcall GetLogicalDrives(void);
extern __declspec(dllimport) unsigned int __stdcall GetDriveTypeA(const char *root);
/*005dd5a0*/ extern unsigned short g_course_5dd5a0;
/*005e12d8*/ extern char PTR_g_data_636e75_005e12d8[];
/*005616f0*/ extern void * __cdecl FUN_005616f0(int size, const char *name, const void *data);
/*00560a60*/ extern int __cdecl RtFSManagerRegister(void *file_system);
/*00560cf0*/ extern void __cdecl FUN_00560cf0(void *file_system);
/* compile: /GS */

/* game_register_native_filesystems */

/*
 * Registers an RtFS native file system for the current directory and one for every logical drive (DRIVE_UNKNOWN and
 * DRIVE_NO_ROOT_DIR excluded) and makes one of them the default: the current directory's when it has no drive letter,
 * else the one of its drive. Returns 1, or 0 when the directory cannot be read or a registration fails.
 */
/* TIER2: unvalidated 2026-10-29 authority=diffemu */
int game_register_native_filesystems(void)
{
  char cwd[0x104] = {0};
  char drive_name[8];
  unsigned long length;
  unsigned long drives;
  unsigned int drive_type;
  int drive;
  char *end;
  void *cwd_fs;
  void *drive_fs;

  length = GetCurrentDirectoryA(0x104, cwd);
  if (length == 0 || length > 0x104) {
    return 0;
  }
  end = cwd;
  while (*end != '\0') {
    end++;
  }
  *(unsigned short *)end = g_course_5dd5a0;
  cwd_fs = FUN_005616f0(0x14, cwd, PTR_g_data_636e75_005e12d8);
  if (cwd_fs == 0 || RtFSManagerRegister(cwd_fs) == 0) {
    return 0;
  }
  CharUpperA(cwd);
  drive = 2;
  for (drives = GetLogicalDrives() >> 1; drives != 0; drives >>= 1) {
    if (drives & 1) {
      drive_name[0] = (char)(drive + '@');
      drive_name[1] = ':';
      drive_name[2] = '\\';
      drive_name[3] = 0;
      drive_type = GetDriveTypeA(drive_name);
      if (drive_type != 0 && drive_type != 1) {
        drive_name[2] = 0;
        drive_name[4] = drive_name[0];
        drive_name[5] = 0;
        drive_fs = FUN_005616f0(0x14, drive_name, drive_name + 4);
        if (drive_fs == 0 || RtFSManagerRegister(drive_fs) == 0) {
          return 0;
        }
        if (cwd[1] != ':') {
          FUN_00560cf0(cwd_fs);
        } else if (drive_name[0] == cwd[0]) {
          FUN_00560cf0(drive_fs);
        }
      }
    }
    drive++;
  }
  return 1;
}
