/* B3 (own CRT layer): the two fatal-error exits of wincrt0.obj / crt0msg (0x4b4b5e, 0x4b4b83).  Both print the run-time error message (with the
   "runtime error" banner when the error mode asks for stderr) and end the process with code 255; _amsg_exit through the exit routine pointer
   (_exit by default), fast_error_exit (start-up failures before the CRT is up) directly through __crtExitProcess.  The start-up code itself
   (entry 0x4b4ba7) stays original. */
#define EXIT_FATAL 255
#define ERROR_MODE_STDERR 1

/*00771b20*/ extern int __error_mode;
/*0062c664*/ extern void (__cdecl *_aexit_rtn)(int code);
extern void __cdecl _FF_MSGBANNER(void);
extern void __cdecl _NMSG_WRITE(int rterrnum);
extern void __cdecl __crtExitProcess(int code);

/* TIER2: unvalidated 2026-11-04 authority=diffemu */
void __cdecl _amsg_exit(int rterrnum)
{
    if (__error_mode == ERROR_MODE_STDERR)
        _FF_MSGBANNER();
    _NMSG_WRITE(rterrnum);
    _aexit_rtn(EXIT_FATAL);
}

/* TIER2: unvalidated 2026-11-04 authority=diffemu */
void __cdecl fast_error_exit(int rterrnum)
{
    if (__error_mode == ERROR_MODE_STDERR)
        _FF_MSGBANNER();
    _NMSG_WRITE(rterrnum);
    __crtExitProcess(EXIT_FATAL);
}
