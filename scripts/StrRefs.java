//Decompile functions referencing strings containing any key. Args: <outfile> <key>...
//@category Mashed
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;

public class StrRefs extends GhidraScript {
    @Override
    protected void run() throws Exception {
        String[] a = getScriptArgs();
        Map<Function, List<String>> fns = new TreeMap<>(Comparator.comparing(Function::getEntryPoint));
        for (Data d : currentProgram.getListing().getDefinedData(true)) {
            if (!d.hasStringValue()) continue;
            String s = String.valueOf(d.getValue());
            boolean hit = false;
            for (int i = 1; i < a.length; i++) if (s.toLowerCase().contains(a[i].toLowerCase())) hit = true;
            if (!hit) continue;
            for (Reference r : getReferencesTo(d.getAddress())) {
                Function f = getFunctionContaining(r.getFromAddress());
                if (f != null) fns.computeIfAbsent(f, k -> new ArrayList<>()).add(s.replace("\n", "\\n"));
            }
        }
        DecompInterface di = new DecompInterface();
        di.openProgram(currentProgram);
        StringBuilder sb = new StringBuilder();
        for (Map.Entry<Function, List<String>> e : fns.entrySet()) {
            Function f = e.getKey();
            DecompileResults r = di.decompileFunction(f, 60, monitor);
            sb.append("// ==== ").append(f.getName()).append(" @ ").append(f.getEntryPoint()).append("  strings: ").append(e.getValue()).append("\n");
            if (r.decompileCompleted()) sb.append(r.getDecompiledFunction().getC()).append("\n");
        }
        java.nio.file.Files.writeString(java.nio.file.Path.of(a[0]), sb.toString());
        println("wrote " + a[0] + " " + fns.size() + " functions");
    }
}
