//Decompile the functions that contain the given addresses (no function creation). Args: <outfile> <addr>...
//@category Mashed
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.listing.*;

public class DecompContaining extends GhidraScript {
    @Override
    protected void run() throws Exception {
        String[] a = getScriptArgs();
        Set<Function> fns = new TreeSet<>(Comparator.comparing(Function::getEntryPoint));
        for (int i = 1; i < a.length; i++) {
            Function f = getFunctionContaining(toAddr(a[i]));
            println(a[i] + " -> " + (f == null ? "none" : f.getName() + " @ " + f.getEntryPoint()));
            if (f != null) fns.add(f);
        }
        DecompInterface di = new DecompInterface();
        di.openProgram(currentProgram);
        StringBuilder sb = new StringBuilder();
        for (Function f : fns) {
            DecompileResults r = di.decompileFunction(f, 90, monitor);
            sb.append("// ==== ").append(f.getName()).append(" @ ").append(f.getEntryPoint()).append("\n");
            if (r.decompileCompleted()) sb.append(r.getDecompiledFunction().getC()).append("\n");
        }
        java.nio.file.Files.writeString(java.nio.file.Path.of(a[0]), sb.toString());
    }
}
