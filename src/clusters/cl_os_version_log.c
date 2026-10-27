// cluster-entries: os_version_log
/* Installed cluster os_version_log (tools/cluster_install.py from output/cluster_pilot_kg/os_version_log.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall GetVersionExA(void *info);
extern __declspec(dllimport) int __stdcall RegOpenKeyExA(unsigned int key, const char *sub_key, int options, int sam, unsigned int *result);
extern __declspec(dllimport) int __stdcall RegQueryValueExA(unsigned int key, const char *name, int *reserved, int *type, unsigned char *data, unsigned int *size);
extern __declspec(dllimport) int __stdcall RegCloseKey(unsigned int key);
extern __declspec(dllimport) int __stdcall lstrcmpiA(const char *a, const char *b);
/*004a5550*/ extern int log_printf();
/*005e1680*/ extern char g_os_version_5e1680[];
/*00771a20*/ extern int g_os_771a20;
/* os_version_log */
/* compile: /GS */

#define HKEY_LOCAL_MACHINE_ 0x80000002
#define KEY_QUERY_VALUE_ 1
#define VER_PLATFORM_WIN32s_ 0
#define VER_PLATFORM_WIN32_WINDOWS_ 1
#define VER_PLATFORM_WIN32_NT_ 2
#define VER_NT_WORKSTATION_ 1
#define VER_NT_SERVER_ 3
#define VER_SUITE_ENTERPRISE_ 0x2
#define VER_SUITE_DATACENTER_ 0x80
#define VER_SUITE_PERSONAL_ 0x200

/* values stored in g_os_771a20 */
#define OS_NT4_OR_OLDER 1
#define OS_WIN95 2
#define OS_WIN98 3
#define OS_WINME 4
#define OS_WIN2000 5
#define OS_WINXP 6
#define OS_WIN32S 7

/* OSVERSIONINFOEXA (0x9c bytes; the plain OSVERSIONINFOA is the first 0x94) */
typedef struct OsVersionInfo {
  unsigned int size;
  unsigned int major;
  unsigned int minor;
  unsigned int build;
  unsigned int platform;
  char service_pack[128];
  unsigned short service_pack_major;
  unsigned short service_pack_minor;
  unsigned short suite_mask;
  unsigned char product_type;
  unsigned char reserved;
} OsVersionInfo;

/* "Operating System:" line of the log: Windows flavour, edition and build, and sets g_os_771a20 for the rest of the game */
void os_version_log(void)

{
  int ok;
  unsigned int key;
  unsigned int value_size;
  OsVersionInfo info = {0};
  unsigned char product[84];
  const char *edition;

  log_printf("Operating System:\n");
  info.size = sizeof(OsVersionInfo);
  ok = GetVersionExA(&info);
  if (ok == 0) {
    info.size = 0x94;
    if (GetVersionExA(&info) == 0) {
      log_printf("Unable to get OS version\n");
      return;
    }
  }
  if (info.platform == VER_PLATFORM_WIN32s_) {
    g_os_771a20 = OS_WIN32S;
    log_printf("Microsoft Win32s ");
  }
  else if (info.platform == VER_PLATFORM_WIN32_WINDOWS_) {
    if (info.major == 4) {
      if (info.minor == 0) {
        g_os_771a20 = OS_WIN95;
        log_printf("Microsoft Windows 95 ");
        if (info.service_pack[1] == 'C') {
          log_printf("OSR2 ");
        }
      }
      if (info.minor == 10) {
        g_os_771a20 = OS_WIN98;
        log_printf("Microsoft Windows 98 ");
        if (info.service_pack[1] == 'A') {
          log_printf(g_os_version_5e1680);
        }
      }
      if (info.minor == 0x5a) {
        g_os_771a20 = OS_WINME;
        log_printf("Microsoft Windows Me ");
      }
    }
  }
  else {
    if (info.platform != VER_PLATFORM_WIN32_NT_) {
      log_printf("Unknown OS");
      log_printf("\n");
      return;
    }
    if (info.major < 5) {
      g_os_771a20 = OS_NT4_OR_OLDER;
      log_printf("Microsoft Windows NT ");
    }
    if (info.major == 5) {
      if (info.minor == 0) {
        g_os_771a20 = OS_WIN2000;
        log_printf("Microsoft Windows 2000 ");
      }
      else if (info.minor == 1) {
        g_os_771a20 = OS_WINXP;
        log_printf("Whistler ");
      }
    }
    if (ok == 0) {
      RegOpenKeyExA(HKEY_LOCAL_MACHINE_, "SYSTEM\\CurrentControlSet\\Control\\ProductOptions", 0, KEY_QUERY_VALUE_, &key);
      RegQueryValueExA(key, "ProductType", 0, 0, product, &value_size);
      RegCloseKey(key);
      if (lstrcmpiA("WINNT", (const char *)product) == 0) {
        log_printf("Workstation ");
      }
      if (lstrcmpiA("SERVERNT", (const char *)product) == 0) {
        log_printf("Server ");
      }
    }
    else {
      edition = 0;
      if (info.product_type == VER_NT_WORKSTATION_) {
        edition = ((info.suite_mask & VER_SUITE_PERSONAL_) == 0) ? "Professional " : "Personal ";
      }
      else if (info.product_type == VER_NT_SERVER_) {
        if ((char)info.suite_mask < 0) {
          edition = "DataCenter Server ";
        }
        else if ((info.suite_mask & VER_SUITE_ENTERPRISE_) == 0) {
          edition = "Server ";
        }
        else {
          edition = "Advanced Server ";
        }
      }
      if (edition != 0) {
        log_printf(edition);
      }
    }
    if (info.major < 5) {
      log_printf("version %d.%d %s (Build %d)\n", info.major, info.minor, info.service_pack, info.build & 0xffff);
    }
    else {
      log_printf("%s (Build %d)\n", info.service_pack, info.build & 0xffff);
    }
  }
  log_printf("\n");
  return;
}

