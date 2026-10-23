//Context of the functions in a list file (first column hex): inbound refs with from-function / from-data symbol, strings, named callees. Args: <listfile> <outfile>
//@category Mashed
import java.io.*;
import java.nio.file.*;
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
public class GapContext extends GhidraScript {
    String sym(Address a) {
        Listing li = currentProgram.getListing(); SymbolTable st = currentProgram.getSymbolTable();
        Symbol s = st.getPrimarySymbol(a);
        if (s != null && s.getSymbolType() == SymbolType.LABEL) return s.getName() + "+0";
        Data d = li.getDataContaining(a);
        if (d != null) { Symbol t = st.getPrimarySymbol(d.getMinAddress()); long off = a.subtract(d.getMinAddress()); if (t != null) return t.getName() + "+" + off; }
        // nearest label before
        Address q = a;
        for (int i = 0; i < 0x400 && q != null; i++) { q = q.previous(); if (q == null) break; Symbol t = st.getPrimarySymbol(q); if (t != null && t.getSymbolType() == SymbolType.LABEL) return t.getName() + "+" + a.subtract(q); }
        return "-";
    }
    protected void run() throws Exception {
        String[] a = getScriptArgs();
        Listing li = currentProgram.getListing(); ReferenceManager rm = currentProgram.getReferenceManager();
        try (PrintWriter w = new PrintWriter(new FileWriter(a[1]))) {
            for (String line : Files.readAllLines(Path.of(a[0]))) {
                String[] p = line.trim().split("\\s+"); if (p.length == 0 || p[0].isEmpty()) continue;
                Address ad = toAddr(p[0]); ghidra.program.model.listing.Function f = getFunctionAt(ad); if (f == null) continue;
                for (Reference r : rm.getReferencesTo(ad)) {
                    Address fr = r.getFromAddress(); ghidra.program.model.listing.Function ff = getFunctionContaining(fr);
                    boolean inI = li.getInstructionContaining(fr) != null;
                    w.println("R\t" + p[0] + "\t" + fr + "\t" + r.getReferenceType() + "\t" + (ff == null ? "-" : ff.getName() + "@" + ff.getEntryPoint() + ":" + ff.getBody().getNumAddresses()) + "\t" + (inI ? "I" : "D") + "\t" + (inI ? li.getInstructionContaining(fr).toString().replace("\t"," ") : sym(fr)));
                }
                LinkedHashSet<String> strs = new LinkedHashSet<>(), callees = new LinkedHashSet<>(), glob = new LinkedHashSet<>();
                for (Address x : f.getBody().getAddresses(true)) for (Reference r : rm.getReferencesFrom(x)) {
                    if (r.getReferenceType().isCall()) { ghidra.program.model.listing.Function c = getFunctionAt(r.getToAddress()); if (c != null) callees.add(c.getName()); }
                    else if (r.getReferenceType().isData()) { Data d = li.getDataAt(r.getToAddress()); if (d != null && d.hasStringValue()) { String s = String.valueOf(d.getValue()).replace("\n", "\\n").replace("\t", " "); strs.add(s.length() > 80 ? s.substring(0, 80) : s); } else { Symbol s = currentProgram.getSymbolTable().getPrimarySymbol(r.getToAddress()); if (s != null && glob.size() < 8) glob.add(s.getName()); } }
                }
                w.println("F\t" + p[0] + "\t" + f.getBody().getNumAddresses() + "\t" + String.join(" | ", strs) + "\t" + String.join(",", callees) + "\t" + String.join(",", glob));
            }
        }
    }
}
