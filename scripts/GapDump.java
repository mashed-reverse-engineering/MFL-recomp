//Per-byte class (F instr in function, O instr no function, D defined data, U undefined) + hex + inbound refs for gap items. Args: <listfile> <outfile> <refsfile>
//@category Mashed
import java.io.*;
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.mem.*;
import ghidra.program.model.symbol.*;

public class GapDump extends GhidraScript {
    @Override
    protected void run() throws Exception {
        String[] a = getScriptArgs();
        Listing li = currentProgram.getListing();
        Memory mem = currentProgram.getMemory();
        ReferenceManager rm = currentProgram.getReferenceManager();
        try (PrintWriter w = new PrintWriter(new FileWriter(a[1])); PrintWriter wr = new PrintWriter(new FileWriter(a[2])); BufferedReader r = new BufferedReader(new FileReader(a[0]))) {
            String ln;
            while ((ln = r.readLine()) != null) {
                String[] p = ln.trim().split("\\s+");
                if (p.length < 2) continue;
                Address lo = toAddr(Long.parseLong(p[0], 16)), hi = toAddr(Long.parseLong(p[1], 16));
                int size = (int) hi.subtract(lo);
                StringBuilder cls = new StringBuilder(), hx = new StringBuilder();
                byte[] b = new byte[size]; try { mem.getBytes(lo, b); } catch (Exception e) {}
                for (byte x : b) hx.append(String.format("%02x", x));
                for (int i = 0; i < size; i++) {
                    Address ad = lo.add(i);
                    CodeUnit cu = li.getCodeUnitContaining(ad);
                    char c = 'U';
                    if (cu instanceof Instruction) c = getFunctionContaining(ad) != null ? 'F' : 'O';
                    else if (cu instanceof Data && ((Data) cu).isDefined()) c = 'D';
                    cls.append(c);
                }
                w.println(p[0] + "," + p[1] + "," + cls + "," + hx);
                AddressSet s = new AddressSet(lo, hi.subtract(1));
                for (Address to : rm.getReferenceDestinationIterator(s, true)) {
                    for (Reference rf : rm.getReferencesTo(to)) {
                        Address f = rf.getFromAddress();
                        if (s.contains(f)) continue;
                        ghidra.program.model.listing.Function ff = getFunctionContaining(f);
                        wr.println(p[0] + "," + to + "," + f + "," + rf.getReferenceType() + "," + (ff == null ? "-" : ff.getName() + "@" + ff.getEntryPoint()));
                    }
                }
            }
        }
    }
}
