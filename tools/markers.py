#!/usr/bin/env python3
"""Address markers (the idea of reccmp's FUNCTION annotations): a comment `// FUNCTION: MFL 0x<va>` above every function of src/*.c that is listed in src/manifest.txt.
Usage: markers.py --add     insert the missing markers (idempotent)
       markers.py --check   verify them: every manifest function has exactly one marker with its own address, no address is used twice, no marker without a function
The manifest stays the source of truth; the markers make a source file readable on its own and let other tools find the original function."""
import glob, os, re, sys
R = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "src")
MARK = re.compile(r"^// FUNCTION: MFL 0x([0-9a-f]{8})\s*$")

def manifest():
    m = {}
    for l in open(os.path.join(R, "manifest.txt")):
        l = l.strip()
        if l and not l.startswith("#"):
            p = l.split()
            if len(p) >= 3 and p[0] != "(none)":
                m[(p[0], p[1])] = int(p[2], 16)
    return m

def find_def(lines, sym):
    pat = re.compile(r"^(?:__declspec\(naked\)\s+)?[A-Za-z_][\w\s\*]*?\b%s\s*\([^;{]*\)\s*(?:/\*.*\*/)?\s*$" % re.escape(sym))
    for i, l in enumerate(lines):
        if pat.match(l) and i + 1 < len(lines) and lines[i + 1].startswith("{"):
            return i
    return None

def symbols():
    d = {}
    for l in open(os.path.join(R, "symbols.txt")):
        p = l.split()
        if len(p) == 2:
            try: d.setdefault(p[0], []).append(int(p[1], 16))
            except ValueError: pass
    return d

def main():
    mode = sys.argv[1] if len(sys.argv) > 1 else "--check"
    m, bad, added = manifest(), 0, 0
    sy = symbols()
    for (f, sname), va in m.items():          # the overlay patches the address that symbols.txt gives for the name: it must be the manifest address
        if not sy.get(sname) or sy[sname][0] != va:
            print("manifest %s %s 0x%08x: the overlay resolves the name to %s (first entry of symbols.txt)" % (f, sname, va, ['0x%08x' % a for a in sy.get(sname, [])] or 'nothing')); bad += 1
        elif len(set(sy[sname])) > 1:
            print("note: %s exists at %d addresses in symbols.txt (the first one is ours)" % (sname, len(set(sy[sname]))))
    byfile = {}
    for (f, s), va in m.items(): byfile.setdefault(f, []).append((s, va))
    seen = {}
    for f, items in sorted(byfile.items()):
        path = os.path.join(R, f)
        if not os.path.exists(path): print("missing file", f); bad += 1; continue
        lines = open(path, encoding="latin-1").read().split("\n")
        for s, va in items:
            i = find_def(lines, s)
            if i is None: print("definition not found: %s %s" % (f, s)); bad += 1; continue
            j = i - 1
            while j >= 0 and lines[j].startswith("__declspec"): j -= 1
            cur = MARK.match(lines[j]) if j >= 0 else None
            if cur and int(cur.group(1), 16) != va:
                print("%s %s: marker %s but manifest %08x" % (f, s, cur.group(1), va)); bad += 1
            elif not cur:
                if mode == "--add":
                    lines.insert(i, "// FUNCTION: MFL 0x%08x" % va); added += 1
                else:
                    print("no marker: %s %s" % (f, s)); bad += 1
        if mode == "--add":
            open(path, "w", encoding="latin-1").write("\n".join(lines))
        for k, l in enumerate(lines):
            c = MARK.match(l)
            if c:
                a = int(c.group(1), 16)
                if a in seen and seen[a] != (f, k): print("address %08x marked twice: %s and %s" % (a, seen[a][0], f)); bad += 1
                seen[a] = (f, k)
    print("%d markers added, %d problems" % (added, bad))
    sys.exit(1 if bad and mode == "--check" else 0)

main()
