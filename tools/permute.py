#!/usr/bin/env python3
"""Source permuter for byte matching (idea from decomp-permuter): mutates one C function (operand order, comparison direction,
declaration order/initialisers, ++ forms), compiles up to N variants in ONE translation unit with cl 13.00.9466 and keeps the
variant whose code is closest to the bytes at the original address (difflib ratio on the relocation-masked bytes).
Usage: permute.py <file.c> <function> <VA hex> [rounds=6] [variants per round=40] [seed]
Writes the best source to <file>.best.c (only the function body changes) and prints the score per round; score 1.0 = byte identical."""
import difflib, os, random, re, shutil, subprocess, sys, tempfile
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import cmpfn
import asmdiff

OPS = ["+", "*", "==", "!=", "&&", "||", "&", "|"]
FLIP = {"<": ">", ">": "<", "<=": ">=", ">=": "<="}
ATOM = r"(?:[A-Za-z_][\w\.]*(?:\[[^\]\[]*\])?(?:->\w+)?|0x[0-9a-fA-F]+|-?\d+)"


def split_function(src, name):
    m = re.search(r"^[^\n;{}]*\b%s\([^;{]*\)\s*\n?\{" % re.escape(name), src, re.M)
    if not m:
        raise SystemExit("function %s not found" % name)
    i = m.end()
    depth = 1
    while depth:
        c = src[i]
        depth += (c == "{") - (c == "}")
        i += 1
    return src[:m.start()], src[m.start():i], src[i:]


def mutate(fn, rng):
    kinds = ["swap", "flip", "decl_order", "split_init", "join_init", "incr", "stmt_swap", "temp", "sign", "zero", "ifelse", "preinc"]
    for _ in range(20):
        k = rng.choice(kinds)
        if k == "swap":
            ms = list(re.finditer(r"(%s) (%s) (%s)" % (ATOM, "|".join(re.escape(o) for o in OPS), ATOM), fn))
            if ms:
                m = rng.choice(ms)
                return fn[:m.start()] + "%s %s %s" % (m.group(3), m.group(2), m.group(1)) + fn[m.end():]
        elif k == "flip":
            ms = list(re.finditer(r"(%s) (<=|>=|<|>) (%s)" % (ATOM, ATOM), fn))
            if ms:
                m = rng.choice(ms)
                return fn[:m.start()] + "%s %s %s" % (m.group(3), FLIP[m.group(2)], m.group(1)) + fn[m.end():]
        elif k == "decl_order":
            lines = fn.split("\n")
            idx = [i for i, l in enumerate(lines) if re.match(r"\s+(?:struct \w+|unsigned|int|float|char|short|void)\b[^();=]*;\s*$", l) or re.match(r"\s+(?:struct \w+|unsigned|int|float|char)\b[^();]*=[^;]*;\s*$", l)]
            adj = [i for i in idx if i + 1 in idx and "=" not in lines[i + 1].split(";")[0] or (i in idx and i + 1 in idx)]
            if adj:
                i = rng.choice(adj)
                lines[i], lines[i + 1] = lines[i + 1], lines[i]
                return "\n".join(lines)
        elif k == "stmt_swap":
            lines = fn.split("\n")
            idx = [i for i in range(len(lines) - 1) if re.match(r"\s+[^\s{}()][^{};]*;\s*$", lines[i]) and re.match(r"\s+[^\s{}()][^{};]*;\s*$", lines[i + 1])
                   and not re.match(r"\s+(?:return|struct|unsigned|int|float|char|short)\b", lines[i]) and not re.match(r"\s+(?:return|struct|unsigned|int|float|char|short)\b", lines[i + 1])]
            if idx:
                i = rng.choice(idx)
                lines[i], lines[i + 1] = lines[i + 1], lines[i]
                return "\n".join(lines)
        elif k == "temp":      # a = b; -> new temp loaded first (helps load/store ordering)
            ms = list(re.finditer(r"^(\s+)(\w+) = (\w+\[\d+\]);$", fn, re.M))
            if ms:
                m = rng.choice(ms)
                t = "t%d" % rng.randrange(100)
                decl = "\n    int %s;" % t
                head = re.search(r"\{\n", fn)
                new = fn[:m.start()] + "%s%s = %s;\n%s%s = %s;" % (m.group(1), t, m.group(3), m.group(1), m.group(2), t) + fn[m.end():]
                h = head.end()
                return new[:h] + "    int %s;\n" % t + new[h:]
        elif k == "sign":        # Echo rule "toggle signedness": int <-> unsigned int for one declaration / parameter
            ms = list(re.finditer(r"\b(unsigned int|int)\b(?= \**\w+[,;)=\[])", fn))
            if ms:
                m = rng.choice(ms)
                new = "int" if m.group(1) == "unsigned int" else "unsigned int"
                return fn[:m.start()] + new + fn[m.end():]
        elif k == "zero":        # x != 0 <-> x, x == 0 <-> !x
            ms = list(re.finditer(r"\b(\w+(?:->\w+|\[\w+\])?) (!=|==) 0\b", fn))
            if ms:
                m = rng.choice(ms)
                new = m.group(1) if m.group(2) == "!=" else "!" + m.group(1)
                return fn[:m.start()] + new + fn[m.end():]
        elif k == "ifelse":      # Echo rule "invert branch": if (c) A else B -> if (!(c)) B else A (single statement bodies only)
            ms = list(re.finditer(r"if \(([^\n]+)\)\n(\s+)([^\n{};]+;)\n\s+else\n\s+([^\n{};]+;)", fn))
            if ms:
                m = rng.choice(ms)
                return fn[:m.start()] + "if (!(%s))\n%s%s\n%selse\n%s%s" % (m.group(1), m.group(2), m.group(4), m.group(2)[:-4], m.group(2), m.group(3)) + fn[m.end():]
        elif k == "preinc":
            ms = list(re.finditer(r"\b(\w+)(\+\+|--);", fn))
            if ms:
                m = rng.choice(ms)
                return fn[:m.start()] + "%s%s;" % (m.group(2), m.group(1)) + fn[m.end():]
        elif k == "split_init":
            ms = list(re.finditer(r"^(\s+)((?:struct \w+|unsigned int|unsigned|int|float|char|short) \*?)(\w+) = ([^;]+);$", fn, re.M))
            if ms:
                m = rng.choice(ms)
                return fn[:m.start()] + "%s%s%s;\n%s%s = %s;" % (m.group(1), m.group(2), m.group(3), m.group(1), m.group(3), m.group(4)) + fn[m.end():]
        elif k == "join_init":
            ms = list(re.finditer(r"^(\s+)((?:struct \w+|unsigned int|unsigned|int|float|char|short) \*?)(\w+);\n\s+\3 = ([^;]+);$", fn, re.M))
            if ms:
                m = rng.choice(ms)
                return fn[:m.start()] + "%s%s%s = %s;" % (m.group(1), m.group(2), m.group(3), m.group(4)) + fn[m.end():]
        elif k == "incr":
            ms = list(re.finditer(r"(\b\w+)(\+\+|--)(?=;)|(\b\w+) \+= 1;|(\b\w+) = \4 \+ 1;", fn))
            if ms:
                m = rng.choice(ms)
                v = m.group(1) or m.group(3) or m.group(4)
                op = "-" if (m.group(2) == "--") else "+"
                alt = rng.choice(["%s%s%s" % (v, op, op), "%s %s= 1" % (v, op), "%s = %s %s 1" % (v, v, op)])
                return fn[:m.start()] + alt + fn[m.end():]
    return fn


def score_all(pre, variants, post, name, va, flags):
    os.makedirs(cmpfn.WORKROOT, exist_ok=True)
    w = tempfile.mkdtemp(prefix="permute_", dir=cmpfn.WORKROOT)        # below the mount of the persistent compiler container
    try:
        body = pre
        for i, v in enumerate(variants):
            body += re.sub(r"\b%s\(" % re.escape(name), "%s_v%d(" % (name, i), v, count=1) + "\n"
        body += post
        open(os.path.join(w, "f.c"), "w").write(body)
        d = os.path.dirname(os.path.abspath(sys.argv[1]))
        for h in os.listdir(d):
            if h.endswith(".h"):
                shutil.copy(os.path.join(d, h), w)
        subprocess.run([cmpfn.VC7, w, "cl.exe", "/nologo", "/c", "/Gy"] + flags + ["f.c"], capture_output=True, text=True)
        out = []
        for i in range(len(variants)):
            try:
                code, masked = cmpfn.obj_function(os.path.join(w, "f.obj"), "%s_v%d" % (name, i))
            except (SystemExit, FileNotFoundError):
                out.append(0.0); continue
            code = code.rstrip(b"\x90\xcc")
            cm = bytearray(code)
            ms = sorted(masked)
            for g in range(0, len(ms) - 3, 4):      # relocated fields -> placeholder address (same as cmpfn)
                for k, v in enumerate((0x00, 0x00, 0x50, 0x00)):
                    if ms[g + k] < len(cm):
                        cm[ms[g + k]] = v
            orig = cmpfn.exe_bytes(va, len(code) + 32)
            if all(c == o for i, (c, o) in enumerate(zip(code, orig)) if i not in masked) and len(code) <= len(orig):
                out.append(1.0)
                continue
            kind, sc, _ = asmdiff.classify(bytes(cm), orig)
            out.append(sc if kind != "IDENTICAL" else 0.9999)      # IDENTICAL after normalisation but not byte equal (e.g. SIB order)
        return out
    finally:
        shutil.rmtree(w, ignore_errors=True)


def main():
    src_path, name, va = sys.argv[1], sys.argv[2], int(sys.argv[3], 16)
    rounds = int(sys.argv[4]) if len(sys.argv) > 4 else 6
    nvar = int(sys.argv[5]) if len(sys.argv) > 5 else 40
    rng = random.Random(int(sys.argv[6]) if len(sys.argv) > 6 else 1)
    pre, fn, post = split_function(open(src_path).read(), name)
    best, best_s = fn, score_all(pre, [fn], post, name, va, ["/O2"])[0]
    print("start", round(best_s, 4))
    for r in range(rounds):
        vs = []
        for _ in range(nvar):
            v = mutate(best, rng)
            if rng.random() < 0.4:
                v = mutate(v, rng)
            vs.append(v)
        sc = score_all(pre, vs, post, name, va, ["/O2"])
        i = max(range(len(vs)), key=lambda k: sc[k])
        if sc[i] > best_s:
            best, best_s = vs[i], sc[i]
            open(src_path[:-2] + ".best.c", "w").write(pre + best + post)      # written at every improvement: a timeout (permute_one.sh) keeps the best so far
        print("round", r + 1, round(best_s, 4))
        if best_s >= 1.0:
            break
    open(src_path[:-2] + ".best.c", "w").write(pre + best + post)
    print(best)


if __name__ == "__main__":
    main()
