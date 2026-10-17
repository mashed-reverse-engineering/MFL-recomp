"""/GS stack-cookie functions: Ghidra shows `uint unaff_retaddr; ... local_N = __security_cookie ^ unaff_retaddr;` (the check is hidden in an
injection comment).  The compiler regenerates both with /GS, so the C text drops them and carries the marker GS_MARK, which
build_overlay.py / diffemu.compile_obj turn into cl /GS for that file."""
import re
GS_MARK = "/* compile: /GS */"
def strip(body):
    """-> body without the cookie statements + marker, or unchanged when the function does not have exactly the plain cookie pattern"""
    m = re.search(r"^[ \t]*(\w+) = __security_cookie \^ unaff_retaddr;[ \t]*\n", body, re.M)
    if not m or body.count("unaff_retaddr") != 2: return body
    v = m.group(1)
    if len(re.findall(r"\b%s\b" % v, body)) != 2: return body      # declaration + the assignment only (a use of the cookie slot elsewhere is not the plain pattern)
    b = body[:m.start()] + body[m.end():]
    b, k = re.subn(r"^[ \t]*\w+ unaff_retaddr;[ \t]*\n", "", b, flags=re.M)
    b, j = re.subn(r"^[ \t]*[\w ]+ %s;[ \t]*\n" % v, "", b, flags=re.M)
    if k != 1 or j != 1: return body
    return GS_MARK + "\n" + b
if __name__ == "__main__":
    import sys
    t = open("output/named/MFL_app_named.c", errors="replace").read(); ok = 0; n = 0
    for p in re.split(r"^// ==== ", t, flags=re.M):
        if "unaff_retaddr" in p:
            n += 1; ok += strip(p) != p
    print(ok, "of", n)
