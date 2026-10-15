#!/usr/bin/env python3
"""Register bridges: C implementations of functions that take (some of) their parameters in registers (stdlib only).
Usage: bridgegen.py <outdir> [--limit N] [--chunk N]

Inputs: output/named/MFL_app_named.c (decompilation; the functions have register parameters since scripts/SetRegisterParams.java), output/sigs.txt (scripts/ExportSigs.java: storage of every
parameter), output/reginputs.csv (tools/reginputs.py: register inputs and `ret N` from the disassembly), the original exe (which registers a function writes), src/manifest.txt
(hand written functions are left alone), output/decls.h (data declarations), src/auto/{prelude.h,ghidra_types.h}.

For every function F with at least one register parameter (a "register function") that compiles:
  F_c(p1..pn)   the decompiled body as a plain C function: register parameters first, then the stack parameters (cdecl, so any C caller can use it)
  F             a __declspec(naked) stub at the original address (the overlay patches the symbol F): loads the register parameters and copies the stack parameters, calls F_c, restores
                the caller saved registers the original never touched (a whole program optimised caller may keep values in ecx/edx), returns with `ret N` like the original
Calls from the generated bodies to other register functions go through  F_c (if F is bridged) or F_r (an adapter that sets the registers and calls the original).
Functions the generator cannot express (extraout_ results, 64 bit returns, partial registers, labels, unions of leftovers) are listed in <outdir>/skipped.txt."""
import csv, os, re, subprocess, sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.join(HERE, "..")
EXE = os.path.join(ROOT, "..", "MashedGame", "FullyLoaded", "installed", "App_Executables", "MFL.exe")
sys.path.insert(0, HERE)
import clobber_util  # noqa: E402  (registers written by raw function bytes)

REGS32 = {"EAX": "eax", "ECX": "ecx", "EDX": "edx", "EBX": "ebx", "ESI": "esi", "EDI": "edi", "EBP": "ebp"}
UNRELIABLE = set()
CALLEE_SAVED = ("ebx", "esi", "edi", "ebp")


def load():
    text = open(os.path.join(ROOT, "output", "named", "MFL_app_named.c"), errors="replace").read()
    blocks = {}
    for m in re.finditer(r"// ==== (\w+) @ ([0-9a-f]{8})\n(.*?)(?=\n// ==== |\Z)", text, re.S):
        blocks[m.group(1)] = (int(m.group(2), 16), m.group(3))
    sigs = {}
    for l in open(os.path.join(ROOT, "output", "sigs.txt")):
        p = l.rstrip("\n").split("|")
        if len(p) < 7: continue
        params = []
        for q in [x for x in p[5].split(";") if x]:
            t, n, st = q.split(":", 2) if q.count(":") >= 2 else (q, "", "")
            params.append((t, n, st))
        sigs[p[0]] = dict(addr=int(p[1], 16), conv=p[2], params=params)
    regin = {}
    for r in csv.DictReader(open(os.path.join(ROOT, "output", "reginputs.csv"))):
        regin[r["name"]] = (r["reg_inputs"], int(r["ret_n"]))
    man = set()
    for l in open(os.path.join(ROOT, "src", "manifest.txt")):
        p = l.split()
        if len(p) > 3 and p[0] != "#": man.add(p[1])
    return blocks, sigs, regin, man


def parse_proto(body):
    """-> (ret type, [(type, name)]) from the decompiled prototype (the line(s) before the first '{')"""
    head = body.split("\n{", 1)[0]
    head = re.sub(r"/\*.*?\*/", " ", head, flags=re.S)
    head = " ".join(head.split())
    m = re.match(r"(.*?)\b(\w+)\((.*)\)$", head)
    if not m: return None
    ret = re.sub(r"\b__(?:thiscall|fastcall|stdcall|cdecl)\b", "", m.group(1)).strip()
    ps = []
    arg = m.group(3).strip()
    if arg and arg != "void":
        for a in re.split(r",(?![^()]*\))", arg):
            a = a.strip()
            mm = re.match(r"(.*?)(\w+)((?:\[\d*\])*)$", a)
            if not mm: return None
            ps.append((re.sub(r"\b__(?:thiscall|fastcall|stdcall|cdecl)\b", "", mm.group(1)).strip() + (" *" if False else ""), mm.group(2) + mm.group(3)))
    return ret, ps


def func_bytes(addr, nxt):
    with open(EXE, "rb") as f:
        f.seek(addr - 0x401000 + 0x1000)
        return f.read(max(0, nxt - addr))


def stack_off(storage):
    m = re.match(r"Stack\[(-?0x[0-9a-fA-F]+|-?\d+)\]:(\d+)", storage)
    return (int(m.group(1), 16) if "0x" in m.group(1) else int(m.group(1)), int(m.group(2))) if m else None


def analyse(name, blocks, sigs, regin, man, nexts):
    """-> dict with everything needed to generate F_c / F / F_r, or (None, reason)"""
    if name not in blocks or name not in sigs: return None, "no decompilation / signature"
    addr, body = blocks[name]
    pr = parse_proto(body)
    if not pr: return None, "prototype not parsed"
    ret, plist = pr
    sg = sigs[name]["params"]
    store = {n: st for t, n, st in sg}
    if not any(st.split(":")[0].upper() in REGS32 for t, n, st in sg): return None, "no register parameter"
    params = []   # (ctype, name, kind, where)
    for ctype, pname in plist:
        base = re.sub(r"\[.*", "", pname)
        st = store.get(base, "")
        if st.split(":")[0].upper() in REGS32 and st.endswith(":4"):
            params.append((ctype, pname, "reg", REGS32[st.split(":")[0].upper()]))
        elif st.startswith("Stack"):
            so = stack_off(st)
            if not so: return None, "stack storage not parsed: " + st
            params.append((ctype, pname, "stack", so))
        else:
            return None, "parameter storage not supported: %s %s" % (base, st)
    if not any(p[2] == "reg" for p in params): return None, "no register parameter"
    regs = [p[3] for p in params if p[2] == "reg"]
    if len(set(regs)) != len(regs): return None, "register used twice"
    if "extraout_" in body: return None, "extraout_ result (a callee leaves a value in a register)"
    if re.search(r"\b(in_[A-Z]{2,3}|in_stack_\w+|[iu]?Stack[0-9a-f]{8}|unaff_\w+|stack0x\w+|param_\d+_00\w+|CONCAT\d+|SUB\d+|POPCOUNT|halt_baddata|unaff_retaddr|fpatan|fsin|fcos|fptan|fyl2x|f2xm1|ABS|SBORROW\d|CARRY\d)\b", body): return None, "decompiler pseudo variables left"
    if "LAB_" in body or "goto " in body: return None, "labels / goto"
    if re.match(r"(float10|double|longlong|ulonglong|undefined8)\b", ret) and False: pass
    if ret in ("undefined8", "longlong", "ulonglong", "ulong8"): return None, "64 bit return"
    nxt = nexts.get(addr)
    code = func_bytes(addr, nxt) if nxt else b""
    return dict(name=name, addr=addr, ret=ret, params=params, body=body[body.index("\n{") + 1:], purge=regin.get(name, ("", 0))[1], code=code), None


def adapter_ok(blocks, sigs, regin, man, nexts):
    """register functions that get a call adapter F_r (analyse works and the decompiled register params are exactly the register inputs of the disassembly)"""
    ok = set()
    for n in blocks:
        f, _ = analyse(n, blocks, sigs, regin, man, nexts)
        if f and {p[3] for p in f["params"] if p[2] == "reg"} == set(regin.get(n, ("",))[0].split("|")): ok.add(n)
    return ok


def reads_stack_args(code):
    """True when the straight line prefix of the function (up to the first branch / call) reads a stack argument ([ebp+8..], or [esp+k] beyond what it pushed itself): the callee would take stack
    arguments that an adapter without known stack types cannot push"""
    import tempfile
    t = tempfile.NamedTemporaryFile(delete=False); t.write(code[:64]); t.close()
    out = subprocess.run(["objdump", "-w", "-D", "-b", "binary", "-mi386", "-M", "intel", t.name], capture_output=True, text=True).stdout
    os.unlink(t.name)
    delta = 0
    for l in out.splitlines():
        m = re.match(r"\s*[0-9a-f]+:\s+(?:[0-9a-f]{2} )+\s*(\w+)\s*(.*)", l)
        if not m: continue
        op, args = m.group(1), m.group(2)
        mm = re.search(r"\[ebp\+0x([0-9a-f]+)\]", args)
        if mm and int(mm.group(1), 16) >= 8: return True
        mm = re.search(r"\[esp(?:\+0x([0-9a-f]+))?\]", args)
        if mm and int(mm.group(1) or "0", 16) - delta >= 4: return True
        if op == "push": delta += 4
        elif op == "pop": delta -= 4
        elif op == "sub" and args.startswith("esp,"): delta += int(args.split(",")[1], 16)
        elif op == "add" and args.startswith("esp,"): delta -= int(args.split(",")[1], 16)
        elif op.startswith("j") or op in ("call", "ret", "retn"): break
    return False


def synth_adapters(blocks, regin, regfuncs):
    """F_r adapters for register callees without a usable decompiled signature (output/sigs.txt has no parameters, regsigs.csv may have empty stack types): the registers are the reginputs.csv inputs (all `int`),
    the stack parameters the regsigs.csv types (unknown = none, checked against ret N and the first instructions of the original).  -> (added {name: f}, rejected {name: reason}); regfuncs is extended"""
    rs = {r["name"]: r for r in csv.DictReader(open(os.path.join(ROOT, "output", "regsigs.csv")))}
    ri = {r["name"]: r for r in csv.DictReader(open(os.path.join(ROOT, "output", "reginputs.csv")))}
    addrs = sorted({int(r["addr"], 16) for r in ri.values()} | {a for a, _ in blocks.values()})
    added, rej = {}, {}
    for n, r in rs.items():
        if n in regfuncs: continue
        if not re.fullmatch(r"[A-Za-z_]\w*", n): rej[n] = "mangled name"; continue
        x = ri.get(n, {})
        regs = [q for q in r["regs"].split("|") if q]
        if x.get("x87_in", "0") not in ("", "0"): rej[n] = "x87 input"; continue
        if not regs or any(q not in REGS32.values() for q in regs) or len(set(regs)) != len(regs): rej[n] = "registers " + r["regs"]; continue
        stk = [t for t in r["stack"].split(";") if t]
        ret_n = int(r["ret_n"])
        if ret_n and ret_n != 4 * len(stk): rej[n] = "ret %d but %d known stack parameters" % (ret_n, len(stk)); continue
        a = int(r["addr"], 16); i = addrs.index(a)
        nxt = addrs[i + 1] if i + 1 < len(addrs) else a + 0x40
        code = func_bytes(a, min(nxt, a + 64))
        if not stk and not ret_n and reads_stack_args(code): rej[n] = "reads stack arguments, none known"; continue
        ret = "int"
        if n in blocks:
            pr = parse_proto(blocks[n][1])
            if pr: ret = pr[0]
        if ret in ("undefined8", "longlong", "ulonglong"): rej[n] = "64 bit return"; continue
        params = [("int", "reg_" + q, "reg", q) for q in regs] + [(t, "stk%d" % k, "stack", (4 + 4 * k, 4)) for k, t in enumerate(stk)]
        f = dict(name=n, addr=a, ret=ret, params=params, body=None, purge=ret_n, code=code, written={"eax", "ecx", "edx"}, synth=True)
        regfuncs[n] = added[n] = f
    return added, rej


def c_params(params):
    return ", ".join("%s %s" % (t, n) if "[" not in n else "%s %s" % (t, n) for t, n, k, w in params) or "void"


def gen_stub(f, written, target=None):
    """naked stub at the original address (target: the C function it calls, default F_c)"""
    regs = [(p[3], i) for i, p in enumerate(f["params"]) if p[2] == "reg"]
    stk = [(i, p[3]) for i, p in enumerate(f["params"]) if p[2] == "stack"]
    n = len(f["params"])
    L = ["__declspec(naked) void %s(void)" % f["name"], "{", "    __asm {"]
    keep = [r for r in ("eax", "ecx", "edx") if r not in written and not (r == "eax" and f["ret"] != "void")]
    for r in keep: L.append("        push %s" % r)
    pushed = len(keep)
    # C argument order: p0 .. p(n-1); cdecl pushes the last first
    for i in range(n - 1, -1, -1):
        p = f["params"][i]
        if p[2] == "stack":
            off, size = p[3]
            for k in range(size // 4 - 1, -1, -1):
                L.append("        push dword ptr [esp + %d]" % (off + 4 * k + 4 * pushed))
                pushed += 1
        else:
            L.append("        push %s" % p[3]); pushed += 1
    L.append("        call %s" % (target or f["name"] + "_c"))
    L.append("        add esp, %d" % (4 * (pushed - len(keep))))
    for r in reversed(keep): L.append("        pop %s" % r)
    L.append("        ret %d" % f["purge"] if f["purge"] else "        ret")
    L += ["    }", "}", ""]
    return "\n".join(L)


def gen_adapter(f, target=None):
    """F_r: C callable adapter that sets the registers and calls the original F (cdecl or callee cleaned); target: what it calls instead of F (the trampoline of the shadow build)"""
    L = ["__declspec(naked) %s %s_r(%s)" % (f["ret"], f["name"], c_params(f["params"])), "{", "    __asm {"]
    saved = [r for r in CALLEE_SAVED if any(p[2] == "reg" and p[3] == r for p in f["params"])]
    for r in saved: L.append("        push %s" % r)
    base = 4 + 4 * len(saved)           # offset of the first C argument from esp
    # C arguments in order: p0..p(n-1) at base + 4 * index (8 byte params take two slots; keep to 4 byte slots)
    offs, o = [], base
    for t, n, k, w in f["params"]:
        offs.append(o); o += 8 if t in ("double", "float10", "longlong", "ulonglong") else 4
    pushed = 0
    stk = [(i, p) for i, p in enumerate(f["params"]) if p[2] == "stack"]
    for i, p in sorted(stk, key=lambda x: -x[1][3][0]):
        size = p[3][1]
        for k in range(size // 4 - 1, -1, -1):
            L.append("        push dword ptr [esp + %d]" % (offs[i] + 4 * k + 4 * pushed)); pushed += 1
    for i, p in enumerate(f["params"]):
        if p[2] == "reg":
            L.append("        mov %s, dword ptr [esp + %d]" % (p[3], offs[i] + 4 * pushed))
    L.append("        call %s" % (target or f["name"]))
    if not f["purge"] and pushed: L.append("        add esp, %d" % (4 * pushed))
    for r in reversed(saved): L.append("        pop %s" % r)
    L += ["        ret", "    }", "}", ""]
    return "\n".join(L)


def ctype_decl(t, n):
    return "%s %s" % (t, n)


# tools/defectlint.py findings of the bridge sources: (old text, new text), applied by fix_known (diffemu / defectlint: unsigned compares of undefined4 globals, float dwords read / stored by int conversion)
BRIDGE_FIX = [(o, n) for o, n in (
    ("  uVar1 = FUN_005543d0(g_rw_701c9c,reg_eax,0xffffffff,7);\n  FUN_0056a210(uVar1);", "  uVar1 = FUN_005543d0(g_rw_701c9c);\n  FUN_0056a210(uVar1,reg_eax,0xffffffff,7);"),      # rwp_world_current_activate (defarity): rwSListGetBegin takes ONE argument, the other three pushes stay pending and are FUN_0056a210's 2nd..4th arguments
    ("0 < g_fx_object_count", "0 < (int)g_fx_object_count"), ("iVar2 < g_fx_object_count", "iVar2 < (int)g_fx_object_count"),
    ("0 < weapon_descriptor_count", "0 < (int)weapon_descriptor_count"), ("iVar2 < weapon_descriptor_count", "iVar2 < (int)weapon_descriptor_count"),
    ("0 < g_car_6c28c0", "0 < (int)g_car_6c28c0"), ("iVar2 < g_car_6c28c0", "iVar2 < (int)g_car_6c28c0"),
    ("0 < g_app_771500", "0 < (int)g_app_771500"), ("(int)WVar4 < g_app_771500", "(int)WVar4 < (int)g_app_771500"),
    ("0xff < g_particles_759630", "0xff < (int)g_particles_759630"),
    ("((float)piVar6[9] <= (float)piVar6[8])", "(*(float *)&piVar6[9] <= *(float *)&piVar6[8])"),
    ("(float)piVar6[5] * (float)piVar6[0xd]", "*(float *)&piVar6[5] * *(float *)&piVar6[0xd]"),
    ("(float)piVar6[6] * (float)piVar6[0xd]", "*(float *)&piVar6[6] * *(float *)&piVar6[0xd]"),
    ("(float)piVar6[7] * (float)piVar6[0xd]", "*(float *)&piVar6[7] * *(float *)&piVar6[0xd]"),
    ("  float fVar2;\n  int iVar3;\n  undefined1 local_10;", "  float fVar2;\n  int iVar3;\n  int iCnt;\n  undefined1 local_10;"),      # weapon_mortar_trail_particles_add: the counter lived in the float local_c, _ftol() had no argument (original: alpha = ftol(100 - t * 90))
    ("  local_c = 0.0;\n  local_10 = 0x80;", "  iCnt = 0;\n  local_10 = 0x80;"),
    ("    fVar2 = local_c;\n    local_c = (float)(int)local_c * k_f_0_06666667;\n    local_d = _ftol();", "    local_c = (float)iCnt * k_f_0_06666667;\n    local_d = (undefined1)(int)(k_f_100 - local_c * k_f_90);"),
    ("    local_c = (float)((int)fVar2 + 1);\n    iVar3 = iVar3 + 0xc;\n  } while ((int)local_c < 0xf);", "    iCnt = iCnt + 1;\n    iVar3 = iVar3 + 0xc;\n  } while (iCnt < 0xf);"),
    ("/* weapon_mortar_trail_particles_add */\nvoid weapon_mortar_trail_particles_add_c(int reg_esi);", "/* weapon_mortar_trail_particles_add */\nextern float k_f_100;\nextern float k_f_90;\nvoid weapon_mortar_trail_particles_add_c(int reg_esi);"),
    ("extern void btb_panel_slot_set_value(int param_1, undefined4 param_2);", "extern void btb_panel_slot_set_value(int param_1, float param_2);"),      # the float was converted (_ftol) into the undefined4 parameter; the original pushes the dword
    ("/* course_sandsnow_cb1 */\nvoid course_sandsnow_cb1_c(", "/* course_sandsnow_cb1 */\nextern void func_0x0045bad0(float p0);      /* implicit `int f()` pushed the float as a double and read the float result of 0x4757e0 from EAX */\nextern float func_0x004757e0(int p0);\nextern void func_0x00449c40(int p0, float p1);\nvoid course_sandsnow_cb1_c("),
    ("camera_matrix = get_viewport_camera_matrix(0,(float)fVar2);\n  func_0x00449c40(camera_matrix);", "camera_matrix = get_viewport_camera_matrix(0);\n  func_0x00449c40(camera_matrix,(float)fVar2);"),
    ("BR_controller_cfg_path()", "BR_controller_cfg_path(reg_eax)"),      # EAX = pad index (third sprintf argument), decompiled without the register input
    ("BR_rwp_world_current_deactivate()", "BR_rwp_world_current_deactivate(*(int *)(reg_eax + 0x10))"),      # EAX = body (the original jumps with eax = [body + 0x10])
    ("(&g_car_7e8b74)[g_event_log_head * 8] = local_30;", "*(float *)&(&g_car_7e8b74)[g_event_log_head * 8] = local_30;"),
    ("(&g_car_7e8b78)[g_event_log_head * 8] = local_2c;", "*(float *)&(&g_car_7e8b78)[g_event_log_head * 8] = local_2c;"),
    ("(&g_car_7e8b7c)[g_event_log_head * 8] = local_28;", "*(float *)&(&g_car_7e8b7c)[g_event_log_head * 8] = local_28;"),
    # frame layout (defectlint class 9, 2026-10-12): scalar locals whose neighbours the callee / the code reads are one frame object
    ('undefined4 hudpanel_marker_ray_cast_c(undefined4 * reg_eax, int reg_edx, undefined4 reg_ecx)\n{\n  int iVar1;\n  undefined4 *puVar2;\n  float local_8c;\n  float local_88;\n  float local_84;\n  float local_80;\n  float local_7c;\n  float local_78;\n  int local_74;\n  undefined1 local_70 [36];\n  int local_4c;\n  undefined4 local_48;\n  int local_44;\n  undefined4 local_40 [16];\n  \n  iVar1 = FUN_004c4de0(reg_ecx,reg_edx,&local_80);\n  if ((iVar1 != 0) &&\n     (*(int *)(*(int *)(*(int *)(local_44 + 0x10) +\n                       ((uint)*(ushort *)(*(int *)(local_4c + 4) + 6 + local_74 * 8) +\n                       (uint)*(ushort *)(local_4c + 0x80)) * 4) + 4) != -0xfefeff)) {\n    FUN_004c4720(&local_8c,reg_edx,reg_edx + 0xc,local_48);\n    local_8c = local_80 * k_f_0_05 + local_8c;\n    local_88 = local_7c * k_f_0_05 + local_88;\n    local_84 = local_78 * k_f_0_05 + local_84;\n    FUN_004c51a0(local_40,local_70,&local_8c);\n    if (reg_eax != (undefined4 *)0x0) {\n      puVar2 = local_40;\n      for (iVar1 = 0x10; iVar1 != 0; iVar1 = iVar1 + -1) {\n        *reg_eax = *puVar2;\n        puVar2 = puVar2 + 1;\n        reg_eax = reg_eax + 1;\n      }\n    }\n    return 1;\n  }\n  return 0;\n}\n',
     'undefined4 hudpanel_marker_ray_cast_c(undefined4 * reg_eax, int reg_edx, undefined4 reg_ecx)\n{\n  int iVar1;\n  undefined4 *puVar2;\n  RwV3d local_8c;       /* 12 byte vector: passed by address */\n  struct { float x, y, z; int i74; undefined1 b70[36]; int i4c; undefined4 d48; int i44; } hit;  /* one 64 byte result object written by FUN_004c4de0 (the C reads its members afterwards) */\n  undefined4 local_40 [16];\n  \n  iVar1 = FUN_004c4de0(reg_ecx,reg_edx,&hit);\n  if ((iVar1 != 0) &&\n     (*(int *)(*(int *)(*(int *)(hit.i44 + 0x10) +\n                       ((uint)*(ushort *)(*(int *)(hit.i4c + 4) + 6 + hit.i74 * 8) +\n                       (uint)*(ushort *)(hit.i4c + 0x80)) * 4) + 4) != -0xfefeff)) {\n    FUN_004c4720(&local_8c,reg_edx,reg_edx + 0xc,hit.d48);\n    local_8c.x = hit.x * k_f_0_05 + local_8c.x;\n    local_8c.y = hit.y * k_f_0_05 + local_8c.y;\n    local_8c.z = hit.z * k_f_0_05 + local_8c.z;\n    FUN_004c51a0(local_40,hit.b70,&local_8c);\n    if (reg_eax != (undefined4 *)0x0) {\n      puVar2 = local_40;\n      for (iVar1 = 0x10; iVar1 != 0; iVar1 = iVar1 + -1) {\n        *reg_eax = *puVar2;\n        puVar2 = puVar2 + 1;\n        reg_eax = reg_eax + 1;\n      }\n    }\n    return 1;\n  }\n  return 0;\n}\n'),
    ('void time_trial_panel_step_part_b_c(int reg_edi)\n{\n  int iVar1;\n  int iVar2;\n  RwReal RVar3;\n  float local_50;\n  RwV3d local_4c;\n  undefined1 local_40 [48];\n  float local_10;\n  float local_c;\n  float local_8;\n  \n  vehicle_get_matrix_local(0,local_40);\n  iVar2 = 0;\n  do {\n    iVar1 = *(int *)(reg_edi + iVar2 * 4);\n    local_4c.x = local_10 - *(float *)(iVar1 + 0x30);\n    local_4c.y = local_c - *(float *)(iVar1 + 0x34);\n    local_4c.z = local_8 - *(float *)(iVar1 + 0x38);\n    RVar3 = RwV3dLength(&local_4c);\n    local_50 = RVar3 * k_f_0_1;\n    if (k_f_0_5 < local_50 == (k_f_0_5 == local_50)) {\n      if (local_50 < k_f_0) {\n        local_50 = 0.0;\n      }\n    }\n    else {\n      local_50 = 0.5;\n    }\n    btb_panel_slot_set_value(iVar2,local_50);\n    iVar2 = iVar2 + 1;\n  } while (iVar2 < 2);\n  return;\n}\n',
     'void time_trial_panel_step_part_b_c(int reg_edi)\n{\n  int iVar1;\n  int iVar2;\n  RwReal RVar3;\n  float local_50;\n  RwV3d local_4c;\n  RwMatrix local_40;       /* the callee writes the whole 64 byte matrix; the code reads its position (+0x30) behind the 48 byte part */\n  \n  vehicle_get_matrix_local(0,(undefined4 *)&local_40);\n  iVar2 = 0;\n  do {\n    iVar1 = *(int *)(reg_edi + iVar2 * 4);\n    local_4c.x = local_40.pos.x - *(float *)(iVar1 + 0x30);\n    local_4c.y = local_40.pos.y - *(float *)(iVar1 + 0x34);\n    local_4c.z = local_40.pos.z - *(float *)(iVar1 + 0x38);\n    RVar3 = RwV3dLength(&local_4c);\n    local_50 = RVar3 * k_f_0_1;\n    if (k_f_0_5 < local_50 == (k_f_0_5 == local_50)) {\n      if (local_50 < k_f_0) {\n        local_50 = 0.0;\n      }\n    }\n    else {\n      local_50 = 0.5;\n    }\n    btb_panel_slot_set_value(iVar2,local_50);\n    iVar2 = iVar2 + 1;\n  } while (iVar2 < 2);\n  return;\n}\n'),
    # vehicle_debris_update_one_c: not regenerated by bridgegen (C2198 vehicle_smoke_emitters_spawn), hand fixed in src/bridges/b002.c only (struct `part[18]`)
    ('void weapon_mortar_trail_particles_add_c(int reg_esi)\n{\n  int iVar1;\n  float fVar2;\n  int iVar3;\n  int iCnt;\n  undefined1 local_10;\n  undefined1 local_f;\n  undefined1 local_e;\n  undefined1 local_d;\n  float local_c;\n  float local_8;\n  float local_4;\n  \n  iCnt = 0;\n  local_10 = 0x80;\n  local_f = 0x80;\n  local_e = 0x80;\n  iVar3 = reg_esi + 0x5c;\n  do {\n    local_c = (float)iCnt * k_f_0_06666667;\n    local_d = (undefined1)(int)(k_f_100 - local_c * k_f_90);\n    local_8 = (k_f_1_4 - local_c) * k_f_0_3;\n    local_4 = local_8;\n    particle_batch_add(*(undefined4 *)(reg_esi + 0x58),iVar3,&local_10);\n    particle_batch_set_uv_a(*(undefined4 *)(reg_esi + 0x58),&local_8);\n    particle_batch_set_size(*(undefined4 *)(reg_esi + 0x58),local_c * k_f_3_1415927);\n    iCnt = iCnt + 1;\n    iVar3 = iVar3 + 0xc;\n  } while (iCnt < 0xf);\n  particle_batch_mark_current_active(*(undefined4 *)(reg_esi + 0x58));\n  fx_system_flush(*(undefined4 *)(reg_esi + 0x58));\n  iVar3 = 0xf;\n  while (iVar3 = iVar3 + -1, iVar3 != 0) {\n    iVar1 = reg_esi + iVar3 * 0xc;\n    *(undefined4 *)(iVar1 + 0x5c) = *(undefined4 *)(iVar1 + 0x50);\n    *(undefined4 *)(iVar1 + 0x60) = *(undefined4 *)(iVar1 + 0x54);\n    *(undefined4 *)(iVar1 + 100) = *(undefined4 *)(iVar1 + 0x58);\n  }\n  *(undefined4 *)(reg_esi + 0x5c) = *(undefined4 *)(reg_esi + 0x14);\n  *(undefined4 *)(reg_esi + 0x60) = *(undefined4 *)(reg_esi + 0x18);\n  *(undefined4 *)(reg_esi + 100) = *(undefined4 *)(reg_esi + 0x1c);\n  return;\n}\n',
     'void weapon_mortar_trail_particles_add_c(int reg_esi)\n{\n  int iVar1;\n  float fVar2;\n  int iVar3;\n  int iCnt;\n  undefined1 col [4];       /* RGBA colour and 2 floats uv: read through the pointers as a whole */\n  float local_c;\n  float uv [2];\n  \n  iCnt = 0;\n  col[0] = 0x80;\n  col[1] = 0x80;\n  col[2] = 0x80;\n  iVar3 = reg_esi + 0x5c;\n  do {\n    local_c = (float)iCnt * k_f_0_06666667;\n    col[3] = (undefined1)(int)(k_f_100 - local_c * k_f_90);\n    uv[0] = (k_f_1_4 - local_c) * k_f_0_3;\n    uv[1] = uv[0];\n    particle_batch_add(*(undefined4 *)(reg_esi + 0x58),iVar3,col);\n    particle_batch_set_uv_a(*(undefined4 *)(reg_esi + 0x58),(undefined4 *)uv);\n    particle_batch_set_size(*(undefined4 *)(reg_esi + 0x58),local_c * k_f_3_1415927);\n    iCnt = iCnt + 1;\n    iVar3 = iVar3 + 0xc;\n  } while (iCnt < 0xf);\n  particle_batch_mark_current_active(*(undefined4 *)(reg_esi + 0x58));\n  fx_system_flush(*(undefined4 *)(reg_esi + 0x58));\n  iVar3 = 0xf;\n  while (iVar3 = iVar3 + -1, iVar3 != 0) {\n    iVar1 = reg_esi + iVar3 * 0xc;\n    *(undefined4 *)(iVar1 + 0x5c) = *(undefined4 *)(iVar1 + 0x50);\n    *(undefined4 *)(iVar1 + 0x60) = *(undefined4 *)(iVar1 + 0x54);\n    *(undefined4 *)(iVar1 + 100) = *(undefined4 *)(iVar1 + 0x58);\n  }\n  *(undefined4 *)(reg_esi + 0x5c) = *(undefined4 *)(reg_esi + 0x14);\n  *(undefined4 *)(reg_esi + 0x60) = *(undefined4 *)(reg_esi + 0x18);\n  *(undefined4 *)(reg_esi + 100) = *(undefined4 *)(reg_esi + 0x1c);\n  return;\n}\n'),
)]


def fix_known(src):
    """decompiler prototype errors of the bridge sources (diffemu: clump_set_flag_on_atomics is (clump, flag), every original caller pushes 0 here; FUN_004f4920 takes the clump only)"""
    src = re.sub(r"(extern void (?:__cdecl )?clump_set_flag_on_atomics\(RpClump \* clump)\)", r"\1, int flag)", src)
    src = re.sub(r"(clump_set_flag_on_atomics\([^;,]*?)(?:,0)*\);", r"\1,0);", src)      # every original bridge caller pushes the flag 0 (idempotent)
    src = re.sub(r"(FUN_004f4920\([^;,]*?),clump,uVar1\)", r"\1)", src)
    for o, n in BRIDGE_FIX: src = src.replace(n, "\x00").replace(o, n).replace("\x00", n)      # idempotent: `n` may contain `o`
    return src


def proto_of(f, name):
    return "%s %s(%s)" % (f["ret"], name, c_params(f["params"]))


def plain_protos(blocks, regin, regnames):
    """typed extern prototypes of all non register application functions (from the decompiled prototypes; __stdcall when the original pops its arguments)"""
    out = {}
    for n, (a, body) in blocks.items():
        if n in regnames: continue
        pr = parse_proto(body)
        if not pr: continue
        ret, ps = pr
        if re.search(r"__(thiscall|fastcall)", re.sub(r"/\*.*?\*/", " ", body.split("\n{", 1)[0], flags=re.S)):
            UNRELIABLE.add(n)       # Ghidra believes in an ECX / EDX parameter that the disassembly does not show: call sites have fewer arguments than the prototype
            continue
        conv = "__stdcall " if regin.get(n, ("", 0))[1] > 0 else ""
        out[n] = "extern %s %s%s(%s);" % (ret, conv, n, ", ".join("%s %s" % (t, nm) for t, nm in ps) or "void")
    return out



def tramp_bytes(f):
    """-> (bytes of the first whole instructions covering >= 5 bytes, n) or None when they cannot be moved (branches / calls / relative addressing, or the function is too short)"""
    import tempfile
    code = f["code"][:32]
    if len(code) < 8: return None
    t = tempfile.NamedTemporaryFile(delete=False); t.write(code); t.close()
    out = subprocess.run(["objdump", "-w", "-D", "-b", "binary", "-mi386", "-M", "intel", t.name], capture_output=True, text=True).stdout
    os.unlink(t.name)
    n = 0
    for l in out.splitlines():
        m = re.match(r"\s*([0-9a-f]+):\s+((?:[0-9a-f]{2} )+)\s*(\w+)\s*(.*)", l)
        if not m: continue
        op, args = m.group(3), m.group(4)
        if op.startswith("j") or op in ("call", "ret", "retn", "loop", "int3", "hlt") or "eip" in args or "(bad)" in l: return None
        n += len(m.group(2).split())
        if n >= 5: return code[:n], n
    return None


def gen_tramp(f, tb):
    code, n = tb
    L = ["__declspec(naked) void %s_tramp(void)" % f["name"], "{", "    __asm {"]
    for b in code: L.append("        _emit 0x%02x" % b)
    L += ["        push 0x%x" % (f["addr"] + n), "        ret", "    }", "}", ""]
    return "\n".join(L)


def bits_expr(t, v):
    """C expression: the raw 32 bits of a value of C type t"""
    if t in ("float",): return "(*(unsigned *)&(%s))" % v
    return "((unsigned)(%s))" % v


def gen_shadow(f, fid):
    """C function F_shadow with the signature of F_c: runs the original and F_c on the same state and compares (see src/harness_shadow/shadow.c)"""
    ret = f["ret"]
    args = ", ".join(p[1] for p in f["params"])
    params = c_params(f["params"])
    isvoid = ret == "void"
    isf = bool(re.match(r"(float10|double|float)$", ret))
    L = ["%s %s_shadow(%s)" % (ret, f["name"], params), "{"]
    if not isvoid: L.append("    %s ra, rb;" % ret)
    if isf: L.append("    float fa, fb;")
    if isvoid: L.append("    if (!shadow_take(%d)) { %s_r(%s); return; }" % (fid, f["name"], args))
    else: L.append("    if (!shadow_take(%d)) return %s_r(%s);" % (fid, f["name"], args))
    L.append("    shadow_depth = 1;")
    bits = ["0"] * 4
    for i, p in enumerate(f["params"][:4]):
        if "[" in p[1] or p[0] in ("double", "float10"): continue
        bits[i] = bits_expr(p[0], p[1])
    L.append("    shadow_begin(%s);" % ", ".join(bits))
    L.append("    %s%s_r(%s);" % ("ra = " if not isvoid else "", f["name"], args))
    L.append("    shadow_save1(); shadow_restore0();")
    L.append("    %s%s_c(%s);" % ("rb = " if not isvoid else "", f["name"], args))
    if isvoid: L.append("    shadow_compare(%d, 0, 0, 0);" % fid)
    elif isf: L.append("    fa = (float)ra; fb = (float)rb;\n    shadow_compare(%d, *(unsigned *)&fa, *(unsigned *)&fb, 1);" % fid)
    else: L.append("    shadow_compare(%d, (unsigned)ra, (unsigned)rb, 0);" % fid)
    L.append("    shadow_restore1();")
    L.append("    shadow_depth = 0;")
    if not isvoid: L.append("    return ra;")
    L += ["}", ""]
    return "\n".join(l for l in L if l != "")

def build_chunk(fs, ok, regfuncs, plain, decl_lines, data_names, path, shadow=None):
    """one translation unit: prototypes, an adapter F_r for every register function that is called, implementations F_c and stubs F of fs.
    Calls from the bodies go through BR_F, defined in bridge_sel.h as F_c (bridged function) or F_r (adapter: the original)."""
    decl_lines = [re.sub(r"extern undefined (float_\w+);", r"extern float \1;", l) for l in decl_lines]      # float literals Ghidra left untyped (as tools/autocompile.py)
    declared = set()
    for l in decl_lines:
        m_ = re.search(r"extern [^;(]*?(\w+)(?:\[[^\]]*\])?;", l)
        if m_: declared.add(m_.group(1))
    under = re.compile(r"\b_((?:g_|float_|double_|k_f_|DAT_)\w*)\b")      # `_g_x` / `_float_1_0`: a global overlapping smaller symbols; the declaration has no underscore
    fs = [dict(f, body=under.sub(lambda m_: m_.group(1) if m_.group(1) in declared and m_.group(0) not in declared else m_.group(0), f["body"])) for f in fs]
    body_text = "\n".join(f["body"] for f in fs)
    used = set(re.findall(r"[A-Za-z_]\w*", body_text))
    defined = {f["name"] for f in fs}
    out = ['#include "prelude.h"', '#include "ghidra_types.h"']
    for l in decl_lines:
        m = re.search(r"extern [^;]*?([A-Za-z_]\w*)(\(|\s*\[|;)", l)
        if not m or m.group(1) not in used or m.group(1) in plain or m.group(1) in regfuncs: continue
        out.append(l)
    for n in sorted(used & set(plain)):
        if n not in defined: out.append(plain[n])
    called = sorted((used & set(regfuncs)) | ({f["name"] for f in fs} if shadow is not None else set()))      # the shadow function of F calls F_r too
    if shadow is not None:
        out.append("extern int shadow_take(int); extern volatile int shadow_depth; extern void shadow_begin(unsigned, unsigned, unsigned, unsigned);")
        out.append("extern void shadow_save1(void); extern void shadow_restore0(void); extern void shadow_restore1(void); extern void shadow_compare(int, unsigned, unsigned, int);")
    for n in called:
        f = regfuncs[n]
        out.append("static %s;" % proto_of(f, n + "_r"))
        if n in ok and n not in defined: out.append("extern %s;" % proto_of(f, n + "_c"))
    out.append('#include "bridge_sel.h"')
    out.append("")
    for n in called:
        f = regfuncs[n]
        tgt = None
        if shadow is not None and n in shadow["tramp"] and n in ok:       # a function that is not patched needs no trampoline: its original is untouched
            out.append("extern void %s_tramp(void);" % n); tgt = n + "_tramp"
        out.append("/* %s_r */" % n); out.append("extern void %s(void);" % n); out.append(gen_adapter(f, tgt))
    for f in fs:
        body = f["body"]
        for n in sorted(set(re.findall(r"\b(\w+)\(", body)) & set(regfuncs)):
            body = re.sub(r"\b%s\(" % n, "BR_%s(" % n, body)
        out.append("/* %s */" % f["name"])
        out.append("%s;" % proto_of(f, f["name"] + "_c"))
        out.append("%s %s_c(%s)\n%s" % (f["ret"], f["name"], c_params(f["params"]), body))
        if shadow is not None:
            out.append(gen_tramp(f, shadow["tramp"][f["name"]]))
            out.append("%s;" % proto_of(f, f["name"] + "_shadow"))
            out.append(gen_shadow(f, shadow["id"][f["name"]]))
            out.append(gen_stub(f, f["written"], f["name"] + "_shadow"))
        else:
            out.append(gen_stub(f, f["written"]))
    src = "\n".join(out) + "\n"
    src = fix_known(src)
    from fix_decls import fix
    open(path, "w").write(fix(src))
    spans, cur = {}, None
    for i, l in enumerate(fix(src).split("\n"), 1):
        mm = re.match(r"/\* (\S+) \*/$", l)
        if mm: cur = mm.group(1)
        if cur: spans.setdefault(cur, [i, i])[1] = i
    return spans


def write_sel(path, regfuncs, ok, enabled=None):
    """bridge_sel.h: BR_F -> F_c for the bridged functions that are enabled, F_r (the original, through the adapter) for all others"""
    with open(path, "w") as f:
        f.write("/* generated by tools/bridgegen.py (build_overlay.py rewrites it when BRIDGE_ENABLED is set): which register functions run as C */\n")
        for n in sorted(regfuncs):
            on = n in ok and (enabled is None or n in enabled)
            f.write("#define BR_%s %s_%s\n" % (n, n, "c" if on else "r"))


def compile_syntax(workdir, tag):
    vc7 = os.path.join(HERE, "vc7.sh")
    r = subprocess.run([vc7, workdir, "cl.exe", "/nologo", "/c", "/Zs", "/W0", tag + ".c"], capture_output=True, text=True)
    return r.stdout + r.stderr


def main():
    out = os.path.abspath(sys.argv[1])
    limit = int(sys.argv[sys.argv.index("--limit") + 1]) if "--limit" in sys.argv else 10 ** 9
    csz = int(sys.argv[sys.argv.index("--chunk") + 1]) if "--chunk" in sys.argv else 30
    os.makedirs(out, exist_ok=True)
    blocks, sigs, regin, man = load()
    addrs = sorted(a for a, _ in blocks.values())
    nexts = {a: addrs[i + 1] if i + 1 < len(addrs) else a + 0x400 for i, a in enumerate(addrs)}
    regfuncs, cand, skipped = {}, {}, {}
    for name in sorted(blocks, key=lambda n: blocks[n][0]):
        f, why = analyse(name, blocks, sigs, regin, man, nexts)
        if f is None:
            if why != "no register parameter": skipped[name] = why
            continue
        f["written"] = clobber_util.written(f["code"]) if f["code"] else {"eax", "ecx", "edx"}
        regfuncs[name] = f
        if name not in man and len(cand) < limit: cand[name] = f
    synth_adapters(blocks, regin, regfuncs)      # register callees without a decompiled signature: F_r adapter only
    plain = plain_protos(blocks, regin, set(regfuncs) | set(skipped))
    bad_callees = UNRELIABLE | set(skipped)     # functions whose calling convention we cannot express: their callers are skipped as well
    for n in list(cand):
        called = set(re.findall(r"\b(\w+)\(", cand[n]["body"])) - {n}
        hit = sorted(called & bad_callees)
        if hit:
            skipped[n] = "calls a function with an unreliable / unsupported calling convention: " + ", ".join(hit[:3])
            del cand[n]; regfuncs.pop(n, None)
    decl_lines = open(os.path.join(ROOT, "output", "decls.h"), errors="replace").read().split("\n")
    for h in ("prelude.h", "ghidra_types.h"):
        open(os.path.join(out, h), "w").write(open(os.path.join(ROOT, "src", "auto", h)).read())
    ok = dict(cand)
    shadow = None
    if "--shadow" in sys.argv:       # shadow build: only functions whose first instructions can be moved into a trampoline
        shadow = {"tramp": {}, "id": {}}
        no_tramp = []
        for n in list(ok):
            tb = tramp_bytes(ok[n])
            if tb is None or re.match(r"(longlong|ulonglong|undefined8)$", ok[n]["ret"]) or "(" in " ".join(p[1] for p in ok[n]["params"]):
                no_tramp.append(n); del ok[n]
            else: shadow["tramp"][n] = tb
        open(os.path.join(out, "shadow_skipped.txt"), "w").write("\n".join(no_tramp) + "\n")
        print("shadow build: %d functions, %d without a trampoline" % (len(ok), len(no_tramp)))
    failed = {}
    for rnd in range(6):
        names = list(ok)
        chunks = [[ok[n] for n in names[i:i + csz]] for i in range(0, len(names), csz)]
        bad = {}
        for ci, fs in enumerate(chunks):
            tag = "b%03d" % (ci + 1)
            write_sel(os.path.join(out, "bridge_sel.h"), regfuncs, ok, set() if shadow else None)
            if shadow: shadow["id"] = {n: i for i, n in enumerate(names)}
            spans = build_chunk(fs, ok, regfuncs, plain, decl_lines, None, os.path.join(out, tag + ".c"), shadow)
            text = compile_syntax(out, tag)
            for l in text.split("\n"):
                m = re.match(r"\S+\((\d+)\) : (?:fatal )?error (\w+): (.*)", l)
                if not m: continue
                ln = int(m.group(1))
                for n, (a, b) in spans.items():
                    if a <= ln <= b:
                        bad.setdefault(n.replace("_r", "") if n.endswith("_r") else n, m.group(2) + " " + m.group(3)); break
                else:
                    bad.setdefault("__chunk_%s" % tag, m.group(2) + " " + m.group(3))
        print("round %d: %d functions, %d failed" % (rnd + 1, len(ok), len(bad)), file=sys.stderr)
        if not bad: break
        for n, e in bad.items():
            if n in ok: failed[n] = e; del ok[n]
            elif n.startswith("__chunk_"): failed[n] = e
        if all(n.startswith("__chunk_") for n in bad):
            print("errors outside any function:", bad, file=sys.stderr); break
    write_sel(os.path.join(out, "bridge_sel.h"), regfuncs, ok, set() if shadow else None)
    if shadow:
        names = list(ok)
        open(os.path.join(out, "shadow_names.txt"), "w").write("\n".join(names) + "\n")
    nchunks = (len(ok) + csz - 1) // csz
    for k in range(nchunks + 1, 100):        # chunk files of earlier rounds that are no longer part of the result
        stale = os.path.join(out, "b%03d.c" % k)
        if os.path.exists(stale): os.remove(stale)
    with open(os.path.join(out, "failed.txt"), "w") as fh:
        for n, e in sorted(failed.items()): fh.write("%s\t%s\n" % (n, e))
    with open(os.path.join(out, "skipped.txt"), "w") as fh:
        for n, w in sorted(skipped.items()): fh.write("%s\t%s\n" % (n, w))
    print("register functions: %d candidates, %d compile, %d failed, %d skipped" % (len(cand), len(ok), len(failed), len(skipped)))


if __name__ == "__main__":
    main()
