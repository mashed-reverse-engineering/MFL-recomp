//Carry function names from one executable to another (same code base, different addresses) by an address independent signature.
//  export <out.tsv>             writes: signature hash, instruction count, name, old address, for every function that has a real name
//  import <in.tsv> <report.csv> renames the still unnamed FUN_* functions whose signature matches exactly one exported name
//Signature = MD5 over "mnemonic operand,operand" of every instruction with address-like hex numbers (5+ digits) replaced by '#'.
//Several exported functions with the same signature, or several unnamed candidates, are skipped as ambiguous.
//@category Mashed
import java.io.*;
import java.nio.file.*;
import java.security.MessageDigest;
import java.util.*;
import java.util.regex.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.SourceType;

public class PortNames extends GhidraScript {
    static final Pattern ADDR = Pattern.compile("0x[0-9a-fA-F]{5,}");
    static final Pattern ANYNUM = Pattern.compile("-?0x[0-9a-fA-F]+|\\b[0-9]+\\b");
    boolean loose = false;

    String signature(Function f, int[] count) throws Exception {
        StringBuilder sb = new StringBuilder();
        int n = 0;
        for (Instruction in : currentProgram.getListing().getInstructions(f.getBody(), true)) {
            sb.append(in.getMnemonicString());
            for (int i = 0; i < in.getNumOperands(); i++) {
                String r = in.getDefaultOperandRepresentation(i);
                sb.append(' ').append(loose ? ANYNUM.matcher(r).replaceAll("#") : ADDR.matcher(r).replaceAll("#"));
            }
            sb.append(';');
            n++;
        }
        count[0] = n;
        MessageDigest md = MessageDigest.getInstance("MD5");
        StringBuilder hex = new StringBuilder();
        for (byte b : md.digest(sb.toString().getBytes("UTF-8"))) hex.append(String.format("%02x", b));
        return hex.toString();
    }

    // the exported file maps string -> name; ex is keyed by name, so keep a second index by string
    Map<String, List<String[]>> strIdx = null;
    List<String[]> strIndex(Map<String, List<String[]>> ex, String key) {
        if (strIdx == null) {
            strIdx = new HashMap<>();
            for (List<String[]> l : ex.values()) for (String[] p : l) strIdx.computeIfAbsent(p[0], k -> new ArrayList<>()).add(p);
        }
        return strIdx.get(key);
    }

    @Override
    protected void run() throws Exception {
        String[] a = getScriptArgs();
        loose = a.length > 0 && a[a.length - 1].equals("loose");
        int minCount = loose ? 12 : 4;
        if (a[0].equals("strexport") || a[0].equals("strimport")) {
            // function <-> strings it references; a string is a key when exactly one function references it
            Map<String, Set<Function>> byString = new HashMap<>();
            for (Data d : currentProgram.getListing().getDefinedData(true)) {
                if (!d.hasStringValue()) continue;
                String s = String.valueOf(d.getValue());
                if (s.length() < 6) continue;
                for (ghidra.program.model.symbol.Reference r : getReferencesTo(d.getAddress())) {
                    Function f = getFunctionContaining(r.getFromAddress());
                    if (f != null) byString.computeIfAbsent(s, k -> new HashSet<>()).add(f);
                }
            }
            if (a[0].equals("strexport")) {
                try (PrintWriter w = new PrintWriter(new FileWriter(a[1]))) {
                    int n = 0;
                    for (Map.Entry<String, Set<Function>> e : byString.entrySet()) {
                        if (e.getValue().size() != 1) continue;
                        Function f = e.getValue().iterator().next();
                        String nm = f.getName();
                        if (nm.startsWith("FUN_") || nm.startsWith("thunk_FUN_")) continue;
                        w.println(e.getKey().replace("\t", " ").replace("\n", "\\n") + "\t" + nm + "\t" + f.getEntryPoint());
                        n++;
                    }
                    println("exported " + n + " string keys");
                }
                return;
            }
            Map<String, List<String[]>> ex = new HashMap<>();
            for (String line : Files.readAllLines(Path.of(a[1]))) {
                String[] p = line.split("\t");
                if (p.length >= 3) ex.computeIfAbsent(p[1], k -> new ArrayList<>()).add(p);
            }
            // a function is ported when all its matching strings agree on one name
            Map<Function, Set<String>> votes = new HashMap<>();
            for (Map.Entry<String, Set<Function>> e : byString.entrySet()) {
                if (e.getValue().size() != 1) continue;
                String key = e.getKey().replace("\t", " ").replace("\n", "\\n");
                Function f = e.getValue().iterator().next();
                if (!f.getName().startsWith("FUN_")) continue;
                for (Map.Entry<String, List<String[]>> x : ex.entrySet()) { /* filled below */ break; }
                List<String[]> hits = strIndex(ex, key);
                if (hits != null) for (String[] h : hits) votes.computeIfAbsent(f, k -> new TreeSet<>()).add(h[1]);
            }
            StringBuilder csv = new StringBuilder("# address,name,comment  (ported by PortNames.java strimport)\n");
            Set<String> used = new HashSet<>();
            int ok = 0, amb = 0;
            for (Map.Entry<Function, Set<String>> e : votes.entrySet()) {
                if (e.getValue().size() != 1) { amb++; continue; }
                String name = e.getValue().iterator().next();
                if (!used.add(name) || !getSymbols(name, null).isEmpty()) { amb++; continue; }
                e.getKey().setName(name, SourceType.USER_DEFINED);
                csv.append(String.format("%s,%s,ported by a unique string%n", e.getKey().getEntryPoint(), name));
                ok++;
            }
            try (FileWriter w = new FileWriter(a[2])) { w.write(csv.toString()); }
            println("ported " + ok + " by strings, ambiguous " + amb);
            return;
        }
        if (a[0].equals("export")) {
            try (PrintWriter w = new PrintWriter(new FileWriter(a[1]))) {
                int n = 0;
                for (Function f : currentProgram.getListing().getFunctions(true)) {
                    String nm = f.getName();
                    if (nm.startsWith("FUN_") || nm.startsWith("thunk_FUN_") || nm.startsWith("LAB_")) continue;
                    int[] c = new int[1];
                    String sig = signature(f, c);
                    if (c[0] < minCount) continue;
                    w.println(sig + "\t" + c[0] + "\t" + nm + "\t" + f.getEntryPoint());
                    n++;
                }
                println("exported " + n);
            }
            return;
        }
        Map<String, List<String[]>> exported = new HashMap<>();
        for (String line : Files.readAllLines(Path.of(a[1]))) {
            String[] p = line.split("\t");
            if (p.length < 4) continue;
            exported.computeIfAbsent(p[0], k -> new ArrayList<>()).add(p);
        }
        Map<String, List<Function>> cand = new HashMap<>();
        for (Function f : currentProgram.getListing().getFunctions(true)) {
            if (!f.getName().startsWith("FUN_")) continue;
            int[] c = new int[1];
            String sig = signature(f, c);
            if (c[0] < minCount) continue;
            cand.computeIfAbsent(sig, k -> new ArrayList<>()).add(f);
        }
        StringBuilder csv = new StringBuilder("# address,name,comment  (ported by PortNames.java)\n");
        Set<String> used = new HashSet<>();
        int ok = 0, amb = 0, missing = 0;
        for (Map.Entry<String, List<Function>> e : cand.entrySet()) {
            List<String[]> ex = exported.get(e.getKey());
            if (ex == null) { missing++; continue; }
            Set<String> names = new HashSet<>();
            for (String[] p : ex) names.add(p[2]);
            if (e.getValue().size() != 1 || names.size() != 1) { amb++; continue; }
            String name = names.iterator().next();
            if (!used.add(name) || !getSymbols(name, null).isEmpty()) { amb++; continue; }
            Function f = e.getValue().get(0);
            f.setName(name, SourceType.USER_DEFINED);
            csv.append(String.format("%s,%s,ported from %s%n", f.getEntryPoint(), name, ex.get(0)[3]));
            ok++;
        }
        try (FileWriter w = new FileWriter(a[2])) { w.write(csv.toString()); }
        println("ported " + ok + ", ambiguous " + amb + ", no match " + missing);
    }
}
