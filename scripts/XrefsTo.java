import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import java.util.*;

// args: <outfile> <addr>...  decompiles every function referencing the addresses
public class XrefsTo extends GhidraScript {
    public void run() throws Exception {
        String[] a = getScriptArgs();
        Set<Function> fns = new TreeSet<>(Comparator.comparing(Function::getEntryPoint));
        for (int i = 1; i < a.length; i++)
            for (Reference r : getReferencesTo(toAddr(a[i]))) {
                Function f = getFunctionContaining(r.getFromAddress());
                println(a[i] + " <- " + r.getFromAddress() + " " + (f == null ? "?" : f.getName()));
                if (f != null) fns.add(f);
            }
        DecompInterface di = new DecompInterface();
        di.openProgram(currentProgram);
        StringBuilder sb = new StringBuilder();
        for (Function f : fns) {
            DecompileResults r = di.decompileFunction(f, 60, monitor);
            sb.append("// ==== ").append(f.getName()).append(" @ ").append(f.getEntryPoint()).append("\n");
            if (r.decompileCompleted()) sb.append(r.getDecompiledFunction().getC()).append("\n");
        }
        java.nio.file.Files.writeString(java.nio.file.Path.of(a[0]), sb.toString());
        println("wrote " + a[0] + " " + fns.size());
    }
}
