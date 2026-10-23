#include "mfl_tier2.h"
/*004a93a0*/ extern char *app_get_71908();
/*004b3ab9*/ extern char *strtok();
/*004c2b6f*/ extern int __cdecl __stricmp(char *p0, char *p1);
/*0062ad34*/ extern undefined4 g_app_62ad34;
/*0062ad38*/ extern undefined4 g_app_62ad38;
/*0076e86c*/ extern undefined4 g_app_76e86c;
/*0076e870*/ extern undefined4 g_app_76e870;
/*0076e874*/ extern undefined4 g_language_offset;
#define CMD_LINE_BUF_SIZE (256)
#define CMD_LINE_DELIM (0x20)
/* app_parse_command_line */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "parses the command line (app_get_71908) with __stricmp" */

/*
 * Parses the game's command line and sets the option globals it finds.
 * Takes no parameters. Copies the string returned by app_get_71908() into a
 * local buffer, splits it on spaces with strtok, and compares each token with
 * __stricmp:
 *   -vs0 / -vs1  set g_app_62ad34 and g_app_76e86c to 0 / 1
 *   -cs0 / -cs1  set g_app_62ad38 and g_app_76e870 to 0 / 1
 *   -l0 .. -l9   set g_language_offset to N
 * Unknown tokens are ignored. Always returns 1.
 * (The meaning of the vs / cs prefixes is unknown; the globals are not named
 * in this file.)
 */

undefined4 app_parse_command_line(void)

{
  char ch;
  char *cmdline_src;
  int cmp_result;
  char *token;
  char *tokenize_input;
  struct {
    undefined2 local_108 [2];
    char local_104 [CMD_LINE_BUF_SIZE];
  } frame;
  
  tokenize_input = frame.local_104;
  frame.local_108[0] = CMD_LINE_DELIM;
  cmdline_src = (char *)app_get_71908();
  /* Copy the command line, byte by byte including the terminator, into
     frame.local_104. `token` is reused here as the write cursor.
     NOTE: no bounds check against CMD_LINE_BUF_SIZE; a longer command line
     overflows the buffer. */
  token = frame.local_104;
  do {
    ch = *cmdline_src;
    cmdline_src = cmdline_src + 1;
    *token = ch;
    token = token + 1;
  } while (ch != '\0');
  /* Walk the tokens. The first strtok call gets the buffer; later calls pass
     NULL to continue from the previous token. Stops when strtok returns NULL. */
  while( true ) {
    token = (char *)strtok(tokenize_input,frame.local_108);
    tokenize_input = (char *)0x0;
    if (token == (char *)0x0) break;
    /* Option flags. -vs*, -cs* and -l0..-l5 form one else-if chain, so only the
       first match runs. -l6..-l9 are separate ifs tested on every token. */
    cmp_result = __stricmp(token,"-vs0");
    if (cmp_result == 0) {
      g_app_62ad34 = 0;
      g_app_76e86c = 0;
    }
    else {
      cmp_result = __stricmp(token,"-vs1");
      if (cmp_result == 0) {
        g_app_62ad34 = 1;
        g_app_76e86c = 1;
      }
      else {
        cmp_result = __stricmp(token,"-cs0");
        if (cmp_result == 0) {
          g_app_62ad38 = 0;
          g_app_76e870 = 0;
        }
        else {
          cmp_result = __stricmp(token,"-cs1");
          if (cmp_result == 0) {
            g_app_62ad38 = 1;
            g_app_76e870 = 1;
          }
          else {
            cmp_result = __stricmp(token,"-l0");
            if (cmp_result == 0) {
              g_language_offset = 0;
            }
            else {
              cmp_result = __stricmp(token,"-l1");
              if (cmp_result == 0) {
                g_language_offset = 1;
              }
              else {
                cmp_result = __stricmp(token,"-l2");
                if (cmp_result == 0) {
                  g_language_offset = 2;
                }
                else {
                  cmp_result = __stricmp(token,"-l3");
                  if (cmp_result == 0) {
                    g_language_offset = 3;
                  }
                  else {
                    cmp_result = __stricmp(token,"-l4");
                    if (cmp_result == 0) {
                      g_language_offset = 4;
                    }
                    else {
                      cmp_result = __stricmp(token,"-l5");
                      if (cmp_result == 0) {
                        g_language_offset = 5;
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
    cmp_result = __stricmp(token,"-l6");
    if (cmp_result == 0) {
      g_language_offset = 6;
    }
    cmp_result = __stricmp(token,"-l7");
    if (cmp_result == 0) {
      g_language_offset = 7;
    }
    cmp_result = __stricmp(token,"-l8");
    if (cmp_result == 0) {
      g_language_offset = 8;
    }
    cmp_result = __stricmp(token,"-l9");
    if (cmp_result == 0) {
      g_language_offset = 9;
    }
  }
  return 1;
}


