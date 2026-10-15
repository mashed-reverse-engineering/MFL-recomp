#!/usr/bin/env python3
"""Return types of the `extern undefined name();` declarations in autocompile output (stdlib only).
Ghidra's `undefined` is a 1 byte type, so every call result is truncated to 8 bits by cl (movzx eax,al): a returned pointer or int is destroyed, a float
returned in st(0) is read from eax.  Rewrites each such declaration to `int` (eax) or `double` when the body uses the call as a float
((float10)name(...) / (float)name(...)).  Usage: fix_decls.py file.c ...   (in place)"""
import os, re, sys
_wp = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "names", "MFL_ret_wide.txt")
WIDE = {l.strip() for l in open(_wp) if l.strip() and not l.startswith("#")} if os.path.exists(_wp) and "ret_wide" in os.environ.get("FIX_RULES", "widths,ret_wide,conv,locals,imports,libprotos").split(",") else set()      # tools/ret_widths.py: callers read the whole eax


_cp = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "names", "MFL_conv_fix.txt")
CONV = {l.split()[0]: l.split()[1] for l in open(_cp) if len(l.split()) == 2 and not l.startswith("#")} if os.path.exists(_cp) and "conv" in os.environ.get("FIX_RULES", "widths,ret_wide,conv,locals,imports,libprotos").split(",") else {}      # tools/conv_audit.py

_ip = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "names", "MFL_import_protos.txt")
IMPORTS = {re.search(r"__stdcall (\w+)\(", l).group(1): l.strip() for l in open(_ip) if l.strip()} if os.path.exists(_ip) and "imports" in os.environ.get("FIX_RULES", "widths,ret_wide,conv,locals,imports,libprotos").split(",") else {}      # tools/import_protos.py

_lp = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "names", "MFL_lib_protos.txt")
LIBP = {re.search(r"(\w+)\(", l).group(1): l.strip() for l in open(_lp) if l.startswith("/*")} if os.path.exists(_lp) and "libprotos" in os.environ.get("FIX_RULES", "widths,ret_wide,conv,locals,imports,libprotos").split(",") else {}      # tools/lib_protos.py: Lua 4.0 API / RenderWare SDK prototypes instead of `extern ... f();`


def fix(text):
    if LIBP:      # before CONV: a conv fix still overrides the __cdecl of the prototype
        text = re.sub(r"^(?:/\*[0-9a-f]+\*/ )?extern [^;(]*?\b(\w+)\(\);", lambda m: LIBP.get(m.group(1), m.group(0)), text, flags=re.M)
    if IMPORTS:
        body = text
        add = [IMPORTS[n] for n in sorted(IMPORTS) if re.search(r"\b%s\(" % n, body) and not re.search(r"extern [^;]*\b%s\(" % n, body)]
        if add:
            k = text.index("\n", text.rindex("#include")) + 1 if "#include" in text else 0
            text = text[:k] + "\n".join(add) + "\n" + text[k:]
    if CONV:
        text = re.sub(r"^((?:/\*[0-9a-f]+\*/ )?extern [^;(]*?)(?:__cdecl |__stdcall )?(\w+)\(", lambda m: (m.group(1) + "__%s " % CONV[m.group(2)] + m.group(2) + "(") if m.group(2) in CONV else m.group(0), text, flags=re.M)
    floats = set(re.findall(r"\(float10\)\s*(\w+)\s*\(", text)) | set(re.findall(r"\(float\)\s*(\w+)\s*\(", text))

    def sub(m):
        return "%sextern %s %s();" % (m.group(1), "double" if m.group(2) in floats else "int", m.group(2))
    text = re.sub(r"^(/\*[0-9a-f]+\*/ )extern undefined (\w+)\(\);", sub, text, flags=re.M)
    return re.sub(r"^((?:/\*[0-9a-f]+\*/ )?extern )undefined((?: __\w+)? (\w+)\(.+\);)$", lambda m: m.group(1) + ("int" if m.group(3) in WIDE else "undefined") + m.group(2), text, flags=re.M)


if __name__ == "__main__":
    for p in sys.argv[1:]:
        t = open(p).read()
        n = fix(t)
        if n != t:
            open(p, "w").write(n)
            print("fixed", p, sum(1 for a, b in zip(t.splitlines(), n.splitlines()) if a != b), "declarations")
