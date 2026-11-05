/* B3 (own CRT layer): mlock.obj, the multithread CRT lock table (0x4b773d-0x4b78a0).  36 slots of (critical section, kind); kind 1 = one of the
   static critical sections of lclcritsects, set up at start-up; the others are created on first use under the lock-table lock (_LOCKTAB_LOCK 10). */
typedef struct _RTL_CRITICAL_SECTION CRITICAL_SECTION;
struct _RTL_CRITICAL_SECTION { void *debug_info; long lock_count, recursion_count; void *owner, *lock_semaphore; unsigned long spin_count; };

extern __declspec(dllimport) void __stdcall DeleteCriticalSection(CRITICAL_SECTION *cs);
extern __declspec(dllimport) void __stdcall EnterCriticalSection(CRITICAL_SECTION *cs);
extern __declspec(dllimport) void __stdcall LeaveCriticalSection(CRITICAL_SECTION *cs);
extern int __cdecl __crtInitCritSecAndSpinCount(CRITICAL_SECTION *cs, unsigned long spin);
extern void *__cdecl malloc(unsigned int size);
extern void __cdecl free(void *block);
extern int *__cdecl _errno(void);
extern void __cdecl _amsg_exit(int code);
void __cdecl _lock(int locknum);
void __cdecl _unlock(int locknum);

#define TOTAL_LOCKS 36
#define LOCKTAB_LOCK 10
#define LK_PREALLOC 1      /* the slot's critical section is one of lclcritsects */
#define LOCK_SPIN_COUNT 4000
#define ENOMEM 12
#define RT_LOCK 17          /* _RT_LOCK: "unable to initialize a lock" */

struct lock_slot {
    CRITICAL_SECTION *lock;
    int kind;
};
/*0062c9a8*/ extern struct lock_slot _locktable[TOTAL_LOCKS];
/*00771b38*/ extern CRITICAL_SECTION lclcritsects[];

/* start-up: give every preallocated slot its static critical section; 0 when one cannot be initialised */
/* TIER2: unvalidated 2026-11-04 authority=diffemu */
int __cdecl _mtinitlocks(void)
{
    CRITICAL_SECTION *next = lclcritsects;
    int i;

    for (i = 0; i < TOTAL_LOCKS; i++) {
        if (_locktable[i].kind == LK_PREALLOC) {
            _locktable[i].lock = next++;
            if (!__crtInitCritSecAndSpinCount(_locktable[i].lock, LOCK_SPIN_COUNT)) {
                _locktable[i].lock = 0;
                return 0;
            }
        }
    }
    return 1;
}

/* shutdown: delete and free the dynamically created locks, then delete the static ones */
/* TIER2: unvalidated 2026-11-04 authority=diffemu */
void __cdecl _mtdeletelocks(void)
{
    struct lock_slot *slot;

    for (slot = _locktable; slot < _locktable + TOTAL_LOCKS; slot++) {
        if (slot->lock != 0 && slot->kind != LK_PREALLOC) {
            DeleteCriticalSection(slot->lock);
            free(slot->lock);
            slot->lock = 0;
        }
    }
    for (slot = _locktable; slot < _locktable + TOTAL_LOCKS; slot++) {
        if (slot->lock != 0 && slot->kind == LK_PREALLOC)
            DeleteCriticalSection(slot->lock);
    }
}

/* TIER2: unvalidated 2026-11-04 authority=diffemu */
void __cdecl _unlock(int locknum)
{
    LeaveCriticalSection(_locktable[locknum].lock);
}

/* create the critical section of slot `locknum` (once; another thread may win the race under the table lock); 0 = out of memory (errno ENOMEM) */
/* TIER2: unvalidated 2026-11-04 authority=diffemu */
int __cdecl _mtinitlocknum(int locknum)
{
    struct lock_slot *slot = &_locktable[locknum];
    CRITICAL_SECTION *cs;

    if (slot->lock != 0)
        return 1;
    cs = (CRITICAL_SECTION *)malloc(sizeof(CRITICAL_SECTION));
    if (cs == 0) {
        *_errno() = ENOMEM;
        return 0;
    }
    _lock(LOCKTAB_LOCK);
    if (slot->lock == 0) {
        if (!__crtInitCritSecAndSpinCount(cs, LOCK_SPIN_COUNT)) {
            free(cs);
            _unlock(LOCKTAB_LOCK);
            *_errno() = ENOMEM;
            return 0;
        }
        slot->lock = cs;
    } else {
        free(cs);
    }
    _unlock(LOCKTAB_LOCK);
    return 1;
}

/* TIER2: unvalidated 2026-11-04 authority=diffemu */
void __cdecl _lock(int locknum)
{
    if (_locktable[locknum].lock == 0 && !_mtinitlocknum(locknum))
        _amsg_exit(RT_LOCK);
    EnterCriticalSection(_locktable[locknum].lock);
}
