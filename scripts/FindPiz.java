import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import java.util.*;

public class FindPiz extends GhidraScript {
    public void run() throws Exception {
        Set<Function> fns = new TreeSet<>(Comparator.comparing(Function::getEntryPoint));
        String[] keys = {"piz", "PIZ", ".dat"};
        for (Data d : currentProgram.getListing().getDefinedData(true)) {
            if (!d.hasStringValue()) continue; String s = String.valueOf(d.getValue());
            boolean hit = false;
            for (String k : keys) if (s.contains(k)) hit = true;
            if (!hit) continue;
            println("STR " + d.getAddress() + " \"" + s.replace("\n", "\\n") + "\"");
            for (Reference r : getReferencesTo(d.getAddress())) {
                Function f = getFunctionContaining(r.getFromAddress());
                println("   xref " + r.getFromAddress() + " in " + (f == null ? "?" : f.getName()));
                if (f != null) fns.add(f);
            }
        }
        DecompInterface di = new DecompInterface();
        di.openProgram(currentProgram);
        StringBuilder sb = new StringBuilder();
        for (Function f : fns) {
            DecompileResults r = di.decompileFunction(f, 60, monitor);
            sb.append("// ==== ").append(f.getName()).append(" @ ").append(f.getEntryPoint()).append("\n");
            if (r.decompileCompleted()) sb.append(r.getDecompiledFunction().getC()).append("\n");
        }
        String out = System.getProperty("user.home") + "/ghidra-projects/" + currentProgram.getName() + "_piz.c";
        java.nio.file.Files.writeString(java.nio.file.Path.of(out), sb.toString());
        println("wrote " + out + " (" + fns.size() + " functions)");
    }
}
