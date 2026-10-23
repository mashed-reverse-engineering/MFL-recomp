//Create functions for code that Ghidra left as orphan instructions / undefined bytes inside the application gap_ ownership items.
//Args: <listfile "lo hi"> <outprefix> [commit]. Without "commit" every change is rolled back (dry run). Entry rules:
//  CALL ref, DATA ref from an instruction (push/mov offset), DATA ref from .data/.rdata (callback tables), UNCONDITIONAL_JUMP from a function to a target outside that function's
//  address span (tail call), and the item start when the item is code nobody falls into. Jump-table pointers inside .text and jumps inside one function span are chunks, not entries.
//@category Mashed
import java.io.*;
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.mem.*;
import ghidra.program.model.symbol.*;

public class GapCreate extends GhidraScript {
    Set<Long> jumpTargets = new HashSet<>();
    static final long TEXT_LO = 0x401000L, TEXT_HI = 0x4b2a60L;
    @Override
    protected void run() throws Exception {
        String[] a = getScriptArgs();
        boolean commit = a.length > 2 && a[2].equals("commit");
        Listing li = currentProgram.getListing();
        ReferenceManager rm = currentProgram.getReferenceManager();
        AddressSet all = new AddressSet();
        List<Address[]> items = new ArrayList<>();
        try (BufferedReader r = new BufferedReader(new FileReader(a[0]))) {
            String ln;
            while ((ln = r.readLine()) != null) {
                String[] p = ln.trim().split("\\s+"); if (p.length < 2) continue;
                Address lo = toAddr(Long.parseLong(p[0], 16)), hi = toAddr(Long.parseLong(p[1], 16));
                items.add(new Address[]{lo, hi}); all.add(lo, hi.subtract(1));
            }
        }
        int tx = currentProgram.startTransaction("gap functions");
        TreeMap<Address, String> entries = new TreeMap<>();
        // table targets: 3+ consecutive .text pointers in bytes that are not instructions = jump table entries (case blocks, not functions)
        Set<Long> J = new HashSet<>();
        for (Address[] it : items) {
            int size = (int) it[1].subtract(it[0]);
            for (int o = 0; o + 12 <= size; o++) {
                Address q = it.length > 0 ? it[0].add(o) : null;
                if (li.getInstructionContaining(q) != null) continue;
                if (!(ptrAt(q, q) && ptrAt(q.add(4), q) && ptrAt(q.add(8), q))) continue;
                int k = 0; while (o + 4 * (k + 1) <= size + 64 && ptrAt(q.add(4 * k), q) && li.getInstructionContaining(q.add(4 * k)) == null) { try { J.add(currentProgram.getMemory().getInt(q.add(4 * k)) & 0xffffffffL); } catch (Exception e) { break; } k++; }
                o += 4 * k - 1;
            }
        }
        jumpTargets = J;
        for (Address[] it : items) {
            AddressSet s = new AddressSet(it[0], it[1].subtract(1));
            for (Address to : rm.getReferenceDestinationIterator(s, true)) {
                CodeUnit cu = li.getCodeUnitContaining(to);
                if (cu instanceof Data && ((Data) cu).isDefined()) continue;        // data target
                if (getFunctionContaining(to) != null) continue;                      // already inside a function
                if (cu != null && !cu.getAddress().equals(to)) continue;              // mid-instruction
                if (to.getOffset() == 0x4c4b40L) continue;                             // misaligned (inside the instruction run of 4c4a80..4c4b4d, which Ghidra's original orphans conflict with)
                if (ptrAt(to, to)) continue;                                          // starts with a .text pointer: jump / pointer table, not code
                for (Reference rf : rm.getReferencesTo(to)) {
                    Address f = rf.getFromAddress(); RefType t = rf.getReferenceType();
                    { Instruction fi = li.getInstructionContaining(f); if (fi != null && fi.getFlowType().isComputed()) continue; }   // jmp [reg*4+table]
                    String why = null;
                    if (t.isCall()) why = "call";
                    else if (t.isData() && li.getInstructionContaining(f) != null) why = "dataI";
                    else if (t.isData() && (f.getOffset() < TEXT_LO || f.getOffset() >= TEXT_HI)) why = "dataD";
                    else if (t.isJump() && !t.isConditional()) {
                        ghidra.program.model.listing.Function ff = getFunctionContaining(f);
                        if (ff != null) {
                            AddressSetView b = ff.getBody();
                            if (to.compareTo(b.getMinAddress()) < 0 || to.compareTo(b.getMaxAddress()) > 0) why = "tail";
                        }
                    }
                    if (why != null && !why.equals("call") && jumpTargets.contains(to.getOffset())) why = null;      // jump-table target without a call: case block
                    if (why != null) { String o = entries.get(to); if (o == null || !o.contains(why)) entries.put(to, (o == null ? "" : o + "+") + why); }
                }
            }
        }
        // create in address order; skip entries that became part of an earlier created function
        int made = 0; Set<Address> newFns = new TreeSet<>();
        try (PrintWriter w = new PrintWriter(new FileWriter(a[1] + "_entries.csv"))) {
            w.println("entry,why,created,size,ninstr,ranges,calls,end");
            for (Map.Entry<Address, String> e : entries.entrySet()) {
                Address ad = e.getKey();
                if (getFunctionContaining(ad) != null) { w.println(ad + "," + e.getValue() + ",inside,0,0,0,0,"); continue; }
                Address[] itm = null; for (Address[] it : items) if (ad.compareTo(it[0]) >= 0 && ad.compareTo(it[1]) < 0) itm = it;
                AddressSet snap = new AddressSet(); if (itm != null) for (Instruction ii : li.getInstructions(new AddressSet(itm[0], itm[1].subtract(1)), true)) snap.add(ii.getAddress(), ii.getAddress().add(ii.getLength() - 1));
                boolean pre = li.getInstructionAt(ad) != null;
                if (!pre) disassemble(ad);
                ghidra.program.model.listing.Function f = li.getInstructionAt(ad) == null ? null : createFunction(ad, null);
                if (f == null) { w.println(ad + "," + e.getValue() + ",FAIL,0,0,0,0,"); continue; }
                String bad = validate(f, itm);
                for (int retry = 0; retry < 3 && bad != null && bad.startsWith("end:"); retry++) {        // flow stopped at a misaligned orphan instruction decoded by the original analysis: drop that instruction, redo
                    Instruction lastI = null; for (Instruction i : li.getInstructions(f.getBody(), true)) lastI = i;
                    if (lastI == null) break;
                    Address nx = lastI.getMaxAddress().add(1); Instruction ov = li.getInstructionContaining(nx);
                    if (ov == null) for (int k = 1; k <= 3 && ov == null; k++) ov = li.getInstructionContaining(nx.add(k));
                    if (ov == null || getFunctionContaining(ov.getAddress()) != null) break;
                    if (ov.getAddress().equals(nx)) break;
                    removeFunction(f); li.clearCodeUnits(ov.getMinAddress(), ov.getMaxAddress(), false); disassemble(ad); f = createFunction(ad, null);
                    if (f == null) break; bad = validate(f, itm); if (bad == null) w.println(ad + ",retry-fixed,,,,,,");
                }
                if (f == null) { w.println(ad + "," + e.getValue() + ",FAIL2,0,0,0,0,"); continue; }
                if (bad != null) {
                    AddressSet bb = new AddressSet(f.getBody()); removeFunction(f);
                    if (itm != null) for (AddressRange rg : bb.intersect(new AddressSet(itm[0], itm[1].subtract(1))).subtract(snap)) li.clearCodeUnits(rg.getMinAddress(), rg.getMaxAddress(), false);
                    w.println(ad + "," + e.getValue() + ",REJECT:" + bad + ",0,0,0,0,"); continue;
                }
                made++; newFns.add(ad);
                int ni = 0, calls = 0; String end = "";
                for (Instruction i : li.getInstructions(f.getBody(), true)) { ni++; if (i.getFlowType().isCall()) calls++; end = i.getMnemonicString(); }
                w.println(ad + "," + e.getValue() + ",yes," + f.getBody().getNumAddresses() + "," + ni + "," + f.getBody().getNumAddressRanges() + "," + calls + "," + end);
            }
        }
        switchPhase(items, newFns, a[1] + "_switch1.csv");
        println("entries " + entries.size() + " created " + made + (commit ? " (commit)" : " (dry run)"));

        // phase 2: linear sweep over the still-undefined bytes of every item; accept only clean function-shaped code
        int swept = 0, rejected = 0;
        try (PrintWriter w = new PrintWriter(new FileWriter(a[1] + "_sweep.csv"))) {
            w.println("entry,result,size,ninstr,calls,end,note");
            for (Address[] it : items) {
                Address pos = it[0];
                while (pos.compareTo(it[1]) < 0) {
                    CodeUnit cu = li.getCodeUnitContaining(pos);
                    if (cu == null) { pos = pos.add(1); continue; }
                    if ((cu instanceof Instruction && getFunctionContaining(pos) != null) || (cu instanceof Data && ((Data) cu).isDefined())) { pos = cu.getMaxAddress().add(1); continue; }
                    int v = getByte(pos) & 0xff;
                    if (v == 0xcc || v == 0x90) { pos = pos.add(1); continue; }
                    { int padlen = nopPad(pos); if (padlen > 0) { pos = pos.add(padlen); continue; } }
                    if (pos.equals(it[0]) && pos.compareTo(toAddr(0x401000L)) > 0) {        // item start right behind undefined non-padding bytes: the item continues a code run of the previous item, not a function start
                        Address pv = pos.subtract(1); CodeUnit pc = li.getCodeUnitContaining(pv); int pb = getByte(pv) & 0xff;
                        if (pc instanceof Data && !((Data) pc).isDefined() && pb != 0x90 && pb != 0xcc && pb != 0x00 && getFunctionContaining(pv) == null) {
                            Address q = pos; while (q.compareTo(it[1]) < 0) { int b2 = getByte(q) & 0xff; CodeUnit c2 = li.getCodeUnitContaining(q); if (b2 == 0xcc || b2 == 0x90 || !(c2 instanceof Data) || ((Data) c2).isDefined()) break; q = q.add(1); }
                            pos = q.compareTo(pos) > 0 ? q : pos.add(1); continue;
                        }
                    }
                    if (ptrRun(pos, it[1])) { Address q = pos; while (q.compareTo(it[1]) < 0 && ptrAt(q, it[1].add(0x100000))) q = q.add(4); pos = q; continue; }
                    if (v == 0 && getByte(pos.add(1)) == 0) { pos = pos.add(1); continue; }
                    if (jumpTargets.contains(pos.getOffset())) { pos = pos.add(1); while (pos.compareTo(it[1]) < 0 && li.getCodeUnitContaining(pos) instanceof Data && !((Data) li.getCodeUnitContaining(pos)).isDefined() && !jumpTargets.contains(pos.getOffset()) && (getByte(pos) & 0xff) != 0xcc) pos = pos.add(1); continue; }
                    // trial
                    boolean ok = false; String note = ""; ghidra.program.model.listing.Function f = null;
                    AddressSet snap = new AddressSet(); for (Instruction ii : li.getInstructions(new AddressSet(it[0], it[1].subtract(1)), true)) snap.add(ii.getAddress(), ii.getAddress().add(ii.getLength() - 1)); AddressSet sfn = new AddressSet(); for (ghidra.program.model.listing.Function ff : currentProgram.getFunctionManager().getFunctions(new AddressSet(it[0], it[1].subtract(1)), true)) sfn.add(ff.getBody()); boolean pre = li.getInstructionAt(pos) != null; boolean fl = pre || disassemble(pos); if (fl && hitsExisting(pos, it, sfn)) { fl = false; note = "hits-existing-fn"; } if (fl) f = createFunction(pos, null);
                    int ni = 0, calls = 0; String end = ""; long size = 0;
                    if (f != null) {
                        ok = true;
                        for (Instruction i : li.getInstructions(f.getBody(), true)) {
                            ni++; end = i.getMnemonicString(); String m = end;
                            if (i.getFlowType().isCall()) calls++;
                            if (m.equals("INT3") || m.equals("HLT") || m.equals("IN") || m.equals("OUT") || m.equals("INSB") || m.equals("INSW") || m.equals("OUTSB") || m.equals("OUTSW") || m.equals("CLI") || m.equals("STI") || m.equals("ARPL") || m.equals("BOUND") || m.equals("LES") || m.equals("LDS") || m.equals("SALC") || m.equals("AAM") || m.equals("AAD") || m.equals("DAA") || m.equals("DAS") || m.equals("AAA") || m.equals("AAS") || m.equals("INTO") || m.equals("IRET") || m.equals("RETF") || m.equals("XLAT") || m.equals("WAIT") || m.equals("LOCK")) { ok = false; note = "bad:" + m; break; }
                            if (i.getBytes().length == 2 && i.getByte(0) == 0 && i.getByte(1) == 0) { ok = false; note = "add[eax],al"; break; }
                            if (i.getAddress().compareTo(it[1]) >= 0) { ok = false; note = "leaves item"; break; }
                        }
                        if (ok && !(end.equals("RET") || end.equals("JMP"))) { ok = false; note = "end:" + end; }
                        if (ok && f.getBody().getMaxAddress().compareTo(it[1]) >= 0) { ok = false; note = "past item"; }
                        size = f.getBody().getNumAddresses();
                    } else if (note.isEmpty()) note = "nofn";
                    if (ok) { swept++; newFns.add(pos); w.println(pos + ",yes," + size + "," + ni + "," + calls + "," + end + ","); AddressSetView b = f.getBody(); pos = b.getMaxAddress().add(1); }
                    else {
                        rejected++; w.println(pos + ",no," + size + "," + ni + "," + calls + "," + end + "," + note);
                        if (f != null) { AddressSet bb = new AddressSet(f.getBody()); removeFunction(f); AddressSet fresh = bb.intersect(new AddressSet(it[0], it[1].subtract(1))).subtract(snap); for (AddressRange rg : fresh) li.clearCodeUnits(rg.getMinAddress(), rg.getMaxAddress(), false); }
                        else { Instruction i0 = li.getInstructionAt(pos); if (i0 != null && !pre) li.clearCodeUnits(pos, pos.add(Math.max(0, i0.getLength() - 1)), false); }
                        // skip to the end of this non-pad run
                        Address q = pos.add(1);
                        while (q.compareTo(it[1]) < 0) { CodeUnit c2 = li.getCodeUnitContaining(q); if (c2 == null || (c2 instanceof Instruction && getFunctionContaining(q) != null) || (c2 instanceof Data && ((Data) c2).isDefined())) break; int b2 = getByte(q) & 0xff; if ((b2 == 0xcc)) break; q = q.add(1); }
                        pos = q;
                    }
                }
            }
        }
        println("sweep accepted " + swept + " rejected " + rejected);

        switchPhase(items, newFns, a[1] + "_switch2.csv");
        // residual report per item
        try (PrintWriter w = new PrintWriter(new FileWriter(a[1] + "_resid.csv"))) {
            for (Address[] it : items) {
                StringBuilder cs = new StringBuilder(); int F = 0, O = 0, D = 0, U = 0; int size = (int) it[1].subtract(it[0]);
                for (int i = 0; i < size; i++) {
                    Address ad = it[0].add(i); CodeUnit cu = li.getCodeUnitContaining(ad); 
                    if (cu instanceof Instruction) { if (getFunctionContaining(ad) != null) { F++; cs.append('F'); } else { O++; cs.append('O'); } }
                    else if (cu instanceof Data && ((Data) cu).isDefined()) { D++; cs.append('D'); } else { U++; cs.append('U'); }
                }
                w.println(it[0] + "," + it[1] + "," + F + "," + O + "," + D + "," + U + "," + cs);
            }
        }
        currentProgram.endTransaction(tx, commit);
    }

    private boolean ptrAt(Address q, Address lim) {
        try { long v = currentProgram.getMemory().getInt(q) & 0xffffffffL; return v >= TEXT_LO && v < 0x5e0000L; } catch (Exception e) { return false; }
    }
    private boolean ptrRun(Address q, Address lim) {
        return ptrAt(q, lim) && ptrAt(q.add(4), lim);
    }

    private int nopPad(Address q) {
        int[][] pats = {{0x8d,0xa4,0x24,0,0,0,0},{0x8d,0x9b,0,0,0,0},{0x8d,0xb6,0,0,0,0},{0x8d,0x80,0,0,0,0},{0x8d,0x64,0x24,0},{0x8d,0x74,0x26,0},{0x8d,0x49,0},{0x8d,0x40,0},{0x8d,0x76,0},{0x8d,0x52,0},{0x8d,0x64,0x24,0},{0x8b,0xff},{0x89,0xf6},{0x8b,0xc0},{0x8b,0xc9},{0x8b,0xd2}};
        for (int[] p : pats) { boolean m = true; for (int i = 0; i < p.length && m; i++) { try { if ((getByte(q.add(i)) & 0xff) != p[i]) m = false; } catch (Exception e) { m = false; } } if (m) return p.length; }
        return 0;
    }
    private boolean hitsExisting(Address pos, Address[] it, AddressSet sfn) {
        Listing li = currentProgram.getListing();
        Deque<Address> wl = new ArrayDeque<>(); wl.add(pos); Set<Address> seen = new HashSet<>();
        while (!wl.isEmpty()) {
            Address c = wl.poll(); if (!seen.add(c)) continue;
            if (sfn.contains(c)) return true;
            Instruction ci = li.getInstructionAt(c); if (ci == null) continue;
            if (ci.getFlowType().isTerminal()) { if (ci.getFlowType().isJump() && !ci.getFlowType().isComputed()) for (Address d : ci.getFlows()) wl.add(d); continue; }
            for (Address d : ci.getFlows()) if (!ci.getFlowType().isCall()) wl.add(d);
            if (ci.getFallThrough() != null) wl.add(ci.getFallThrough());
        }
        return false;
    }

    private String validate(ghidra.program.model.listing.Function f, Address[] it) {
        Listing li = currentProgram.getListing(); String end = "";
        for (Instruction i : li.getInstructions(f.getBody(), true)) {
            end = i.getMnemonicString(); String m = end;
            if (m.equals("INT3") || m.equals("HLT") || m.equals("IN") || m.equals("OUT") || m.equals("INSB") || m.equals("INSW") || m.equals("OUTSB") || m.equals("OUTSW") || m.equals("CLI") || m.equals("STI") || m.equals("ARPL") || m.equals("BOUND") || m.equals("LES") || m.equals("LDS") || m.equals("SALC") || m.equals("AAM") || m.equals("AAD") || m.equals("DAA") || m.equals("DAS") || m.equals("AAA") || m.equals("AAS") || m.equals("INTO") || m.equals("IRET") || m.equals("RETF") || m.equals("XLAT") || m.equals("WAIT") || m.equals("LOCK")) return "bad:" + m;
            try { if (i.getLength() == 2 && i.getByte(0) == 0 && i.getByte(1) == 0) return "add[eax],al"; } catch (Exception e) {}
        }
        if (!(end.equals("RET") || end.equals("JMP"))) return "end:" + end;
        return null;
    }
    private void switchPhase(List<Address[]> items, Set<Address> newFns, String out) throws Exception {
        Listing li = currentProgram.getListing(); int sw = 0, swb = 0;
        try (PrintWriter w = new PrintWriter(new FileWriter(out))) {
            w.println("fn,jmp,table,ntargets,addedbytes");
            for (Address fe : newFns) {
                ghidra.program.model.listing.Function f = getFunctionAt(fe); if (f == null) continue;
                Address[] item = null; for (Address[] it : items) if (fe.compareTo(it[0]) >= 0 && fe.compareTo(it[1]) < 0) item = it;
                if (item == null) continue;
                for (int round = 0; round < 3; round++) {
                    AddressSet body = new AddressSet(f.getBody()); int add0 = (int) body.getNumAddresses();
                    for (Instruction i : li.getInstructions(f.getBody(), true)) {
                        if (!i.getFlowType().isComputed() || !i.getFlowType().isJump()) continue;
                        long tb = -1;
                        for (int op = 0; op < i.getNumOperands(); op++) for (Object o : i.getOpObjects(op)) if (o instanceof ghidra.program.model.scalar.Scalar) { long v = ((ghidra.program.model.scalar.Scalar) o).getUnsignedValue(); if (v >= TEXT_LO && v < 0x5e0000L) tb = v; }
                        if (tb < 0) continue;
                        Address tbl = toAddr(tb); int nt = 0;
                        for (int k = 0; k < 512; k++) {
                            long v; try { v = currentProgram.getMemory().getInt(tbl.add(4 * k)) & 0xffffffffL; } catch (Exception e) { break; }
                            Address t = toAddr(v);
                            if (!inItems(items, t) || Math.abs(t.subtract(fe)) > 0x3000) break;
                            if (getFunctionContaining(t) != null && !f.getBody().contains(t)) break;
                            if (body.contains(t)) { nt++; continue; }
                            if (li.getInstructionAt(t) == null) { if (!disassemble(t)) break; }
                            // flow closure
                            Deque<Address> wl = new ArrayDeque<>(); wl.add(t);
                            while (!wl.isEmpty()) {
                                Address c = wl.poll(); if (body.contains(c)) continue;
                                if (!inItems(items, c)) continue;
                                Instruction ci = li.getInstructionAt(c); if (ci == null) continue;
                                if (getFunctionContaining(c) != null && !f.getBody().contains(c)) continue;
                                body.add(c, c.add(ci.getLength() - 1));
                                if (ci.getFlowType().isTerminal()) { if (ci.getFlowType().isJump() && !ci.getFlowType().isComputed()) for (Address d : ci.getFlows()) wl.add(d); continue; }
                                for (Address d : ci.getFlows()) if (!ci.getFlowType().isCall()) wl.add(d);
                                if (ci.getFallThrough() != null) wl.add(ci.getFallThrough());
                            }
                            nt++;
                        }
                        w.println(fe + "," + i.getAddress() + "," + tbl + "," + nt + ",");
                    }
                    { AddressSet span = new AddressSet(f.getBody().getMinAddress(), f.getBody().getMaxAddress()); // orphan instruction blocks inside the function's own address span
                      for (Instruction i : li.getInstructions(span, true)) if (getFunctionContaining(i.getAddress()) == null && inItems(items, i.getAddress())) body.add(i.getAddress(), i.getAddress().add(i.getLength() - 1)); }
                    int added = (int) body.getNumAddresses() - add0;
                    if (added <= 0) break;
                    try { f.setBody(body); sw++; w.println(fe + ",body,,," + added); } catch (Exception e) { swb++; w.println(fe + ",ERR,,," + e.getMessage()); break; }
                }
            }
        }
        println("switch bodies extended " + sw + " errors " + swb);
    }
    private boolean inItems(List<Address[]> items, Address q) { for (Address[] it : items) if (q.compareTo(it[0]) >= 0 && q.compareTo(it[1]) < 0) return true; return false; }
}
