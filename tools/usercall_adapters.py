#!/usr/bin/env python3
"""usercall_adapters.py: adapters for the 'usercall' blocker class of output/tier2_census.csv (reuses tools/bridgegen.py: analyse / build_chunk / gen_stub / gen_adapter, nothing enabled).
Usage: usercall_adapters.py [outdir=output/usercall_candidates]
Per usercall function (census): entry thunk F (naked, at the original address: register args -> cdecl F_c, ret N, callee-saved untouched) + C body F_c + call adapter F_r (C callers -> original),
all emitted by bridgegen.build_chunk; bridge_sel.h maps every BR_F to F_r (= nothing enabled).  Not adaptable: no C prototype / pseudo registers / extraout / 64 bit / labels / register inputs the
decompiler lost (reginputs.csv != register parameters) / no register input known.  Output: <outdir>/b###.c + bridge_sel.h + names.txt, output/usercall_plan.csv.
plan columns: function,size,register_inputs,ret_n,has_c_candidate,c_callers,existing_bridge,status (generated | needs shadow | not adaptable),reason"""
import csv, glob, os, re, sys
HERE = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, HERE)
import bridgegen as bg, clobber_util
G = bg.ROOT
out = os.path.abspath(sys.argv[1] if len(sys.argv) > 1 else os.path.join(G, "output", "usercall_candidates"))
os.makedirs(out, exist_ok=True)
census = [r for r in csv.DictReader(open(os.path.join(G, "output", "tier2_census.csv"))) if r["blocker"] == "usercall"]
blocks, sigs, regin, man = bg.load()
addrs = sorted(a for a, _ in blocks.values())
nexts = {a: addrs[i + 1] if i + 1 < len(addrs) else a + 0x400 for i, a in enumerate(addrs)}
bridged = set(); disabled = set()
for f in glob.glob(os.path.join(G, "src", "bridges", "b*.c")): bridged |= set(re.findall(r"^__declspec\(naked\) void (\w+)\(void\)", open(f).read(), re.M))
disabled = bridged - set(open(os.path.join(G, "src", "bridges", "enabled.txt")).read().split())
callers = {}      # name -> number of our C files (outside bridges / candidates) that call it
for f in glob.glob(os.path.join(G, "src", "**", "*.c"), recursive=True):
    if "/bridges" in f: continue
    for n in set(re.findall(r"\b(\w+)\(", open(f, errors="replace").read())): callers[n] = callers.get(n, 0) + 1
RESID = re.compile(r"\b(in_[A-Z]{2,3}|unaff_\w+|extraout_\w+|stack0x\w+|param_\d+_00\w+|[iu]?Stack[0-9a-f]{8}|in_stack_\w+|POPCOUNT|SBORROW\d|CARRY\d|halt_baddata|ABS|fpatan|fsin|fcos|fptan|fyl2x|f2xm1)\b")
regfuncs_ok = bg.adapter_ok(blocks, sigs, regin, man, nexts)      # callees that bridgegen can give an F_r adapter
cand, regfuncs = {}, {}
synth, synth_rej = bg.synth_adapters(blocks, regin, regfuncs)      # register callees without a decompiled signature (empty stack types in regsigs.csv): F_r only
print(len(synth), "synthesised F_r adapters,", len(synth_rej), "rejected", file=sys.stderr)
regfuncs_ok |= set(synth)
status = {}      # name -> (status, reason)
for r in census:
    n = r["name"]; ri = regin.get(n, ("", 0))
    f, why = bg.analyse(n, blocks, sigs, regin, man, nexts)
    body = blocks[n][1] if n in blocks else ""
    if not ri[0] and n in blocks and re.search(r"\breg_e\w\w\b", body) and not RESID.search(body):
        # `reg_eax` is the name Ghidra gives a LOCAL that is passed in a register to a callee that has register parameters (SetRegisterParams): the function itself is plain cdecl, no entry thunk
        rc = sorted({c for c in re.findall(r"\b(\w+)\s*\(", body) if regin.get(c, ("",))[0] and c != n})
        miss = [c for c in rc if c not in regfuncs_ok]
        status[n] = ("reg-callee (plain function)", "calls register functions %s%s" % (",".join(rc[:4]), "; no adapter for " + ",".join(miss[:3]) if miss else " (all have F_r)"))
        continue
    if f is None:
        status[n] = ("not adaptable", why if ri[0] else "pseudo register, no known register input: " + why); continue
    decl = {p[3] for p in f["params"] if p[2] == "reg"}
    if set(ri[0].split("|")) - {""} != decl:
        status[n] = ("not adaptable", "register inputs %s vs decompiled register params %s (a lost input would be skipped by the thunk)" % (ri[0], ",".join(sorted(decl)))); continue
    f["written"] = clobber_util.written(f["code"]) if f["code"] else {"eax", "ecx", "edx"}
    regfuncs[n] = cand[n] = f
for n in set(blocks) - set(cand):        # F_r adapters for register callees outside the census set (already bridged / ledger)
    f, why = bg.analyse(n, blocks, sigs, regin, man, nexts)
    if f: f["written"] = {"eax", "ecx", "edx"}; regfuncs[n] = f
plain = bg.plain_protos(blocks, regin, set(regfuncs) | {r["name"] for r in census})
decl_lines = open(os.path.join(G, "output", "decls.h"), errors="replace").read().split("\n")
for h in ("prelude.h", "ghidra_types.h"): open(os.path.join(out, h), "w").write(open(os.path.join(G, "src", "auto", h)).read())
ok = dict(cand); failed = {}
for rnd in range(12):
    names = list(ok); bad = {}
    bg.write_sel(os.path.join(out, "bridge_sel.h"), regfuncs, ok, set())
    for ci in range(0, len(names), 30):
        tag = "b%03d" % (ci // 30 + 1)
        spans = bg.build_chunk([ok[n] for n in names[ci:ci + 30]], ok, regfuncs, plain, decl_lines, None, os.path.join(out, tag + ".c"))
        for l in bg.compile_syntax(out, tag).split("\n"):
            m = re.match(r"\S+\((\d+)\) : (?:fatal )?error (\w+): (.*)", l)
            if m:
                for n, (a, b) in spans.items():
                    if a <= int(m.group(1)) <= b: bad.setdefault(n[:-2] if n.endswith("_r") else n, m.group(2) + " " + m.group(3)); break
    print("round", rnd + 1, len(ok), "functions,", len(bad), "failed", file=sys.stderr)
    if not bad: break
    for n, e in bad.items():
        if n in ok: failed[n] = e; del ok[n]
        else: print("chunk-level error", n, e, file=sys.stderr); sys.exit(1)
nch = (len(ok) + 29) // 30
for k in range(nch + 1, 60):
    p = os.path.join(out, "b%03d.c" % k)
    if os.path.exists(p): os.remove(p)
open(os.path.join(out, "names.txt"), "w").write("\n".join(ok) + "\n")
rows = []
for r in census:
    n = r["name"]; ri = regin.get(n, ("", 0))
    if n in ok: st, why = ("needs shadow", "bridge exists but disabled (shadow / sweep culprit)") if n in disabled else ("generated", "")
    elif n in failed: st, why = "not adaptable", "C body does not compile: " + failed[n][:100]
    else: st, why = status[n]
    rows.append(dict(function=n, size=r["size"], register_inputs=ri[0], ret_n=ri[1], has_c_candidate="yes" if n in ok else "no", c_callers=callers.get(n, 0),
                     existing_bridge=("disabled" if n in disabled else "enabled" if n in bridged else ""), status=st, reason=why))
rows.sort(key=lambda r: -int(r["size"]))
with open(os.path.join(G, "output", "usercall_plan.csv"), "w", newline="") as f:
    w = csv.DictWriter(f, list(rows[0])); w.writeheader(); w.writerows(rows)
import collections
c, b = collections.Counter(), collections.Counter()
for r in rows:
    k = (r["status"], bool(r["register_inputs"])); c[k] += 1; b[k] += int(r["size"])
for k in sorted(c): print("%-14s known-reg-input=%-5s %4d fns %7d bytes" % (k[0], k[1], c[k], b[k]))
