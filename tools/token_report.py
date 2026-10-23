#!/usr/bin/env python3
"""Token usage per day and per agent from Claude Code transcripts.

Usage: token_report.py [session.jsonl ...]   (default: every session of this project)
Reads the main transcript and its subagents/*.jsonl; each assistant message id is counted once.
Columns: out = output tokens, new = input + cache-creation tokens, cache = cache-read tokens.
Agent names come from the first user line of the subagent transcript (Name yourself X-...)."""
import glob, json, os, re, sys, collections

PROJ = os.path.expanduser("~/.claude/projects/-home-daniel-git-mashed")

def scan(path):
    seen, tot = set(), collections.defaultdict(lambda: [0, 0, 0, 0])   # day -> out,new,cache,msgs
    name = None
    with open(path, errors="replace") as f:
        for line in f:
            if name is None and '"type":"user"' in line[:200]:
                m = re.search(r"Name yourself ([A-Za-z0-9-]+)", line)
                if m: name = m.group(1)
            if '"usage"' not in line: continue
            try: d = json.loads(line)
            except ValueError: continue
            msg = d.get("message") or {}
            u, mid = msg.get("usage"), msg.get("id")
            if not u or (mid and mid in seen): continue
            if mid: seen.add(mid)
            t = tot[(d.get("timestamp") or "")[:10]]
            t[0] += u.get("output_tokens", 0)
            t[1] += u.get("input_tokens", 0) + u.get("cache_creation_input_tokens", 0)
            t[2] += u.get("cache_read_input_tokens", 0); t[3] += 1
    return name, tot

def snapshot(tot):
    """Append cumulative tokens + replaced code bytes to output/token_ledger.csv (tokens per KB = diff of two rows)."""
    import subprocess, datetime
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    subprocess.run([sys.executable, root + "/tools/progress.py"], capture_output=True)
    d = json.load(open(root + "/progress/progress.json"))
    rep = d["replaced_in_pipeline"]; fn = sum(v["functions"] for v in rep.values()) + d["clusters"]["functions"]
    by = sum(v["bytes"] for v in rep.values()) + d["clusters"]["bytes"]
    path = root + "/output/token_ledger.csv"; new = not os.path.exists(path)
    with open(path, "a") as f:
        if new: f.write("time,out,new_in,cache_read,replaced_functions,replaced_bytes\n")
        f.write("%s,%d,%d,%d,%d,%d\n" % (datetime.datetime.now().isoformat(timespec="minutes"), *tot, fn, by))
    print("snapshot ->", path)

def main():
    snap = "--snapshot" in sys.argv
    sys.argv = [a for a in sys.argv if a != "--snapshot"]
    files = sys.argv[1:] or sorted(glob.glob(PROJ + "/*.jsonl"))
    rows = collections.defaultdict(lambda: [0, 0, 0, 0]); days = collections.defaultdict(lambda: [0, 0, 0, 0])
    for p in files:
        subs = sorted(glob.glob(p[:-6] + "/subagents/agent-*.jsonl"))
        for q, label in [(p, "main " + os.path.basename(p)[:8])] + [(s, None) for s in subs]:
            name, tot = scan(q)
            lab = label or (name or "sub " + os.path.basename(q)[6:14])
            for day, v in tot.items():
                for i in range(4): rows[lab][i] += v[i]; days[day][i] += v[i]
    for title, tab in (("per day", days), ("per agent (top 25 by output)", rows)):
        print(f"\n{title}\n{'':26}{'out':>11}{'new in':>13}{'cache read':>15}{'msgs':>8}")
        items = sorted(tab.items(), key=lambda kv: kv[0] if title == "per day" else -kv[1][0])
        for k, v in items[:25 if title != "per day" else None]:
            print(f"{k:26}{v[0]:>11,}{v[1]:>13,}{v[2]:>15,}{v[3]:>8,}")
    if snap: snapshot(tuple(sum(v[i] for v in days.values()) for i in range(3)))

main()
