import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import java.util.*;

// args: <outfile> <depth> <addr>...   decompiles functions and their callees to <depth>
public class DecompAt extends GhidraScript {
    public void run() throws Exception {
        String[] a = getScriptArgs();
        int depth = Integer.parseInt(a[1]);
        DecompInterface di = new DecompInterface();
        di.openProgram(currentProgram);
        Set<Function> seen = new LinkedHashSet<>();
        List<Function> cur = new ArrayList<>();
        for (int i = 2; i < a.length; i++) { Address ad = toAddr(a[i]); Function fn = getFunctionAt(ad); if (fn == null) { disassemble(ad); fn = createFunction(ad, null); } cur.add(fn); }
        StringBuilder sb = new StringBuilder();
        for (int d = 0; d <= depth; d++) {
            List<Function> next = new ArrayList<>();
            for (Function f : cur) {
                if (f == null || !seen.add(f)) continue;
                DecompileResults r = di.decompileFunction(f, 60, monitor);
                sb.append("// ==== ").append(f.getName()).append(" @ ").append(f.getEntryPoint()).append(" (depth ").append(d).append(")\n");
                if (r.decompileCompleted()) sb.append(r.getDecompiledFunction().getC()).append("\n");
                next.addAll(f.getCalledFunctions(monitor));
            }
            cur = next;
        }
        java.nio.file.Files.writeString(java.nio.file.Path.of(a[0]), sb.toString());
        println("wrote " + a[0] + " " + seen.size() + " functions");
    }
}
