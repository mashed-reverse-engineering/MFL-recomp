// cluster-entries: rwa_stream_container_state_machine_005c1720 rwa_stream_container_update_005c1870
/* Installed cluster rwa_stream_container_state_machine_005c1720 (tools/cluster_install.py from output/cluster_pilot_l2c/rwa_stream_container_state_machine_005c1720.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* Stream container (_rwaStreamContainerStateMachine, register function: params eax, container edx, stream ecx) and its caller,
   the container update (5c1870) that applies the pending parameter changes.  States (+0x08): 2 open, 3 opening, 4 built,
   5 playing, 6 stopped.  Register callees outside the cluster go through generated adapters. */
#include "mfl_tier2.h"

/*005c10b0*/ extern int FUN_005c10b0(char *container, char *stream);      /* build the pipeline */
/*005c19d0*/ extern void *rwa_unit246d50_005c19d0(void *list, RwUInt32 size, void *data);
/*005c1ee0*/ extern void *RwaObjIntGetParamData(void *obj, void *iface, int param);
/*005c1f10*/ extern void RwaObjIntSetParamBool(void *obj, void *iface, int param, RwUInt32 value);
/*005dd170*/ __declspec(dllimport) long __stdcall InterlockedExchange(long *target, long value);

#define STATE(c) (*(RwUInt32 *)((c) + 8))

static void FUN_005bfee0_r(char * p0);
extern void FUN_005bfee0(void);
static int FUN_005c02a0_r(void * p0, char * p1);
extern void FUN_005c02a0(void);
static void _rwaStreamContainerInstaniateSource_r(char * p0, char * p1);
extern void _rwaStreamContainerInstaniateSource(void);
static void _RwaStreamContainerUpdateStreamPos_r(char * p0, char * p1, RwUInt32 * p2);
extern void _RwaStreamContainerUpdateStreamPos(void);
static void _rwaStreamContainerSetPitch_r(RwUInt32 * p0, char * p1);
extern void _rwaStreamContainerSetPitch(void);
static void FUN_005c09e0_r(RwUInt32 * p0, char * p1, char * p2);
extern void FUN_005c09e0(void);
static void FUN_005c01e0_r(char * p0, char * p1, RwUInt32 p2);
extern void FUN_005c01e0(void);
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_stream_container_state_machine_005c1720_n(RwUInt32 *params, char *container, char *stream)
{
  int stop = 0;
  void *p;

  switch (STATE(container)) {
  case 2:
    if (!params[2])
      goto stopped;
    InterlockedExchange((long *)container, 0);
    InterlockedExchange((long *)(container + 4), 1);
    *(RwUInt32 *)(container + 0x38) = 1;
    if (!FUN_005c02a0_r((void *)params[2], container))
      goto destroy;
    STATE(container) = 3;
    return;
  case 3:
    if (*(RwUInt32 *)container)
      goto destroy;
    if (*(RwUInt32 *)(container + 4))
      return;
    p = *(void **)(container + 0x14);
    *(void **)(container + 0xc) = RwaObjIntGetParamData(p, *(void **)((char *)p + 0x58), 2);
    if (!FUN_005c10b0(container, stream))
      goto destroy;
    STATE(container) = 4;
    /* fall through */
  case 4:
    if (*(RwUInt32 *)container) {
      stop = 1;
      STATE(container) = 6;
    }
    if (!*(RwUInt32 *)(container + 4)) {
      p = rwa_unit246d50_005c19d0(params + 6, 0xc0, *(void **)(container + 0xc));
      *(void **)(stream + 4) = p;
      if (!p)
        goto destroy;
      _rwaStreamContainerInstaniateSource_r(container, stream);
      STATE(container) = 5;
    }
    if (stop)
      goto destroy;
    return;
  default:
    FUN_005bfee0_r(container);
    STATE(container) = 6;
    return;
  }
destroy:
  FUN_005bfee0_r(container);
stopped:
  STATE(container) = 6;
}

/* Container update: stop request, the state machine up to playing, then the pending parameter changes of params[0] (8 stream
   position, 0x20 pitch, 0x10 volume, 0x40 loop, 0x80 chain parameter).  1: became playing in this call. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_stream_container_update_005c1870(char *container, char *stream, RwUInt32 *params)
{
  int started = 0;
  RwUInt32 s;
  char *p;

  if ((params[0] & 4) && !*(RwUInt32 *)(container + 4)) {
    FUN_005bfee0_r(container);
    STATE(container) = 2;
    params[0] &= ~4u;
  }
  s = STATE(container);
  if ((RwInt32)s < 5) {
    rwa_stream_container_state_machine_005c1720_n(params, container, stream);
    s = STATE(container);
    if ((RwInt32)s < 5)
      return 0;
    started = 1;
  }
  if (s == 6)
    return started;
  if (params[0] & 8) {
    _RwaStreamContainerUpdateStreamPos_r(container, stream, params);
    params[0] &= ~8u;
  }
  if (params[0] & 0x20) {
    _rwaStreamContainerSetPitch_r(params, container);
    params[0] &= ~0x20u;
  }
  if (params[0] & 0x10) {
    FUN_005c09e0_r(params + 6, container, stream);
    params[0] &= ~0x10u;
  }
  if (params[0] & 0x40) {
    *(RwUInt32 *)(container + 0x30) = params[5];
    p = *(char **)(container + 0x14);
    RwaObjIntSetParamBool(p, *(void **)(p + 0x58), 1, params[5]);
    params[0] &= ~0x40u;
  }
  if (*(signed char *)params < 0) {
    if (*(RwUInt32 *)(container + 0x38) != params[1]) {
      FUN_005c01e0_r(stream, container, params[1]);
      *(RwUInt32 *)(container + 0x38) = params[1];
    }
    params[0] &= ~0x80u;
  }
  return started;
}

/* FUN_005bfee0_r: C -> original FUN_005bfee0 (registers esi) */
__declspec(naked) void FUN_005bfee0_r(char * p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call FUN_005bfee0
        pop esi
        ret
    }
}

/* FUN_005c02a0_r: C -> original FUN_005c02a0 (registers ebx,s0) */
__declspec(naked) int FUN_005c02a0_r(void * p0, char * p1)
{
    __asm {
        push ebx
        push dword ptr [esp + 12]
        mov ebx, dword ptr [esp + 12]
        call FUN_005c02a0
        add esp, 4
        pop ebx
        ret
    }
}

/* _rwaStreamContainerInstaniateSource_r: C -> original _rwaStreamContainerInstaniateSource (registers edi,s0) */
__declspec(naked) void _rwaStreamContainerInstaniateSource_r(char * p0, char * p1)
{
    __asm {
        push edi
        push dword ptr [esp + 12]
        mov edi, dword ptr [esp + 12]
        call _rwaStreamContainerInstaniateSource
        add esp, 4
        pop edi
        ret
    }
}

/* _RwaStreamContainerUpdateStreamPos_r: C -> original _RwaStreamContainerUpdateStreamPos (registers eax,s0,s1) */
__declspec(naked) void _RwaStreamContainerUpdateStreamPos_r(char * p0, char * p1, RwUInt32 * p2)
{
    __asm {
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        mov eax, dword ptr [esp + 12]
        call _RwaStreamContainerUpdateStreamPos
        add esp, 8
        ret
    }
}

/* _rwaStreamContainerSetPitch_r: C -> original _rwaStreamContainerSetPitch (registers eax,ebx) */
__declspec(naked) void _rwaStreamContainerSetPitch_r(RwUInt32 * p0, char * p1)
{
    __asm {
        push ebx
        mov eax, dword ptr [esp + 8]
        mov ebx, dword ptr [esp + 12]
        call _rwaStreamContainerSetPitch
        pop ebx
        ret
    }
}

/* FUN_005c09e0_r: C -> original FUN_005c09e0 (registers edx,s0,s1) */
__declspec(naked) void FUN_005c09e0_r(RwUInt32 * p0, char * p1, char * p2)
{
    __asm {
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        mov edx, dword ptr [esp + 12]
        call FUN_005c09e0
        add esp, 8
        ret
    }
}

/* FUN_005c01e0_r: C -> original FUN_005c01e0 (registers eax,ecx,edi) */
__declspec(naked) void FUN_005c01e0_r(char * p0, char * p1, RwUInt32 p2)
{
    __asm {
        push edi
        mov eax, dword ptr [esp + 8]
        mov ecx, dword ptr [esp + 12]
        mov edi, dword ptr [esp + 16]
        call FUN_005c01e0
        pop edi
        ret
    }
}

/* entry thunk at the original address of rwa_stream_container_state_machine_005c1720: register inputs (eax,edx,ecx) -> natural C rwa_stream_container_state_machine_005c1720_n; ret 0 */
__declspec(naked) void rwa_stream_container_state_machine_005c1720(void)
{
    __asm {
        push ecx
        push edx
        push eax
        call rwa_stream_container_state_machine_005c1720_n
        add esp, 12
        ret
    }
}
