//Name unnamed functions after a "<name>:" prefix of a string they reference (qhull's trace messages start with the function name).
//Args: <regex with one group matching the name at the start of the string> [<csv out>]   e.g. ^(qh_[A-Za-z0-9_]+):
//A function is named only if it is still FUN_*, and every matching string it references gives the same name.
//@category Mashed
import java.io.*;
import java.util.*;
import java.util.regex.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;

public class NameByStrings extends GhidraScript {
    @Override
    protected void run() throws Exception {
        String[] a = getScriptArgs();
        Pattern p = Pattern.compile(a[0]);
        Map<Function, Set<String>> names = new HashMap<>();
        for (Data d : currentProgram.getListing().getDefinedData(true)) {
            if (!d.hasStringValue()) continue;
            Matcher m = p.matcher(String.valueOf(d.getValue()));
            if (!m.find()) continue;
            for (Reference r : getReferencesTo(d.getAddress())) {
                Function f = getFunctionContaining(r.getFromAddress());
                if (f != null) names.computeIfAbsent(f, k -> new TreeSet<>()).add(m.group(1));
            }
        }
        Map<String, Integer> used = new HashMap<>();
        int n = 0, amb = 0, taken = 0;
        StringBuilder csv = new StringBuilder();
        for (Map.Entry<Function, Set<String>> e : names.entrySet()) {
            Function f = e.getKey();
            if (!f.getName().startsWith("FUN_")) continue;
            if (e.getValue().size() != 1) { amb++; continue; }
            String name = e.getValue().iterator().next();
            if (used.containsKey(name) || !getSymbols(name, null).isEmpty()) { taken++; continue; }
            used.put(name, 1);
            f.setName(name, SourceType.USER_DEFINED);
            csv.append(String.format("%s,%s,named from its trace strings%n", f.getEntryPoint(), name));
            n++;
        }
        if (a.length > 1) try (FileWriter w = new FileWriter(a[1])) { w.write(csv.toString()); }
        println("named " + n + ", ambiguous " + amb + ", name already used " + taken);
    }
}
